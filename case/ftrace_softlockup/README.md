# 问题现象
在arm64环境, redhat kernel 4.18.0-147内核上，开启`ftrace function/function graph`
跟踪器可能会导致kernel softlockup。

经后来复现，发现在使用`qemu-kvm`运行虚机后，该问题会必现。

PS: 该必现田间实际上是在分析vmcore后，才找到。

# 定位过程

## 复现步骤
* `echo function > /sys/kernel/debug/tracing/current_event` #设置ftrace function 跟踪器
* qemu-kvm启动虚机

PS: 上面步骤不分先后顺序，均可复现问题

## 其他操作
* `echo 1 > /proc/sys/kernel/softlockup_panic` # 当检测到softlockup，会触发kernel panic
* `echo 1 > /proc/sys/kernel/softlockup_all_cpu_backtrace` # 当检测到softlockup，会将所有CPU状态
  (堆栈和寄存器信息)打印到串口，但是在我们服务器上不可用

## 分析过程
执行上面的步骤后，会导致kernel softlockup，并触发panic，kexec会dump当前cpu堆栈信息，
形成vmcore文件, 首先分析vmcore文件

### 分析vmcore

```

# crash vmcore vmlinux

...
WARNING: cannot find NT_PRSTATUS note for cpu: 23
WARNING: cannot find NT_PRSTATUS note for cpu: 47
please wait... (determining panic task)
WARNING: cannot determine starting stack frame for task ffff803dda21cb00
WARNING: cannot determine starting stack frame for task ffff805fcdd42200
...
crash> bt
PID: 1214   TASK: ffff805fcc3afd00  CPU: 27  COMMAND: "kworker/27:2"
 #0 [ffff00001224fb40] __crash_kexec at ffff00001019bcc4
 #1 [ffff00001224fcd0] panic at ffff0000100e3dc4
 #2 [ffff00001224fdb0] watchdog_timer_fn at ffff0000101d2948
 #3 [ffff00001224fe10] __hrtimer_run_queues at ffff000010179130
 #4 [ffff00001224fe90] hrtimer_interrupt at ffff000010179f0c
 #5 [ffff00001224ff00] arch_timer_handler_phys at ffff00001076d8e8
 #6 [ffff00001224ff20] handle_percpu_devid_irq at ffff00001015ca4c
 #7 [ffff00001224ff60] generic_handle_irq at ffff0000101560a8
 #8 [ffff00001224ff80] __handle_domain_irq at ffff000010156988
 #9 [ffff00001224ffc0] gic_handle_irq at ffff0000100816b0
--- <IRQ stack> ---
#10 [ffff00003358fbb0] el1_irq at ffff0000100830ac
     PC: ffff000010191044  [smp_call_function_many+796]
     LR: ffff000010191000  [smp_call_function_many+728]
     SP: ffff00003358fbc0  PSTATE: 80c00009
    X29: ffff00003358fbc0  X28: 0000000000001000  X27: ffff805fffc1ee88
    X26: 0000000000000000  X25: ffff000010190700  X24: ffff00001111ec00
    X23: 0000000000000001  X22: ffff00001145c164  X21: ffff000011453778
    X20: ffff805fffc1ec88  X19: ffff805fffc1ec80  X18: 000000000000001e
    X17: 0000ffffaf186828  X16: 0000aaaae468fea8  X15: 0000ce10b984f814
    X14: 0008e0c18a938e5c  X13: 0000000060b99fa6  X12: 0000000000000018
    X11: 0000000009aed244  X10: 0000000000000d10   X9: 0000000000000000
     X8: ffff805fffc1f088   X7: 0000000000000000   X6: ffffa05ffbf83780
     X5: ffffa05ffbf83780   X4: fffffffff7ffffff   X3: 0000000000000000
     X2: ffff803ffffc3798   X1: 0000000000000003   X0: 0000000000000017
#11 [ffff00003358fbc0] smp_call_function_many at ffff000010191040
#12 [ffff00003358fc30] kick_all_cpus_sync at ffff000010191184
#13 [ffff00003358fc40] aarch64_insn_patch_text at ffff00001095d810
#14 [ffff00003358fca0] arch_jump_label_transform at ffff000010099cd8
#15 [ffff00003358fcf0] __jump_label_update at ffff00001025e2e8
#16 [ffff00003358fd30] jump_label_update at ffff00001025ea20
#17 [ffff00003358fd60] static_key_enable_cpuslocked at ffff00001025ed88
#18 [ffff00003358fd80] static_key_enable at ffff00001025ede8
#19 [ffff00003358fda0] netstamp_clear at ffff0000107c671c
#20 [ffff00003358fdb0] process_one_work at ffff0000101013c0
#21 [ffff00003358fe00] worker_thread at ffff000010101640
#22 [ffff00003358fe60] kthread at ffff000010108494
```

