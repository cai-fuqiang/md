# CPU 5
```
 #0 [ff5de484c0c0f5b8] __pv_queued_spin_lock_slowpath at ffffffff9e1563fb
 #1 [ff5de484c0c0f5f8] _raw_spin_lock at ffffffff9ea8d041
 #2 [ff5de484c0c0f600] raw_spin_rq_lock_nested at ffffffff9e121d89
 #3 [ff5de484c0c0f618] try_to_wake_up at ffffffff9e127632
 #4 [ff5de484c0c0f678] __queue_work at ffffffff9e10c884
 #5 [ff5de484c0c0f6c0] queue_work_on at ffffffff9e10caf0
 #6 [ff5de484c0c0f6d0] soft_cursor at ffffffff9e5b69b8
 #7 [ff5de484c0c0f728] bit_cursor at ffffffff9e5b6577
 #8 [ff5de484c0c0f7f0] hide_cursor at ffffffff9e652c27
 #9 [ff5de484c0c0f800] vt_console_print at ffffffff9e65381a
#10 [ff5de484c0c0f868] call_console_drivers.constprop.0 at ffffffff9e160038
#11 [ff5de484c0c0f890] console_unlock at ffffffff9e160f40
#12 [ff5de484c0c0f938] vprintk_emit at ffffffff9e16204f
#13 [ff5de484c0c0f980] printk at ffffffff9ea395b9
#14 [ff5de484c0c0f9e0] __warn_printk at ffffffff9ea33444
#15 [ff5de484c0c0fa48] __update_blocked_fair at ffffffff9e13186d
#16 [ff5de484c0c0fac0] update_blocked_averages at ffffffff9e1319ae
#17 [ff5de484c0c0fb00] newidle_balance at ffffffff9e13b637

/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/sched/core.c: 3788
0xffffffff9e127624 <try_to_wake_up+356>:        add    (%rsp),%rbp
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/sched/sched.h: 1350
0xffffffff9e127628 <try_to_wake_up+360>:        xor    %esi,%esi
0xffffffff9e12762a <try_to_wake_up+362>:        mov    %rbp,%rdi
0xffffffff9e12762d <try_to_wake_up+365>:        call   0xffffffff9e121d70 <raw_spin_rq_lock_nested>   param1: rdi param2: rsi
```

代码
```
static void ttwu_queue(struct task_struct *p, int cpu, int wake_flags)
{
        struct rq *rq = cpu_rq(cpu);
        struct rq_flags rf;

        if (ttwu_queue_wakelist(p, cpu, wake_flags))
                return;

        rq_lock(rq, &rf);
        update_rq_clock(rq);
        ttwu_do_activate(rq, p, wake_flags, &rf);
        rq_unlock(rq, &rf);
}
```
`raw_spin_rq_lock_nested` 反汇编
```
0xffffffff9e121d70 <raw_spin_rq_lock_nested>:   nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9e121d75 <raw_spin_rq_lock_nested+5>: push   %r12
0xffffffff9e121d77 <raw_spin_rq_lock_nested+7>: incl   %gs:0x61ef5182(%rip)        # 0x16f00
0xffffffff9e121d7e <raw_spin_rq_lock_nested+14>:        push   %rbp
0xffffffff9e121d7f <raw_spin_rq_lock_nested+15>:        mov    %rdi,%rbp
0xffffffff9e121d82 <raw_spin_rq_lock_nested+18>:        xchg   %ax,%ax
0xffffffff9e121d84 <raw_spin_rq_lock_nested+20>:        call   0xffffffff9ea8d020 <_raw_spin_lock>
```
`bt -f `
```
 #2 [ff5de484c0c0f600] raw_spin_rq_lock_nested at ffffffff9e121d89
    ff5de484c0c0f608: ff356b1f7f568a00[lock] 0000000000000000
    ff5de484c0c0f618: ffffffff9e127632
 #3 [ff5de484c0c0f618] try_to_wake_up at ffffffff9e127632
    ff5de484c0c0f620: ff356b1f7f540000 ffffffffffffffb6
    ff5de484c0c0f630: 0000000000000014 ff356b12b7204300
    ff5de484c0c0f640: c276a3ecdefd4d00 0000000000000046
    ff5de484c0c0f650: ff356b108004d200 0000000000000005
    ff5de484c0c0f660: 00000000000259e8 ff356b108fba6ed0
    ff5de484c0c0f670: ff356b1f7f56e700 ffffffff9e10c884
```

