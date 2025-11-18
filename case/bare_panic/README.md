## dmesg 报错
```
[2627118.846449] BUG: unable to handle kernel NULL pointer dereference at 00000000000000fd
[2627118.855403] IP: [<ffffffff810ce830>] update_blocked_averages+0x1f0/0x700
[2627118.863093] PGD 283711067 PUD 457753067 PMD 0
[2627118.868265] Oops: 0000 [#1] SMP
[
```
## bt
```
 #8 [ffff88047fd03cc0] do_page_fault at ffffffff816bc835
 #9 [ffff88047fd03cf0] page_fault at ffffffff816b8768
    [exception RIP: update_blocked_averages+496]
    RIP: ffffffff810ce830  RSP: ffff88047fd03da0  RFLAGS: 00010006
    RAX: 0000000000000004  RBX: ffff880463f2a9a0  RCX: ffff8800736f1700
    RDX: ffff8800736f1ac0  RSI: 0000000000000032  RDI: 0000000000000000
    RBP: ffff88047fd03e08   R8: ffff88047fd18b00   R9: 0000000000000000
    R10: 0000000000000000  R11: 000000000000b93b  R12: ffff880292b1ca00
    R13: 0000000000000001  R14: ffff88047fd18a80  R15: ffff88047fd192e0
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
#10 [ffff88047fd03e10] rebalance_domains at ffffffff810d5e8d
#11 [ffff88047fd03e78] run_rebalance_domains at ffffffff810d6212
#12 [ffff88047fd03ed8] __do_softirq at ffffffff81094035
#13 [ffff88047fd03f48] call_softirq at ffffffff816c4afc
#14 [ffff88047fd03f60] do_softirq at ffffffff8102d435
#15 [ffff88047fd03f80] irq_exit at ffffffff810943b5
#16 [ffff88047fd03f98] smp_apic_timer_interrupt at ffffffff816c5e78
#17 [ffff88047fd03fb0] apic_timer_interrupt at ffffffff816c2732
--- <IRQ stack> ---
#18 [ffff880179ccfdb8] apic_timer_interrupt at ffffffff816c2732
    [exception RIP: cpuidle_enter_state+87]
```

## 反汇编
```
/usr/src/debug/kernel-3.10.0-693.21.6.el7/linux-3.10.0-693.21.6.el7.x86_64/kernel/sched/fair.c: 2336
0xffffffff810ce81d <update_blocked_averages+477>:       mov    %r11d,0x84(%rbx)
0xffffffff810ce824 <update_blocked_averages+484>:       jmp    0xffffffff810ce892 <update_blocked_averages+594>
0xffffffff810ce826 <update_blocked_averages+486>:       nopw   %cs:0x0(%rax,%rax,1)
/usr/src/debug/kernel-3.10.0-693.21.6.el7/linux-3.10.0-693.21.6.el7.x86_64/kernel/sched/fair.c: 3289
0xffffffff810ce830 <update_blocked_averages+496>:       mov    0xfc(%r13),%eax --> here
0xffffffff810ce837 <update_blocked_averages+503>:       test   %eax,%eax
0xffffffff810ce839 <update_blocked_averages+505>:       je     0xffffffff810ce6dc <update_blocked_averages+156>
/usr/src/debug/kernel-3.10.0-693.21.6.el7/linux-3.10.0-693.21.6.el7.x86_64/kernel/sched/fair.c: 5364
0xffffffff810ce83f <update_blocked_averages+511>:       mov    0xb0(%r12),%rax
```
可以看到`r13` 为`0x1`, `0x1+0xfc=0xfd`

## 具体代码
```cpp
static inline int throttled_hierarchy(struct cfs_rq *cfs_rq)
{
    return cfs_bandwidth_used() && cfs_rq->throttle_count;
}
```
估计是访问`cfs_rq->throttle_count`, 查看其偏移量:
```
crash> struct cfs_rq.throttle_count -o
struct cfs_rq {
  [252] int throttle_count;
}
crash> eval 0xfc
hexadecimal: fc
    decimal: 252
    ...
```
可以发现确实是访问该变量.

## 结合代码分析crash现场