#### 初步分析

从上面可以看出来，实际上造成softlockup的原因是`kick_all_cpus_sync`调用了
`smp_call_function_many`, 而`smp_call_function_many`实际上是一个多核之间通信
的机制，调用`smp_call_function_many`该函数的核会向其他核发送IPI中断，等待其他核
回应，估计是有部分核没有收到IPI中断，或者收到了未回应，导致调用者一直等，另外
红帽内核是不支持抢占的(`CONFIG_PREEMPT=n`)，会导致softlockup触发。

PS: 
* 这里面涉及static-key机制，也是一个比较有意思，但是对提升性能很有效的机制，
如果感兴趣的话，请参考 https://www.dazhuanlan.com/2019/10/10/5d9f4b6a20f82
* 这里还涉及self modify code后，如何sync的问题, 之后会更新该部分内容。
* 以上两点不会影响对该问题的分析


那么`smp_call_function_many`肯定是在等某些CPU回应，在栈中可能会有些信息可以
获取，那么我们结合vmcore和代码开看下相关信息。为了寻找出调用者在等待哪个核心。
并打印出该核的堆栈，看看这个核在干什么。


#### 找出等待的CPU核

根据 `#11 [ffff00003358fbc0] smp_call_function_many at ffff000010191040`
分析`smp_call_function_many`调用情况
```

crash> dis -l smp_call_function_many
...
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/kernel/smp.c: 468
0xffff000010191014 <smp_call_function_many+748>:        add     x1, x1, x2
0xffff000010191018 <smp_call_function_many+752>:        add     x2, x1, #0x18
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/kernel/smp.c: 108
0xffff00001019101c <smp_call_function_many+756>:        ldar    w1, [x2]
0xffff000010191020 <smp_call_function_many+760>:        tbz     w1, #0, 0xffff000010190ff8 <smp_call_function_many+720>
0xffff000010191024 <smp_call_function_many+764>:        nop
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/arch/arm64/include/asm/cmpxchg.h: 246
0xffff000010191028 <smp_call_function_many+768>:        mov     w1, w1
0xffff00001019102c <smp_call_function_many+772>:        sevl
0xffff000010191030 <smp_call_function_many+776>:        wfe
0xffff000010191034 <smp_call_function_many+780>:        ldxr    w3, [x2]
0xffff000010191038 <smp_call_function_many+784>:        eor     w3, w3, w1
0xffff00001019103c <smp_call_function_many+788>:        cbnz    w3, 0xffff000010191044 <smp_call_function_many+796>
0xffff000010191040 <smp_call_function_many+792>:        wfe
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/kernel/smp.c: 108
0xffff000010191044 <smp_call_function_many+796>:        ldar    w1, [x2]

...

```
查看smp_call_function_many+796对应的kernel/smp.c: 108

```C/C++
//===kernel/smp.c:106
static __always_inline void csd_lock_wait(call_single_data_t *csd) 
{   //这里实际上类似于自旋锁                                                            
    smp_cond_load_acquire(&csd->flags, !(VAL & CSD_FLAG_LOCK));    //108行
}


//=====kernel/smp.c: smp_call_function_many
void smp_call_function_many(const struct cpumask *mask,             
                smp_call_func_t func, void *info, bool wait)
{
	...
	if (wait) {                                
	    for_each_cpu(cpu, cfd->cpumask) {      
	        call_single_data_t *csd;           
	                                           
	        csd = per_cpu_ptr(cfd->csd, cpu);  
			//在这个地方调用的
	        csd_lock_wait(csd);                		//486行
	    }                                      
	}
	...
}
```

