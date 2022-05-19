# kernel_patch分析
## patch
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=0e1645557d19fc6d88d3c40431f63a3c3a4c417b

```
arm64: smp: Increase secondary CPU boot timeout value
arm64: smp: Increase secondary CPU boot timeout value
When many debug options are enabled simultaneously (e.g. PROVE_LOCKING,
KMEMLEAK, DEBUG_PAGE_ALLOC, KASAN etc), it is possible for us to timeout
when attempting to boot a secondary CPU and give up. Unfortunately, the
CPU will /eventually/ appear, and sit in the background happily stuck
in a recursive exception due to a NULL stack pointer.

Increase the timeout to 5s, which will of course be enough for anybody.

Reviewed-by: Mark Rutland <mark.rutland@arm.com>
Signed-off-by: Will Deacon <will@kernel.org>

diff --git a/arch/arm64/kernel/smp.c b/arch/arm64/kernel/smp.c
index 018a33e01b0ed..63c7a7682e93e 100644
--- a/arch/arm64/kernel/smp.c
+++ b/arch/arm64/kernel/smp.c
@@ -123,7 +123,7 @@ int __cpu_up(unsigned int cpu, struct task_struct *idle)
 		 * time out.
 		 */
 		wait_for_completion_timeout(&cpu_running,
-					    msecs_to_jiffies(1000));
+					    msecs_to_jiffies(5000));
 
 		if (!cpu_online(cpu)) {
 			pr_crit("CPU%u: failed to come online\n", cpu);
```
patch中的解释是说，当开启某些debug选项的时候，启动会变慢，就会造成等待
secondary 超时，所以增加了timeout到5s


## 分析相关kernel代码
### cpu_up()
首先看下`__cpu_up()`代码:
```
int __cpu_up(unsigned int cpu, struct task_struct *idle)
{
    int ret;
    long status;

    /*
    ¦* We need to tell the secondary core where to find its stack and the
    ¦* page tables.
    ¦*/
    secondary_data.task = idle;
    secondary_data.stack = task_stack_page(idle) + THREAD_SIZE;
    update_cpu_boot_status(CPU_MMU_OFF);
    __flush_dcache_area(&secondary_data, sizeof(secondary_data));

    /*
    ¦* Now bring the CPU into our world.
    ¦*/
    ret = boot_secondary(cpu, idle);
    if (ret == 0) {
        /*
        ¦* CPU was successfully started, wait for it to come online or
        ¦* time out.
        ¦*/
        wait_for_completion_timeout(&cpu_running,
                    ¦   msecs_to_jiffies(1000));
	...
```
根据`__cpu_up()`的代码来看，boot cpu 会调用`boot_secondary()`启动从cpu.
而`boot_secondary()`执行完后，会执行一个wait操作,该wait的操作是在wait 
completion work `cpu_running`而`cpu_running`completion work 
会在`secondary_start_kernel()`比较靠后的流程中complete 该work.

```
asmlinkage notrace void secondary_start_kernel(void)
{
    ...
    complete(&cpu_running);

    local_daif_restore(DAIF_PROCCTX);

    /*
    ¦* OK, it's off to the idle thread for us
    ¦*/
    cpu_startup_entry(CPUHP_AP_ONLINE_IDLE);
}
```

### boot_secondary()

我们接着看下`boot_secondary()`相关内容:
```
static int boot_secondary(unsigned int cpu, struct task_struct *idle)
{
    if (cpu_ops[cpu]->cpu_boot)
        return cpu_ops[cpu]->cpu_boot(cpu);

    return -EOPNOTSUPP;
}
```

其中调用`cpu_ops[cpu]->cpu_boot`, 从dmesg打印来看，该cpu启动方式是`psci`
所以对应的ops为:

```
const struct cpu_operations cpu_psci_ops = {
    .name       = "psci",
#ifdef CONFIG_CPU_IDLE
    .cpu_init_idle  = psci_cpu_init_idle,
    .cpu_suspend    = psci_cpu_suspend_enter,
#endif
    .cpu_init   = cpu_psci_cpu_init,
    .cpu_prepare    = cpu_psci_cpu_prepare,
    .cpu_boot   = cpu_psci_cpu_boot,
#ifdef CONFIG_HOTPLUG_CPU
    .cpu_disable    = cpu_psci_cpu_disable,
    .cpu_die    = cpu_psci_cpu_die,
    .cpu_kill   = cpu_psci_cpu_kill,
#endif
};
```
`cpu_boot`成员为`cpu_psci_cpu_boot`, 查看该代码:

```
static int cpu_psci_cpu_boot(unsigned int cpu)
{
    int err = psci_ops.cpu_on(cpu_logical_map(cpu), __pa_symbol(secondary_entry));
    if (err)
        pr_err("failed to boot CPU%d (%d)\n", cpu, err);

    return err;
}
```
该函数调用了`psci_ops.cpu_on()`，这里面要注意第二个参数，传入的是`secondary_entry`
符号的物理地址，这个地址是从cpu执行的首地址

这里的cpu_on会在系统初始化的时候赋值，具体流程不展开，函数为`psci_cpu_on()`

```
static int psci_cpu_on(unsigned long cpuid, unsigned long entry_point)
{
    int err;
    u32 fn;

    fn = psci_function_id[PSCI_FN_CPU_ON];
    err = invoke_psci_fn(fn, cpuid, entry_point, 0);
    return psci_to_linux_errno(err);
}
```
其中会调用`invoke_psci_fn()`, 该值会根据acpi进行赋值：
```
static void set_conduit(enum psci_conduit conduit)
{
    switch (conduit) {
    case PSCI_CONDUIT_HVC:
        invoke_psci_fn = __invoke_psci_fn_hvc;
        break;
    case PSCI_CONDUIT_SMC:
        invoke_psci_fn = __invoke_psci_fn_smc;
        break;
    default:
        WARN(1, "Unexpected PSCI conduit %d\n", conduit);
    }

    psci_ops.conduit = conduit;
}
```
这里我们直接查看虚机内存，
```
invoke_psci_fn = $6 = (psci_fn *) 0xffff00001075e4a0 <__invoke_psci_fn_hvc>
```
可以看出，这里使用的是hvc

ps: 通过查看host内存，发现在物理结点上，该值使用的是smc。

如果guest执行hvc指令, 会陷入到el2也就是host kvm, 如果host执行smc指令，则会
陷入到el3，这时估计会陷入到acpi的固件代码.

