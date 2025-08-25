## 堆栈
```
 #6 [ffff88051fb73ae0] bad_area_nosemaphore at ffffffff816abd05
 #7 [ffff88051fb73af0] __do_page_fault at ffffffff816bf66e
 #8 [ffff88051fb73b50] do_page_fault at ffffffff816bf815
 #9 [ffff88051fb73b80] page_fault at ffffffff816bb768
    [exception RIP: blk_free_flush_queue+18]
    RIP: ffffffff81306392  RSP: ffff88051fb73c30  RFLAGS: 00010286
    RAX: dead000000000200  RBX: ffff87c5eafa5600  RCX: dead000000000200
    RDX: ffffffff81ac0d60  RSI: ffff880612184000  RDI: ffff87c5eafa5600
    RBP: ffff88051fb73c38   R8: ffff880612184198   R9: a1c0000000000000
    R10: df291b7aca4eb438  R11: 0000000000000000  R12: ffffc90050001108
    R13: 0000000000000000  R14: 0000000000000080  R15: ffffc90050001140
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
#10 [ffff88051fb73c40] blk_mq_exit_hctx at ffffffff8130bb19
#11 [ffff88051fb73c70] blk_mq_free_queue at ffffffff8130f040
#12 [ffff88051fb73ca8] blk_cleanup_queue at ffffffff81301299
#13 [ffff88051fb73cd0] _deinit_dev at ffffffffc0a19b95 [cbd]
#14 [ffff88051fb73d10] _delete_dev at ffffffffc0a1abe8 [cbd]
#15 [ffff88051fb73d28] cbd_delete_dev at ffffffffc0a1b488 [cbd]
#16 [ffff88051fb73e48] cbd_misc_ioctl at ffffffffc0a1b7e8 [cbd]
#17 [ffff88051fb73e80] do_vfs_ioctl at ffffffff8121d540
#18 [ffff88051fb73f00] sys_ioctl at ffffffff8121d7f1
```

dmesg 打印:
```
[21567758.226558] Hardware name: Dell Inc. PowerEdge R640/0W23H8, BIOS 2.11.2 004/21/2021
[21567758.226593] task: ffff8804f836eeb0 ti: ffff88051fb70000 task.ti: ffff88051fb70000
[21567758.226628] RIP: 0010:[<ffffffff81306392>]  [<ffffffff81306392>] blk_free_flush_queue+0x12/0x30
[21567758.226692] RSP: 0018:ffff88051fb73c30  EFLAGS: 00010286
[21567758.226720] RAX: dead000000000200 RBX: ffff87c5eafa5600 RCX: dead000000000200
[21567758.226757] RDX: ffffffff81ac0d60 RSI: ffff880612184000 RDI: ffff87c5eafa5600
[21567758.226793] RBP: ffff88051fb73c38 R08: ffff880612184198 R09: a1c0000000000000
[21567758.226830] R10: df291b7aca4eb438 R11: 0000000000000000 R12: ffffc90050001108
[21567758.226864] R13: 0000000000000000 R14: 0000000000000080 R15: ffffc90050001140
[21567758.226901] FS:  00007f9540eeb700(0000) GS:ffff882f7dac0000(0000) knlGS:0000000000000000
[21567758.226942] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[21567758.226973] CR2: ffff87c5eafa5640 CR3: 00000005a23d8000 CR4: 00000000003627e0
[21567758.227009] DR0: 0000000000000000 DR1: 0000000000000000 DR2: 0000000000000000
[21567758.227046] DR3: 0000000000000000 DR6: 00000000ffff0ff0 DR7: 0000000000000400
[21567758.227082] Call Trace:
[21567758.227100]  [<ffffffff8130bb19>] blk_mq_exit_hctx.isra.33+0x99/0xd0
[21567758.227134]  [<ffffffff8130f040>] blk_mq_free_queue+0xf0/0x160
[21567758.227167]  [<ffffffff81301299>] blk_cleanup_queue+0xd9/0x150
[21567758.227200]  [<ffffffffc0a19b95>] _deinit_dev+0x55/0x90 [cbd]
[21567758.227231]  [<ffffffffc0a1abe8>] _delete_dev+0x38/0x60 [cbd]
[21567758.227263]  [<ffffffffc0a1b488>] cbd_delete_dev+0x548/0x740 [cbd]
[21567758.228850]  [<ffffffffc0a1b7e8>] cbd_misc_ioctl+0x168/0x240 [cbd]
[21567758.230365]  [<ffffffff8121d540>] do_vfs_ioctl+0x350/0x560
[21567758.231837]  [<ffffffff81254968>] ? SyS_epoll_ctl+0x118/0xc60
[21567758.233312]  [<ffffffff812286d5>] ? __fd_install+0x25/0x60
[21567758.234745]  [<ffffffff8121d7f1>] SyS_ioctl+0xa1/0xc0
[21567758.236152]  [<ffffffff816c46d5>] system_call_fastpath+0x1c/0x21
[21567758.237567] Code: be d0 80 00 00 e8 0f 16 ee ff eb 9c 0f 1f 00 66 2e 0f 1f 84 00 00 00 00 00 0f 1f 44 00 00 55 48 85 ff 48 89 e5 53 48 89 fb 74 11 <48> 8b 7f 40 e8 65 01 ee ff 48 89 df e8 5d 01 ee ff 5b 5d c3 66
[21567758.240607] RIP  [<ffffffff81306392>] blk_free_flush_queue+0x12/0x30
[21567758.242073]  RSP <ffff88051fb73c30>
[21567758.243535] CR2: ffff87c5eafa5640
```

