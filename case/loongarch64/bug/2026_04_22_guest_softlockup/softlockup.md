## CPU 0 寄存器信息

cpu0 寄存器信息 :
```
CPU#0
 PC=90000000015d3f40  FCSR0 0x00010000
 GPR00: r0 0000000000000000 r1 9000000000224708 r2 90000000022f8000 r3 90000001003cf6d0
 GPR04: r4 9000000003450000 r5 0000000000000101 r6 90000001003cf6c8 r7 ffffffffffffffff
 GPR08: r8 0000000000000000 r9 000000000000000b r10 0000000000000001 r11 0000000000000001
 GPR12: r12 0000000000040101 r13 0000000000040101 r14 0000000000040000 r15 900000000802b740
 GPR16: r16 0000000000000000 r17 0000000000000101 r18 0000000000000000 r19 0000000000000101
 GPR20: r20 0000000000000001 r21 0000000006904000 r22 9000000101a3d180 r23 9000000003450000
 GPR24: r24 9000000001727740 r25 900000000802b740 r26 0000000000000000 r27 0000000000000000
 GPR28: r28 9000000002391000 r29 90000001003cf8c0 r30 90000001003cfa4c r31 90000001003cf928
CRMD=00000000000000b0
PRMD=0000000000000004
EUEN=0000000000000000
ESTAT=0000000000001808
ERA=90000000002ddd74
BADV=0000000000071c29
BADI=000000002a0031a4
EENTRY=9000000003460000
PRCFG1=00000000000072f8, PRCFG2=000000003ffff000, PRCFG3=00000000008073f2
TLBRENTRY=000000000346a000
TLBRBADV=ffff8000151d8000
TLBRERA=9000000000dcbad0
TCFG=000000000005eab5
TVAL=0000ffffffffffff
 f0 0000000000000000 f1 0000000000000000 f2 0000000000000000 f3 00000000c0000000
 f4 0000000000000000 f5 000000002ab6f4b4 f6 000000007f2e0b8e f7 0000000064b0fbf8
 f8 0000000000007078 f9 000000000b285432 f10 00000000ffffffff f11 00000000ffffffff
 f12 00000000ffffffff f13 00000000ffffffff f14 00000000ffffffff f15 00000000ffffffff
 f16 00000000ffffffff f17 00000000ffffffff f18 00000000ffffffff f19 00000000ffffffff
 f20 00000000ffffffff f21 00000000ffffffff f22 00000000ffffffff f23 0000000000000000
 f24 00000000ffffffff f25 00000000ffffffff f26 00000000ffffffff f27 00000000ffffffff
 f28 00000000ffffffff f29 00000000ffffffff f30 00000000ffffffff f31 00000000ffffffff
```

cpu 0 的`ESTAT` bit 3(TI)为1，表示该时钟中断一直在pending...

**pending** 中断, 

查看 `PC` 指向地址 `0x90000000015d3f40`所在的代码:
```
crash> dis 0x90000000015d3f40
0x90000000015d3f40 <queued_spin_lock_slowpath+600>:     ldptr.w         $t0, $s0, 0
```

可以发现卡在了`queued_spin_lock_slowpath`. 但是指令有点奇怪，是个`load ptr指令`

## 手动回溯堆栈

我们来手动回溯下堆栈:

