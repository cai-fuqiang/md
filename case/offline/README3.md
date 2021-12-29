# 验证异常代码
修改kernel　代码:
```
__secondary_switched:
        adr_l   x5, vectors
        msr     vbar_el1, x5
        isb
1:                                              //加入死循环
        b       1b
        adr_l   x0, secondary_data
        ldr     x1, [x0, #CPU_BOOT_STACK]       // get secondary_data.stack
        mov     sp, x1
        ldr     x2, [x0, #CPU_BOOT_TASK]
        msr     sp_el0, x2
        mov     x29, #0
        mov     x30, #0
        b       secondary_start_kernel
ENDPROC(__secondary_switched)

int __cpu_up(unsigned int cpu, struct task_struct *idle)
{
    ...
    //除cpu1之外的其他cpu才会等待2s
    if (cpu != 1) {
            wait_for_completion_timeout(&cpu_running,
                                        msecs_to_jiffies(2000));
    }
    ...
}
```
dmesg打印信息:

```
[    0.030990] smp: Bringing up secondary CPUs ...
[    0.031505] CPU1 : task_stack is ffff0000161f0000
[    0.031507] CPU1: failed to come online
[    0.031514] CPU1: failed in unknown state : 0x0
[    0.031549] smp: Brought up 1 node, 1 CPU
[    0.031554] SMP: Total of 1 processors activated.
[    0.031570] CPU features: detected: GIC system register CPU interface
[    0.581039] CPU: All CPU(s) started at EL1
```

可以看到主CPU没有等待secondary　cpu
通过qemu-monitor `info register  -a`查看寄存器信息:

```
CPU#1
PC=ffff00001098e214  SP=0000000000000000
```
查看pc代码:
```
crash> dis __secondary_switched
0xffff00001098e204 <__secondary_switched>:      adrp    x5, 0xffff000010082000 <vectors>
0xffff00001098e208 <__secondary_switched+4>:    add     x5, x5, #0x0
0xffff00001098e20c <__secondary_switched+8>:    msr     vbar_el1, x5
0xffff00001098e210 <__secondary_switched+12>:   isb

//下面代码为PC代码(死循环)
0xffff00001098e214 <__secondary_switched+16>:   b       0xffff00001098e214 <__secondary_switched+16>

0xffff00001098e218 <__secondary_switched+20>:   adrp    x0, 0xffff000011650000 <reset_devices>
0xffff00001098e21c <__secondary_switched+24>:   add     x0, x0, #0x8a8
0xffff00001098e220 <__secondary_switched+28>:   ldr     x1, [x0]
0xffff00001098e224 <__secondary_switched+32>:   mov     sp, x1
0xffff00001098e228 <__secondary_switched+36>:   ldr     x2, [x0,#8]
0xffff00001098e22c <__secondary_switched+40>:   msr     sp_el0, x2
0xffff00001098e230 <__secondary_switched+44>:   mov     x29, #0x0                       // #0
0xffff00001098e234 <__secondary_switched+48>:   mov     x30, #0x0                       // #0
0xffff00001098e238 <__secondary_switched+52>:   b       0xffff000010094948 <secondary_start_kernel>
```

所以可以推测，访问secondary_data中的数据成员会造成异常.
