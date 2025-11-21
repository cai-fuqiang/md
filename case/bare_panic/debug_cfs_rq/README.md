### 查看出问题的cfs_rq
```
crash> list -o cfs_rq.leaf_cfs_rq_list -s cfs_rq. -H ffff88047fd192e0
ffff880070d43400
ffff880292b1ca00
ffff88007679a800
ffff88047fd18b00
```

查看有问题的第二个是否被释放 :
```
crash> kmem ffff880292b1ca00
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME ffff88017fc03600      512       1550      2176     68    16k  kmalloc-512
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffea000a4ac700  ffff880292b1c000     0     32         15    17
  FREE / [ALLOCATED]
   ffff880292b1ca00

      PAGE        PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffea000a4ac700 292b1c000                0 ffff880292b1cc00  1 2fffff00004080 slab,head
```

可以发现是被释放的.

另外，我们可以关注下里面的成员, 并将和正常的cfs_rq做对比,
对比文件 

![diff](./cfs_rq0_rq1.diff)

展示几个成员的值:
```
crash> cfs_rq.load,nr_running,h_nr_running,rb_leftmost,runnable_load_avg,tg ffff880292b1ca00
  load = {
    weight = 18446612143365287424,
    inv_weight = 0
  }
  nr_running = 1
  h_nr_running = 1
  rb_leftmost = 0xffff880046c45f58
  runnable_load_avg = 1023
  tg = 0xffff880070f4e400
```
可以看到该队列中仍有活跃的任务(`nr_running`, `runnable_load_avg`, `rb_leftmost`), 
当然有可能是, 释放的slab object 被其他人用了，又给释放了。我们来看下 `rb_leftmost`
看下该任务的`task_struct`

该成员指向`task_struct.se.run_node`
```
crash> struct sched_entity.run_node -o
struct sched_entity {
   [16] struct rb_node run_node;
}
crash> struct task_struct.se -o
struct task_struct {
   [104] struct sched_entity se;
}
```

我们首先看下这个内存是不是位于`task_struct`中，由于`task_struct`也是`slab`,所以
直接通过`kmem`命令查看
```
crash> kmem 0xffff880046c45f58
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff88017fd70100     4048        558       712     89    32k  task_struct
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffea00011b1000  ffff880046c40000     0      8          4     4
  FREE / [ALLOCATED]
  [ffff880046c45ee0]

    PID: 49399
COMMAND: "bash"
   TASK: ffff880046c45ee0  [THREAD_INFO: ffff8801c93e0000]
    CPU: 4
  STATE: TASK_RUNNING

      PAGE        PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffea00011b1140  46c45000                0        0  0 1fffff00008000 tail

crash> bt 49399
PID: 49399  TASK: ffff880046c45ee0  CPU: 4   COMMAND: "bash"
 #0 [ffff8801c93e3f58] __schedule at ffffffff816b4364
    RIP: 00007f8a92f33291  RSP: 00007ffe99e4b0c0  RFLAGS: 00000246
    RAX: 0000000000000000  RBX: 0000000000000000  RCX: ffffffffffffffff
    RDX: 0000000000000000  RSI: 0000000000000000  RDI: 0000000001200011
    RBP: 00007ffe99e4b0e0   R8: 00007f8a93873740   R9: 0000000000000000
    R10: 00007f8a93873a10  R11: 0000000000000246  R12: 0000000000000000
    R13: 0000000000735cc0  R14: 0000000000000001  R15: 0000000000000000
    ORIG_RAX: 0000000000000038  CS: 0033  SS: 002b
```

可以看到是一个睡眠的`bash`进程

最关键的是, 该进程的`cgroup` 恰好也是被释放的有问题的`cgroup`.
```
crash> task_struct.sched_task_group ffff880046c45ee0
  sched_task_group = 0xffff880070f4e400
```

ok, 那说明该数据结构大概率没有重新分配, 而`cfs_rq.load.weight`这么奇怪，是因为
该位置是slab freelist位置。

**那说明该cgroup似乎是在带着任务的状态下删除的??**

一般`bash`进程都有父进程，我们来看下:
```
crash> ps |grep 49275
  49275      1   4  ffff880046c43f40  IN   0.0  115252   1420  bash
  49399  49275   4  ffff880046c45ee0  RU   0.0  115252    200  bash
  49400  49275   0  ffff880074899fa0  IN   0.0  112660    968  grep
```

可以看到4 cpu上有两个bash进程其中问题进程(`49399`)的父进程为`49275`,
父进程的父进程（爷进程）为1。说明 `49275` 为守护进程. 来看下其
`task_group`
```
crash> task_struct.sched_task_group ffff880046c43f40
  sched_task_group = 0xffffffff81ea6600 <root_task_group>
```

可以看到是root group

另外，非常奇怪的是, 该进程虽然是`RU`， 但是其堆栈打印不出来:

```
crash> bt ffff880046c45ee0
PID: 49399  TASK: ffff880046c45ee0  CPU: 4   COMMAND: "bash"
 #0 [ffff8801c93e3f58] __schedule at ffffffff816b4364
    RIP: 00007f8a92f33291  RSP: 00007ffe99e4b0c0  RFLAGS: 00000246
    RAX: 0000000000000000  RBX: 0000000000000000  RCX: ffffffffffffffff
    RDX: 0000000000000000  RSI: 0000000000000000  RDI: 0000000001200011
    RBP: 00007ffe99e4b0e0   R8: 00007f8a93873740   R9: 0000000000000000
    R10: 00007f8a93873a10  R11: 0000000000000246  R12: 0000000000000000
    R13: 0000000000735cc0  R14: 0000000000000001  R15: 0000000000000000
    ORIG_RAX: 0000000000000038  CS: 0033  SS: 002b
```

使用如下命令:
```
[线上Bastion][root@A01-R26-I173-168: /export/wangfuqiang49/crash/bare_crash] # cat foreach_bt.txt |grep -A 2 PID  |grep -B 2 RIP
PID: 49399  TASK: ffff880046c45ee0  CPU: 4   COMMAND: "bash"
 #0 [ffff8801c93e3f58] __schedule at ffffffff816b4364
    RIP: 00007f8a92f33291  RSP: 00007ffe99e4b0c0  RFLAGS: 00000246
--
PID: 49964  TASK: ffff88025e6cdee0  CPU: 0   COMMAND: "sh"
 #0 [ffff88027e2d7f58] __schedule at ffffffff816b4364
    RIP: 00007f5338752291  RSP: 00007ffe9fb5ba80  RFLAGS: 00000246
--
PID: 49965  TASK: ffff88025e6caf70  CPU: 0   COMMAND: "sh"
 #0 [ffff8802aca97f58] __schedule at ffffffff816b4364
    RIP: 00007f5338752291  RSP: 00007ffe9fb5ba80  RFLAGS: 00000246
```
有三个进程有这样的行为:
```
crash> bt ffff880046c45ee0
PID: 49399  TASK: ffff880046c45ee0  CPU: 4   COMMAND: "bash"
 #0 [ffff8801c93e3f58] __schedule at ffffffff816b4364
    RIP: 00007f8a92f33291  RSP: 00007ffe99e4b0c0  RFLAGS: 00000246
    RAX: 0000000000000000  RBX: 0000000000000000  RCX: ffffffffffffffff
    RDX: 0000000000000000  RSI: 0000000000000000  RDI: 0000000001200011
    RBP: 00007ffe99e4b0e0   R8: 00007f8a93873740   R9: 0000000000000000
    R10: 00007f8a93873a10  R11: 0000000000000246  R12: 0000000000000000
    R13: 0000000000735cc0  R14: 0000000000000001  R15: 0000000000000000
    ORIG_RAX: 0000000000000038  CS: 0033  SS: 002b
crash> bt ffff88025e6cdee0
PID: 49964  TASK: ffff88025e6cdee0  CPU: 0   COMMAND: "sh"
 #0 [ffff88027e2d7f58] __schedule at ffffffff816b4364
    RIP: 00007f5338752291  RSP: 00007ffe9fb5ba80  RFLAGS: 00000246
    RAX: 0000000000000000  RBX: 0000000000000000  RCX: ffffffffffffffff
    RDX: 0000000000000000  RSI: 0000000000000000  RDI: 0000000001200011
    RBP: 00007ffe9fb5baa0   R8: 00007f5339092740   R9: 000000000000c32b
    R10: 00007f5339092a10  R11: 0000000000000246  R12: 0000000000000000
    R13: 0000000001441d30  R14: 0000000000000001  R15: 0000000000000000
    ORIG_RAX: 0000000000000038  CS: 0033  SS: 002b
crash> bt ffff88025e6caf70
PID: 49965  TASK: ffff88025e6caf70  CPU: 0   COMMAND: "sh"
 #0 [ffff8802aca97f58] __schedule at ffffffff816b4364
    RIP: 00007f5338752291  RSP: 00007ffe9fb5ba80  RFLAGS: 00000246
    RAX: 0000000000000000  RBX: 0000000000000000  RCX: ffffffffffffffff
    RDX: 0000000000000000  RSI: 0000000000000000  RDI: 0000000001200011
    RBP: 00007ffe9fb5baa0   R8: 00007f5339092740   R9: 000000000000c32b
    R10: 00007f5339092a10  R11: 0000000000000246  R12: 0000000000000000
    R13: 00000000014407e0  R14: 0000000000000001  R15: 0000000000000000
    ORIG_RAX: 0000000000000038  CS: 0033  SS: 002b
```
三个堆栈信息类似.

我们以`49964`为例，查看下其进程tree:
```
1-> 140160(periodic_operat) -> 49952(sh) -> 49963(sh) -> 49964(sh)
```
可以看到触发点是`periodic_operat` 脚本.


