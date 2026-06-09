## 环境信息

* 内核版本:
  + 6.6.0-32.15.v2505.ky11.aarch64
* CPU信息
  + HUAWEI Kunpeng 920 7262C

## 问题
使用下面参数启动虚拟机，会卡住很长时间:

```
smp maxcpus=256,cpus=4,sockets=2,cores=64,threads=2
```

去掉`maxcpus`后，不再有问题。

## 调试方法

比较好的思路, 在guest卡住的时候，获取guest vmcore，然后在通过crash
调试该vmcore:

具体步骤，在 `qemu monitor` 中执行:

```
(qemu) dump-guest-memory /root/wangfuqiang49/vmcore
```

但是如果直接使用`crash`直接打开`vmcore` 会遇到下面错误:
```
[root@11-212-1-119 18:27:18 wangfuqiang49]# crash vmcore usr/lib/debug/lib/modules/6.6.0-132.0.0.111.oe2403sp3.aarch64/vmlinux
...
crash: read error: kernel virtual address: ffff800080efeb08  type: "kernel_config_data"
WARNING: cannot read kernel_config_data
crash: read error: kernel virtual address: ffff800081eeee88  type: "possible"
WARNING: cannot read cpu_possible_map
crash: read error: kernel virtual address: ffff800081eef290  type: "present"
WARNING: cannot read cpu_present_map
crash: read error: kernel virtual address: ffff800081eeec88  type: "online"
WARNING: cannot read cpu_online_map
crash: read error: kernel virtual address: ffff800081eef498  type: "active"
WARNING: cannot read cpu_active_map
crash: read error: kernel virtual address: ffff8000835d6068  type: "shadow_timekeeper xtime_sec"
crash: read error: kernel virtual address: ffff8000823ca268  type: "init_uts_ns"
crash: usr/lib/debug/lib/modules/6.6.0-132.0.0.111.oe2403sp3.aarch64/vmlinux and vmcore do not match!

Usage:
```

这时候，我们需要增加三个参数:
* vabits_actual
  + vabits_actual 表示vm的地址宽度，默认为48
* kimage_voffset
  + 这是一个内核全局变量，需要在guest中crash获取.
* phys_offset
  + 可以在guest中crash获取

获取信息后，执行命令:
```
crash  \
usr/lib/debug/lib/modules/6.6.0-132.0.0.111.oe2403sp3.aarch64/vmlinux  \
vmcore -m vabits_actual=48  \
-m kimage_voffset=0xffff7fff55c00000  \
-m phys_offset=0x0000000040000000
```



## 调试过程

使用`bt` 获取 每个cpu的堆栈，可以发现都是swapper ,说明阻塞进程被调度走了
```
crash> bt -a |grep swap
PID: 0        TASK: ffff800081f03680  CPU: 0    COMMAND: "swapper/0"
bt: invalid kernel virtual address: 0  type: "IRQ stack contents"
bt: read of IRQ stack at 0 failed
PID: 0        TASK: ffff0000c0375100  CPU: 1    COMMAND: "swapper/1"
bt: invalid kernel virtual address: 0  type: "IRQ stack contents"
bt: read of IRQ stack at 0 failed
PID: 0        TASK: ffff0000c038e540  CPU: 2    COMMAND: "swapper/2"
bt: invalid kernel virtual address: 0  type: "IRQ stack contents"
bt: read of IRQ stack at 0 failed
PID: 0        TASK: ffff0000c038a880  CPU: 3    COMMAND: "swapper/3"
bt: invalid kernel virtual address: 0  type: "IRQ stack contents"
bt: read of IRQ stack at 0 failed
crash> bt |grep swap
PID: 0        TASK: ffff800081f03680  CPU: 0    COMMAND: "swapper/0"
bt: invalid kernel virtual address: 0  type: "IRQ stack contents"
bt: read of IRQ stack at 0 failed
```

