# 检测hung task的内核线程
## init
```cpp
static int __init hung_task_init(void)
{
    atomic_notifier_chain_register(&panic_notifier_list, &panic_block);

    /* Disable hung task detector on suspend */
    pm_notifier(hungtask_pm_notify, 0);

    watchdog_task = kthread_run(watchdog, NULL, "khungtaskd");

    return 0;
}
```
从这里可见，为`khungtaskd`, 系统中只有一个这样的内核线程, 并非`per-cpu`的
```
[root@node-1 ~]# ps aux |grep hung
root       597  0.0  0.0      0     0 ?        S     2022   0:39 [khungtaskd]
root     54799  0.0  0.0 216640  1536 pts/1    S+   20:36   0:00 grep --color=auto hung
```
## watchdog
```cpp
static int watchdog(void *dummy)
{
    //============(1)==============
    unsigned long hung_last_checked = jiffies;

    set_user_nice(current, 0);

    for ( ; ; ) {
        unsigned long timeout = sysctl_hung_task_timeout_secs;
        //============(2)==============
        long t = hung_timeout_jiffies(hung_last_checked, timeout);

        //============(3)==============
        if (t <= 0) {
            if (!atomic_xchg(&reset_hung_task, 0) &&
                !hung_detector_suspended)
                check_hung_uninterruptible_tasks(timeout);
            hung_last_checked = jiffies;
            continue;
        }
        schedule_timeout_interruptible(t);
    }

    return 0;
}
```
大致流程
1. 开始获取当前的时刻: jiffies
2. 进入循环, 这里计算的是，看看当前时刻是否满足 > 上一次检查 + 
`sysctl_hung_task_timeout_secs`, 如果满足t > 0，则表示没有到达超时时间，
如果 < 0, 说明到达了超时时间，需要检测每个hung task, 看看有哪些超时了。</br>
`sysctl_hung_task_timeout_secs`
通过`/proc/sys/kernel/hung_task_timeout_secs`可以修改其值.

```cpp
static struct ctl_table sysctl_base_table[] = {
	...
	{
    .procname   = "hung_task_timeout_secs",
    .data       = &sysctl_hung_task_timeout_secs,
    .maxlen     = sizeof(unsigned long),
    .mode       = 0644,
    .proc_handler   = proc_dohung_task_timeout_secs,
    .extra2     = &hung_task_timeout_max,
	},
	...
};
```

3. 关于`reset_hung_task`, 如果该值为1, 则表示此次不再检查
hung task,相关代码:
```cpp
void reset_hung_task_detector(void)
{
    atomic_set(&reset_hung_task, 1);
    
}
void pvclock_touch_watchdogs(void)
{
    touch_softlockup_watchdog_sync();
    clocksource_touch_watchdog();
    rcu_cpu_stall_reset();
    reset_hung_task_detector();
}
```
目前就pvclock(kvmclock)用到了。
实际上是为了防止挂起qemu进程导致guest触发softlockup,hung task

## check_hung_uninterruptible_tasks
```cpp
static void check_hung_uninterruptible_tasks(unsigned long timeout)
{
    int max_count = sysctl_hung_task_check_count;
    int batch_count = HUNG_TASK_BATCHING;
    struct task_struct *g, *t;

    /*
     * If the system crashed already then all bets are off,
     * do not report extra hung tasks:
     */
    if (test_taint(TAINT_DIE) || did_panic)
        return;

    hung_task_show_lock = false;
    rcu_read_lock();
    for_each_process_thread(g, t) {
		//=========(1)============
        if (!max_count--)
            goto unlock;
        if (!--batch_count) {
            batch_count = HUNG_TASK_BATCHING;
            if (!rcu_lock_break(g, t))
                goto unlock;
        }
        /* use "==" to skip the TASK_KILLABLE tasks waiting on NFS */
		//=========(2)============
        if (t->state == TASK_UNINTERRUPTIBLE)
            check_hung_task(t, timeout);
    }
 unlock:
    rcu_read_unlock();
    if (hung_task_show_lock)
        debug_show_all_locks();
    if (hung_task_call_panic) {
        trigger_all_cpu_backtrace();
        panic("hung_task: blocked tasks");
    }
}
```
1. 可以通过proc文件系统配置每次检测hung task 的最大数量
2. 当task的state是`TASK_UNINTERRUPTIBLE`时，去检查当前task是否是hung住了