从这里可以看出csd实际上是一个per-cpu变量，通过查看per-cpu变量的区间可以
得到该csd是哪个cpu的。

那么这里就要从汇编中推导csd的值，推导过程如下:
smp_call_function_many+748 C代码为 kernel/smp.c:468，而下面的

`0xffff000010191018 <smp_call_function_many+752>:        add     x2, x1, #0x18`
则说明作了一个偏移，这个偏移实际上就是取csd->flags地址, 可以通过crash命令查看下偏移量
```
crash> p ((struct __call_single_data *)0)->flags
Cannot access memory at address 0x18
								 ^
```
而之后，x2寄存器的值也没有变化，所以x2 - 0x18 就是csd变量的首地址。
x2寄存器的值为`ffff803ffffc3798`, x2-0x18为`ffff803ffffc3780`, 打印下
该地址中的信息
```
crash> struct __call_single_data ffff803ffffc3780
struct __call_single_data {
  llist = {
    next = 0xffff803ffffc3920
  },
  func = 0xffff000010190700 <do_nothing>,
  info = 0x0,
  flags = 3
}
```
这个也是正常信息，说明没有计算错误

我们使用crash随便查看一个全局的per-cpu变量,
这里我们查看的是`smp_call_function_many`使用的csd_data

```
crash> p cfd_data
struct call_function_data cfd_data;
PER-CPU ADDRESSES:
...
  [21]: ffff803ffff5ec80
  [22]: ffff803ffff8ec80
  [23]: ffff803ffffbec80
  [24]: ffff805fffb8ec80
...

```
可以看出是csd(ffff803ffffc3780)在23 - 24之间，
说明是CPU 23。

**PS:** 也可以通过汇编代码查找到cfd的地址，但是较麻烦。

### 分析该核堆栈

```
crash> bt -c 23
PID: 64593  TASK: ffff803dda21cb00  CPU: 23  COMMAND: "CPU 0/KVM"
bt: WARNING: cannot determine starting stack frame for task ffff803dda21cb00
```

发现并不能打印出该核堆栈，但是可以得到task_struct中的一些信息, 通过
`COMMAND: "CPU 0/KVM"`可以看出，该进程实际上是qemu-kvm进程中，负责
模拟vcpu的线程。

可以通过设置`echo 0 > /proc/sys/kernel/softlockup_panic`, 在产生softlock_up前后,
在`/proc/${QEMU-KVM PID}/tasks/${TPID}/stack`获取相关信息。

计划实施中也是比较困难，经常会遇到所有核心卡死的现象, 或者是在开启
function跟踪器后，获取不到堆栈（这个有待分析）。

在数次模拟后，获取到了如下堆栈

* 设置function跟踪器前，QEMU-KVM线程堆栈
```
[<0>] __switch_to+0x6c/0x90
[<0>] kvm_vcpu_block+0xb0/0x330
[<0>] kvm_handle_wfx+0x10c/0x1c0
[<0>] handle_exit+0x150/0x1b0
[<0>] kvm_arch_vcpu_ioctl_run+0x4d4/0x778
[<0>] kvm_vcpu_ioctl+0x3c8/0x868
[<0>] do_vfs_ioctl+0xc4/0x890
[<0>] ksys_ioctl+0x84/0xb8
[<0>] __arm64_sys_ioctl+0x28/0x38
[<0>] el0_svc_handler+0xa0/0x128
[<0>] el0_svc+0x8/0xc
[<0>] 0xffffffffffffffff
```

* 设置function跟踪器后，QEMU-KVM线程堆栈
```
[<0>] __switch_to+0x6c/0x90
[<0>] ftrace_call+0x4/0x4
[<0>] __hyp_call_panic_vhe+0x2c/0x68
[<0>] hyp_panic+0x84/0xb0
[<0>] ftrace_call+0x4/0x4
[<0>] ftrace_call+0x4/0x4
[<0>] sysreg_save_guest_state_vhe+0x18/0x40
[<0>] kvm_vcpu_run_vhe+0x94/0xd8
[<0>] kvm_arch_vcpu_ioctl_run+0x518/0x778
[<0>] kvm_vcpu_ioctl+0x3c8/0x868
[<0>] do_vfs_ioctl+0xc4/0x890
[<0>] ksys_ioctl+0x84/0xb8
[<0>] __arm64_sys_ioctl+0x28/0x38
[<0>] el0_svc_handler+0xa0/0x128
[<0>] el0_svc+0x8/0xc
```