出错函数:
```cpp
static void update_blocked_averages(int cpu)
{
    struct rq *rq = cpu_rq(cpu);
    struct cfs_rq *cfs_rq;
    unsigned long flags;

    raw_spin_lock_irqsave(&rq->lock, flags);
    update_rq_clock(rq);
    /*
     * Iterates the task_group tree in a bottom up fashion, see
     * list_add_leaf_cfs_rq() for details.
     */
    for_each_leaf_cfs_rq(rq, cfs_rq) {
        /*
         * Note: We may want to consider periodically releasing
         * rq->lock about these updates so that creating many task
         * groups does not result in continually extending hold time.
         */
        __update_blocked_averages_cpu(cfs_rq->tg, rq->cpu);
    }

    raw_spin_unlock_irqrestore(&rq->lock, flags);
}
```

cpu是crash cpu 为4, 我们来具体获取下.
```
0xffffffff810d5e79 <rebalance_domains+57>:      movl   $0x1,-0x34(%rbp)
0xffffffff810d5e80 <rebalance_domains+64>:      mov    %eax,%edi
## eax 赋值到r15
0xffffffff810d5e82 <rebalance_domains+66>:      mov    %eax,%r15d
0xffffffff810d5e85 <rebalance_domains+69>:      mov    %eax,-0x4c(%rbp)
0xffffffff810d5e88 <rebalance_domains+72>:      callq  0xffffffff810ce640 <update_blocked_averages>

crash> dis update_blocked_averages
0xffffffff810ce640 <update_blocked_averages>:   nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff810ce645 <update_blocked_averages+5>: push   %rbp
0xffffffff810ce646 <update_blocked_averages+6>: movslq %edi,%rdi
0xffffffff810ce649 <update_blocked_averages+9>: mov    %rsp,%rbp
0xffffffff810ce64c <update_blocked_averages+12>:        push   %r15

crash> bt -f
    ffff88047fd03d98: 0000000000000018 0000000000000000
    ffff88047fd03da8: 0000000000000000 1a715f99cce61063
    ffff88047fd03db8: ffff880179c54f10 ffff88047fd03e10
    ffff88047fd03dc8: 0000000000000246 ffff880292b1cab0
    ffff88047fd03dd8: 1a715f99cce61063 000000019c92c93e
    ffff88047fd03de8: 0000000000000000 ffff88047fd18a80
    ffff88047fd03df8: 000000019c92c93e 0000000000000004[r15]
    ffff88047fd03e08: ffff88047fd03e70p[rbp] ffffffff810d5e8d[call push ip]
#10 [ffff88047fd03e10] rebalance_domains at ffffffff810d5e8d
    ffff88047fd03e18: ffff88047fd03e10 00000004066ff300
    ffff88047fd03e28: ffff88047fd18a80 ffff88047fd03e68
    ffff88047fd03e38: 00000001cce61063 1a715f99cce61063
    ffff88047fd03e48: 0000000000000004 ffff88047fd18a80
    ffff88047fd03e58: ffff88047fd18a80 000000019c92c93e
    ffff88047fd03e68: 0000000000000005 ffff88047fd03ed0
    ffff88047fd03e78: ffffffff810d6212
```
可以发现确实是4。

查看rq链表上的成员.
```
crash> runq -c 4
CPU 4 RUNQUEUE: ffff88047fd18a80
  CURRENT: PID: 0      TASK: ffff880179c54f10  COMMAND: "swapper/4"
  RT PRIO_ARRAY: ffff88047fd18c10
     [no tasks queued]
  CFS RB_ROOT: ffff88047fd18b28
     [no tasks queued]
```
该runq 的cpu是idle的, 查看`rq->leaf_cfs_rq_list`链表:
```
crash> rq.leaf_cfs_rq_list ffff88047fd18a80 -o
struct rq {
  [ffff88047fd192e0] struct list_head leaf_cfs_rq_list;
}

crash> list -H ffff88047fd192e0
ffff880070d434b0
ffff880292b1cab0
ffff88007679a8b0
ffff88047fd18bb0
```
可以发现有四个成员.

我们先确认下链表是否异常:
```
crash> list -o cfs_rq.leaf_cfs_rq_list -s cfs_rq.leaf_cfs_rq_list -H ffff88047fd192e0
ffff880070d43400
  leaf_cfs_rq_list = {
    next = 0xffff880292b1cab0,
    prev = 0xffff88047fd192e0
  }
ffff880292b1ca00
  leaf_cfs_rq_list = {
    next = 0xffff88007679a8b0,
    prev = 0xffff880070d434b0
  }
ffff88007679a800
  leaf_cfs_rq_list = {
    next = 0xffff88047fd18bb0,
    prev = 0xffff880292b1cab0
  }
ffff88047fd18b00
  leaf_cfs_rq_list = {
    next = 0xffff88047fd192e0,
    prev = 0xffff88007679a8b0
  }
```
可以发现并无异常.