```
0x90000000015d3ce0 <queued_spin_lock_slowpath>: move            $t0, $ra
0x90000000015d3ce4 <queued_spin_lock_slowpath+4>:       andi            $zero, $zero, 0x0
/usr/src/debug/kernel-6.6.0-97.0.0.102.oe2403sp2.loongarch64/linux-6.6.0-97.0.0.102.oe2403sp2.loongarch64/kernel/locking/qspinlock.c: 367
0x90000000015d3ce8 <queued_spin_lock_slowpath>: addi.d          $sp, $sp, -48(0xfd0)
0x90000000015d3cec <queued_spin_lock_slowpath+4>:       st.d            $s0, $sp, 32(0x20)
0x90000000015d3cf0 <queued_spin_lock_slowpath+8>:       st.d            $ra, $sp, 40(0x28)
0x90000000015d3cf4 <queued_spin_lock_slowpath+12>:      addi.w          $t0, $zero, 256(0x100)
/usr/src/debug/kernel-6.6.0-97.0.0.102.oe2403sp2.loongarch64/linux-6.6.0-97.0.0.102.oe2403sp2.loongarch64/kernel/locking/qspinlock.c: 348
0x90000000015d3cf8 <queued_spin_lock_slowpath+16>:      move            $s0, $a0
/usr/src/debug/kernel-6.6.0-97.0.0.102.oe2403sp2.loongarch64/linux-6.6.0-97.0.0.102.oe2403sp2.loongarch64/kernel/locking/qspinlock.c: 367
0x90000000015d3cfc <queued_spin_lock_slowpath+20>:      beq             $a1, $t0, 292(0x124)    # 0x90000000015d3e20 <queued_spin_lock_slowpath+312>
```

`loongarch64` 看起来和引入`ORC` 的`x86_64` 差不多, 只向堆栈中存储`$ra`, 并不存储`$sp`

根据链接1, 我们知道通用寄存器的作用
```
================= =============== =================== ============
Name              Alias           Usage               Preserved
                                                      across calls
================= =============== =================== ============
``$r0``           ``$zero``       Constant zero       Unused
``$r1``           ``$ra``         Return address      No
``$r2``           ``$tp``         TLS/Thread pointer  Unused
``$r3``           ``$sp``         Stack pointer       Yes
``$r4``-``$r11``  ``$a0``-``$a7`` Argument registers  No
``$r4``-``$r5``   ``$v0``-``$v1`` Return value        No
``$r12``-``$r20`` ``$t0``-``$t8`` Temp registers      No
``$r21``          ``$u0``         Percpu base address Unused
``$r22``          ``$fp``         Frame pointer       Yes
``$r23``-``$r31`` ``$s0``-``$s8`` Static registers    Yes
================= =============== =================== ============
```

其中:
* r1: $ra
* r3: $sp(90000001003cf6d0)
* pc: 90000000015d3f40

因为堆栈是向下增长的。我们查看`$sp(90000001003cf6d0)` 向下的一些内存,
再结合汇编代码

```
0x90000000015d3ce8 <queued_spin_lock_slowpath>: addi.d          $sp, $sp, -48(0xfd0)
0x90000000015d3cec <queued_spin_lock_slowpath+4>:       st.d            $s0, $sp, 32(0x20)
0x90000000015d3cf0 <queued_spin_lock_slowpath+8>:       st.d            $ra, $sp, 40(0x28)
```

其调用者pc存储在 `0x90000001003cf6d0 + 40 = 0x90000001003cf6f8`

以此类推，结果如下:

```
## 0x90000000015d3f40 <queued_spin_lock_slowpath+600>: ldptr.w         $t0, $s0, 0
90000001003cf6d0:  9000000105ae2400 0000000000000000   .$..............
90000001003cf6e0:  9000000001a5bb08 90000001003cf760   ........`.<.....
90000001003cf6f0:  9000000003450000

## 0x9000000000224704 <die+284>:   bl queued_spin_lock_slowpath
90000001003cf6f8:  9000000000224708                    ..E......G".....
90000001003cf700:  0000000000000010 0000000000071c29   ........).......
90000001003cf710:  0000000000000000 0000000000000001   ................
90000001003cf720:  90000001003cf760 

## 0x90000000015d4b3c <do_page_fault+84>
90000001003cf728:  90000000015d4b3c   `.<.....<K].....
90000001003cf730:  0000000000000024 90000001a27a3c00   $........<z.....
90000001003cf740:  900000011691e000 90000001003cf8f0   ..........<.....
90000001003cf750:  90000000035363c0 

## 0x900000000023b2a0 <tlb_do_page_fault_0+280>
90000001003cf728:  900000000023b2a0   .cS.......#.....
90000001003cf760:  0000000000000000 90000000013fa4b8   ..........?.....
90000001003cf770:  90000000022f8000 90000001003cf8a0   ../.......<.....
90000001003cf780:  0000000000000001 900000011691e000   ................
90000001003cf790:  90000001003cf8f0 0000000000000001   ..<.............
90000001003cf7a0:  0000000000000000 900000011691e000   ................
90000001003cf7b0:  0000000000000001 9000000105ae2400   .........$......
90000001003cf7c0:  00000000000000fd 0000000000071c1d   ................
90000001003cf7d0:  0000000000000002 0000000000000002   ................
90000001003cf7e0:  0000000000000008 000000000000000b   ................
90000001003cf7f0:  0000000000000000 900000011ffd8918   ................
```
看起来是在一个异常的堆栈里。