### 根据获取到的堆栈分析相关代码

因为这里涉及了`ftrace_call`相关代码，所以简单描述下为什么会
调用`ftrace_call`

#### ftrace部分原理概述
这里面实际上涉及在设置ftrace function 跟踪器，ftrace会有一个类似
于kprobe的self modify code的过程，但是这个会结合gcc的profile特性，
会在函数开始的部分插入一段stub代码, ftrace会修改这部分代码来实现
trace功能。当然也可以让该函数不插入stub代码，对应`kernel`代码中的notrace
宏定义
```C/C++
#define notrace __attribute__((no_instrument_function))  
```

我们可以做个实验，步骤如下:
1. 设置nop 跟踪器<br />
`echo nop > /sys/kernel/debug/tracing/current_tracer`
2. 查看`printk`开始部分的代码
```
crash> x/20i printk
   0xffff0000101545e4 <printk>: stp     x29, x30, [sp,#-176]!
   0xffff0000101545e8 <printk+4>:       mov     x29, sp
   0xffff0000101545ec <printk+8>:       stp     x19, x20, [sp,#16]
   0xffff0000101545f0 <printk+12>:      mov     x20, x0
   0xffff0000101545f4 <printk+16>:      mov     x0, x30
   0xffff0000101545f8 <printk+20>:      stp     x1, x2, [sp,#120]
   0xffff0000101545fc <printk+24>:      adrp    x19, 0xffff000011453000 <page_wait_table+5312>
   0xffff000010154600 <printk+28>:      stp     x3, x4, [sp,#136]
   0xffff000010154604 <printk+32>:      add     x19, x19, #0x708
   0xffff000010154608 <printk+36>:      stp     x5, x6, [sp,#152]
   0xffff00001015460c <printk+40>:      str     x7, [sp,#168]
   0xffff000010154610 <printk+44>:      nop
```
可以看出`printk+44`实际上就是GCC profile特性插入的stub代码---- **nop**

3. 设置function跟踪器<br />
`echo function > /sys/kernel/debug/tracing/current_tracer`

4. 查看`printk`开始部分的代码
```
crash> x/20i printk
   0xffff0000101545e4 <printk>: stp     x29, x30, [sp,#-176]!
   0xffff0000101545e8 <printk+4>:       mov     x29, sp
   0xffff0000101545ec <printk+8>:       stp     x19, x20, [sp,#16]
   0xffff0000101545f0 <printk+12>:      mov     x20, x0
   0xffff0000101545f4 <printk+16>:      mov     x0, x30
   0xffff0000101545f8 <printk+20>:      stp     x1, x2, [sp,#120]
   0xffff0000101545fc <printk+24>:      adrp    x19, 0xffff000011453000 <page_wait_table+5312>
   0xffff000010154600 <printk+28>:      stp     x3, x4, [sp,#136]
   0xffff000010154604 <printk+32>:      add     x19, x19, #0x708
   0xffff000010154608 <printk+36>:      stp     x5, x6, [sp,#152]
   0xffff00001015460c <printk+40>:      str     x7, [sp,#168]
   0xffff000010154610 <printk+44>:      bl      0xffff0000100958fc <ftrace_caller>
```
5.  查看`sysreg_save_guest_state_vhe`开始部分代码
```
crash> x/20i sysreg_save_guest_state_vhe
   0xffff0000100d7ba0 <sysreg_save_guest_state_vhe>:    stp     x29, x30, [sp,#-32]!
   0xffff0000100d7ba4 <sysreg_save_guest_state_vhe+4>:  mov     x29, sp
   0xffff0000100d7ba8 <sysreg_save_guest_state_vhe+8>:  stp     x19, x20, [sp,#16]
   0xffff0000100d7bac <sysreg_save_guest_state_vhe+12>: mov     x19, x0
   0xffff0000100d7bb0 <sysreg_save_guest_state_vhe+16>: mov     x20, x1
   0xffff0000100d7bb4 <sysreg_save_guest_state_vhe+20>: mov     x0, x30
   0xffff0000100d7bb8 <sysreg_save_guest_state_vhe+24>: bl      0xffff0000100958fc <ftrace_caller>
```