查看异常指令:
```
crash> dis -l blk_free_flush_queue
/usr/src/debug/kernel-3.10.0-693.21.9.el7/linux-3.10.0-693.21.9.el7.x86_64/block/blk-flush.c: 549
0xffffffff81306380 <blk_free_flush_queue>:      nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff81306385 <blk_free_flush_queue+5>:    push   %rbp
/usr/src/debug/kernel-3.10.0-693.21.9.el7/linux-3.10.0-693.21.9.el7.x86_64/block/blk-flush.c: 551
0xffffffff81306386 <blk_free_flush_queue+6>:    test   %rdi,%rdi
/usr/src/debug/kernel-3.10.0-693.21.9.el7/linux-3.10.0-693.21.9.el7.x86_64/block/blk-flush.c: 549
0xffffffff81306389 <blk_free_flush_queue+9>:    mov    %rsp,%rbp
0xffffffff8130638c <blk_free_flush_queue+12>:   push   %rbx
0xffffffff8130638d <blk_free_flush_queue+13>:   mov    %rdi,%rbx
/usr/src/debug/kernel-3.10.0-693.21.9.el7/linux-3.10.0-693.21.9.el7.x86_64/block/blk-flush.c: 551
0xffffffff81306390 <blk_free_flush_queue+16>:   je     0xffffffff813063a3 <blk_free_flush_queue+35>
/usr/src/debug/kernel-3.10.0-693.21.9.el7/linux-3.10.0-693.21.9.el7.x86_64/block/blk-flush.c: 554
0xffffffff81306392 <blk_free_flush_queue+18>:   mov    0x40(%rdi),%rdi   <--- 这里
0xffffffff81306396 <blk_free_flush_queue+22>:   callq  0xffffffff811e6500 <kfree>
```

访问该成员:
```
548 void blk_free_flush_queue(struct blk_flush_queue *fq)
549 {
550     /* bio based request queue hasn't flush queue */
551     if (!fq)
552         return;
553
554     kfree(fq->flush_rq);   <--- 访问该成员
555     kfree(fq);
556 }

```

调用者:
```
0xffffffff8130ba80 <blk_mq_exit_hctx>:  nopl   0x0(%rax,%rax,1) [FTRACE NOP]
...
0xffffffff8130ba9d <blk_mq_exit_hctx+29>:       mov    %rsi,%rbx  --> 这里是hctx, 但是rsi表示第二个参数，但是hctx是第三个参数
...
0xffffffff8130bb08 <blk_mq_exit_hctx+136>:      callq  0xffffffff81310cc0 <blk_mq_unregister_cpu_notifier>
0xffffffff8130bb0d <blk_mq_exit_hctx+141>:      mov    0x2a0(%rbx),%rdi --> offset 672
0xffffffff8130bb14 <blk_mq_exit_hctx+148>:      callq  0xffffffff81306380 <blk_free_flush_queue>

crash> struct blk_mq_hw_ctx -o
struct blk_mq_hw_ctx {
  [672] struct blk_flush_queue *fq;
}
```

bt -f 输出:
```
 #9 [ffff88051fb73b80] page_fault at ffffffff816bb768
    [exception RIP: blk_free_flush_queue+18]
    RIP: ffffffff81306392  RSP: ffff88051fb73c30  RFLAGS: 00010286
    RAX: dead000000000200  RBX: ffff87c5eafa5600  RCX: dead000000000200
    RDX: ffffffff81ac0d60  RSI: ffff880612184000  RDI: ffff87c5eafa5600
    RBP: ffff88051fb73c38   R8: ffff880612184198   R9: a1c0000000000000
    R10: df291b7aca4eb438  R11: 0000000000000000  R12: ffffc90050001108
    R13: 0000000000000000  R14: 0000000000000080  R15: ffffc90050001140
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
    ffff88051fb73b88: ffffc90050001140 0000000000000080
    ffff88051fb73b98: 0000000000000000 ffffc90050001108
    ffff88051fb73ba8: ffff88051fb73c38 ffff87c5eafa5600
    ffff88051fb73bb8: 0000000000000000 df291b7aca4eb438
    ffff88051fb73bc8: a1c0000000000000 ffff880612184198
    ffff88051fb73bd8: dead000000000200 dead000000000200
    ffff88051fb73be8: ffffffff81ac0d60 ffff880612184000
    ffff88051fb73bf8: ffff87c5eafa5600 ffffffffffffffff
    ffff88051fb73c08: ffffffff81306392 0000000000000010
    ffff88051fb73c18: 0000000000010286 ffff88051fb73c30 [sp]
    ffff88051fb73c28: 0000000000000018[ss] ffff880612184000[rbx -- hctx]
    ffff88051fb73c38: ffff88051fb73c68[rbp] ffffffff8130bb19 [ip]
#10 [ffff88051fb73c40] blk_mq_exit_hctx at ffffffff8130bb19
    ffff88051fb73c48: 0000000000000001 ffff88359b2eac10[r12 -- request_queue]
    ffff88051fb73c58: ffffc90050001108[r13] 0000000000000001[r14]
    ffff88051fb73c68: ffff88051fb73ca0[rbp] ffffffff8130f040[ip]
#11 [ffff88051fb73c70] blk_mq_free_queue at ffffffff8130f040
    ffff88051fb73c78: ffff88359b2eac10 ffff88359b2eb060
    ffff88051fb73c88: ffff88359b2eb2b8 ffffc90050001000
    ffff88051fb73c98: ffffc90050007258 ffff88051fb73cc8
    ffff88051fb73ca8: ffffffff81301299
```
通过堆栈获取到`hctx`的值`ffff880612184000`, 打印fq，
```
crash> struct blk_mq_hw_ctx.fq ffff880612184000
  fq = 0xffff87c5eafa5600
```
可以发现获取的fq的值和pt regs 中的 RDI可以对上:
```
RDX: ffffffff81ac0d60  RSI: ffff880612184000  RDI: ffff87c5eafa5600
```