另外，打印详细的栈信息，三个栈几乎都是这样的 :
```

ffff8801c93e3e40:  ffff8801c93e39f0 ffff8801c93e39f0
ffff8801c93e3e50:  [task_struct]    [task_struct]
ffff8801c93e3e60:  ffff8801c93e3e70 schedule+41
ffff8801c93e3e70:  ffff8801c93e3f08 do_exit+1723
ffff8801c93e3e80:  [task_struct]    000000012b56fa71
ffff8801c93e3e90:  [task_struct]    00007fba4c4544f0
ffff8801c93e3ea0:  ffff8801c93e3f58 [task_struct]
ffff8801c93e3eb0:  [mm_struct]      ffff8801c93e3f18
ffff8801c93e3ec0:  [task_struct]    ffff8801c93e3ec8
ffff8801c93e3ed0:  ffff8801c93e3ec8 000000002b56fa71
ffff8801c93e3ee0:  [signal_cache]   0000000000000000
ffff8801c93e3ef0:  [task_struct]    0000000000000000
ffff8801c93e3f00:  0000000000000000 ffff8801c93e3f38
ffff8801c93e3f10:  do_group_exit+63 0000000000000000
ffff8801c93e3f20:  0000000000000000 0000000000000000
ffff8801c93e3f30:  0000000000000000 ffff8801c93e3f48
ffff8801c93e3f40:  sys_exit_group+20 0000000000000000
ffff8801c93e3f50:  system_call_fastpath+28 0000000000000000 [sp]
ffff8801c93e3f60:  0000000000000001 0000000000735cc0
ffff8801c93e3f70:  0000000000000000 00007ffe99e4b0e0
ffff8801c93e3f80:  0000000000000000 0000000000000246
ffff8801c93e3f90:  00007f8a93873a10 0000000000000000
ffff8801c93e3fa0:  00007f8a93873740 0000000000000000
ffff8801c93e3fb0:  ffffffffffffffff 0000000000000000
```
看起来像是在执行`do_group_exit`. 但是需要注意sp 的位置!!
似乎sp指向了存放 `system_call_fastpath`的下一个地址!!!

```
crash> task_struct.thread ffff880046c45ee0 -x |grep sp
    sp0 = 0xffff8801c93e4000,
    sp = 0xffff8801c93e3f58,
```
查看存放的sp，确实指向这个位置, 这个sp非常奇怪，正常`context_switch->switch_to`
路径不会存这么奇怪的地址.

查看代码，发现在:
```sh
copy_thread
=> p->thread.sp0 = (unsigned long)task_stack_page(p) + THREAD_SIZE;
=> childregs = task_pt_regs(p);
```
而`x86_64` 中 则是这样定义的:
```
#define task_pt_regs(tsk)  ((struct pt_regs *)(tsk)->thread.sp0 - 1)
```

那也就是说，该进程是子进程被创建还未调度!

另外，还有一个点比较奇怪:

通过`task_struct->sched_task_group` 看到的 `task_group` 和  `task_struct->cgroups`
看到的不同:
```
crash> task_struct.sched_task_group ffff880046c45ee0
  sched_task_group = 0xffff880070f4e400
crash> task_struct.cgroups ffff880046c45ee0
  cgroups = 0xffff8804681bed80
crash> css_set 0xffff8804681bed80
struct css_set {
  ...
  subsys = {0xffffffff81a50680 <top_cpuset>, 
      0xffffffff81ea6600 <root_task_group>,
      0xffffffff81a49aa0 <root_cpuacct>, 0xffff88017a2f0000, 0xffff88017fd11900, 0xffff88017fd158a0,
      0xffff88017fd15900, 0xffffffff81ab9520 <blkcg_root>, 0xffff88017fd15960,
      0xffff88017fd14e40, 0xffff88017fd159c0, 0xffff88017fd50400},
  callback_head = {
    next = 0xffffffff810ba940 <put_cred_rcu>,
    func = 0x0
  }
}
```

可以看到是`root_task_group`

我们来看下相关代码:
```sh
sched_move_task
=> tg = container_of(task_subsys_state_check(tsk, cpu_cgroup_subsys_id, true),
                  struct task_group, css);
=> tg = autogroup_task_group(tsk, tg);
=> tsk->sched_task_group = tg;
```
赋值 `tsk->sched_task_group` 最终会根据`autogroup_task_group()` 来. 从而会获得和
`css_set` 不同的task_cgroup, 我们来看下 `autogroup_task_group()` 相关代码:
```cpp
static inline struct task_group *
autogroup_task_group(struct task_struct *p, struct task_group *tg)
{
    int enabled = ACCESS_ONCE(sysctl_sched_autogroup_enabled);

    if (enabled && task_wants_autogroup(p, tg))
        return p->signal->autogroup->tg;

    return tg;
}
```

我们来看下相关数据结构:
```sh
crash> task_struct.signal ffff880046c45ee0
  signal = 0xffff880468135100
  signal = 0xffff880468135100
crash> struct signal_struct.autogroup 0xffff880468135100
  autogroup = 0xffff88045a6cb4c0
crash> struct autogroup.tg 0xffff88045a6cb4c0
  tg = 0xffff880073157400
```
可以发现这里面的tg的值和`cfs_rq->tg` 报告的不一样:

> NOTE
>
> 当然也可能是`cfs_rq` 被释放后重新赋值，但是按照我们之前的分析，
> 这种可能非常小，我们先按照 `cfs_rq` 被释放后，没有重新分配。
> 那这样就说明，中间经历过`signal->autogroup_tg`的释放。