这也就是`sysreg_save_guest_state_vhe`在C代码中未调用到`ftrace_caller`，
但是实际调用到的原因。

#### KVM相关代码

注: 如果想要该部分理清KVM相关代码，实际上需要了解ARM-v8 
arch和memory moudle, exp module和virt VHE相关内容。
在下面仅给出相关链接。


从代码堆栈上来看，用户态qemu-kvm通过ioctl(KVM_RUN), 进入内核态，内核通过
`kvm_arch_vcpu_ioctl_run`来运行guest。

```C/C++
int kvm_arch_vcpu_ioctl_run(struct kvm_vcpu *vcpu, struct kvm_run *run) 
{
...
	if (has_vhe()) {
	    kvm_arm_vhe_guest_enter();
	    ret = kvm_vcpu_run_vhe(vcpu);
	    kvm_arm_vhe_guest_exit();
	} else {
	    ret = kvm_call_hyp_ret(__kvm_vcpu_run_nvhe, vcpu);
	}
...
}
```

这里会通过HOST CPU是否支持VHE特性，走不同的代码流程。
我这里测试的CPU支持VHE特性。
所以进入了kvm_vcpu_run_vhe

```C/C++
int kvm_vcpu_run_vhe(struct kvm_vcpu *vcpu)                               
{                                                                         
    struct kvm_cpu_context *host_ctxt;                                    
    struct kvm_cpu_context *guest_ctxt;                                   
    u64 exit_code;                                                        
                                                                          
    host_ctxt = vcpu->arch.host_cpu_context; //保存host上下文
    host_ctxt->__hyp_running_vcpu = vcpu; 	             
    guest_ctxt = &vcpu->arch.ctxt;           //保存guest上下文                             
                                                                          
    sysreg_save_host_state_vhe(host_ctxt);	 //主要是讲当前的mdscr_el1和sp_el0保存到host_ctxt
                                                                          
    /*                                                                    
     * ARM erratum 1165522 requires us to configure both stage 1 and      
     * stage 2 translation for the guest context before we clear          
     * HCR_EL2.TGE.                                                       
     *                                                                    
     * We have already configured the guest's stage 1 translation in      
     * kvm_vcpu_load_sysregs above.  We must now call __activate_vm       
     * before __activate_traps, because __activate_vm configures          
     * stage 2 translation, and __activate_traps clear HCR_EL2.TGE        
     * (among other things).                                              
     */                                                                   
    __activate_vm(vcpu->kvm);
	
	/*
	 * 这里面比较重要的是设置了vbar_el1寄存器, 实际上是将中断向量表设置为
	 * __kvm_hyp_vector
	 * 还一个重要的寄存器是hcr_el2，host支持VHE 的hcr_el2，和guest hcr_el2
	 * 是不同的,  值分别为HCR_HOST_VHE_FLAGS, 和HCR_GUEST_FLAGS
	 */
    __activate_traps(vcpu);   				

	//加载guest mdscr_el1, sp_el0, spsr, 
	//以及设置guest异常返回地址elr
    sysreg_restore_guest_state_vhe(guest_ctxt);                           
    __debug_switch_to_guest(vcpu);                                        
                                                                          
    __set_guest_arch_workaround_state(vcpu);                              
                                                                          
    do {                                                                  
        /* Jump in the fire! */
		//进入guest
        exit_code = __guest_enter(vcpu, host_ctxt);                       
                                                                          
        /* And we're baaack! */                                           
    } while (fixup_guest_exit(vcpu, &exit_code));                         
                                                                          
    __set_host_arch_workaround_state(vcpu);                               

	//保存guest mdscr_el1, sp_el0, spsr寄存器
	//以及保存elr 为ctxt->gp_regs.regs.pc
    sysreg_save_guest_state_vhe(guest_ctxt);
	
	//保存hcr_el2， 设置host hcr_el2， 设置cpacr_el1, 恢复vbar_el1 为vectors(host的异常向量表)
    __deactivate_traps(vcpu);                                             
	
	//加载host mdscr_el1, sp_el0, spsr
    sysreg_restore_host_state_vhe(host_ctxt);                             
                                                                          
    if (vcpu->arch.flags & KVM_ARM64_FP_ENABLED)                          
        __fpsimd_save_fpexc32(vcpu);                                      

    __debug_switch_to_host(vcpu);

    return exit_code;
}                                                                         
```
实际上根据ARM-v8手册中, `D5.6.3 System and Special-purpose register redirection`
考虑到开启VHE兼容性，部分EL1结尾的寄存器，会redirect到相关el2寄存器，例如write_sysreg(vbar_el1),
实际上等同于write_sysreg(vbar_el2)