另外request_queue为`ffff88359b2eac10`
```
crash> struct request_queue.nr_hw_queues,queue_hw_ctx ffff88359b2eac10
  nr_hw_queues = 1
  queue_hw_ctx = 0xffff883574f80000
crash> x/4xg 0xffff883574f80000
0xffff883574f80000:     0xffff880612184000[hctx]      0x0000000000000000
0xffff883574f80010:     0x0000000000000000      0x0000000000000000
```
也可以索引到hctx

而hctx的其他成员都正常:
```
crash> blk_mq_hw_ctx.tags,queue,queue_num ffff880612184000
  tags = 0xffff883248cff080
  queue = 0xffff88359b2eac10
  queue_num = 0
```

并且hctx和request_queue都是allocated slab
```
crash> kmem ffff880612184000
CACHE            NAME                 OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE
ffff88017fc07500 kmalloc-1024            1024      19546     28736    898    32k
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffea0018486000  ffff880612180000     0     32          4    28
  FREE / [ALLOCATED]
  [ffff880612184000]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffea0018486100  612184000                0        0  0 2fffff00008000 tail
crash> kmem ffff88359b2eac10
CACHE            NAME                 OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE
ffff885f3efe4600 blkdev_queue            2256       1142      1778    127    32k
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffea00d66cba00  ffff88359b2e8000     1     14          1    13
  FREE / [ALLOCATED]
  [ffff88359b2eac10]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffea00d66cba80 359b2ea000                0        0  0 6fffff00008000 tail
```

hctx->fq 初始化
```
blk_mq_init_queue
|-> blk_mq_init_allocated_queue
    |-> blk_mq_realloc_hw_ctxs
        |-> blk_mq_init_hctx
            |-> hctx->fq = blk_alloc_flush_queue(q, hctx->numa_node, set->cmd_size);
    |-> if q->nr_hw_queues == 1
        \-> blk_queue_make_request(q, blk_sq_make_request);
    |-> if (set->ops->init_request)
        \-> set->ops->init_request(set->driver_data,
                   hctx->fq->flush_rq, hctx_idx, --> 这里访问了hctx->fq中的内存
                   flush_start_tag + hctx_idx, node)
```
所以, 在初始化时赋值fq后，随后又访问该部分内存。所以初始化流程没有问题.

而其他的流程并没有 `hctx->fq` 的赋值流程
```
  3 block/blk-mq.c|1761 col 12| hctx->fq->flush_rq, hctx_idx,
  4 block/blk-mq.c|1771 col 23| blk_free_flush_queue(hctx->fq);
  5 block/blk-mq.c|1842 col 2| hctx->fq = blk_alloc_flush_queue(q, hctx->numa_node, set->cmd_size);
  6 block/blk-mq.c|1843 col 7| if (!hctx->fq)
  7 block/blk-mq.c|1848 col 8| hctx->fq->flush_rq, hctx_idx,
  8 block/blk-mq.c|1858 col 8| kfree(hctx->fq);
```

## hctx中还有其他的成员不一致

```
crash> struct blk_mq_hw_ctx.fq,nr_ctx,ctxs,kobj 0xffff880612184000
  fq = 0xffff87c5eafa5600
  nr_ctx = 80
  ctxs = 0xffff882f806ea000
  kobj = {
    name = 0xffff8803cf7dd8c8 "h\330}\317\003\210\377\377\070\322}\317\003\210\377\377`\332}\317\003\210\377\377\310\332}\317\003\210\377\377\310\335}\317\003\210\377\377\300\325}\317\003\210\377\377h\326}\317\003\210\377\377(\322}\317\003\210\377\377\070\323}\317\003\210\377\377",
    entry = {
      next = 0xffff8806121841c0,
      prev = 0xffff8806121841c0
    },
    parent = 0x0,
    kset = 0x0,
    ktype = 0xffffffff81ac0aa0 <blk_mq_hw_ktype>,
    sd = 0x0,
    kref = {
      refcount = {
        counter = 0
      }
    },
    state_initialized = 1,
    state_in_sysfs = 0,
    state_add_uevent_sent = 0,
    state_remove_uevent_sent = 0,
    uevent_suppress = 0
  }
