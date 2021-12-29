# 调查offline cpu陷入异常原因

## 查看正常cpu的堆栈 : CPU2

```
[    5.324322 ] CPU2 : task_stack is ffff000012330000
[   10.410065 ] secondary cpu (2) start
[   10.410148 ] Detected PIPT I-cache on CPU2
[   10.410157 ] secondary cpu notify (2) start
[   10.410194 ] GICv3: CPU2: found redistributor 2 region 0:0x00000000080e0000
[   10.410522 ] GICv3: CPU2: using allocated LPI pending table @0x0000000800350000
[   10.410594 ] secondary cpu notify (2) end
```

cpu2是可以正常启动的，其中task_struct 为 0xffff000012330000, 
大小实际上是2 * PAGE_SIZE = 128K

查看该堆栈:

```
ffff00001232ff40: 0x0000000000000085 0xffff0000116508a0
ffff00001232ff50: 0x0000000000000002 0x0000000000000002
ffff00001232ff60: 0xffff8007c35a9900 0x0000000000000000
ffff00001232ff70: 0x0000000000000000 0x0000000000000000
ffff00001232ff80: 0x0000000000000000 0xffff00001011f4c8
ffff00001232ff90: 0x0000000000000085 0xefea1f2e980fd700
ffff00001232ffa0: 0xffff00001232ffc0 0xffff000010094ad0         <====(1)
ffff00001232ffb0: 0x0000000000000002 0x0000000000000002
ffff00001232ffc0: 0x0000000000000000 0x0000000000000000
ffff00001232ffd0: 0x0000000000000000 0x0000000000000000
ffff00001232ffe0: 0x0000000000000000 0x0000000000000000
ffff00001232fff0: 0x0000000000000000 0x0000000000000000
```

我们直看栈底的一些内容:

1. ffff00001232ffa0处看起来像是函数开头部分的STP指令

查看0xffff000010094ad0地址指令
```
crash> dis -l 0xffff000010094ad0
/root/rpmbuild/BUILD/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.wang4.aarch64/arch/arm64/kernel/smp.c: 284
0xffff000010094ad0 <secondary_start_kernel+392>:        ldp     x19, x20, [sp,#16]
```

查看下`secondary_start_kernel`整个的反汇编

```
crash> dis -l secondary_start_kernel |grep -v "sadfasdf"
0xffff000010094948 <secondary_start_kernel>:    stp     x29, x30, [sp,#-64]!
0xffff00001009494c <secondary_start_kernel+4>:  mov     x29, sp
0xffff000010094950 <secondary_start_kernel+8>:  stp     x19, x20, [sp,#16]
...
0xffff000010094acc <secondary_start_kernel+388>:        bl      0xffff00001011f4a0 <cpu_startup_entry>
0xffff000010094ad0 <secondary_start_kernel+392>:        ldp     x19, x20, [sp,#16]
...
```

可以看到这个函数，会开辟64 bit 大小的栈空间, 所以这个函数的栈顶为
`ffff00001232ffc0`

而`ffff00001232ffb0 -> ffff00001232ffa8`实际上是`cpu_startup_entry`
的堆栈
可以看下`cpu_startup_entry`的反汇编

```
crash> dis -l cpu_startup_entry
/root/rpmbuild/BUILD/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.wang4.aarch64/kernel/sched/idle.c: 349
0xffff00001011f4a0 <cpu_startup_entry>: stp     x29, x30, [sp,#-32]!
0xffff00001011f4a4 <cpu_startup_entry+4>:       mov     x29, sp
```
这个函数开辟了32 bit大小的栈空间, 并将x29, x30存放在了栈顶

## 查看异常cpu的堆栈 : CPU3
```
[   10.411837 ] CPU3 : task_stack is ffff000012350000
[   12.494890 ] CPU3: failed to come online
[   12.494914 ] CPU3: failed in unknown state : 0x0
```

查看堆栈

```
ffff00001234ffa0: 0x0000000000000000 0x0000000000000000
ffff00001234ffb0: 0x0000000000000000 0x0000000000000000
ffff00001234ffc0: 0x0000000000000000 0x0000000000000005
ffff00001234ffd0: 0x0000000000000000 0x0000000000000000
ffff00001234ffe0: 0x0000000000000000 0x0000000000000000
ffff00001234fff0: 0x0000000000000000 0x0000000000000000
```

没有保存什么信息
根据qemu monitor `info register -a `命令获取PC寄存器:
```
CPU#3
PC=ffff000010082e00  SP=1de6ec7eecbfc5be
X00=ffff0000116508a8 X01=0000000000000000 X02=0000000000000000 X03=0000000000000000
X04=0000000000000000 X05=ffff000010082000 X06=0000000000000005 X07=0000000000000000
X08=ffff00001098e204 X09=0000000000000000 X10=00400034f5507510 X11=0000000000000000
```
根据pc的值打印来看

```
dis -l ffff000010082e00
/root/rpmbuild/BUILD/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.wang4.aarch64/arch/arm64/kernel/entry.S: 539
0xffff000010082e00 <el1_sync>:  stp     x0, x1, [sp]
0xffff000010082e04 <el1_sync+4>:        stp     x2, x3, [sp,#16]
0xffff000010082e08 <el1_sync+8>:        stp     x4, x5, [sp,#32]
0xffff000010082e0c <el1_sync+12>:       stp     x6, x7, [sp,#48]
```
在发生异常的时候，`kernel_ventry`宏定义，会做一些寄存器上下文的save操作
而该堆栈中没有信息，说明，在切换堆栈之前就已经发生了异常:
```
__secondary_switched:
    adr_l   x5, vectors
    msr vbar_el1, x5
    isb

    adr_l   x0, secondary_data
    ldr x1, [x0, #CPU_BOOT_STACK]   // get secondary_data.stack
    mov sp, x1						//<=====切换堆栈
    ldr x2, [x0, #CPU_BOOT_TASK]
    msr sp_el0, x2
    mov x29, #0
    mov x30, #0
    b   secondary_start_kernel
ENDPROC(__secondary_switched)
```
这里我们要看下在切换堆栈之前的堆栈地址是多少