根据qemu-kvm的堆栈可以看出`sysreg_save_guest_state_vhe`在调用ftrace_call后，会调用到hyp_panic中，
通过查看代码，可以看出调用链`el2_sync->__hyp_panic->hyp_panic`

而el2_sync实际上是在为guest运行单独设置的异常向量表`__kvm_hyp_vector`中,
代码如下:
```C/C++
ENTRY(__kvm_hyp_vector)                                      
    invalid_vect    el2t_sync_invalid   // Synchronous EL2t  
    invalid_vect    el2t_irq_invalid    // IRQ EL2t          
    invalid_vect    el2t_fiq_invalid    // FIQ EL2t          
    invalid_vect    el2t_error_invalid  // Error EL2t        
                                                             
    valid_vect  el2_sync        // Synchronous EL2h          
    invalid_vect    el2h_irq_invalid    // IRQ EL2h          
    invalid_vect    el2h_fiq_invalid    // FIQ EL2h          
    valid_vect  el2_error       // Error EL2h                
                                                             
    valid_vect  el1_sync        // Synchronous 64-bit EL1    
    valid_vect  el1_irq         // IRQ 64-bit EL1            
    invalid_vect    el1_fiq_invalid     // FIQ 64-bit EL1    
    valid_vect  el1_error       // Error 64-bit EL1          
                                                             
    valid_vect  el1_sync        // Synchronous 32-bit EL1    
    valid_vect  el1_irq         // IRQ 32-bit EL1            
    invalid_vect    el1_fiq_invalid     // FIQ 32-bit EL1    
    valid_vect  el1_error       // Error 32-bit EL1          
ENDPROC(__kvm_hyp_vector)                                    
```

从这里可以看出，调用ftrace_call后陷入了异常，导致进入了el2_sync
其实，陷入el2_sync之前，硬件会将导致陷入异常的指令(PC)存入elr中，也就是说在
el2_sync函数中，获取到elr寄存器的值，就可以获取到陷入异常的指令。

因为crash文件获取不到堆栈，只能通过其他方法获取。
欲通过`printk`打印该寄存器值，会因为`printk`函数本身就会调用`ftrace_call`,
也会陷入异常。


通过修改代码，重新编译内核获取信息。

### 修改代码，调试内核

* 修改代码

```C/C++
# diff -Naru /tmp/switch.c arch/arm64/kvm/hyp/switch.c
--- /tmp/switch.c       2021-06-16 17:01:59.297927939 +0800
+++ arch/arm64/kvm/hyp/switch.c 2021-06-15 16:59:44.000000000 +0800

@@ -668,12 +672,24 @@
 }
 NOKPROBE_SYMBOL(__hyp_call_panic_vhe);

+u64 my_spsr =  0;
+u64 my_elr =  0;
+u64 my_par = 0;
+
+struct kvm_cpu_context my_kc_txt;
+
 void __hyp_text __noreturn hyp_panic(struct kvm_cpu_context *host_ctxt)
 {
        u64 spsr = read_sysreg_el2(spsr);
        u64 elr = read_sysreg_el2(elr);
        u64 par = read_sysreg(par_el1);
-
+
+    memcpy(&my_kc_txt, host_ctxt, sizeof(my_kc_txt));
+
+    my_spsr = spsr;
+    my_elr = elr;
+    my_par = par;
+
        if (!has_vhe())
                __hyp_call_panic_nvhe(spsr, elr, par, host_ctxt);
        else

``` 