```
其中`kobj.name`是一个kmalloc-8的free object
```
crash> kmem 0xffff8803cf7dd8c8
CACHE            NAME                 OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE
ffff88017fc07e00 kmalloc-8                  8      53826    195072    381     4k
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffea000f3df740  ffff8803cf7dd000     0    512         15   497
  FREE / [ALLOCATED]
   ffff8803cf7dd8c8  (cpu 32 cache)
```
是cpu 32 的cache.

而`ctxs`是一个 `kmalloc-4096`的alloc object.

```
crash> kmem -S 0xffff882f806ea000
CACHE            NAME                 OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE
ffff88017fc07300 kmalloc-4096            4096       1777      2856    357    32k
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffea00be01ba00  ffff882f806e8000     0      8          6     2
  FREE / [ALLOCATED]
  [ffff882f806e8000]
   ffff882f806e9000
  [ffff882f806ea000]
  ^^^^^^^^^^^^^^^^^^
   ffff882f806eb000
  [ffff882f806ec000]
  [ffff882f806ed000]
  [ffff882f806ee000]
  [ffff882f806ef000]
```

## 猜测
目前怀疑是`hctx  0xffff880612184000` 是`use-after-free`, 搜索该地址
```
ffff88002f836490: ffff880612184000
ffff88002f8364d0: ffff880612184000
ffff88002f8364f0: ffff880612184000
ffff88002f836530: ffff880612184000
...

ffff8801000040b0: ffff880612184000
ffff8801000040d0: ffff880612184000
ffff880100004170: ffff880612184000
ffff880100004190: ffff880612184000
```
发现有大量的输出

随便看一个地址, `ffff88002f836490`:
```
crash> kmem ffff88002f836490
CACHE            NAME                 OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE
ffff88017fc07c00 kmalloc-32                32   20798809  23485184  183478     4k
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffea0000be0d80  ffff88002f836000     0    128        127     1
  FREE / [ALLOCATED]
  [ffff88002f836480]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffea0000be0d80   2f836000                0 ffff88002f836fa0  1 1fffff00000080 slab
```
是一个正在申请的`kmalloc-32`

查看该地址:
```
ffff88002f836480:  ffff8803c9ea6a20 ffffffff810409f0    j..............
ffff88002f836490:  ffff880612184000 0000000000000000   .@..............
                   ^^^^^^^^^^^^^^^^
ffff88002f8364a0:  ffff880379a0c3e0 ffffffff810409f0   ...y............
ffff88002f8364b0:  ffff885f3da32c00 0000000000000000   .,.=_...........
ffff88002f8364c0:  ffff8803c9ea6e20 ffffffff810409f0    n..............
ffff88002f8364d0:  ffff880612184000 0000000000000000   .@..............
ffff88002f8364e0:  ffff88002f836700 ffffffff810409f0   .g./............
ffff88002f8364f0:  ffff880612184000 0000000000000000   .@..............
                   ^^^^^^^^^^^^^^^^
ffff88002f836500:  ffff8835a8716160 ffffffff810409f0   `aq.5...........
ffff88002f836510:  ffff885f3da32c00 0000000000000000   .,.=_...........
```

发现几乎都是申请的同一种结构, 里面有个符号:`ffffffff810409f0`, 查看该符号，发现
是`move_myself`

```
crash> dis ffffffff810409f0 |head -1
0xffffffff810409f0 <move_myself>:       nopl   0x0(%rax,%rax,1) [FTRACE NOP]
```
而该函数会在`__rdtgroup_move_task`中创建 callbak, 然后执行`task_work_add()`.
```cpp
static int __rdtgroup_move_task(struct task_struct *tsk,
                struct rdtgroup *rdtgrp)
{
    struct task_move_callback *callback;
    int ret;

    callback = kzalloc(sizeof(*callback), GFP_KERNEL);
    if (!callback)
        return -ENOMEM;
    callback->work.func = move_myself;
    callback->rdtgrp = rdtgrp;

    /*
     * Take a refcount, so rdtgrp cannot be freed before the
     * callback has been invoked.
     */
    atomic_inc(&rdtgrp->waitcount);
    ret = task_work_add(tsk, &callback->work, true);
```
而`task_move_callback`大小为24
```
crash> p sizeof(struct task_move_callback)
$2 = 24
```
使用的正好是`kmalloc-32`

rdtgrp的大小是`728`
```
crash> p sizeof(struct rdtgroup)
$3 = 728
```
**_和`blk_mq_hw_ctx`使用的是相同的slab!! (kmalloc-1024)_**