再获取下tg:
```
crash> list -o cfs_rq.leaf_cfs_rq_list -s cfs_rq.tg -H ffff88047fd192e0
ffff880070d43400
  tg = 0xffff88046a9ce000
ffff880292b1ca00
  tg = 0xffff880070f4e400
ffff88007679a800
  tg = 0xffff8802a3772800
ffff88047fd18b00
  tg = 0xffffffff81ea6600 <root_task_group>
```

读取每个`tg->se[4]`
```
## 第一个
crash> struct task_group.cfs_rq 0xffff88046a9ce000
  cfs_rq = 0xffff880452543b40
crash> x/8xg 0xffff880452543b40
0xffff880452543b40:     0xffff8804643be400      0xffff8804643bc400
0xffff880452543b50:     0xffff8804643bde00      0xffff880070d42000
0xffff880452543b60:     0xffff880070d43400      0xffff880070d42c00
0xffff880452543b70:     0xffff880070d43c00      0xffff880467f58600

## 第二个


crash> x/8xg 0xffff8800736f1ac0
0xffff8800736f1ac0:     0xffff8800736a57a0      0xffff8802b3abc1e0
0xffff8800736f1ad0:     0xffff8800736a5818      0xffff8800736f1a90
### 有问题
0xffff8800736f1ae0:     0x0000000000000001      0xffff88028fef3960
0xffff8800736f1af0:     0xffff88028fef34a0      0x00000000000001bd


## 第三个
crash> struct task_group.cfs_rq 0xffff8802a3772800
  cfs_rq = 0xffff88028dee1180
crash> rd 0xffff88028dee1180 8
ffff88028dee1180:  ffff88007679b000 ffff880076798400   ..yv......yv....
ffff88028dee1190:  ffff88007679aa00 ffff880076798800   ..yv......yv....
ffff88028dee11a0:  ffff88007679a800 ffff88007679be00   ..yv......yv....
ffff88028dee11b0:  ffff880076799c00 ffff880076799800   ..yv......yv....

## 第四个
crash> struct task_group.cfs_rq 0xffffffff81ea6600
  cfs_rq = 0xffff88017fd12040
crash> x/8xg 0xffff88017fd12040
0xffff88017fd12040:     0xffff88047fc18b00      0xffff88047fc58b00
0xffff88017fd12050:     0xffff88047fc98b00      0xffff88047fcd8b00
0xffff88017fd12060:     0xffff88047fd18b00      0xffff88047fd58b00
0xffff88017fd12070:     0xffff88047fd98b00      0xffff88047fdd8b00
```

可以看到第二个tg出问题了

**总结:** task_group(`0xffff88046a9ce000`)

## 先看下这个cgroup
```

crash> task_group.css.cgroup 0xffff88046a9ce000
  css.cgroup = 0xffff8804643bcc00,
crash> struct cgroup.name 0xffff880070f4d400
  name = 0x0
```

cgroup 看起来已经被释放了

## 分析代码

### tg->cfs_rq生命周期

* 创建

  在创建task group 时，会为每个cpu分配 cfs_rq
  具体代码:
  ```sh
  sched_create_group
  => tg = kzalloc(sizeof(*tg), GFP_KERNEL);
  => alloc_fair_sched_group(tg, parent)
     => tg->cfs_rq = kzalloc(sizeof(cfs_rq) * nr_cpu_ids, GFP_KERNEL);
     => tg->se = kzalloc(sizeof(se) * nr_cpu_ids, GFP_KERNEL);
     => for_each_possible_cpu(i) {
           cfs_rq = kzalloc_node(sizeof(struct cfs_rq), GFP_KERNEL, cpu_to_node(i));
           se = kzalloc_node(sizeof(struct sched_entity), GFP_KERNEL, cpu_to_node(i));
        }
  => alloc_rt_sched_group(tg, parent)
  ```
