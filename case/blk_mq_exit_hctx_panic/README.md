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

## 猜测
现在有两种猜测:
1. cbd 模块可能会修改 hctx->fq
2. 内存踩踏(比较意外的是，hctx中的其他成员看起来都正常)

## search 该内存
```
crash> search 0xffff87c5eafa5600
ffff88051fb73808: ffff87c5eafa5600
ffff88051fb73850: ffff87c5eafa5600
ffff88051fb73968: ffff87c5eafa5600
ffff88051fb739b0: ffff87c5eafa5600
ffff88051fb73bb0: ffff87c5eafa5600
ffff88051fb73bf8: ffff87c5eafa5600
ffff8806121842a0: ffff87c5eafa5600
```
前面的都是堆栈，只有最后一个是`slab`

## 附录
查看`set->ops->init_request`
```
crash> struct request_queue.tag_set ffff88359b2eac10
  tag_set = 0xffffc90050001108
crash> struct blk_mq_tag_set.ops 0xffffc90050001108
  ops = 0xffffffffc0a21060
crash> struct blk_mq_ops 0xffffffffc0a21060
struct blk_mq_ops {
  queue_rq = 0xffffffffc0a1cc00,
  map_queue = 0xffffffff8130ab30 <blk_mq_map_queue>,
  {
    timeout = 0x0,
    __UNIQUE_ID_rh_kabi_hide39 = {
      timeout = 0x0
    },
    {<No data fields>}
  },
  complete = 0x0,
  init_request = 0xffffffffc0a19060,
  exit_request = 0x0,
  reinit_request = 0x0,
  map_queues = 0x0,
  init_hctx = 0xffffffffc0a19010,
  exit_hctx = 0x0
}
```
dis init_request
```
crash> dis 0xffffffffc0a19060
0xffffffffc0a19060 <cbd_init_request>:  nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc0a19065 <cbd_init_request+5>:        testb  $0x4,0x8074(%rip)        # 0xffffffffc0a210e0
```