使用`foreach bt`命令:
```
PID: 1        TASK: ffff0000c02ee540  CPU: 3    COMMAND: "swapper/0"
 #0 [ffff800083f4ba40] __switch_to at ffff800080eab71c
 #1 [ffff800083f4ba90] __schedule at ffff800080eabbfc
 #2 [ffff800083f4bae0] schedule at ffff800080eac14c
 #3 [ffff800083f4bb60] schedule_timeout at ffff800080eb45a4
 #4 [ffff800083f4bbb0] wait_for_common at ffff800080ead238
 #5 [ffff800083f4bc10] wait_for_completion at ffff800080ead45c
 #6 [ffff800083f4bc20] cpuhp_issue_call at ffff8000800bbb48
 #7 [ffff800083f4bc60] __cpuhp_setup_state_cpuslocked at ffff8000800bbf9c
 #8 [ffff800083f4bcd0] __cpuhp_setup_state at ffff8000800bc1c4
 #9 [ffff800083f4bd10] cpuinfo_regs_init at ffff800081946510
#10 [ffff800083f4bda0] do_one_initcall at ffff8000800159a0
#11 [ffff800083f4bdd0] do_initcalls at ffff800081941a70
#12 [ffff800083f4be70] kernel_init_freeable at ffff800081941d3c
#13 [ffff800083f4be90] kernel_init at ffff800080ea7e90
#14 [ffff800083f4bfe0] ret_from_fork at ffff80008001778c
     PC: 0000000000000000   LR: 0000000000000000   SP: 0000000000000000
    X29: 0000000000000000  X28: 0000000000000000  X27: 0000000000000000
    X26: 0000000000000000  X25: 0000000000000000  X24: 0000000000000000
    X23: 0000000000000000  X22: 0000000000000000  X21: 0000000000000000
    X20: 0000000000000000  X19: 0000000000000000  X18: 0000000000000000
    X17: 0000000000000000  X16: 0000000000000000  X15: 0000000000000000
    X14: 0000000000000000  X13: 0000000000000000  X12: 0000000000000000
    X11: 0000000000000000  X10: 0000000000000000   X9: 0000000000000000
     X8: 0000000000000000   X7: 0000000000000000   X6: 0000000000000000
     X5: 0000000000000000   X4: 0000000000000000   X3: 0000000000000000
     X2: 0000000000000000   X1: 0000000000000000   X0: 0000000000000000
    ORIG_X0: 0000000000000000  SYSCALLNO: 0  PSTATE: 00101005
```

找到一个很奇怪的进程，居然在`CPU3`上运行了一个`swapper 0`进程, 而这个进程
为1号进程，正在等待 `cpuhp` 流程结束:

查看汇编:
```
0xffff8000800bc1ac <__cpuhp_setup_state+60>:    mov     w5, w24 ##multi_instance
0xffff8000800bc1b0 <__cpuhp_setup_state+64>:    mov     x4, x22 ## x4 teardown
0xffff8000800bc1b4 <__cpuhp_setup_state+68>:    mov     x3, x21 ## x3 startup
0xffff8000800bc1b8 <__cpuhp_setup_state+72>:    mov     w2, w23 ## w2为invoke
0xffff8000800bc1bc <__cpuhp_setup_state+76>:    mov     x1, x20 ## w1为name
0xffff8000800bc1c0 <__cpuhp_setup_state+80>:    mov     w0, w19 ## w0为state
0xffff8000800bc1c4 <__cpuhp_setup_state+84>:    bl      0xffff8000800bbe80 <__cpuhp_setup_state_cpuslocked>

0xffff8000800bbe80 <__cpuhp_setup_state_cpuslocked>:    mov     x9, x30
0xffff8000800bbe84 <__cpuhp_setup_state_cpuslocked+4>:  nop
0xffff8000800bbe88 <__cpuhp_setup_state_cpuslocked+8>:  paciasp
0xffff8000800bbe8c <__cpuhp_setup_state_cpuslocked+12>: stp     x29, x30, [sp, #-112]!
0xffff8000800bbe90 <__cpuhp_setup_state_cpuslocked+16>: mov     x29, sp
0xffff8000800bbe94 <__cpuhp_setup_state_cpuslocked+20>: stp     x19, x20, [sp, #16]
0xffff8000800bbe98 <__cpuhp_setup_state_cpuslocked+24>: mov     w20, w0
0xffff8000800bbe9c <__cpuhp_setup_state_cpuslocked+28>: sub     w0, w0, #0x1
0xffff8000800bbea0 <__cpuhp_setup_state_cpuslocked+32>: cmp     w0, #0xed
0xffff8000800bbea4 <__cpuhp_setup_state_cpuslocked+36>: ccmp    x1, #0x0, #0x4, ls  // ls = plast
0xffff8000800bbea8 <__cpuhp_setup_state_cpuslocked+40>: b.eq    0xffff8000800bc15c <__cpuhp_setup_state_cpuslocked+732>  // b.none
0xffff8000800bbeac <__cpuhp_setup_state_cpuslocked+44>: stp     x25, x26, [sp, #64]
0xffff8000800bbeb0 <__cpuhp_setup_state_cpuslocked+48>: adrp    x25, 0xffff800081f16000 <event_exit__set_tid_address+24>
0xffff8000800bbeb4 <__cpuhp_setup_state_cpuslocked+52>: add     x25, x25, #0x738
0xffff8000800bbeb8 <__cpuhp_setup_state_cpuslocked+56>: stp     x21, x22, [sp, #32] ## 21 22
0xffff8000800bbebc <__cpuhp_setup_state_cpuslocked+60>: add     x22, x25, #0x98
0xffff8000800bbec0 <__cpuhp_setup_state_cpuslocked+64>: mov     x0, x22
0xffff8000800bbec4 <__cpuhp_setup_state_cpuslocked+68>: mov     x19, x1
0xffff8000800bbec8 <__cpuhp_setup_state_cpuslocked+72>: mov     x26, x4
0xffff8000800bbecc <__cpuhp_setup_state_cpuslocked+76>: and     w21, w5, #0xff
0xffff8000800bbed0 <__cpuhp_setup_state_cpuslocked+80>: stp     x23, x24, [sp, #48] ## 23 24
0xffff8000800bbed4 <__cpuhp_setup_state_cpuslocked+84>: mov     x23, x3
0xffff8000800bbed8 <__cpuhp_setup_state_cpuslocked+88>: stp     x27, x28, [sp, #80]

 #7 [ffff800083f4bc60] __cpuhp_setup_state_cpuslocked at ffff8000800bbf9c
    ffff800083f4bc60: ffff800083f4bcd0 ffff8000800bc1c8
    ffff800083f4bc70: 00000000000000cc[state] ffff80008126d000[name]
    ffff800083f4bc80: ffff800080024870[startup] ffff8000800247a0[teardown]
    ffff800083f4bc90: 0000000000000001[invoken] 0000000000000000[muti_instance]
    ffff800083f4bca0: 0000000000000159 ffff8000814334d8
    ffff800083f4bcb0: ffff800081389fe0 ffff800081940130
    ffff800083f4bcc0: 00000000000000d4 ffff8000800bc1ac
```

根据上面信息可知:
* name: `ffff80008126d000` :
  ```
  crash> p  (char *)0xffff80008126d000
  $2 = 0xffff80008126d000 "arm64/cpuinfo:online"
  ```
* state: `0xcc`
  ```
  crash> enum cpuhp_state
    CPUHP_AP_BASE_CACHEINFO_ONLINE = 203
    CPUHP_AP_ONLINE_DYN = 204   ---> here
    CPUHP_AP_ONLINE_DYN_END = 234
  ```

* startup
  ```
  crash> dis ffff800080024870 |head -1
    0xffff800080024870 <cpuid_cpu_online>:	mov	x9, x30
  ```
* teardown
  ```
  crash> dis ffff8000800247a0 |head -1
  0xffff8000800247a0 <cpuid_cpu_offline>:	mov	x9, x30
  ```
* invoken: true
* multi_instance: false

## 再来看看 `cpuhp_issue_call`

