<!--
## 代码路径
 ```sh
 mlx5e_napi_poll
 => 
```
-->

## perf 抓取
```
-   43.17%    43.13%  ksoftirqd/31     [kernel.kallsyms]         [k] __pv_queued_spin_lock_slowpath        ▒
     43.13% ret_from_fork                                                                                  ▒
        kthread                                                                                            ▒
      - smpboot_thread_fn                                                                                  ▒
         - 43.06% run_ksoftirqd                                                                            ▒
            - __softirqentry_text_start                                                                    ▒
               - 41.59% net_rx_action                                                                      ▒
                  - 37.89% mlx5e_napi_poll                                                                 ▒
                     - 33.40% mlx5e_post_rx_mpwqes                                                         ▒
                          __page_pool_alloc_pages_slow                                                     ▒
                          __alloc_pages_nodemask                                                           ▒
                          get_page_from_freelist                                                           ▒
                          _raw_spin_lock                                                                   ▒
                          __pv_queued_spin_lock_slowpath                                                   ▒
```
## 查看 `__pv_queued_spin_lock_slowpath` 具体热点
```
  0.03 │       movzbl    0x14(%r12),%edx                                                                   ▒
  0.62 │       test      %dl,%dl                                                                           ▒
       │     ↓ jne       1fb                                                                               ▒
  0.10 │1b3:   pause                                                                                       ▒
 77.11 │       sub       $0x1,%eax                                                                         ▒
       │     ↓ je        1fb                                                                               ▒
  6.09 │1ba:   mov       0x8(%rbp),%edx                                                                    ▒
  6.00 │       test      %edx,%edx                                                                         ▒
 
```
`__pv_queued_spin_lock_slowpath`反汇编:
```
0xffffffffbbd0a1b0 <__pv_queued_spin_lock_slowpath>:    nopl   0x0(%rax,%rax,1) [FTRACE NOP]

a1b0+1b3=a363
```

查看代码:
```
/usr/src/debug/kernel-4.18.0-193.el8/linux-4.18.0-193.el8.x86_64/kernel/locking/qspinlock_paravirt.h: 269
0xffffffffbbd0a355 <__pv_queued_spin_lock_slowpath+421>:        test   %al,%al
0xffffffffbbd0a357 <__pv_queued_spin_lock_slowpath+423>:        jne    0xffffffffbbd0a363 <__pv_queued_spin_lock_slowpath+435>
/usr/src/debug/kernel-4.18.0-193.el8/linux-4.18.0-193.el8.x86_64/./include/linux/compiler.h: 193
0xffffffffbbd0a359 <__pv_queued_spin_lock_slowpath+425>:        movzbl 0x14(%r12),%edx
/usr/src/debug/kernel-4.18.0-193.el8/linux-4.18.0-193.el8.x86_64/kernel/locking/qspinlock_paravirt.h: 272
0xffffffffbbd0a35f <__pv_queued_spin_lock_slowpath+431>:        test   %dl,%dl
0xffffffffbbd0a361 <__pv_queued_spin_lock_slowpath+433>:        jne    0xffffffffbbd0a3ab <__pv_queued_spin_lock_slowpath+507>
/usr/src/debug/kernel-4.18.0-193.el8/linux-4.18.0-193.el8.x86_64/./arch/x86/include/asm/processor.h: 678
0xffffffffbbd0a363 <__pv_queued_spin_lock_slowpath+435>:        pause
```

查看这个pause指令还有其他的地方可以跳转么? 发现没有!
```
[root@A06-R08-I134-73-919XB72 root]# cat __pv_queued_spin_lock_slowpath  |grep a363
0xffffffffbbd0a357 <__pv_queued_spin_lock_slowpath+423>:        jne    0xffffffffbbd0a363 <__pv_queued_spin_lock_slowpath+435>
0xffffffffbbd0a363 <__pv_queued_spin_lock_slowpath+435>:        pause
```

也就是说，要么从`+423` 的地方跳转，要么从`+433`的地方不跳转。对应的代码为:


```cpp

//这个地方决定是否要spin，还是要halt
//return:
//    true: halt
//    falseL: spin
266 static inline bool
267 pv_wait_early(struct pv_node *prev, int loop)
268 {
        //这里为了防止访问prev->state造成的cacheline 抖动，影响
        //spin效率, 每隔0xff 访问一次 prev->state
269     if ((loop & PV_PREV_CHECK_MASK) != 0)
270         return false;
271
        //如果上一个节点没有在运行, 考虑halt
272     return READ_ONCE(prev->state) != vcpu_running;
273 }

static void pv_wait_node(struct mcs_spinlock *node, struct mcs_spinlock *prev)
{
    struct pv_node *pn = (struct pv_node *)node;
    struct pv_node *pp = (struct pv_node *)prev;
    int loop;
    bool wait_early;

    for (;;) {
        for (wait_early = false, loop = SPIN_THRESHOLD; loop; loop--) {
            if (READ_ONCE(node->locked))
                return;
            //返回false, 才会走到 relax
            if (pv_wait_early(pp, loop)) {
                wait_early = true;
                break;
            }
            //走到这里.
            cpu_relax();
        }
    ...
}
```

关于qspinlock原理这里不过多介绍，详细见<sup>1</sup>, 我们直接看pv 相关代码:
```cpp
void queued_spin_lock_slowpath(struct qspinlock *lock, u32 val)
{
    struct mcs_spinlock *prev, *next, *node;
    u32 old, tail;
    int idx;

    BUILD_BUG_ON(CONFIG_NR_CPUS >= (1U << _Q_TAIL_CPU_BITS));

    //如果queued_spin_lock_slowpath 被编译为 pv_xxx, 则在这里直接跳转
    if (pv_enabled())
        goto pv_queue;
    ...

queue:
    lockevent_inc(lock_slowpath);

    //跳转到这里
pv_queue:
    // 获取第一个mcs，count++
    node = this_cpu_ptr(&qnodes[0].mcs);
    idx = node->count++;

    //将(cpu, idex) 构造一个标识 tail
    tail = encode_tail(smp_processor_id(), idx);

    /*
     * 4 nodes are allocated based on the assumption that there will
     * not be nested NMIs taking spinlocks. That may not be true in
     * some architectures even though the chance of needing more than
     * 4 nodes will still be extremely unlikely. When that happens,
     * we fall back to spinning on the lock directly without using
     * any MCS node. This is not the most elegant solution, but is
     * simple enough.
     */
    // 这个先不看
    if (unlikely(idx >= MAX_NODES)) {
        lockevent_inc(lock_no_node);
        while (!queued_spin_trylock(lock))
            cpu_relax();
        goto release;
    }

    /*
     * Keep counts of non-zero index values:
     */
    lockevent_cond_inc(lock_use_node2 + idx - 1, idx);

    /*
     * Ensure that we increment the head node->count before initialising
     * the actual node. If the compiler is kind enough to reorder these
     * stores, then an IRQ could overwrite our assignments.
     */
    barrier();

    //初始化
    node->locked = 0;
    node->next = NULL;

    /*
     * 初始化pv_node其他成员 cpu, state
     */
    pv_init_node(node);

    /*
     * We touched a (possibly) cold cacheline in the per-cpu queue node;
     * attempt the trylock once more in the hope someone let go while we
     * weren't watching.
     */
    //这里会尝试直接trylock下
    if (queued_spin_trylock(lock))
        goto release;

    /*
     * Ensure that the initialisation of @node is complete before we
     * publish the updated tail via xchg_tail() and potentially link
     * @node into the waitqueue via WRITE_ONCE(prev->next, node) below.
     */
    smp_wmb();

    /*
     * Publish the updated tail.
     * We have already touched the queueing cacheline; don't bother with
     * pending stuff.
     *
     * p,*,* -> n,*,*
     */
    //更新 lock->tail 为当前node
    old = xchg_tail(lock, tail);

    //old 为上一个排队的node
    next = NULL;
    /*
     * if there was a previous node; link it and wait until reaching the
     * head of the waitqueue.
     */
    //如果有上一个
    if (old & _Q_TAIL_MASK) {
        //使用old 获取 prev mcs node
        prev = decode_tail(old);

        /* Link @node into the waitqueue. */
        //串联到这个链表上.
        WRITE_ONCE(prev->next, node);

        //先调用pv_wait_node(), 等待其上一个node 获取到锁，通知该node 换spin的地方
        //从spin mcs_node.lock-> spin  this spinlock (pending lock)
        pv_wait_node(node, prev);
        //pv走完了, 这里不确定pv_wait_node()因什么流程走完，所以这里会再次检查
        //mcs_node.lock
        arch_mcs_spin_lock_contended(&node->locked);

        /*
         * While waiting for the MCS lock, the next pointer may have
         * been set by another lock waiter. We optimistically load
         * the next pointer & prefetch the cacheline for writing
         * to reduce latency in the upcoming MCS unlock operation.
         */
        //这里tail要可能要易主了，提前获取下该node->next(新加的)
        next = READ_ONCE(node->next);
        if (next)
            prefetchw(next);
    }
    /*
     * we're at the head of the waitqueue, wait for the owner & pending to
     * go away.
     *
     * *,x,y -> *,0,0
     *
     * this wait loop must use a load-acquire such that we match the
     * store-release that clears the locked bit and create lock
     * sequentiality; this is because the set_locked() function below
     * does not imply a full barrier.
     *
     * The PV pv_wait_head_or_lock function, if active, will acquire
     * the lock and return a non-zero value. So we have to skip the
     * atomic_cond_read_acquire() call. As the next PV queue head hasn't
     * been designated yet, there is no way for the locked value to become
     * _Q_SLOW_VAL. So both the set_locked() and the
     * atomic_cmpxchg_relaxed() calls will be safe.
     *
     * If PV isn't active, 0 will be returned instead.
     *
     */
    if ((val = pv_wait_head_or_lock(lock, node)))
        goto locked;

    //等待 (lock, pending) -> (0, 0)
    val = atomic_cond_read_acquire(&lock->val, !(VAL & _Q_LOCKED_PENDING_MASK));

    //抢到锁了
locked:
    /*
     * claim the lock:
     *
     * n,0,0 -> 0,0,1 : lock, uncontended
     * *,*,0 -> *,*,1 : lock, contended
     *
     * If the queue head is the only one in the queue (lock value == tail)
     * and nobody is pending, clear the tail code and grab the lock.
     * Otherwise, we only need to grab the lock.
     */

    /*
     * In the PV case we might already have _Q_LOCKED_VAL set, because
     * of lock stealing; therefore we must also allow:
     *
     * n,0,1 -> 0,0,1
     *
     * Note: at this point: (val & _Q_PENDING_MASK) == 0, because of the
     *       above wait condition, therefore any concurrent setting of
     *       PENDING will make the uncontended transition fail.
     */
    //获取tail的值, 如果和原来的tail相等，说明没有新加task
    if ((val & _Q_TAIL_MASK) == tail) {
        //直接更新lock->val(更新tail)
        if (atomic_try_cmpxchg_relaxed(&lock->val, &val, _Q_LOCKED_VAL))
            //更新成功了，说明没有其他cpu也在更新
            goto release; /* No contention */
    }

    /*
     * Either somebody is queued behind us or _Q_PENDING_VAL got set
     * which will then detect the remaining tail and queue behind us
     * ensuring we'll see a @next.
     */
    // 设置locked
    set_locked(lock);

    /*
     * contended path; wait for next if not observed yet, release.
     */
    //如果没有next, 则重新获取下(说不定又新加了)
    if (!next)
        next = smp_cond_load_relaxed(&node->next, (VAL));

    //设置 next->locked == 1, 让其更改下spin的位置
    arch_mcs_spin_unlock_contended(&next->locked);

    //kick下
    pv_kick_node(lock, next);

release:
    /*
     * release the node
     */
    __this_cpu_dec(qnodes[0].mcs.count);
}
```