而该callbak则会在`task_work_run()`中调用:
```
do_exit
|-> exit_task_work
    |-> task_work_run()

do_notify_resume
|-> if thread_info_flags & _TIF_SIGPENDING
    |-> do_signal
        |-> get_signal
            |-> get_signal_to_deliver
                |-> if unlikely(current->task_works)
                    \-> task_work_run()
|-> if thread_info_flags & _TIF_NOTIFY_RESUME
    |-> tracehook_notify_resume
        |-> if unlikely(current->task_works)
            \-> task_work_run()
```
简单看下该函数:
```cpp
void task_work_run(void)
{
    struct task_struct *task = current;
    struct callback_head *work, *head, *next;

    for (;;) {
        /*
         * work->func() can do task_work_add(), do not set
         * work_exited unless the list is empty.
         */
        //==(1)==
        do {
            work = ACCESS_ONCE(task->task_works);
            head = !work && (task->flags & PF_EXITING) ?
                &work_exited : NULL;
        } while (cmpxchg(&task->task_works, work, head) != work);

        //==(2)==
        if (!work)
            break;
        /*
         * Synchronize with task_work_cancel(). It can't remove
         * the first entry == work, cmpxchg(task_works) should
         * fail, but it can play with *work and other entries.
         */
        raw_spin_unlock_wait(&task->pi_lock);
        smp_mb();

        /* Reverse the list to run the works in fifo order */
        //==(3)==
        head = NULL;
        do {
            next = work->next;
            work->next = head;
            head = work;
            work = next;
        } while (work);

        //==(4)==
        work = head;
        do {
            next = work->next;
            work->func(work);
            work = next;
            cond_resched();
        } while (work);
    }
}
```
1. 如果进程退出，`task->task_works` 置换为`&work_exited`, 否则置换为NULL
   置换之后，处理原来`task->task_works`指向的链表
2. 表示没有任务
3. 将链表方向置反，作为一个fifo的队列.(`task_work_add()`时，是往head加的, 这样做
   的好处是, add的时候，效率高)
4. 处理每一个work

> NOTE !!
>
> 这里将链表中的work的func()调用完，`task_work_run()`的任务就完成了，而work的释放
> 需要在func()中做.

我们再来看下: `move_myself` 函数:
```cpp
static void move_myself(struct callback_head *head)
{
    struct task_move_callback *callback;
    struct rdtgroup *rdtgrp;

    callback = container_of(head, struct task_move_callback, work);
    rdtgrp = callback->rdtgrp;

    /*
     * If resource group was deleted before this task work callback
     * was invoked, then assign the task to root group and free the
     * resource group.
     */
    if (atomic_dec_and_test(&rdtgrp->waitcount) &&
        (rdtgrp->flags & RDT_DELETED)) {
        current->closid = 0;
        kfree(rdtgrp);
    }

    preempt_disable();
    /* update PQR_ASSOC MSR to make resource group go into effect */
    intel_rdt_sched_in();
    preempt_enable();

    kfree(callback);
}
```
`callback`在这个流程中肯定会free, 但是`rdtgrp` 会先查看`rdtgrp->waitcount`的值,
如果`>=2`, 则不会释放


## 还原task_work_list
我们通过search的方式, 尝试还原下`task_work_list`, 看下其是否还在被占用:

从`ffff88002f836480`开始search
```
ffff88058074fe60: ffff88002f836480  -- [kmalloc-32] (allocated)
ffff883503677780: ffff88058074fe60  -- [kmalloc-32] (allocated)
ffff88058074f420: ffff883503677780
ffff880619325be0: ffff88058074f420
ffff880619325ce0: ffff880619325be0
ffff8835a4b59220: ffff880619325ce0
ffff8805b42b9380: ffff8835a4b59220
ffff8835a4b59540: ffff8805b42b9380
ffff8805bba50ca0: ffff8835a4b59540
ffff8805d3a8e3e0: ffff8805bba50ca0
ffff8804dce89f20: ffff8805d3a8e3e0
ffff8805edb49780: ffff8804dce89f20
ffff880607faab60: ffff8805edb49780
ffff8805e7e457c0: ffff880607faab60
ffff882f7d92fea0: ffff8805e7e457c0
ffff8835b52e5680: ffff882f7d92fea0
ffff8805a0bd2800: ffff8835b52e5680
...

太麻烦了, 没找完
```

这种相当于反向查找链表，太费劲了. 我们执行下面命令查找每个task的`task_works`结构:
```
crash> foreach task -R task_works > task_works.txt
```
执行下面命令查找`task_works != 0` 的task