那为什么会卡死在`die()`中呢? 我们来看下die 中的 `spinlock`:
```
die
=> raw_spin_lock_irq(&die_lock)
=> bust_spinlocks(1);
   => if (yes)
      ++oops_in_progress;
=> raw_spin_unlock_irq(&die_lock);
```

查看上面两个变量值
```
crash> p oops_in_progress
oops_in_progress = $2 = 2
crash> p die_lock
die_lock = $3 = {
  raw_lock = {
    {
      val = {
        counter = 262401
      },
      {
        locked = 1 '\001',
        pending = 1 '\001'
      },
      {
        locked_pending = 257,
        tail = 4
      }
    }
  }
}
```
可以看到 locked, pending 都是1, tail 为4.

tail有两部分组成(cpu+1, node_index)，4表示
```
[0(cpu_id)+1] << 2(node_id) + 0 = 4
```

我们查看cpu 0 的 qnode.

```
crash> struct qnode 900000000802b740
struct qnode {
  mcs = {
    next = 0x0,
    locked = 0,
    count = 1
  }
}
```
其确实有一个 mcs node. 那么也就是说，在CPU 0之前, 有两个cpu已经获取该锁了。
并且经查看代码，只有`die()` 过程会加 `die_lock` 锁。


另外，CPU 3 也没有dump下来堆栈, 其寄存器为:

```
crash> dis 90000000015d3d28
0x90000000015d3d28 <queued_spin_lock_slowpath+64>:      ld.bu           $t0, $s0, 0

PID: 2922950  TASK: 90000001a6ff2400  CPU: 3    COMMAND: "telegraf"
 PC=90000000015d3d28  FCSR0 0x00010000
 GPR00: r0 0000000000000000 r1 9000000000224708 r2 0000000000000000 r3 90000001003cf2a0
 ESTAT=00000000000c1800
```

查看堆栈
```
90000001003cf2a0:  900000000259f000 0000000000000001   ..Y.............
90000001003cf2b0:  ffffffffffffffff 0000000000000001   ................
90000001003cf2c0:  9000000100334600 

## 0x90000000002e4048 <handle_irq_event_percpu+88>:        bl              20368(0x4f90)   # 0x90000000002e8fd8 <note_interrupt>
## 0x90000000002e404c <handle_irq_event_percpu+92>
90000000002e404c   .F3.....L@......

90000001003cf2d0:  90000001003cf35c 9000000002cf95b0   \.<.............
90000001003cf2e0:  90000001003cf368 90000001003cf360   h.<.....`.<.....
90000001003cf2f0:  0000000000000000 9000000003485fc0   ........._H.....
90000001003cf300:  0000000000000001 90000000002e13f4   ................
90000001003cf310:  90000000016fc5e0 ffffffffffffffea   ..o.............
90000001003cf320:  90000001003cf3e8 90000001003cf400   ..<.......<.....
90000001003cf330:  0000000000000000 90000001003cf5bf   ..........<.....
90000001003cf340:  9000000003485ff0 90000001003cf3a0   ._H.......<.....
90000001003cf350:  90000001003cf3a0 9000000000254068   ..<.....h@%.....
```

**从`queued_spin_lock_slowpath` 向前推一个栈似乎推的不对。理论上应该是`handle_irq_event_percpu`，
不知道哪里出了问题.**

## 参考链接
1. [Introduction to LoongArch](https://kernel.org/doc/Documentation/arch/loongarch/introduction.rst)