好上面我们将pv_xxx 的动作忽略分析了大致流程(可以把pv_xxx 想像成特殊的等待和通知方法，将这些
流程从代码中全都move走，也不影响整个流程), 我们来看下这些方法的具体实现:

### pv_wait_node
```cpp
/*
 * Wait for node->locked to become true, halt the vcpu after a short spin.
 * pv_kick_node() is used to set _Q_SLOW_VAL and fill in hash table on its
 * behalf.
 */
static void pv_wait_node(struct mcs_spinlock *node, struct mcs_spinlock *prev)
{
    struct pv_node *pn = (struct pv_node *)node;
    struct pv_node *pp = (struct pv_node *)prev;
    int loop;
    bool wait_early;

    //这里先在guest中自旋
    for (;;) {
        for (wait_early = false, loop = SPIN_THRESHOLD; loop; loop--) {
            //获取node->locked 是否被prev 置位
            if (READ_ONCE(node->locked))
                return;
            //上面讲解了不赘述
            if (pv_wait_early(pp, loop)) {
                wait_early = true;
                break;
            }
            //pause
            cpu_relax();
        }

        //走到这里要考虑特殊的等待方法了
        /*
         * Order pn->state vs pn->locked thusly:
         *
         * [S] pn->state = vcpu_halted    [S] next->locked = 1
         *     MB                 MB
         * [L] pn->locked       [RmW] pn->state = vcpu_hashed
         *
         * Matches the cmpxchg() from pv_kick_node().
         */
        smp_store_mb(pn->state, vcpu_halted);

        //如果node->locked 还没有被置位
        if (!READ_ONCE(node->locked)) {
            lockevent_inc(pv_wait_node);
            lockevent_cond_inc(pv_wait_early, wait_early);
            pv_wait(&pn->state, vcpu_halted);
        }

        /*
         * If pv_kick_node() changed us to vcpu_hashed, retain that
         * value so that pv_wait_head_or_lock() knows to not also try
         * to hash this lock.
         */
        //这里等到了, 将vcpu_halted 更改为vcpu_running
        cmpxchg(&pn->state, vcpu_halted, vcpu_running);

        /*
         * If the locked flag is still not set after wakeup, it is a
         * spurious wakeup and the vCPU should wait again. However,
         * there is a pretty high overhead for CPU halting and kicking.
         * So it is better to spin for a while in the hope that the
         * MCS lock will be released soon.
         */
        lockevent_cond_inc(pv_spurious_wakeup,
                  !READ_ONCE(node->locked));
    }

    /*
     * By now our node->locked should be 1 and our caller will not actually
     * spin-wait for it. We do however rely on our caller to do a
     * load-acquire for us.
     */
}
```
`pv_wait_node()`的主要作用是等待`mcs_node.lock`被prev更新为1, 首先先考虑在guest中自旋,
但是guest自璇有个条件，就是其等待的prev, 需要是running的状态，如果不是running的状态，
自璇也没有用，就需要调用`pv_wait()` 将该vcpu halt，将物理cpu的执行权让渡给`prev`