```
0xffff8000800bbf68 <__cpuhp_setup_state_cpuslocked+232>:        add     x0, x22, #0x610
0xffff8000800bbf6c <__cpuhp_setup_state_cpuslocked+236>:        bl      0xffff800080715b90 <_find_next_bit>
0xffff8000800bbf70 <__cpuhp_setup_state_cpuslocked+240>:        mov     x19, x0
0xffff8000800bbf74 <__cpuhp_setup_state_cpuslocked+244>:        ldr     w5, [x27]
0xffff8000800bbf78 <__cpuhp_setup_state_cpuslocked+248>:        mov     w1, w20
0xffff8000800bbf7c <__cpuhp_setup_state_cpuslocked+252>:        mov     x3, #0x0                        // #0
0xffff8000800bbf80 <__cpuhp_setup_state_cpuslocked+256>:        mov     w2, #0x1                        // #1
0xffff8000800bbf84 <__cpuhp_setup_state_cpuslocked+260>:        cmp     w5, w0
0xffff8000800bbf88 <__cpuhp_setup_state_cpuslocked+264>:        b.ls    0xffff8000800bbfe8 <__cpuhp_setup_state_cpuslocked+360>  // b.plast
0xffff8000800bbf8c <__cpuhp_setup_state_cpuslocked+268>:        ldr     x5, [x23, w19, sxtw #3]
0xffff8000800bbf90 <__cpuhp_setup_state_cpuslocked+272>:        ldr     w5, [x5, x21]
0xffff8000800bbf94 <__cpuhp_setup_state_cpuslocked+276>:        cmp     w5, w20
0xffff8000800bbf98 <__cpuhp_setup_state_cpuslocked+280>:        b.lt    0xffff8000800bbfa8 <__cpuhp_setup_state_cpuslocked+296>  // b.tstop
0xffff8000800bbf9c <__cpuhp_setup_state_cpuslocked+284>:        bl      0xffff8000800bba20 <cpuhp_issue_call>

0xffff8000800bba20 <cpuhp_issue_call>:  mov     x9, x30
0xffff8000800bba24 <cpuhp_issue_call+4>:        nop
0xffff8000800bba28 <cpuhp_issue_call+8>:        paciasp
0xffff8000800bba2c <cpuhp_issue_call+12>:       stp     x29, x30, [sp, #-64]!
0xffff8000800bba30 <cpuhp_issue_call+16>:       sbfiz   x4, x1, #2, #32
0xffff8000800bba34 <cpuhp_issue_call+20>:       mov     x29, sp
0xffff8000800bba38 <cpuhp_issue_call+24>:       add     x4, x4, w1, sxtw
0xffff8000800bba3c <cpuhp_issue_call+28>:       stp     x19, x20, [sp, #16]
0xffff8000800bba40 <cpuhp_issue_call+32>:       adrp    x5, 0xffff800081f16000 <event_exit__set_tid_address+24>
0xffff8000800bba44 <cpuhp_issue_call+36>:       stp     x21, x22, [sp, #32]

 #4 [ffff800083f4bbb0] wait_for_common at ffff800080ead238
    ffff800083f4bbb0: ffff800083f4bc10 ffff800080ead460
    ffff800083f4bbc0: ffff0000fff2dc58 00000000000000d4
    ffff800083f4bbd0: 0000000000000001 ffff800081af2c58
    ffff800083f4bbe0: ffff80007e43b000 0000000000000001
    ffff800083f4bbf0: ffff800081f16738 ffff8000800247a0
    ffff800083f4bc00: ffff800081eefa90 0000000000000001
 #5 [ffff800083f4bc10] wait_for_completion at ffff800080ead45c
    ffff800083f4bc10: ffff800083f4bc20 ffff8000800bbb4c
 #6 [ffff800083f4bc20] cpuhp_issue_call at ffff8000800bbb48
    ffff800083f4bc20: ffff800083f4bc60 ffff8000800bbfa0
    ffff800083f4bc30: 0000000000000000[cpu] 00000000000000d4[state]
    ffff800083f4bc40: ffff800081af2c58 ffff800081eeec80
    ffff800083f4bc50: ffff800081ef53a8 ffff800081f16958
 #7 [ffff800083f4bc60] __cpuhp_setup_state_cpuslocked at ffff8000800bbf9c
```
* cpu: 0
* state: 0xd4(212)

那为什么state变了

