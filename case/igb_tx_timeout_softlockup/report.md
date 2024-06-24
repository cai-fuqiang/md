目前来看, 触发softlockup 是因为cpu 80 不响应ipi导致. 打算手动回溯下堆栈

# 80 cpu堆栈分析

栈可能涉及两个:
* 内核栈(el1)
* 中断栈


## 内核栈(el1) --part 1

获取80 cpu stack base:
```
crash> bt -c 80
PID: 0      TASK: ffff5000019f4100  CPU: 80  COMMAND: "swapper/80"
bt: WARNING: cannot determine starting stack frame for task ffff5000019f4100

crash> task_struct.stack ffff5000019f4100
  stack = 0xffff8000237e0000
```

读取内存数据
```
...
ffff8000237efe00:  0000000000000001 ffff80001019ffa0   ................
ffff8000237efe10:  ffff8000237efe40 ffff8000101a0018   @.~#............
ffff8000237efe20:  ffff510eff5b8740 0000000000000001   @.[..Q..........
ffff8000237efe30:  00024ee6e2ec4180 0000000000000001   .A...N..........
ffff8000237efe40:  ffff8000237efe60 ffff8000101a0718   `.~#............
ffff8000237efe50:  ffff510eff5b8d60 ffff510eff5b8780   `.[..Q....[..Q..
ffff8000237efe60:  ffff8000237efed0 ffff8000101b29cc   ..~#.....)......
ffff8000237efe70:  ffff510eff5b8d60 00024ee6e2ec4180   `.[..Q...A...N..
ffff8000237efe80:  00024ee64c1f8500 0000000000000015   ...L.N..........
ffff8000237efe90:  ffff80001178cef8 ffff800011b79254   ..x.....T.......
ffff8000237efea0:  0000000000000000 ffff5000019f4100   .........A...P..
ffff8000237efeb0:  0000000000000000 0000000000000000   ................
ffff8000237efec0:  0000000000000080 579fab6dfbb6a800   ............m..W

ffff8000237efed0:  ffff8000237eff10[x29_2] ffff8000101b3828[LR_2]   ..~#....(8......
ffff8000237efee0:  ffff8000237efef0 ffff800010aa7868   ..~#....hx......
ffff8000237efef0:  ffff8000237eff00 ffff80001018af6c   ..~#....l.......

ffff8000237eff00:  ffff8000237eff10[x29_1] ffff800010aaff74[LR_2]   ..~#....t.......

[ffff8000237eff10] default_idle_call
ffff8000237eff10:  ffff8000237eff40 ffff8000101302f0   @.~#............
ffff8000237eff20:  ffff800011b787c0 0000000000010000   ................
ffff8000237eff30:  ffff800011b7921c ffff800011b78788   ................

[ffff8000237eff40] do_idle at ffff8000101302ec
ffff8000237eff40:  ffff8000237effb0 ffff800010130548   ..~#....H.......
ffff8000237eff50:  0000000000000085 0000000500000000   ................
ffff8000237eff60:  0000000000000050 0000000000000000   P...............
ffff8000237eff70:  0000000000000000 0000000000000000   ................
ffff8000237eff80:  0000000000000000 0000000000000000   ................
ffff8000237eff90:  0000000000000000 ffff800010130544   ........D.......
ffff8000237effa0:  ffff8000237effb0 579fab6dfbb6a800   ..~#........m..W

[ffff8000237effb0] cpu_startup_entry at ffff800010130544
ffff8000237effb0:  ffff8000237effd0 ffff800010095098   ..~#.....P......
ffff8000237effc0:  0000000000000050 579fab6dfbb6a800   P...........m..W

[ffff8000237effd0] secondary_start_kernel at ffff800010095094
ffff8000237effd0:  0000000000000000 0000000000000000   ................
ffff8000237effe0:  0000000000000000 0000000000000000   ................
ffff8000237efff0:  0000000000000000 0000000000000000 
```

直到 `default_idle_call` 调用是没有问题的, 再往前. `default_idle_call`的栈顶
`ffff8000237eff10`, 在当前栈内存中有两个位置:
* x29_1: `ffff8000237eff00`

  存储的lr 为 `LR_1`: `ffff800010aaff74`
  ```
  0xffff800010aaff68 <default_idle_call+72>:      nop
  0xffff800010aaff6c <default_idle_call+76>:      bl      0xffff80001018af58 <rcu_idle_enter>
  0xffff800010aaff70 <default_idle_call+80>:      bl      0xffff8000100874b0 <arch_cpu_idle>  <---调用点
  0xffff800010aaff74 <default_idle_call+84>:      bl      0xffff80001018af78 <rcu_idle_exit>
  0xffff800010aaff78 <default_idle_call+88>:      mov     x0, sp
  ```
  可见, 应该是调用到了`arch_cpu_idle`
  我们按照这个栈进行解释
  ```
  ffff8000237efee0:  ffff8000237efef0 ffff800010aa7868   ..~#....hx......

  ffff8000237efef0:  ffff8000237eff00[x29] ffff80001018af6c[x30]   ..~#....l.......
  [ffff8000237eff00] arch_cpu_idle
  ffff8000237eff00:  ffff8000237eff10[x30_1] ffff800010aaff74[LR1]   ..~#....t.......
  [ffff8000237eff10] default_idle_call at  ffff800010aaff70
  ffff8000237eff10:  ffff8000237eff40 ffff8000101302f0   @.~#............
  ```
  查看 `ffff80001018af6c`反汇编
  ```
  crash> dis ffff80001018af6c
  0xffff80001018af6c <rcu_idle_enter+20>: ldp     x29, x30, [sp],#16
  ```
  可以看到位于 `rcu_idle_enter`, 所以上面的堆栈, 为老的堆栈.
* x29_2: `ffff8000237eff10`
  存储的`LR_2`为 `ffff8000101b3828`
  ```
  crash> dis ffff8000101b3828
  0xffff8000101b3828 <tick_nohz_idle_stop_tick+168>:      b       0xffff8000101b37d8 <tick_nohz_idle_stop_tick+88>
  ```
  位于`tick_nohz_idle_stop_tick`, 所以也不是当前现场的栈.

综合来说, 可能的栈顶位于`do_idle` 或`default_idle_call`内. 

当然期间可能发生中断, 发生中断时, pt_regs 保存在内核栈中. 并且从异常等级
el1 发生中断有以下特征.
* **PSTATE**: 该值比较特殊, 查看所有swapper内核线程. 该值只有一个
  ```
  crash> bt -p 0 |grep PSTATE |awk '{print $4}' |uniq
  WARNING: cannot determine starting stack frame for task ffff5000019f4100
  60c00009
  ```
  但是在该内核栈中并未搜到该值.
* pt_regs: sp == stackframe[0]  pc == stackframe[1]

  内核在生成pt_regs时, 会在pt_regs 中保存stackframe, 而stackframe中的值, 和pt_regs中
  的 sp, pc相等.

  但是在内核栈中, 并没有发生这样特征的内存.

所以综合判断, 即使之前发生过中断, 但是被内核栈覆盖过一次, 我们再结合中断栈现场看下

## 中断栈
```
ffff800013dcfea0:  0000000000000000 ffff800013dcff18   ................
ffff800013dcfeb0:  ffff800013dcfee0 ffff8000101292c0   ................
ffff800013dcfec0:  ffffffffffffffc8 ffff510eff63e580   ..........c..Q..
ffff800013dcfed0:  ffffffffffffffc8 ffff800011b78788   ................

rcu_dynticks_eqs_enter
ffff800013dcfee0:  ffff800013dcfef0 ffff800010aa7994   .........y......

rcu_nmi_exit
ffff800013dcfef0:  ffff800013dcff10 ffff800010aa79c4   .........y......
ffff800013dcff00:  ffff80001178e580 ffff8000100f9c5c   ..x.....\.......

rcu_irq_exit
ffff800013dcff10:  ffff800013dcff20 ffff8000100f9c60    .......`.......

irq_exit
ffff800013dcff20:  ffff800013dcff30 ffff80001012b9a0   0...............

scheduler_ipi

ffff800013dcff30:  ffff800013dcff60 ffff8000100956dc   `........V......
ffff800013dcff40:  ffff80001178de20 ffff800011700018    .x.......p.....
ffff800013dcff50:  ffff8000237efdc0 0000000000000000   ..~#............


handle_IPI
ffff800013dcff60:  ffff800013dcffd0 ffff800010081814   ................
ffff800013dcff70:  0000000000000000 0000000000000000   ................
ffff800013dcff80:  ffff8000237efdc0 ffff8000100874d0   ..~#.....t......
ffff800013dcff90:  0000000060c00009 ffff800011b79254   ...`....T.......
ffff800013dcffa0:  ffff800013dc0000 ffff800013dd0000   ................
ffff800013dcffb0:  ffff80003e38fec0 00000000000313e8   ..8>............
ffff800013dcffc0:  0000000080001000 0000000003eecb60   ........`.......

gic_handle_irq

ffff800013dcffd0:  ffff8000237efef0[x29] ffff8000100831b8[x30]   ..~#.....1......
ffff800013dcffe0:  ffff8000237efdc0 0000ffffffffffff   ..~#............
ffff800013dcfff0:  ffff8000237eff00 0000000000000000 

--- irq stack ^^^^

--- kernel stack



[el1_irq]                           <-- rcu_irq_enter
ffff8000237efef0:  ffff8000237eff00 ffff80001018af6c   ..~#....l.......

[arch_cpu_idle]

ffff8000237eff00:  ffff8000237eff10 ffff800010aaff74   ..~#....t.......

[ffff8000237eff10] default_idle_call at ffff800010aaff70
ffff8000237eff10:  ffff8000237eff40 ffff8000101302f0   @.~#............
```

`gic_handle_irq` 会保存内核栈的现场 -- `el1_irq`, 也就说上次
发生中断时, ip 位于`ffff80001018af68`, 也就是`rcu_irq_enter`
function内, 结合`default_idle_call`代码, 以及内核栈来看,应该为:
```
default_idle_call
  rcu_irq_enter
    --- trigger IRQ---
    --- IRQ exit ----
  arch_cpu_idle
```
并且,上面推出来的栈信息,也很像是一个退栈信息, 以`handle_IPI->scheduler_ipi`
为例
```
0xffff800010095578 <handle_IPI+112>:    mrs     x0, tpidr_el2
0xffff80001009557c <handle_IPI+116>:    str     x22, [x19,x0]
0xffff800010095580 <handle_IPI+120>:    ldp     x19, x20, [sp,#16]
0xffff800010095584 <handle_IPI+124>:    ldp     x21, x22, [sp,#32]
0xffff800010095588 <handle_IPI+128>:    ldp     x23, x24, [sp,#48]
0xffff80001009558c <handle_IPI+132>:    ldp     x25, x26, [sp,#64]
0xffff800010095590 <handle_IPI+136>:    ldp     x29, x30, [sp],#112
0xffff800010095594 <handle_IPI+140>:    ret                                 -- 3. return

0xffff800010095600 <handle_IPI+248>:    nop
0xffff800010095604 <handle_IPI+252>:    mov     x1, sp
0xffff800010095608 <handle_IPI+256>:    b       0xffff800010095578 <handle_IPI+112> -- 2. 跳转


0xffff8000100956d4 <handle_IPI+460>:    b.eq    0xffff8000100957cc <handle_IPI+708>
0xffff8000100956d8 <handle_IPI+464>:    bl      0xffff80001012b8a0 <scheduler_ipi>
0xffff8000100956dc <handle_IPI+468>:    b       0xffff800010095600 <handle_IPI+248> --- 1. LR 指向 跳转
```

从每个堆栈的LR 往下退, 最终都会不调用任何函数, 直接return. (因为调用函数的话一定会留下堆栈痕迹)

综上判断, 该中断已经处理完, 中断栈,退栈.

## 内核栈 -- part2
> 为了方便, 我们再次把内核栈的部分内容贴在这

```
ffff8000237efee0:  ffff8000237efef0 ffff800010aa7868   ..~#....hx......
ffff8000237efef0:  ffff8000237eff00 ffff80001018af6c -- rcu_idle_enter  ..~#....l.......

ffff8000237eff00:  ffff8000237eff10 ffff800010aaff74 -- arch_cpu_idle   ..~#....t.......

[ffff8000237eff10] default_idle_call
ffff8000237eff10:  ffff8000237eff40 ffff8000101302f0   @.~#............
ffff8000237eff20:  ffff800011b787c0 0000000000010000   ................
ffff8000237eff30:  ffff800011b7921c ffff800011b78788   ................
```
我们再回到`default_idle_call`, 我们需要知道, 栈顶究竟是在 `arch_cpu_idle`, 
还是`default_idle_call`, 还是 `do_idle`. 

再次展示下 `default_idle_call`部分反汇编
```
0xffff800010aaff6c <default_idle_call+76>:      bl      0xffff80001018af58 <rcu_idle_enter>
0xffff800010aaff70 <default_idle_call+80>:      bl      0xffff8000100874b0 <arch_cpu_idle>
0xffff800010aaff74 <default_idle_call+84>:      bl      0xffff80001018af78 <rcu_idle_exit>
```

应该为 `rcu_idle_enter` 返回, 执行到 `arch_cpu_idle`, 但是还没有执行到 `rcu_idle_exit`

我们分情况分析
* 如果 `rcu_idle_enter` 没有返回

  `ffff8000237eff00`处就是 `rcu_idle_enter`执行`stp x29, x30`遗留的现场, 但是从`LR -4` 来看:
  ```
  crash> dis ffff800010aaff70
  0xffff800010aaff70 <default_idle_call+80>:      bl      0xffff8000100874b0 <arch_cpu_idle>
  ```
  其跳转的是 `arch_cpu_idle`, 所以 `rcu_idle_enter`一定返回了.

* 如果`rcu_idle_enter`返回了, `arch_cpu_idle`也返回了, 并执行了`rcu_idle_exit`或者后续的函数

  和上面同理, 如果`arch_cpu_idle`返回, 由于`rcu_idle_exit`栈大小是32-byte, 执行 `rcu_idle_exit`
  ```
  crash> dis rcu_idle_exit
  0xffff80001018af78 <rcu_idle_exit>:     stp     x29, x30, [sp,#-32]!
  0xffff80001018af7c <rcu_idle_exit+4>:   mov     x29, sp
  ```
  一定会覆盖`rcu_idle_enter`现场, 目前看并没有覆盖, 所以还未执行到`rcu_idle_exit`

所以综合上述信息, `arch_cpu_idle` 并没有返回.

我们需要关注下, 其流程会不会 在关中断的情况下,一直循环呢? 查看相关代码:

```cpp
void arch_cpu_idle(void)
{
        /*
         * This should do all the clock switching and wait for interrupt
         * tricks
         */
        cpu_do_idle();
        local_irq_enable();
}

void cpu_do_idle(void)
{
        if (system_uses_irq_prio_masking()) //没有使能 CONFIG_ARM64_PSEUDO_NMI , 不走
                __cpu_do_idle_irqprio();
        else
                __cpu_do_idle();
}
static void __cpu_do_idle(void)
{
        dsb(sy);
        wfi();
}
```

`wfi`指令会让cpu进入`low-power state`, 然后等待 WFE wake-up event. 而 wait-up event中
包含 IRQ, 并且不受 PSTATE.{A, I, F} mask bit 影响.

```
WFI wake-up events are never disabled by EDSCR.INTdis, and are never masked by
the PSTATE.{A, I, F} mask bits, or by the state of AllIntMask. If wake-up is
invoked by an interrupt that is disabled or masked, the interrupt is not taken
```

这里的意思是IRQ可以不受 `Physical interrupt masking on page D1-4660` 表中, IRQ屏蔽
条件的影响. IRQ总是能触发WAKEUP, 但是CPU 可以不接受该interrupt. 也就是不立即出发
handler. 

而在`arch_cpu_idle`的末尾会打开interrupt window, 在这里处理中断.

所以综合上述信息, 个人推测, cpu 在`arch_cpu_idle` 可能触发了硬件bug.