* 销毁

  在销毁task cgroup 时，会为每个cpu 销毁cfs_rq
  ```sh
  free_sched_group
  => free_fair_sched_group()
     ##  这个比较关键，下面会讲到
     => destroy_cfs_bandwidth(tg_cfs_bandwidth(tg));
     => for_each_possible_cpu(i) {
        => if (tg->cfs_rq)
           => kfree(tg->cfs_rq[i]);
        }
        => if (tg->se && tg->se[i]) {
           => kfree(tg->se[i]->statistics);
        }
  => free_rt_sched_group()
  => autogroup_free()
  => kfree(tg);
  ```

并没有看到其摘除链表的逻辑，结合前面，估计是`use-after-free`了

我们来确认下, 首先总结下上面的地址分配方式:
* tg: kzalloc
* tg->cfs_rq: kzalloc
* tg->se: kzalloc
* cfs_rq: kzalloc_node
* se: kzalloc_node

可以发现, 都是kzalloc(slab), 查看其slab是否处于分配状态:
```
crash> kmem ffff880070f4e400
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff88017fc03500     1024       2462      3232    101    32k  kmalloc-1024
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffea0001c3d200  ffff880070f48000     0     32         15    17
  FREE / [ALLOCATED]
   ffff880070f4e400

      PAGE        PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffea0001c3d380  70f4e000                0        0  0 1fffff00008000 tail
```

`task_group` 确实已经被释放了。在确认下`task_group->cfs_rq`
```
crash> struct task_group.cfs_rq ffff880070f4e400
  cfs_rq = 0xffff8800736f1ac0
crash> kmem 0xffff8800736f1ac0
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff88017fc03b00       64      62640     68416   1069     4k  kmalloc-64
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffea0001cdbc40  ffff8800736f1000     0     64         64     0
  FREE / [ALLOCATED]
  [ffff8800736f1ac0]

      PAGE        PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffea0001cdbc40  736f1000                0        0  1 1fffff00000080 slab
```

可以看到其被分配了，这是典型的`use-after-free`的现场。

那问题是, `jdog-kunlunmirror`进程被分配到哪个cgroup了呢? 通过crash查看:
```
crash> ps |grep jdog |grep kun|grep jdog
  55841   7954   4  ffff8802b13f1fa0  IN   0.2  401552  30096  jdog-kunlunmirr
  55842   7954   4  ffff880274f00fd0  IN   0.2  401552  30096  jdog-kunlunmirr
  124719   7954   0  ffff88026ea1dee0  IN   0.2  401552  30096  jdog-kunlunmirr
  124738   7954   4  ffff880078684f10  IN   0.2  401552  30096  jdog-kunlunmirr
  124739   7954   4  ffff880078685ee0  IN   0.2  401552  30096  jdog-kunlunmirr
  124740   7954   4  ffff880078686eb0  IN   0.2  401552  30096  jdog-kunlunmirr
  124761   7954   4  ffff88003ee4af70  IN   0.2  401552  30096  jdog-kunlunmirr
  125093   7954   0  ffff880046ca1fa0  IN   0.2  401552  30096  jdog-kunlunmirr
  125100   7954   4  ffff8800789b0000  IN   0.2  401552  30096  jdog-kunlunmirr
  133480   7954   0  ffff8802570e0fd0  IN   0.2  401552  30096  jdog-kunlunmirr
```
随便看一个(看第一个):
```
crash> struct task_struct.sched_task_group ffff8802b13f1fa0
  sched_task_group = 0xffff88046a9ce000
```

这个是链表中的第一个成员...

**总结:** 触发了`use-after-free`, task group, `task_group->cfs_rq`被释放后，重新分配,
代码流程总感觉有问题。为什么不再销毁cgroup 时将摘除链表.

我们在具体分析下代码

## 代码进一步分析

### leaf_cfs_rq_list del

在下面函数中，摘除`cfs_rq->leaf_cfs_rq_list`
```cpp
static inline void list_del_leaf_cfs_rq(struct cfs_rq *cfs_rq)
{
    if (cfs_rq->on_list) {
        list_del_rcu(&cfs_rq->leaf_cfs_rq_list);
        cfs_rq->on_list = 0;
    }
}
```
该函数会有两个调用流程:


* offline css
  ```sh
  cgroup_rmdir
  => cgroup_destroy_locked
     => ss->css_offline()
        => cpu_cgroup_css_offline
            => sched_offline_group
                => unregister_fair_sched_group
                   => list_del_leaf_cfs_rq  
  ```
  由于css已经被release 感觉像是这个流程