关键在于`pv_wait()`函数:
```cpp
/*
 * Replacement function for pv_kick()
 */
static inline void __pv_kick(int cpu)
{
    u64 start = sched_clock();

    per_cpu(pv_kick_time, cpu) = start;
    pv_kick(cpu);
    this_cpu_add(EVENT_COUNT(pv_latency_kick), sched_clock() - start);
}

/*
 * Replacement function for pv_wait()
 */
static inline void __pv_wait(u8 *ptr, u8 val)
{
    u64 *pkick_time = this_cpu_ptr(&pv_kick_time);

    *pkick_time = 0;
    pv_wait(ptr, val);
    if (*pkick_time) {
        this_cpu_add(EVENT_COUNT(pv_latency_wake),
                 sched_clock() - *pkick_time);
        lockevent_inc(pv_kick_wake);
    }
}

#define pv_kick(c)  __pv_kick(c)
#define pv_wait(p, v)   __pv_wait(p, v)
```

这里。。下面的代码看到的`pv_kick()` 为`__pv_kick()`， 而`__pv_kick()`
看到的`pv_kick()`为`arch/x86/include/asm/paravirt.h`中的`pv_kick`

```cpp
static __always_inline void pv_kick(int cpu)
{
    PVOP_VCALL1(pv_lock_ops.kick, cpu);
}
```
此时kick为`kvm_kick_cpu`, wait为`kvm_wait`
```cpp
/* Kick a cpu by its apicid. Used to wake up a halted vcpu */
static void kvm_kick_cpu(int cpu)
{
    int apicid;
    unsigned long flags = 0;

    apicid = per_cpu(x86_cpu_to_apicid, cpu);
    kvm_hypercall2(KVM_HC_KICK_CPU, flags, apicid);
}

#include <asm/qspinlock.h>

static void kvm_wait(u8 *ptr, u8 val)
{
    unsigned long flags;

    if (in_nmi())
        return;

    local_irq_save(flags);

    if (READ_ONCE(*ptr) != val)
        goto out;

    /*
     * halt until it's our turn and kicked. Note that we do safe halt
     * for irq enabled case to avoid hang when lock info is overwritten
     * in irq spinlock slowpath and no spurious interrupt occur to save us.
     */
    if (arch_irqs_disabled_flags(flags))
        halt();
    else
        safe_halt();

out:
    local_irq_restore(flags);
}
```
可以看到`kvm_wait()` 最终就是调用了`halt`指令，该指令会在kvm模拟，让vcpu调度走。
而`kvm_kick()`则是调用hypercall -- `KVM_HC_KICK_CPU`