## check_hung_task
```cpp
static void check_hung_task(struct task_struct *t, unsigned long timeout)
{
    unsigned long switch_count = t->nvcsw + t->nivcsw;
	
	...
	//===========(1)=============
    if (switch_count != t->last_switch_count) {
        t->last_switch_count = switch_count;
        return;
    }
	...
	
    if (sysctl_hung_task_warnings) {
        if (sysctl_hung_task_warnings > 0)
            sysctl_hung_task_warnings--;
		//===================(2)=======================
        pr_err("INFO: task %s:%d blocked for more than %ld seconds.\n",
            t->comm, t->pid, timeout);
        pr_err("      %s %s %.*s\n",
            print_tainted(), init_utsname()->release,
            (int)strcspn(init_utsname()->version, " "),
            init_utsname()->version);
        pr_err("\"echo 0 > /proc/sys/kernel/hung_task_timeout_secs\""
            " disables this message.\n");
        sched_show_task(t);
        hung_task_show_lock = true;
    }
	...
}
```
1. 这里主要是比较的当前的`switch_count`和上一次的`switch_count`看其是否
发生进程切换
2. 如果发生了进程切换，则打印一系列的进程信息

## switch_count
关于switch_count
```cpp
static void __sched notrace __schedule(bool preempt)
{
	unsigned long *switch_count;
	...
	//====================(1)====================
	switch_count = &prev->nivcsw;
	...
	if (!preempt && prev->state) {
	    if (unlikely(signal_pending_state(prev->state, prev))) {
	        prev->state = TASK_RUNNING;
	    } else {
	        deactivate_task(rq, prev, DEQUEUE_SLEEP | DEQUEUE_NOCLOCK);
	        prev->on_rq = 0;
	
	        if (prev->in_iowait) {
	            atomic_inc(&rq->nr_iowait);
	            delayacct_blkio_start();
	        }
	
	        /*
	         * If a worker went to sleep, notify and ask workqueue
	         * whether it wants to wake up a task to maintain
	         * concurrency.
	         */
	        if (prev->flags & PF_WQ_WORKER) {
	            struct task_struct *to_wakeup;
	
	            to_wakeup = wq_worker_sleeping(prev);
	            if (to_wakeup)
	                try_to_wake_up_local(to_wakeup, &rf);
	        }
	    }
		//=============(1)=================
	    switch_count = &prev->nvcsw;
	}
	...
	if (likely(prev != next)) {
    	rq->nr_switches++;
    	rq->curr = next;
    	/*
    	 * The membarrier system call requires each architecture
    	 * to have a full memory barrier after updating
    	 * rq->curr, before returning to user-space.
    	 *
    	 * Here are the schemes providing that barrier on the
    	 * various architectures:
    	 * - mm ? switch_mm() : mmdrop() for x86, s390, sparc, PowerPC.
    	 *   switch_mm() rely on membarrier_arch_switch_mm() on PowerPC
    	 * - finish_lock_switch() for weakly-ordered
    	 *   architectures where spin_unlock is a full barrier,
    	 * - switch_to() for arm64 (weakly-ordered, spin_unlock
    	 *   is a RELEASE barrier),
    	 */
		//=============(1)===================
    	++*switch_count;

    	trace_sched_switch(preempt, prev, next);

    	/* Also unlocks the rq: */
    	rq = context_switch(rq, prev, next, &rf);
	} else {
	    rq->clock_update_flags &= ~(RQCF_ACT_SKIP|RQCF_REQ_SKIP);
	    rq_unlock_irq(rq, &rf);
	}
	
	balance_callback(rq);

}
```
1. 会根据是否抢占和prev->state是否是TASK_RUNNING, 选择更新`nivcsw`或`nvcsw`
2. 在进程切换的时候，将切出进程的上述成员`++`