```
[root@A02-R19-I8-70-11BHTP2 ~]# grep -B 1 -E 'task_works = 0x[1-9a-fA-F][0-9a-fA-F]*' task_works.txt
PID: 1220   TASK: ffff880617aebf40  CPU: 0   COMMAND: "irq/393-mei_me"
  task_works = 0xffff880604ea3e80,
--
PID: 50127  TASK: ffff8806063a0000  CPU: 52  COMMAND: "qemu-kvm"
  task_works = 0xffff8804daf56220,
--
PID: 50233  TASK: ffff8803c4bf0fd0  CPU: 0   COMMAND: "CPU 0/KVM"
  task_works = 0xffff8804daf56440,
--
PID: 50236  TASK: ffff8835193cdee0  CPU: 16  COMMAND: "CPU 1/KVM"
  task_works = 0xffff8804daf56620,
--
PID: 50238  TASK: ffff8835193c8000  CPU: 28  COMMAND: "CPU 2/KVM"
  task_works = 0xffff8804daf56880,
--
PID: 50239  TASK: ffff8835193cbf40  CPU: 18  COMMAND: "CPU 3/KVM"
  task_works = 0xffff8804daf562a0,
--
PID: 50241  TASK: ffff8806158a2f70  CPU: 2   COMMAND: "CPU 4/KVM"
  task_works = 0xffff8805d87d8a20,
--
PID: 50245  TASK: ffff8806158a0fd0  CPU: 4   COMMAND: "CPU 5/KVM"
  task_works = 0xffff8805d87d8120,
--
PID: 50246  TASK: ffff8806158a4f10  CPU: 12  COMMAND: "CPU 6/KVM"
  task_works = 0xffff8805d87d8ca0,
--
PID: 50247  TASK: ffff8806158a0000  CPU: 24  COMMAND: "CPU 7/KVM"
  task_works = 0xffff8805d87d8b00,
--
PID: 50418  TASK: ffff880617fa3f40  CPU: 60  COMMAND: "vnc_worker"
  task_works = 0xffff8805d87d88a0,
--
PID: 127600  TASK: ffff8805d777bf40  CPU: 12  COMMAND: "qemu-kvm"
  task_works = 0xffff8805c1ffe860,
--
PID: 127716  TASK: ffff883553641fa0  CPU: 50  COMMAND: "CPU 1/KVM"
  task_works = 0xffff8805c1ffe4e0,
--
PID: 127717  TASK: ffff880538f6dee0  CPU: 74  COMMAND: "CPU 2/KVM"
  task_works = 0xffff8805c1ffe260,
--
PID: 127718  TASK: ffff8805e22c0000  CPU: 16  COMMAND: "CPU 3/KVM"
  task_works = 0xffff8805c1ffedc0,
--
PID: 127719  TASK: ffff8805e22c2f70  CPU: 22  COMMAND: "CPU 4/KVM"
  task_works = 0xffff8805c1ffe0c0,
--
PID: 127721  TASK: ffff8805e22c6eb0  CPU: 38  COMMAND: "CPU 5/KVM"
  task_works = 0xffff88022daee940,
--
PID: 127725  TASK: ffff8805e22c5ee0  CPU: 34  COMMAND: "CPU 6/KVM"
  task_works = 0xffff88022daeee80,
--
PID: 127726  TASK: ffff88060ed30fd0  CPU: 36  COMMAND: "CPU 7/KVM"
  task_works = 0xffff8804b395b3a0,
--
PID: 128064  TASK: ffff8804c631dee0  CPU: 58  COMMAND: "vnc_worker"
  task_works = 0xffff8804b395b0e0,
--
PID: 133005  TASK: ffff88358abacf10  CPU: 17  COMMAND: "qemu-kvm"
  task_works = 0xffff8803ad318c20,
--
PID: 133135  TASK: ffff882f807eeeb0  CPU: 77  COMMAND: "CPU 1/KVM"
  task_works = 0xffff88021eecc2e0,
--
PID: 133138  TASK: ffff880230b4eeb0  CPU: 13  COMMAND: "CPU 2/KVM"
  task_works = 0xffff88021eeccbc0,
--
PID: 133139  TASK: ffff880230b4dee0  CPU: 5   COMMAND: "CPU 3/KVM"
  task_works = 0xffff8803cf23a640,
--
PID: 133140  TASK: ffff880230b4cf10  CPU: 35  COMMAND: "CPU 4/KVM"
  task_works = 0xffff8803cf23aec0,
--
PID: 133143  TASK: ffff880230b4bf40  CPU: 3   COMMAND: "CPU 5/KVM"
  task_works = 0xffff8803cfbebf60,
--
PID: 133146  TASK: ffff880230b4af70  CPU: 3   COMMAND: "CPU 6/KVM"
  task_works = 0xffff8803cfbebe00,
--
PID: 133147  TASK: ffff880230b48fd0  CPU: 1   COMMAND: "CPU 7/KVM"
  task_works = 0xffff8803cfbeb360,
--
PID: 133281  TASK: ffff880230b48000  CPU: 51  COMMAND: "vnc_worker"
  task_works = 0xffff8803cfbebca0,
```
我们执行下面命令:
```
## 0xffff8804daf56220 为
## PID: 50127  TASK: ffff8806063a0000  CPU: 52  COMMAND: "qemu-kvm"
crash> list task_move_callback.work 0xffff8804daf56220 -s task_move_callback > tmp1.txt
```
将会得到一个非常非常非常长的链表, 链表靠后的位置, 有大量的`rdgrp = 0xffff880612184000`
成员:
```
## 文件开头
ffff8804daf56220
struct task_move_callback {
  work = {
    next = 0xffff8803d26ba0c0,
    func = 0xffffffff810409f0 <move_myself>
  },
  rdtgrp = 0xffff88356b615800
}
ffff8803d26ba0c0
struct task_move_callback {
  work = {
    next = 0xffff88060a32b020,
    func = 0xffffffff810409f0 <move_myself>
  },
  rdtgrp = 0xffff88356b615800
}

...

## 文件末尾
struct task_move_callback {
  work = {
    next = 0xffff88036a619d20,
    func = 0xffffffff810409f0 <move_myself>
  },
  rdtgrp = 0xffff880612184000
}
ffff88036a619d20
struct task_move_callback {
  work = {
    next = 0xffff8805d32d0f60,
    func = 0xffffffff810409f0 <move_myself>
  },
  rdtgrp = 0xffff880612184000
}
ffff8805d32d0f60
```