* 负载均衡
  ```sh
  rebalance_domains
  => update_blocked_averages
     => __update_blocked_averages_cpu
        => throttled_hierarchy(cfs_rq)
        => update_cfs_rq_blocked_load(cfs_rq, 1);
        if se:
           => update_entity_load_avg(se, 1)
           ## 这块表明se已经没有负载，并且cfs_rq中没有正在运行的任务，就没有
           ## 必要参与负载均衡?
           if !se->avg.runnable_avg_sum && !cfs_rq->nr_running
              => list_del_leaf_cfs_rq(cfs_rq);
        else:
           => struct rq *rq = rq_of(cfs_rq);Z
           => update_rq_runnable_avg(rq, rq->nr_running);
  ```
  该流程不太像

### leaf_cfs_rq_list add

```sh
list_add_leaf_cfs_rq
=> if !cfs_rq->on_list
   ## 如果有parent，并且parent 在rq上
   => if (cfs_rq->tg->parent &&
        cfs_rq->tg->parent->cfs_rq[cpu_of(rq_of(cfs_rq))]->on_list) {
           ## 加到头
           list_add_rcu(&cfs_rq->leaf_cfs_rq_list,
              &rq_of(cfs_rq)->leaf_cfs_rq_list);
        } else {
           ## 加到尾
           list_add_tail_rcu(&cfs_rq->leaf_cfs_rq_list,
              &rq_of(cfs_rq)->leaf_cfs_rq_list);
        }
=> cfs_rq->on_list = 1;
=> update_cfs_rq_blocked_load(cfs_rq, 0);
```

而调用栈:
```sh
enqueue_entity
=> account_entity_enqueue(cfs_rq, se);
   => cfs_rq->nr_running++;
=> se->on_rq = 1;
## 这里表示任务数量 由 0->1, 得重新考虑该cfs_rq对负载均衡的影响
=> if (cfs_rq->nr_running == 1)
   => list_add_leaf_cfs_rq(cfs_rq);
```

而`enqueue_entry`有两种调用场景:

**唤醒**
```
ttwu_activate
=> activate_task
   => enqueue_task
      => enqueue_task_fair
          => enqueue_entity
```
这种场景一般是根据task触发，但是此时任务已经move走了。

另一种场景`unthrottle`, 该场景是针对每个`cfs_rq`，该场景有可能。 
```
unthrottle_cfs_rq
=> enqueue_entity
```

难道是`unthrottle_cfs_rq` 和 `css offline`的流程有race ?

`use-after-free`这种问题比较难排查, 主要是造成`use-after-free`的现场已经丢失。我
们来看下社区, 有无相关问题报告

## upstream patch
```
commit b027789e5e50494c2325cc70c8642e7fd6059479
Author: Mathias Krause <minipli@grsecurity.net>
Date:   Wed Nov 3 20:06:13 2021 +0100

    sched/fair: Prevent dead task groups from regaining cfs_rq's
```

大概就是`unthrottle`流程和`destroy cgroup`流程有冲突，之前提到过`destroy_cfs_bandwidth`
流程比较靠后, 在`free_sched_cgroup` 中做的，那将其提到`css offline`流程中问题是
不是就解决了？

还真不是, 该patch报告了另一种场景，如下:
```
        CPU1:                                      CPU2:
          :                                        timer IRQ:
          :                                          do_sched_cfs_period_timer():
          :                                            :
          :                                            distribute_cfs_runtime():
          :                                              rcu_read_lock();
          :                                              :
          :                                              unthrottle_cfs_rq():
        sched_offline_group():                             :
          :                                                walk_tg_tree_from(…,tg_unthrottle_up,…):
          list_del_rcu(&tg->list);                           :
     (1)  :                                                  list_for_each_entry_rcu(child, &parent->children, siblings)
          :                                                    :
     (2)  list_del_rcu(&tg->siblings);                         :
          :                                                    tg_unthrottle_up():
          unregister_fair_sched_group():                         struct cfs_rq *cfs_rq = tg->cfs_rq[cpu_of(rq)];
            :                                                    :
            list_del_leaf_cfs_rq(tg->cfs_rq[cpu]);               :
            :                                                    :
            :                                                    if (!cfs_rq_is_decayed(cfs_rq) || cfs_rq->nr_running)
     (3)    :                                                        list_add_leaf_cfs_rq(cfs_rq);
          :                                                      :
          :                                                    :
          :                                                  :
          :                                                :
          :                                              :
     (4)  :                                              rcu_read_unlock();
```
大概是`offline`场景和`unthrottle`有冲突，在`CPU 0`解除`tg->list`之后，在解除`tg->cfs_rq[cpu]->leaf_cfs_rq_list`之前,
在`CPU 2` 获取到`tg`, 并且执行`list_add_leaf_cfs_rq()`流程，将即将删除的`cfs_rq`链接到`rq->leaf_cfs_rq_list`