找到rq
```
crash> struct rq.__lock -o
struct rq {
     [0] raw_spinlock_t __lock;
}
```
cpu 5的runqueues
```
crash> p runqueues |grep ff356b1f7f568a00
  [5]: ff356b1f7f568a00
```

# update_blocked_averages分析
```
0xffffffff9e13b62a <newidle_balance+266>:       mov    %r12d,%edi
0xffffffff9e13b62d <newidle_balance+269>:       mov    %rax,0x8(%rsp)
0xffffffff9e13b632 <newidle_balance+274>:       call   0xffffffff9e131880 <update_blocked_averages>
0xffffffff9e13b637 <newidle_balance+279>:       call   0xffffffff9e174880 <__rcu_read_lock>

/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/sched/fair.c: 8285
0xffffffff9e131880 <update_blocked_averages>:   nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9e131885 <update_blocked_averages+5>: push   %r14
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/sched/fair.c: 8287
0xffffffff9e131887 <update_blocked_averages+7>: movslq %edi,%rdi
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/sched/fair.c: 8285
0xffffffff9e13188a <update_blocked_averages+10>:        push   %r13
0xffffffff9e13188c <update_blocked_averages+12>:        push   %r12
0xffffffff9e13188e <update_blocked_averages+14>:        push   %rbp


#16 [ff5de484c0c0fac0] update_blocked_averages at ffffffff9e1319ae
    ff5de484c0c0fac8: 0076a3ecdefd4d00 c276a3ecdefd4d00
    ff5de484c0c0fad8: 00000001359d83e8 ff356b108cdc4500
    ff5de484c0c0fae8: 0000000000000005[r12] ff356b1080f5f600[r13]
    ff5de484c0c0faf8: ff356b1f7f568a00[r14] ffffffff9e13b637[rip]
```
# try_to_wake_up task
```
0xffffffff9e10c870 <__queue_work+448>:  cmp    %rsi,%rdx
0xffffffff9e10c873 <__queue_work+451>:  je     0xffffffff9e10c943 <__queue_work+659>
0xffffffff9e10c879 <__queue_work+457>:  mov    %r14,%rsi
0xffffffff9e10c87c <__queue_work+460>:  mov    %r15,%rdi
0xffffffff9e10c87f <__queue_work+463>:  call   0xffffffff9e10b960 <insert_work> <--- 这里?
0xffffffff9e10c884 <__queue_work+468>:  mov    (%r15),%rdi
```