目前来看, 有大量的`callback`未被释放.

> NOTE
>
> **该链表没打完，文件大小就已经5.x M, slab object 65000+, 说明有大量的slab
> object残留**

这里肯定是有use-after-free的情况!

但是，是blk-mq use-after-free, 还是rdtgroup use-after-free呢?

从`blk-mq`的代码来看，其资源申请释放流程非常简单！没有use-after-free的代码逻辑存
在。那么可能就是rdtgroup use-after-free了。

在分析use-after-free之前，我们分析下，为什么会残留大量的`task_remove_callback`

## 为什么不会被释放呢

根据上面的代码可知, 只有`move_myself()`调用才会释放该object, 调用有两条路径:
* 进程退出(不满足)
* 接收信号(qemu 在正常运行的情况下，几乎不接收信号)
* 重新调度(这个有可能，可能qemu进程在长时间休眠)

而下面的路径会一直导致, runtask 增加:
```
write resctrl filesystem FILE : tasks
|-> rdtgroup_tasks_write
    |-> __rdtgroup_move_task
        |-> task_work_add()
```
只要调用该流程就会导致 `task_move_callback`增加一个.

mail list中[\[bug report\] resctrl high memory comsumption](https://lore.kernel.org/lkml/CALvZod7E9zzHwenzf7objzGKsdBmVwTgEJ0nPgs0LUFU3SN5Pw@mail.gmail.com/)

已经报告了该bug

并在
```
commit ae28d1aae48a1258bd09a6f707ebb4231d79a761
Author: Fenghua Yu <fenghua.yu@intel.com>
Date:   Thu Dec 17 14:31:18 2020 -0800

    x86/resctrl: Use an IPI instead of task_work_add() to update PQR_ASSOC MSR
```
下面代码中进行修复.

> 需要询问其他同事，是否会频繁的write tasks文件


## 为什么会use-after-free

这个不好说，查找 kernel patch, 发现
```
commit b8511ccc75c033f6d54188ea4df7bf1e85778740
Author: Xiaochen Shen <xiaochen.shen@intel.com>
Date:   Thu Jan 9 00:28:03 2020 +0800

    x86/resctrl: Fix use-after-free when deleting resource groups
```

可能会造成`use-after-free`:

其中提到两个场景, 我们只看一个:
```
    Scenario 2:
    -----------
    In Thread 1, rdtgroup_tasks_write() adds a task_work callback
    move_myself(). If move_myself() is scheduled to execute after Thread 2
    rdtgroup_rmdir() is finished, referring to earlier rdtgrp memory
    (rdtgrp->waitcount) which was already freed in Thread 2 results in
    use-after-free issue.

    Thread 1 (rdtgroup_tasks_write)        Thread 2 (rdtgroup_rmdir)
    -------------------------------        -------------------------
    rdtgroup_kn_lock_live
      atomic_inc(&rdtgrp->waitcount)
      mutex_lock
    rdtgroup_move_task
      __rdtgroup_move_task
        /*
         * Take an extra refcount, so rdtgrp cannot be freed
         * before the call back move_myself has been invoked
         */
        atomic_inc(&rdtgrp->waitcount)
        /* Callback move_myself will be scheduled for later */
        task_work_add(move_myself)
    rdtgroup_kn_unlock
      mutex_unlock
      atomic_dec_and_test(&rdtgrp->waitcount)
      && (flags & RDT_DELETED)
                                           rdtgroup_kn_lock_live
                                             atomic_inc(&rdtgrp->waitcount)
                                             mutex_lock
                                           rdtgroup_rmdir_ctrl
                                             free_all_child_rdtgrp
                                               /*
                                                * sentry is freed without
                                                * checking refcount
                                                */
                                               kfree(sentry)*[3]
                                             rdtgroup_ctrl_remove
                                               rdtgrp->flags = RDT_DELETED
                                           rdtgroup_kn_unlock
                                             mutex_unlock
                                             atomic_dec_and_test(
                                                         &rdtgrp->waitcount)
                                             && (flags & RDT_DELETED)
                                               kfree(rdtgrp)
    /*
     * Callback is scheduled to execute
     * after rdt_kill_sb is finished
     */
    move_myself
      /*
       * Use-after-free: refer to earlier rdtgrp
       * memory which was freed in [3].
       */
      atomic_dec_and_test(&rdtgrp->waitcount)
      && (flags & RDT_DELETED)
        kfree(rdtgrp)
```
触发的顺序是:
```
1. thread1 执行 rdtgroup_tasks_write, 初始化`task_move_callback->rdtgroup`,
   并向该task的task_works添加`task_move_callback`数据结构，
2. `rdtgroup_rmdir` 释放 rdtgrp
3. 调用上面的callback，则会造成use-after-free，会modify rdtgrp->waitcount
```

而这个过程造成的后果是, `rdtgrp->waitcount` 可能会被修改。我们对比下数据结构的
相关成员偏移:
```
crash> struct rdtgroup.waitcount -o
struct rdtgroup {
  [676] atomic_t waitcount;
}
crash> struct blk_mq_hw_ctx.fq -o
struct blk_mq_hw_ctx {
  [672] struct blk_flush_queue *fq;
}
```
<font color=red size=5><strong>
可以发现, 两个成员在偏移处会重合!!!

从上面调试可知，`task_works`链表会串联很多callback work，所以这里可能会触发很多
次！从而导致将地址写成一个非法地址！

</strong>
</font>

但是`x86/resctrl: Use an IPI instead of task_work_add() to update PQR_ASSOC MSR`
已经删除了原有的逻辑，合入该patch可以解决这两个问题！


## 查看qemu 上次运行时间
查看有问题的task`0xffff8804daf56220`:
```
crash> task_struct.se ffff8806063a0000
  se = {
    load = {
      weight = 1024,
      inv_weight = 4194304
    },
    run_node = {
      __rb_parent_color = 1,
      rb_right = 0x0,
      rb_left = 0x0
    },
    group_node = {
      next = 0xffff8806063a0090,
      prev = 0xffff8806063a0090
    },
    on_rq = 0,
    exec_start = 636632819573610,
    sum_exec_runtime = 8114142,
    vruntime = 1300667212,
    prev_sum_exec_runtime = 8109566,
    nr_migrations = 1,
    parent = 0xffff8805b42ea3c0,
    cfs_rq = 0xffff8805cea65000,
    my_q = 0x0,
    avg = {
      runnable_avg_sum = 2445,
      runnable_avg_period = 46345,
      last_runnable_update = 636632819573610,
      decay_count = 607140370,
      load_avg_contrib = 53
    },
    {
      statistics = 0xffff8806063a0ed8,
      __UNIQUE_ID_rh_kabi_hide16 = {
        rh_reserved1 = 18446612158188490456
      },
      {<No data fields>}
    },
    rh_reserved2 = 0,
    rh_reserved3 = 0,
    rh_reserved4 = 0
  }
crash> runq -t -c 6
 CPU 6: 21567758243841466
        21567758225355224  PID: 84119  TASK: ffff8804f836eeb0  COMMAND: "cbs-agent"
```

对比`cpu6 runq -t`和qemu kvm `task_struct.se.exec_start`, 可以发现，`qemu-kvm`进
程已经很长时间没有运行了.
```
crash> p ((21567758243841466-636632819573610)/1000000000/60/60/24)
$23 = 242
```
大约242天，如果10s一次增加`kmalloc-32` object
大约增加

```
crash> p ((21567758243841466-636632819573610)/1000000000/10)
$2 = 2093112

crash> p ((21567758243841466-636632819573610)/1000000000/10*32/1024/1024)
$24 = 63
```
`2093112`object, 大约增加63MB.

查看其task链表成员个数:
```
crash> task_struct.task_works ffff8806063a0000
  task_works = 0xffff8804daf56220
crash> list 0xffff8804daf56220 |wc -l
2089194
```
其数量级上没有差异(和猜测上少了4000+次，可能是monitor触发频次，没有按照严格的10s
一次)

另外cpu 13 上存在另外一个 qemu-kvm 的线程`CPU 2/KVM`
```
CPU 13: 21567758245281245
        21567758242684349  PID: 133138  TASK: ffff880230b4eeb0  COMMAND: "CPU 2/KVM"
```
该task的`task_works`也存在600+ 链表成员:
```
crash> task_struct.task_works ffff880230b4eeb0
  task_works = 0xffff88021eeccbc0
crash> list 0xffff88021eeccbc0 |wc -l
628
```

这个是为什么呢?

个人猜测, `qemu-kvm`的vcpu thread, 不像其他的用户态进程，其可能会在system space
以及`VMX-non root operation`下运行很长时间，而不退回到用户态, 这样就导致
`do_notify_resume`一直得不到执行.


## 结论

目前来看，是由rdtgroup 模块use-after-free导致。并且该模块会申请造成大量不必要的`kmalloc-32` 
slab, 通过`x86/resctrl: Use an IPI instead of task_work_add() to update PQR_ASSOC MSR` patch
可以解决该问题.

## 相关patch链接
1. [x86/resctrl: Fix use-after-free when deleting resource groupst](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=b8511ccc75c033f6d54188ea4df7bf1e85778740)
2. [x86/resctrl: Use an IPI instead of task_work_add() to update PQR_ASSOC MSR](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=ae28d1aae48a1258bd09a6f707ebb4231d79a761)