在3.10内核中右侧有些许不同, 但最终也会走到类似的流程:
```sh
unthrottle_cfs_rq
=> walk_tg_tree_from(cfs_rq->tg, tg_nop, tg_unthrottle_up, (void *)rq);
   => tg_unthrottle_up
      => cfs_rq->throttle_count--;
      => cfs_rq->throttled_clock_task_time += rq_clock_task(rq) -
             cfs_rq->throttled_clock_task;
=> for_each_sched_entity(se)
   => if (enqueue)
      => enqueue_entity(cfs_rq, se, ENQUEUE_WAKEUP);
         => list_add_leaf_cfs_rq(cfs_rq);
```

可以看到，即便是将`destroy_cfs_bandwidth`提前(里面会cancel hrtimer), 但是`offline`流程仍然会和
`do_sched_cfs_period_timer`冲突, 所以最好还得让`list_add_leaf_cfs_rq`的流程放到下一个宽限期
中执行。

patch 改动后分析(我们以`4.18.0-372`内核为例), free 流程:
```sh
css_release
=> INIT_RCU_WORK(&css->destroy_rwork, css_release_work_fn);
=> queue_work(cgroup_destroy_wq, &css->destroy_work);

css_release_work_fn
=> ss->css_released()
   => cpu_cgroup_css_released
      => sched_release_group
         ##  先摘除链表.
         => list_del_rcu(&tg->list);
         => list_del_rcu(&tg->siblings);
=> INIT_RCU_WORK(&css->destroy_rwork, css_free_rwork_fn);
## 在执行free
=> queue_rcu_work(cgroup_destroy_wq, &css->destroy_rwork);

css_free_rwork_fn
=> ss->css_free(css);
   => css_free_rwork_fn
      => cpu_cgroup_css_free
         ## 内核注释:
         ## Relies on the RCU grace period between css_released() and this.
         => sched_unregister_group
            => unregister_fair_sched_group
               => destroy_cfs_bandwidth
               => for_each_possible_cpu
                  ## 解除 cfq_rq->list_del_leaf_cfs_rq 链表
                  => list_del_leaf_cfs_rq
            ## 最后调用call_rcu释放(等下一个宽限期) 相关object
            => call_rcu(&tg->rcu, sched_free_group_rcu);

sched_free_group_rcu
=> sched_free_group
    => free_fair_sched_group
        => for_each_possible_cpu
           => kfree(tg->cfs_rq[i]);
           => kfree(tg->se[i]);
        => kfree(tg->cfs_rq);
        => kfree(tg->se);
    => kmem_cache_free(task_group_cache, tg);
```

可以看到是先摘除`tg->list, tg->siblings` cpu css层级链表，在解除`cfs_rq->
list_del_leaf_cfs_rq`, **这里最关键的是**, `list_del_leaf_cfs_rq()`放到了free 的流程
而不是offline，这里内核有行注释:
```
Relies on the RCU grace period between css_released() and this.
```

嗯？怎么保证的在下一个宽限期?

原因在queue work时，调用的时queue_rcu_work
```sh
css_release_work_fn
=> queue_rcu_work(cgroup_destroy_wq, &css->destroy_rwork);
   => call_rcu(&rwork->rcu, rcu_work_rcufn);
```

另外，还将`sched_free_group_rcu`放到了rcu work中，相当于放到了下下个宽限期中执行。
原因是，我们将`list_del_leaf_cfs_rq`延后了，放到了free的流程中。

但是`print_cfs_stats`，回去读取这个链表:
```
print_cfs_stats
=> for_each_leaf_cfs_rq_safe
   => print_cfs_rq(m, cpu, cfs_rq);
#define for_each_leaf_cfs_rq_safe(rq, cfs_rq, pos)         \
    list_for_each_entry_safe(cfs_rq, pos, &rq->leaf_cfs_rq_list,   \
                        leaf_cfs_rq_list)
```

所以，需要放到下一个宽限期中释放.

