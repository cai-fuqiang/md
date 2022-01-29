# huge task
## init
```C/C++
static int __init hung_task_init(void)
{
        atomic_notifier_chain_register(&panic_notifier_list, &panic_block);

        /* Disable hung task detector on suspend */
        pm_notifier(hungtask_pm_notify, 0);

        watchdog_task = kthread_run(watchdog, NULL, "khungtaskd");

        return 0;
}
subsys_initcall(hung_task_init);
```

`hung_task`会在启动初初始化一个内核线程，名为`khungtaskd`，执行的函数
为`watchdog`

```
[root@node-3 ~]# ps aux |grep hung
root       598  0.0  0.0      0     0 ?        S    Jan28   0:00 [khungtaskd]
[root@node-3 ~]# cat /proc/598/stack
[<0>] __switch_to+0x6c/0x90
[<0>] watchdog+0x90/0x3a8
[<0>] kthread+0x130/0x138
[<0>] ret_from_fork+0x10/0x18
[<0>] 0xffffffffffffffff
```
## watchdog
```C/C++
static int watchdog(void *dummy)
{
        unsigned long hung_last_checked = jiffies;

        set_user_nice(current, 0);

        for ( ; ; ) {
                //===============(1)=========================
                unsigned long timeout = sysctl_hung_task_timeout_secs;

                //===============(2)=========================
                long t = hung_timeout_jiffies(hung_last_checked, timeout);

                if (t <= 0) {
                        if (!atomic_xchg(&reset_hung_task, 0) &&
                        ¦   !hung_detector_suspended)
                                //===============(3)=========================
                                check_hung_uninterruptible_tasks(timeout);
                        //===============(4)=========================
                        hung_last_checked = jiffies;
                        continue;
                }
                //===============(5)=========================
                schedule_timeout_interruptible(t);
        }

        return 0;
}
```
(1) `hung_task_timeout_secs`可以通过/proc文件系统控制
```
static struct ctl_table kern_table[] = {
{
        ...
        {
                .procname       = "hung_task_timeout_secs",
                .data           = &sysctl_hung_task_timeout_secs,
                .maxlen         = sizeof(unsigned long),
                .mode           = 0644,
                .proc_handler   = proc_dohung_task_timeout_secs,
                .extra2         = &hung_task_timeout_max,
        },
        ...
}


[root@node-3 ~]# cat /proc/sys/kernel/hung_task_timeout_secs
120
```
可以看出timeout时间为120s

(2) `hung_last_checked`记录了上一次检测的`jiffies`, 见(4)
`hung_timeout_jiffies`会对计算当前的jiffies有没有超过
`hung_last_checked + timeout`, 如果超过了，则意味超时了，
返回负数
```
static long hung_timeout_jiffies(unsigned long last_checked,
                                ¦unsigned long timeout)
{
        /* timeout of 0 will disable the watchdog */
        return timeout ? last_checked - jiffies + timeout * HZ :
                MAX_SCHEDULE_TIMEOUT;
}
```

(3) 如果超时，并且`hung_detector_suspended`为假，则进入
`check_hung_uninterruptible_tasks()`, 这里先不管`hung_detector_suspended`
并重新赋值`hung_last_checked`

(5) 如果没有超时，则将`khungtaskd`调度走, 并设置超时时间为t
```
signed long __sched schedule_timeout_interruptible(signed long timeout)
{
        __set_current_state(TASK_INTERRUPTIBLE);
        return schedule_timeout(timeout);
}
```

主要的函数还是 `check_hung_uninterruptible_tasks`