堆栈
```
 #3 [ff5de484c0c0f618] try_to_wake_up at ffffffff9e127632
 #4 [ff5de484c0c0f678] __queue_work at ffffffff9e10c884

```
这里看起来少栈了. 但是是正常的
```
0xffffffff9e10b9fe <insert_work+158>:   pop    %rbx
0xffffffff9e10b9ff <insert_work+159>:   pop    %rbp
0xffffffff9e10ba00 <insert_work+160>:   pop    %r12
0xffffffff9e10ba02 <insert_work+162>:   pop    %r13
0xffffffff9e10ba04 <insert_work+164>:   pop    %r14
0xffffffff9e10ba06 <insert_work+166>:   pop    %r15

/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/./include/linux/list.h: 282
//赋值rdx
0xffffffff9e10b9e4 <insert_work+132>:   mov    0x38(%r15),%rdx
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/workqueue.c: 83
0xffffffff9e10b9e8 <insert_work+136>:   lea    0x38(%r15),%rax
0xffffffff9e10b9ec <insert_work+140>:   cmp    %rax,%rdx
0xffffffff9e10b9ef <insert_work+143>:   je     0xffffffff9e10b9d9 <insert_work+121>
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/workqueue.c: 839
0xffffffff9e10b9f1 <insert_work+145>:   mov    0x38(%r15),%rax
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/workqueue.c: 855
0xffffffff9e10b9f5 <insert_work+149>:   test   %rax,%rax
0xffffffff9e10b9f8 <insert_work+152>:   je     0xffffffff9e10b9d9 <insert_work+121>
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/workqueue.c: 856
0xffffffff9e10b9fa <insert_work+154>:   mov    0x40(%rax),%rdi
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/workqueue.c: 1370
0xffffffff9e10b9fe <insert_work+158>:   pop    %rbx
0xffffffff9e10b9ff <insert_work+159>:   pop    %rbp
0xffffffff9e10ba00 <insert_work+160>:   pop    %r12
0xffffffff9e10ba02 <insert_work+162>:   pop    %r13
0xffffffff9e10ba04 <insert_work+164>:   pop    %r14
0xffffffff9e10ba06 <insert_work+166>:   pop    %r15
弹栈, 并且是jmp调用
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/workqueue.c: 856
0xffffffff9e10ba08 <insert_work+168>:   jmp    0xffffffff9e127a00 <wake_up_process>


没有堆栈, 直接调用
0xffffffff9e127a00 <wake_up_process>:   nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9e127a05 <wake_up_process+5>: xor    %edx,%edx
0xffffffff9e127a07 <wake_up_process+7>: mov    $0x3,%esi
0xffffffff9e127a0c <wake_up_process+12>:        jmp    0xffffffff9e1274c0 <try_to_wake_up>
```

```
 #3 [ff5de484c0c0f618] try_to_wake_up at ffffffff9e127632
    ff5de484c0c0f620: ff356b1f7f540000 ffffffffffffffb6
    ff5de484c0c0f630: 0000000000000014 ff356b12b7204300
    ff5de484c0c0f640: c276a3ecdefd4d00 0000000000000046
    ff5de484c0c0f650: ff356b108004d200[rbp] 0000000000000005[r12]
    ff5de484c0c0f660: 00000000000259e8[r13] ff356b108fba6ed0[r14]
    ff5de484c0c0f670: ff356b1f7f56e700[r15] ffffffff9e10c884
```

edx保存
```
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/sched/core.c: 3971
0xffffffff9e1274c0 <try_to_wake_up>:    nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9e1274c5 <try_to_wake_up+5>:  push   %r15
0xffffffff9e1274c7 <try_to_wake_up+7>:  push   %r14
0xffffffff9e1274c9 <try_to_wake_up+9>:  push   %r13
0xffffffff9e1274cb <try_to_wake_up+11>: push   %r12
0xffffffff9e1274cd <try_to_wake_up+13>: mov    %edx,%r12d

/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/sched/core.c: 472
0xffffffff9e121d70 <raw_spin_rq_lock_nested>:   nopl   0x0(%rax,%rax,1) [FTRACE NOP]
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/./arch/x86/include/asm/preempt.h: 80
0xffffffff9e121d75 <raw_spin_rq_lock_nested+5>: push   %r12


0xffffffff9e121d70 <raw_spin_rq_lock_nested>:   nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9e121d75 <raw_spin_rq_lock_nested+5>: push   %r12
0xffffffff9e121d77 <raw_spin_rq_lock_nested+7>: incl   %gs:0x61ef5182(%rip)        # 0x16f00
0xffffffff9e121d7e <raw_spin_rq_lock_nested+14>:        push   %rbp
```