```cpp
static void kvm_pv_kick_cpu_op(struct kvm *kvm, unsigned long flags, int apicid)
{
    struct kvm_lapic_irq lapic_irq;

    lapic_irq.shorthand = 0;
    lapic_irq.dest_mode = 0;
    lapic_irq.level = 0;
    lapic_irq.dest_id = apicid;
    lapic_irq.msi_redir_hint = false;

    lapic_irq.delivery_mode = APIC_DM_REMRD;
    kvm_irq_delivery_to_apic(kvm, NULL, &lapic_irq, NULL);
}

int kvm_emulate_hypercall(struct kvm_vcpu *vcpu)
{
    ... 
    case KVM_HC_KICK_CPU:
        kvm_pv_kick_cpu_op(vcpu->kvm, a0, a1);
        kvm_sched_yield(vcpu->kvm, a1);
        ret = 0;
        break;
}
```

halt状态是需要外部事件唤醒的。而这里唤醒的方法是中断(但是这个中断没有vector...)

但是，这里使用的`delivery_mode` 为`APIC_DM_REMRD`, 对应手册中关于deliver mode的
`reserved`类型，而 `__apic_accept_irq()` 会对其做特殊处理:
```cpp
static int __apic_accept_irq(struct kvm_lapic *apic, int delivery_mode,
                 int vector, int level, int trig_mode,
                 struct dest_map *dest_map)
{
    case APIC_DM_REMRD:
        result = 1;
        vcpu->arch.pv.pv_unhalted = 1;
        kvm_make_request(KVM_REQ_EVENT, vcpu);
        kvm_vcpu_kick(vcpu);
        break;
}
```
**阶段性总结:** 当qspinlock在等待 prev node 置位cur node的lock变量时，如果发现等待的
节点不是running的状态，认为在这里自旋等待也没有用，还不如将cpu让渡给需要其他cpu执行
临界区代码，早点释放锁

### pv_wait_head_or_lock

**TODO**


## 总结
perf发现pvspinlock占用cpu多，但是其热点主要是在 **guest** 自旋 `mcs_node.lock`(这里也说明了要等待的cpu大概率
是running的，并且当前的场景也很难发生抢占: **咱们的场景也是绑核场景, 0-31核负载也很低，所以当guest使用自旋锁后,
发生抢占的概率也非常低**。另外在自旋锁临界区中，guest kernel也不会（极大的概率）带着锁执行halt...)

所以，其绝大部分的加锁情况都未涉及pv spinlock流程, 所以个人认为和pvspinlock没啥关系，使用native spinlock也会
出现这个问题, 问题的根因还是在于为什么会申请页，并且申请页的过程中，用到了哪个自旋锁。大家为什么都去抢这一把锁。

## 参考链接
1. [qspinlock](https://github.com/cai-fuqiang/my_book/tree/lock/lock/spinlock)