查看该函数
```
static unsigned long __invoke_psci_fn_hvc(unsigned long function_id,
            unsigned long arg0, unsigned long arg1,
            unsigned long arg2)
{
    struct arm_smccc_res res;

    arm_smccc_hvc(function_id, arg0, arg1, arg2, 0, 0, 0, 0, &res);
    return res.a0;
}
#define arm_smccc_hvc(...) __arm_smccc_hvc(__VA_ARGS__, NULL)

ENTRY(__arm_smccc_hvc)
    SMCCC SMCCC_HVC
ENDPROC(__arm_smccc_hvc)

    .macro SMCCC instr
    .cfi_startproc
    \instr  #0
    /* sp 实际上是struct arm_smccc_res */
    ldr x4, [sp]
    /* 存储x0, x1, x2, x3 ->struct arm_smccc_res */
    stp x0, x1, [x4, #ARM_SMCCC_RES_X0_OFFS]
    stp x2, x3, [x4, #ARM_SMCCC_RES_X2_OFFS]
    ldr x4, [sp, #8]
    cbz x4, 1f /* no quirk structure */
    ldr x9, [x4, #ARM_SMCCC_QUIRK_ID_OFFS]
    cmp x9, #ARM_SMCCC_QUIRK_QCOM_A6
    b.ne    1f
    str x6, [x4, ARM_SMCCC_QUIRK_STATE_OFFS]
1:  ret
    .cfi_endproc
    .endm
```
该函数会调用`arm_smccc_hvc()`函数，并且将res作为出参传入，
之后取res.a0作为返回值返回.
我们直接来看`SMCCC`宏定义,首先执行`instr`指令, 陷入更高的异常级别，
从el2返回后，将`x0,x1`存储到[sp], 然后将`x2,x3`存储到[sp , #ARM_SMCCC_RES_X2_OFFS]
可以看出，这里是在存储el2某些返回信息，这些信息在el2存放在了`x0~x3`寄存器
中，而在el2返回后，SMCCC宏代码，将这些信息转储到了堆栈中，这个堆栈地址
代表什么呢。
我们来看下相关反汇编代码:

```
crash> dis __arm_smccc_hvc
0xffff00001009528c <__arm_smccc_hvc>:   hvc     #0x0
0xffff000010095290 <__arm_smccc_hvc+4>: ldr     x4, [sp]
0xffff000010095294 <__arm_smccc_hvc+8>: stp     x0, x1, [x4]
0xffff000010095298 <__arm_smccc_hvc+12>:        stp     x2, x3, [x4,#16]

//0xffff00001076167c <__invoke_psci_fn_hvc+52>:   stp     x0, xzr, [sp], 这个是调用函数存储sp+8的点, 这个地方是不是返回之

0xffff00001009529c <__arm_smccc_hvc+16>:        ldr     x4, [sp,#8]
0xffff0000100952a0 <__arm_smccc_hvc+20>:        cbz     x4, 0xffff0000100952b4 <__arm_smccc_hvc+40>
0xffff0000100952a4 <__arm_smccc_hvc+24>:        ldr     x9, [x4]
0xffff0000100952a8 <__arm_smccc_hvc+28>:        cmp     x9, #0x1
0xffff0000100952ac <__arm_smccc_hvc+32>:        b.ne    0xffff0000100952b4 <__arm_smccc_hvc+40>
0xffff0000100952b0 <__arm_smccc_hvc+36>:        str     x6, [x4,#8]
0xffff0000100952b4 <__arm_smccc_hvc+40>:        ret
```
该符号的花，实际上并没有开辟新的堆栈，所以sp的值还是他的调用者代码，
为: `__invoke_psci_fn_hvc`

```
 dis __invoke_psci_fn_hvc
//构造栈
0xffff000010761648 <__invoke_psci_fn_hvc>:      sub     sp, sp, #0x80
0xffff00001076164c <__invoke_psci_fn_hvc+4>:    stp     x29, x30, [sp,#16]
0xffff000010761650 <__invoke_psci_fn_hvc+8>:    add     x29, sp, #0x10
0xffff000010761654 <__invoke_psci_fn_hvc+12>:   stp     x19, x20, [sp,#32]
0xffff000010761658 <__invoke_psci_fn_hvc+16>:   stp     x21, x22, [sp,#48]
0xffff00001076165c <__invoke_psci_fn_hvc+20>:   str     x23, [sp,#64]	//0x44
0xffff000010761660 <__invoke_psci_fn_hvc+24>:   mov     x20, x0
0xffff000010761664 <__invoke_psci_fn_hvc+28>:   mov     x0, x30
0xffff000010761668 <__invoke_psci_fn_hvc+32>:   mov     x21, x1
0xffff00001076166c <__invoke_psci_fn_hvc+36>:   mov     x22, x2
0xffff000010761670 <__invoke_psci_fn_hvc+40>:   mov     x23, x3
0xffff000010761674 <__invoke_psci_fn_hvc+44>:   nop

//这个地方存储的是struct arm_smccc_res的首地址
0xffff000010761678 <__invoke_psci_fn_hvc+48>:   add     x0, sp, #0x58
//将x0和0压到了栈顶
0xffff00001076167c <__invoke_psci_fn_hvc+52>:   stp     x0, xzr, [sp]
...
```
我们只看前半部分代码: 
在开始部分，将临时变量压栈，压栈后，在`0xffff000010761678`, 将`sp + 0x58 = x0`
并且将`x0 xzr`存放在了`[sp]`, 所以sp地址处存储的值为`sp + 0x58`的地址，这里实际上
代表的是`__invoke_psci_fn_hvc()`中的局部变量`struct arm_smccc_res res;`

也就是说，arm_smccc_hvc在cpu_on流程中的参数解释如下:

```
1. function_id      CPU_ON function
2. cpu_id
3. entry_point      secondary_entry     启动首地址
4. 0
5. 0
6. 0
7. 0
8. 0
9. struct arm_smccc_res                 作为出参传出el2返回的信息
```
回到`psci_cpu_on`, 在调用完`invoke_psci_fn()`后，会调用`psci_to_linux_errno()`
将err转换为errno, 来看下相关代码:

```
static int psci_to_linux_errno(int errno)
{
    switch (errno) {
    case PSCI_RET_SUCCESS:
        return 0;
    case PSCI_RET_NOT_SUPPORTED:
        return -EOPNOTSUPP;
    case PSCI_RET_INVALID_PARAMS:
    case PSCI_RET_INVALID_ADDRESS:
        return -EINVAL;
    case PSCI_RET_DENIED:
        return -EPERM;
    };

    return -EINVAL;
}
```
该case中，有两部分错误打印，首先在启动处，会遇到

```
[    5.327694] CPU27: failed to come online
[    5.327706] CPU27: failed in unknown state : 0x0
```
这样的错误打印，但是该错误是`__cpu_up()`打印的，而psci流程并没有任何打印，
在之后，又会遇到
```
psci: failed to boot CPU28(-22)
CPU27: failed to boot: -22
```
这样的打印，后面我们会接着分析.

那么总结来说，实际上cpu_on的底层操作就是调用了hvc指令，将从cpu_id和entry_point传递给了el2
上面提到过，对于虚机来说，hvc会陷入el2到kvm层，在来看下kvm层的相关代码.

这里我们越过一些中间流程，最终会调用到kvm_psci_vcpu_on()该函数中

```
static unsigned long kvm_psci_vcpu_on(struct kvm_vcpu *source_vcpu)
{
    struct vcpu_reset_state *reset_state;
    struct kvm *kvm = source_vcpu->kvm;
    struct kvm_vcpu *vcpu = NULL;
    unsigned long cpu_id;

    cpu_id = smccc_get_arg1(source_vcpu) & MPIDR_HWID_BITMASK;
    if (vcpu_mode_is_32bit(source_vcpu))
        cpu_id &= ~((u32) 0);

    vcpu = kvm_mpidr_to_vcpu(kvm, cpu_id);

    /*
    ¦* Make sure the caller requested a valid CPU and that the CPU is
    ¦* turned off.
    ¦*/
	//===================(1)=====
    if (!vcpu)
        return PSCI_RET_INVALID_PARAMS;
    if (!vcpu->arch.power_off) {
        if (kvm_psci_version(source_vcpu, kvm) != KVM_ARM_PSCI_0_1)
            return PSCI_RET_ALREADY_ON;
        else
            return PSCI_RET_INVALID_PARAMS;
    }
    reset_state = &vcpu->arch.reset_state;
    reset_state->pc = smccc_get_arg2(source_vcpu);

	...

	kvm_make_request(KVM_REQ_VCPU_RESET, vcpu);
	smp_wmb();

	vcpu->arch.power_off = false;
	kvm_vcpu_wake_up(vcpu);
	return PSCI_RET_SUCCESS;
}
```
从上面的流程来看, 在(1)处会判断vcpu状态，如果vcpu没有或者vcpu->arch.power_off已经不再
为真，则返回PSCI_RET_INVAILID_PARAMS, 而这个就是dmesg遇到的错误, 在之后会将pc设置为
source_vcpu的x2, 也就是entry_point然后make_request KVM_REQ_VCPU_RESET, 将vcpu->arch.power_off
掷为false, 并且唤醒该vcpu。

而该函数返回后，怎么将相关信息传回到guest中(el1)，我们找其中一个调用分支, 在`kvm_psci_0_1_call()`
末尾会调用`smccc_set_retval()`函数, 在该函数中进行guest vcpu寄存器设置，通过寄存器将返回信息传出。

```
static int kvm_psci_0_1_call(struct kvm_vcpu *vcpu)
{
    struct kvm *kvm = vcpu->kvm;
    u32 psci_fn = smccc_get_function(vcpu);
    unsigned long val;

    switch (psci_fn) {
	...
    case KVM_PSCI_FN_CPU_ON:
        mutex_lock(&kvm->lock);
        val = kvm_psci_vcpu_on(vcpu);
        mutex_unlock(&kvm->lock);
        break;
	...
    }
    smccc_set_retval(vcpu, val, 0, 0, 0);
    return 1;
}

static void smccc_set_retval(struct kvm_vcpu *vcpu,
            ¦   ¦unsigned long a0,
            ¦   ¦unsigned long a1,
            ¦   ¦unsigned long a2,
            ¦   ¦unsigned long a3)
{
    vcpu_set_reg(vcpu, 0, a0);
    vcpu_set_reg(vcpu, 1, a1);
    vcpu_set_reg(vcpu, 2, a2);
    vcpu_set_reg(vcpu, 3, a3);
}
```
可以看出，将`kvm_psci_vcpu_on()`的返回值存储在`vcpu->a0`中, 
从dmesg打印来看, 首次调用时，psci并未有错误信息打印，
而之后会有`psci boot: -22`相关打印. 
说明，第一次调用成功了，host将该vcpu标记poweroff成员为假，
但是从cpu启动过慢，导致主cpu等待超时，这时，如果在触发一次
cpu_on，host kvm 判断该cpu已经不是offline的状态, 就会返回
PSCI_RET_INVAILID_PARAMS, 而guest中会报`psci boot: -22`相关错误。

所以，这里可以推测，客户遇到的问题，和之前case中遇到的问题原因基本一致，
都是因为从cpu启动过慢导致主CPU等待超时.


# EAS-66611

## 问题现象
在ft2500扩容云产品节点对应的云主机中vnc console和dmesg中会有如下报错:

```
[    0.020629] smp: Bringing up secondary CPUs ...
...
[    8.124952] CPU3: failed to come online
[    8.124997] CPU3: failed in unknown state : 0x0
[   14.097343] CPU4: failed to come online
[   14.097356] CPU4: failed in unknown state : 0x0

...
[   61.030924] psci: failed to boot CPU3 (-22)
[   61.032017] CPU3: failed to boot: -22
[   61.032829] CPU3: failed in unknown state : 0x0
[   61.041995] psci: failed to boot CPU4 (-22)
[   61.043151] CPU4: failed to boot: -22
...

```
在环境https://192.168.4.199/上做了大量对比测试。有使用麒麟OS、CentOS 8.1、es kernel 10
（指的是4.18.0-147.5.1.el8_1.5es.10.aarch64，下面提到的es kernel 11、15等是同样的）、
es kernel 11、es kernel 14、es kernel 15。node-1节点未打上NUMA相关patch，es kernel 11、
es kernel 14、es kernel 15会复现出该case的情况（虚拟机无法启动）以及必现vcpu offline的问题。
es kernel 11极大概率复现该case描述的问题。es kernel 10以及CentOS 8.1、麒麟OS无任何问题。

node-3节点应用了NUMA相关patch，es kernel 11、es kernel 14、es kernel 15小概率复现该case描述
问题（虚拟机未启动），概率性复现vcpu offline的情况。es kernel 11出现vcpu offline的情况较多。
es kernel 10以及CentOS 8.1、麒麟OS无任何问题。

通过查看kernel的代码提交记录，发现es kernel 11及以后的版本应用了飞腾S2500相关patch。目前定位
是该patch导致的问题。

虚拟机中未使用到FTS2500 patch中关于IRQ的代码，但是有使用到memblock相关代码，尝试去除FTS2500 
patch中关于memblock的代码修改，在es kernel 10上应用并重新编译kernel rpm，在虚拟机中安装新的kernel，
并进行重启测试。未复现出cpu offline和虚拟机启动失败的情况。