# tmp
```
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/drivers/video/fbdev/core/softcursor.c: 74
0xffffffff9e5b69a4 <soft_cursor+404>:   mov    %rbp,%rsi
0xffffffff9e5b69a7 <soft_cursor+407>:   mov    %r12,%rdi
0xffffffff9e5b69aa <soft_cursor+410>:   mov    0x310(%r12),%rax
0xffffffff9e5b69b2 <soft_cursor+418>:   mov    0x68(%rax),%rax
0xffffffff9e5b69b6 <soft_cursor+422>:   call   *%rax
0xffffffff9e5b69b8 <soft_cursor+424>:   nopl   (%rax)

 #4 [ff5de484c0c0f678] __queue_work at ffffffff9e10c884
    ff5de484c0c0f680: 00002000a9449480 ff356b1f7f828300
    ff5de484c0c0f690: 0000000000000082 ff356b108b8a95a0
    ff5de484c0c0f6a0: ff356b10a05d3000[r12] ff356b10834b7290[r13]
    ff5de484c0c0f6b0: 0000000000000001[r14] 0000000000000001[r15]
    ff5de484c0c0f6c0: ffffffff9e10caf0
 #5 [ff5de484c0c0f6c0] queue_work_on at ffffffff9e10caf0
    ff5de484c0c0f6c8: ff5de484c0c0f750 ffffffff9e5b69b8

/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/workqueue.c: 1423
0xffffffff9e10c6b0 <__queue_work>:      nopl   0x0(%rax,%rax,1) [FTRACE NOP]
/usr/src/debug/kernel-5.14.0-70.22.1.el9_0/linux-5.14.0-70.22.1.el9_0.x86_64/kernel/workqueue.c: 1428
0xffffffff9e10c6b5 <__queue_work+5>:    push   %r15
0xffffffff9e10c6b7 <__queue_work+7>:    push   %r14
0xffffffff9e10c6b9 <__queue_work+9>:    mov    %rdx,%r14
0xffffffff9e10c6bc <__queue_work+12>:   push   %r13
0xffffffff9e10c6be <__queue_work+14>:   push   %r12
0xffffffff9e10c6c0 <__queue_work+16>:   mov    %edi,%r12d
0xffffffff9e10c6c3 <__queue_work+19>:   push   %rbp
0xffffffff9e10c6c4 <__queue_work+20>:   mov    %rsi,%rbp
0xffffffff9e10c6c7 <__queue_work+23>:   push   %rbx
0xffffffff9e10c6c8 <__queue_work+24>:   sub    $0x10,%rsp
```

  fbops = 0xffffffffc118dfe0 <drm_fbdev_fb_ops>,
crash> fb_ops.fb_imageblit 0xffffffffc118dfe0
  fb_imageblit = 0xffffffffc1188640 <drm_fbdev_fb_imageblit>,