## 进入secondary_entry堆栈为
在代码中写入了一个死循环:
```
ENTRY(secondary_entry)
1:
        b       1b
        bl      el2_setup                       // Drop to EL1
        bl      set_cpu_boot_mode_flag
        b       secondary_startup
ENDPROC(secondary_entry)
```
使用`info register -a`查看cpu寄存器

```
CPU#1
PC=000000004098e1e0  SP=0000000000000000
X00=0000000000000000 X01=0000000000000000 X02=0000000000000000 X03=0000000000000000
X04=0000000000000000 X05=0000000000000000 X06=0000000000000000 X07=0000000000000000
X08=0000000000000000 X09=0000000000000000 X10=0000000000000000 X11=0000000000000000
X12=0000000000000000 X13=0000000000000000 X14=0000000000000000 X15=0000000000000000
X16=0000000000000000 X17=0000000000000000 X18=0000000000000000 X19=0000000000000000
X20=0000000000000000 X21=0000000000000000 X22=0000000000000000 X23=0000000000000000
X24=0000000000000000 X25=0000000000000000 X26=0000000000000000 X27=0000000000000000
X28=0000000000000000 X29=0000000000000000 X30=0000000000000000
PSTATE=000003c5 ---- EL1h
q00=0000000000000000:0000000000000000 q01=0000000000000000:0000000000000000
q02=0000000000000000:0000000000000000 q03=0000000000000000:0000000000000000
q04=0000000000000000:0000000000000000 q05=0000000000000000:0000000000000000
q06=0000000000000000:0000000000000000 q07=0000000000000000:0000000000000000
q08=0000000000000000:0000000000000000 q09=0000000000000000:0000000000000000
q10=0000000000000000:0000000000000000 q11=0000000000000000:0000000000000000
q12=0000000000000000:0000000000000000 q13=0000000000000000:0000000000000000
q14=0000000000000000:0000000000000000 q15=0000000000000000:0000000000000000
q16=0000000000000000:0000000000000000 q17=0000000000000000:0000000000000000
q18=0000000000000000:0000000000000000 q19=0000000000000000:0000000000000000
q20=0000000000000000:0000000000000000 q21=0000000000000000:0000000000000000
q22=0000000000000000:0000000000000000 q23=0000000000000000:0000000000000000
q24=0000000000000000:0000000000000000 q25=0000000000000000:0000000000000000
q26=0000000000000000:0000000000000000 q27=0000000000000000:0000000000000000
q28=0000000000000000:0000000000000000 q29=0000000000000000:0000000000000000
q30=0000000000000000:0000000000000000 q31=0000000000000000:0000000000000000
FPCR: 00000000  FPSR: 00000000
```

sp是0，而且PC有值

## 查看异常原因
这里怀疑是这个代码流程

boot cpu:
```
int __cpu_up(unsigned int cpu, struct task_struct *idle)     
{                                                            
    int ret;                                                 
    long status;
	...
	
int __cpu_up(unsigned int cpu, struct task_struct *idle)                    
{                                                                           
    int ret;                                                                
    long status;                                                            
                                                                            
    secondary_data.task = idle;                //这里会赋值secondary_data相关数据结构
    secondary_data.stack = task_stack_page(idle) + THREAD_SIZE;
    update_cpu_boot_status(CPU_MMU_OFF);
    __flush_dcache_area(&secondary_data, sizeof(secondary_data));
                                                                            
    /*
     * Now bring the CPU into our world.
     */
	//在这里启动secondary cpu
    ret = boot_secondary(cpu, idle);                                        
    if (ret == 0) {                                                         
        /*                                                                  
         * CPU was successfully started, wait for it to come online or      
         * time out.                                                        
         */ 

	//等待超时                                                 
        wait_for_completion_timeout(&cpu_running,                           
                        msecs_to_jiffies(1000));                            
                                                                            
        if (!cpu_online(cpu)) {                                             
            pr_crit("CPU%u: failed to come online\n", cpu);                 
                                                                            
            if (IS_ENABLED(CONFIG_ARM64_USER_VA_BITS_52) && va52mismatch)   
                pr_crit("CPU%u: does not support 52-bit VAs\n", cpu);       
                                                                            
            ret = -EIO;                                                     
        }                                                                   
    } else {                                                                
        pr_err("CPU%u: failed to boot: %d\n", cpu, ret);                    
    }                                                                       
	//将secondary_data.task 赋值为NULL                                                               
    secondary_data.task = NULL;
    secondary_data.stack = NULL;
```

而secondary cpu代码
```
__secondary_switched:
        adr_l   x5, vectors
        msr     vbar_el1, x5
        isb
        adr_l   x0, secondary_data
        ldr     x1, [x0, #CPU_BOOT_STACK]       // get secondary_data.stack
        mov     sp, x1
        ldr     x2, [x0, #CPU_BOOT_TASK]
        msr     sp_el0, x2
        mov     x29, #0
        mov     x30, #0 
        b       secondary_start_kernel
ENDPROC(__secondary_switched)
```

所以会出现这样的情况
```
BOOT_CPU									BOOT_SECONDARY_CPU
boot_secondary								
											secondary_entry
wait

waittimeout

secondary_data.task stack = NULL
											__secondary_switched
											ldr     x1, [x0, #CPU_BOOT_STACK]		<===触发异常
```

## 验证异常代码
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