* 运行该内核
* 设置softlockup_panic=1
* 设置ftrace function跟踪器
* 开启qemu-kvm 触发softlockup

实际上，上面的步骤就是为了触发panic获取vmcore，然后根据全局变量去获取elr寄存器的值

获取信息如下:

```C/C++
crash> p my_elr
my_elr = $1 = 18446462599003239456
crash> p my_spsr
my_spsr = $2 = 2151678921
crash> p my_par
my_par = $3 = 2089930256695544030
crash> x/10i 18446462599003239456
   0xffff0000101df420 <ftrace_ops_no_ops+16>:   ldr     w2, [x3,#16]
   0xffff0000101df424 <ftrace_ops_no_ops+20>:   ldr     x4, [x3,#3184]
   0xffff0000101df428 <ftrace_ops_no_ops+24>:   tst     w2, #0x1fff00
   0xffff0000101df42c <ftrace_ops_no_ops+28>:   b.eq    0xffff0000101df510 <ftrace_ops_no_ops+256>
   0xffff0000101df430 <ftrace_ops_no_ops+32>:   ldr     w2, [x3,#16]
   0xffff0000101df434 <ftrace_ops_no_ops+36>:   tbz     w2, #20, 0xffff0000101df4f0 <ftrace_ops_no_ops+224>
   0xffff0000101df438 <ftrace_ops_no_ops+40>:   mov     w2, #0x100                      // #256
   0xffff0000101df43c <ftrace_ops_no_ops+44>:   mov     w21, w2
   0xffff0000101df440 <ftrace_ops_no_ops+48>:   tst     w2, w4
   0xffff0000101df444 <ftrace_ops_no_ops+52>:   b.ne    0xffff0000101df4e4 <ftrace_ops_no_ops+212>
crash>
crash>
crash> x/10i ftrace_ops_no_ops
   0xffff0000101df410 <ftrace_ops_no_ops>:      stp     x29, x30, [sp,#-64]!
   0xffff0000101df414 <ftrace_ops_no_ops+4>:    mrs     x3, sp_el0
   0xffff0000101df418 <ftrace_ops_no_ops+8>:    mov     x29, sp
   0xffff0000101df41c <ftrace_ops_no_ops+12>:   stp     x21, x22, [sp,#32]
   0xffff0000101df420 <ftrace_ops_no_ops+16>:   ldr     w2, [x3,#16] 		# 在这里出了问题
   0xffff0000101df424 <ftrace_ops_no_ops+20>:   ldr     x4, [x3,#3184]
   0xffff0000101df428 <ftrace_ops_no_ops+24>:   tst     w2, #0x1fff00
   0xffff0000101df42c <ftrace_ops_no_ops+28>:   b.eq    0xffff0000101df510 <ftrace_ops_no_ops+256>
   0xffff0000101df430 <ftrace_ops_no_ops+32>:   ldr     w2, [x3,#16]
   0xffff0000101df434 <ftrace_ops_no_ops+36>:   tbz     w2, #20, 0xffff0000101df4f0 <ftrace_ops_no_ops+224>
```

出问题的代码流程大概为，将sp_el0寄存器存放在x3, 然后访问[x3, #16] 内存，将内存中的值，存放在w2寄存器。
我们来分析下:

在guest vm exit后,  sp_el0存放的是guest中的sp_el0值，而实际上是在`sysreg_restore_host_state_vhe`
中切换成host sp_el0, 而出问题的函数`sysreg_save_guest_state_vhe`是在`sysreg_restore_host_state_vhe`
之前调用的，这个时候相当于host访问的guest的内存，所以会触发异常。

# 查看5.x内核代码

查看5.x内核代码，已经将该问题修改（修改方法是，在`__guest_exit`和`__guest_enter`中
load store sp_el0寄存器)。

GIT COMMIT: `6e977984f6d8e5689e079de1fd2e337cd17dcca5  KVM: arm64: Save/restore sp_el0 as part of __guest_enter`


[patch 文件](./0001-KVM-arm64-Save-restore-sp_el0-as-part-of-__guest_ent.patch)



# ARM-v8 feature

## VHE
https://developer.arm.com/documentation/102142/latest/Virtualization-Host-Extensions