```
0xffffffffc1188640 <drm_fbdev_fb_imageblit>:    nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc1188645 <drm_fbdev_fb_imageblit+5>:  push   %rbp
0xffffffffc1188646 <drm_fbdev_fb_imageblit+6>:  mov    %rdi,%rbp
0xffffffffc1188649 <drm_fbdev_fb_imageblit+9>:  push   %rbx
...

0xffffffffc118867b <drm_fbdev_fb_imageblit+59>: pop    %rbx
0xffffffffc118867c <drm_fbdev_fb_imageblit+60>: pop    %rbp
0xffffffffc118867d <drm_fbdev_fb_imageblit+61>: jmp    0xffffffffc11882c0 <drm_fb_helper_damage>


0xffffffffc11882c0 <drm_fb_helper_damage>:      nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc11882c5 <drm_fb_helper_damage+5>:    push   %r15
0xffffffffc11882c7 <drm_fb_helper_damage+7>:    push   %r14
0xffffffffc11882c9 <drm_fb_helper_damage+9>:    mov    %ecx,%r14d
0xffffffffc11882cc <drm_fb_helper_damage+12>:   push   %r13
0xffffffffc11882ce <drm_fb_helper_damage+14>:   mov    %r8d,%r13d
0xffffffffc11882d1 <drm_fb_helper_damage+17>:   push   %r12
0xffffffffc11882d3 <drm_fb_helper_damage+19>:   mov    %esi,%r12d
0xffffffffc11882d6 <drm_fb_helper_damage+22>:   push   %rbp
0xffffffffc11882d7 <drm_fb_helper_damage+23>:   mov    %edx,%ebp
0xffffffffc11882d9 <drm_fb_helper_damage+25>:   push   %rbx

0xffffffffc1188383 <drm_fb_helper_damage+195>:  pop    %rbx
0xffffffffc1188384 <drm_fb_helper_damage+196>:  pop    %rbp
0xffffffffc1188385 <drm_fb_helper_damage+197>:  pop    %r12
0xffffffffc1188387 <drm_fb_helper_damage+199>:  pop    %r13
0xffffffffc1188389 <drm_fb_helper_damage+201>:  pop    %r14
0xffffffffc118838b <drm_fb_helper_damage+203>:  pop    %r15
0xffffffffc118838d <drm_fb_helper_damage+205>:  jmp    0xffffffff9e10cad0 <queue_work_on>

crash> dis queue_work_on
0xffffffff9e10cad0 <queue_work_on>:     nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9e10cad5 <queue_work_on+5>:   push   %rbx
0xffffffff9e10cad6 <queue_work_on+6>:   pushf
0xffffffff9e10cad7 <queue_work_on+7>:   pop    %rbx
0xffffffff9e10cad8 <queue_work_on+8>:   cli
0xffffffff9e10cad9 <queue_work_on+9>:   lock btsq $0x0,(%rdx)
0xffffffff9e10cadf <queue_work_on+15>:  jae    0xffffffff9e10caeb <queue_work_on+27>
0xffffffff9e10cae1 <queue_work_on+17>:  xor    %eax,%eax
0xffffffff9e10cae3 <queue_work_on+19>:  and    $0x2,%bh
0xffffffff9e10cae6 <queue_work_on+22>:  je     0xffffffff9e10cae9 <queue_work_on+25>
0xffffffff9e10cae8 <queue_work_on+24>:  sti
0xffffffff9e10cae9 <queue_work_on+25>:  pop    %rbx
0xffffffff9e10caea <queue_work_on+26>:  ret
0xffffffff9e10caeb <queue_work_on+27>:  call   0xffffffff9e10c6b0 <__queue_work>
0xffffffff9e10caf0 <queue_work_on+32>:  mov    $0x1,%eax
0xffffffff9e10caf5 <queue_work_on+37>:  jmp    0xffffffff9e10cae3 <queue_work_on+19>
```

```
drm_fb_helper_damage


crash> p system_wq
system_wq = $2 = (struct workqueue_struct *) 0xff356b108004d200
crash> p system_wq.cpu_pwqs
$3 = (struct pool_workqueue *) 0x2e700
crash> p __per_cpu_offset[5]
$4 = 18389722436461658112
crash> p/x __per_cpu_offset[5]
$5 = 0xff356b1f7f540000
crash> p/x 0xff356b1f7f540000+0x2e700
$6 = 0xff356b1f7f56e700


crash> worker_pool.idle_list 0xff356b1f7f568300 -o
struct worker_pool {
  [ff356b1f7f568338] struct list_head idle_list;
}

ff356b180bd56600
struct worker {
  {
    entry = {
      next = 0xff356b1238d70300,
      prev = 0xff356b1f7f568338
    },
    hentry = {
      next = 0xff356b1238d70300,
      pprev = 0xff356b1f7f568338
    }
  },
  current_work = 0x0,
  current_func = 0x0,
  current_pwq = 0x0,
  current_color = 2147483647,
  scheduled = {
    next = 0xff356b180bd56630,
    prev = 0xff356b180bd56630
  },
  task = 0xff356b18839f8000,
  pool = 0xff356b1f7f568300,
  node = {
    next = 0xff356b1238d70350,
    prev = 0xff356b1f7f5685c0
  },
  last_active = 5194481096,
  flags = 12,
  id = 1,
  sleeping = 0,
  desc = "xfs-conv/vdc\000t_destroy\000",
  rescue_wq = 0x0,
  last_func = 0xffffffffc048e4e0 <xfs_end_io>
}




```