原因在于代码:
```
__cpuhp_setup_state_cpuslocked
=> ret = cpuhp_store_callbacks(state, name, startup, teardown,
      multi_instance);
   => if (name && (state == CPUHP_AP_ONLINE_DYN ||
		     state == CPUHP_BP_PREPARE_DYN))
      => ret = cpuhp_reserve_state
         => 这里做了个事情, 在cpuhp_hp_states的
            [CPUHP_AP_ONLINE_DYN, CPUHP_AP_ONLINE_DYN_END]
            中间找了个空的位置，把index返回
      => state = ret
   => sp = cpuhp_get_step(state);
   ## 初始化该sp
   => sp->startup.single = startup;
	    sp->teardown.single = teardown;
	    sp->name = name;
	    sp->multi_instance = multi_instance;
	    INIT_HLIST_HEAD(&sp->list);
```

我们来看下这个位置被填什么了
```
crash> p *(cpuhp_hp_states + 212)
$18 = {
  name = 0xffff80008126d000 "arm64/cpuinfo:online",
  startup = {
    single = 0xffff800080024870 <cpuid_cpu_online>,
    multi = 0xffff800080024870 <cpuid_cpu_online>
  },
  teardown = {
    single = 0xffff8000800247a0 <cpuid_cpu_offline>,
    multi = 0xffff8000800247a0 <cpuid_cpu_offline>
  },
  list = {
    first = 0x0
  },
  cant_stop = false,
  multi_instance = false
}
crash> p (cpuhp_hp_states + 213)->name
$17 = 0x0
```
`212`已经被填了`online` 相关代码了，但是`213` 还未填.

## 阶段性结论

该流程在做什么，在online cpu0


## 继续分析后面的堆栈

```
/usr/src/debug/kernel-6.6.0-132.0.0.111.oe2403sp3.aarch64/linux-6.6.0-132.0.0.111.oe2403sp3.aarch64/kernel/cpu.c: 265
0xffff8000800bbb44 <cpuhp_issue_call+292>:      csel    x0, x1, x0, eq  // eq = none
0xffff8000800bbb48 <cpuhp_issue_call+296>:      bl      0xffff800080ead440 <wait_for_completion>

static inline void wait_for_ap_thread(struct cpuhp_cpu_state *st, bool bringup)
{
│ struct completion *done = bringup ? &st->done_up : &st->done_down;
│ wait_for_completion(done);
}

```

个人认为是这个堆栈:
```
cpuhp_issue_call
=> cpuhp_invoke_ap_callback
   => __cpuhp_kick_ap(st)
```

`__cpuhp_kick_ap()`代码如下
```cpp
/* Regular hotplug invocation of the AP hotplug thread */
static void __cpuhp_kick_ap(struct cpuhp_cpu_state *st)
{
	if (!st->single && st->state == st->target)
		return;

	st->result = 0;
	/*
	 * Make sure the above stores are visible before should_run becomes
	 * true. Paired with the mb() above in cpuhp_thread_fun()
	 */
	smp_mb();
	st->should_run = true;
	smpboot_thread_fnwake_up_process(st->thread);
	wait_for_ap_thread(st, st->bringup);
}
```

可以看到，其在唤醒`st->thread`, 并调用`wait_for_ap_thread()`等待唤醒成功:
```cpp
static inline void wait_for_ap_thread(struct cpuhp_cpu_state *st, bool bringup)
{
	struct completion *done = bringup ? &st->done_up : &st->done_down;
	wait_for_completion(done);
}
```

调试下这个thread，以及是否`completion`:
```
crash> struct cpuhp_cpu_state.thread ffff0000fff2dc58
  thread = 0xffff0000c038d100,
crash> struct cpuhp_cpu_state.done_up.done ffff0000fff2dc58
  done_up.done = 0,
```

可以看到未完成.

该thread堆栈如下:
```
PID: 18       TASK: ffff0000c038d100  CPU: 0    COMMAND: "cpuhp/0"
 #0 [ffff800084f83d70] __switch_to at ffff800080eab71c
 #1 [ffff800084f83dc0] __schedule at ffff800080eabbfc
 #2 [ffff800084f83e10] schedule at ffff800080eac14c
 #3 [ffff800084f83e30] smpboot_thread_fn at ffff8000800f85d8
 #4 [ffff800084f83e70] kthread at ffff8000800f0360
```

需要将参数推导出来:
```
0xffff8000800f035c <kthread+228>:       mov     x0, x24
0xffff8000800f0360 <kthread+232>:       blr     x23

0xffff8000800f8460 <smpboot_thread_fn>: mov     x9, x30
0xffff8000800f8464 <smpboot_thread_fn+4>:       nop
0xffff8000800f8468 <smpboot_thread_fn+8>:       paciasp
0xffff8000800f846c <smpboot_thread_fn+12>:      stp     x29, x30, [sp, #-64]!
0xffff8000800f8470 <smpboot_thread_fn+16>:      mov     x29, sp
0xffff8000800f8474 <smpboot_thread_fn+20>:      stp     x19, x20, [sp, #16]
0xffff8000800f8478 <smpboot_thread_fn+24>:      mov     x19, x0
0xffff8000800f847c <smpboot_thread_fn+28>:      mrs     x20, sp_el0
0xffff8000800f8480 <smpboot_thread_fn+32>:      stp     x21, x22, [sp, #32]
0xffff8000800f8484 <smpboot_thread_fn+36>:      mov     w21, #0x1                       // #1
0xffff8000800f8488 <smpboot_thread_fn+40>:      stp     x23, x24, [sp, #48]

 #3 [ffff800084f83e30] smpboot_thread_fn at ffff8000800f85d8
    ffff800084f83e30: ffff800084f83e70 ffff8000800f0364
    ffff800084f83e40: ffff0000c03ba7c0 ffff0000c021b700
    ffff800084f83e50: ffff0000c038d100 ffff800083f4bc38
    ffff800084f83e60: ffff8000800f8460 ffff0000c0004420[td]

crash> smpboot_thread_data ffff0000c0004420
struct smpboot_thread_data {
  cpu = 0,
  status = 1,
  ht = 0xffff800081f167f0 <cpuhp_threads>
}

crash> struct smp_hotplug_thread 0xffff800081f167f0
struct smp_hotplug_thread {
  store = 0xffff800081af2c68 <cpuhp_state+16>,
  list = {
    next = 0xffff8000822ead80 <cpu_stop_threads+8>,
    prev = 0xffff800081f22460 <hotplug_threads>
  },
  thread_should_run = 0xffff8000800b9638 <cpuhp_should_run>,
  thread_fn = 0xffff8000800bb2a8 <cpuhp_thread_fun>,
  create = 0x0,
  setup = 0x0,
  cleanup = 0x0,
  park = 0x0,
  unpark = 0x0,
  selfparking = true,
  thread_comm = 0xffff80008127afd8 "cpuhp/%u"
}
```
首先`status 1`, 表示`HP_THREAD_ACTIVE`, 会走下面分支

```
static int smpboot_thread_fn(void *data)
{
	while (1) {
		...
		switch (td->status) {
		if (!ht->thread_should_run(td->cpu)) {
			preempt_enable_no_resched();
			schedule(); --> 在这里被调度走的
		} else {
			__set_current_state(TASK_RUNNING);
			preempt_enable();
			ht->thread_fn(td->cpu);
		}
	}
}
```
我们来看下`ht->thread_should_run`代码:
```cpp
static int cpuhp_should_run(unsigned int cpu)
{
	struct cpuhp_cpu_state *st = this_cpu_ptr(&cpuhp_state);

	return st->should_run;
}
```
我们看下该变量值:
```
crash> struct cpuhp_cpu_state.should_run ffff0000fff2dc58
  should_run = true,
```

可以看到`should_run`为true，难道是没有唤醒?

查看cpu 0 的runq
```
crash> runq -c 0
CPU 0 RUNQUEUE: ffff0000fff42280
  CURRENT: PID: 0      TASK: ffff800081f03680  COMMAND: "swapper/0"
  RT PRIO_ARRAY: ffff0000fff42540
     [no tasks queued]
  CFS RB_ROOT: ffff0000fff42340
     [120] PID: 18     TASK: ffff0000c038d100  COMMAND: "cpuhp/0"
```
可以看到`cpuhp/0`确实已经在`runq`

该`task_struct.__state`, 也是running的状态:
```
crash> task_struct.__state ffff0000c038d100
  __state = 0,
```

## 其他测试

* 使用openeuler 2403 sp3 kernel，测试问题不再复现
* 不设置qemu `maxcpus=256` 问题不再复现
