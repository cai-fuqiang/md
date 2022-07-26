# apic init
stack
```
kvm_vm_ioctl
	kvm_vm_ioctl_create_vcpu
		1. kvm_arch_vcpu_create
			kvm_x86_ops->vcpu_create : vmx_create_vcpu
				vmx_vcpu_load
				vmx_vcpu_setup
				kvm_vcpu_init
					kvm_arch_vcpu_init
						kvm_create_lapic
		2. kvm_arch_vcpu_setup
			kvm_vcpu_reset
				kvm_lapic_reset
			kvm_x86_ops->vcpu_reset(vcpu, init_event); : vmx_vcpu_reset
```
## 1 分支
先看1分支

### vmx_vcpu_setup

在`vmx_vcpu_setup`中会初始化 `posted interrupt descripter`
```cpp
static void vmx_vcpu_setup(struct vcpu_vmx *vmx)  
{
	...
	if (kvm_vcpu_apicv_active(&vmx->vcpu)) {                        
		vmcs_write64(EOI_EXIT_BITMAP0, 0);                          
    	vmcs_write64(EOI_EXIT_BITMAP1, 0);                          
    	vmcs_write64(EOI_EXIT_BITMAP2, 0);                          
    	vmcs_write64(EOI_EXIT_BITMAP3, 0);

    	vmcs_write16(GUEST_INTR_STATUS, 0);
		//==============(1)======================
    	vmcs_write16(POSTED_INTR_NV, POSTED_INTR_VECTOR);           
    	vmcs_write64(POSTED_INTR_DESC_ADDR, __pa((&vmx->pi_desc))); 
	}
	...
}
```

在`posted-interrupt notification vector`字段写入`POSTED_INTR_NV`向量, 
在`posted-interrupt descripter address`处写入`vmx->pi_desc`的物理地址。

### kvm_arch_vcpu_init
在 `kvm_arch_vcpu_init`中, 会执行
```cpp
int kvm_arch_vcpu_init(struct kvm_vcpu *vcpu)
{
	...
	if (irqchip_in_kernel(vcpu->kvm)) {
		//赋值
	    vcpu->arch.apicv_active = kvm_x86_ops->get_enable_apicv(vcpu);
	    r = kvm_create_lapic(vcpu, lapic_timer_advance_ns);
	    if (r < 0)
	        goto fail_mmu_destroy;
	} else
	    static_key_slow_inc(&kvm_no_apic_vcpu);
	...
}
```

### apicv_active
* kvm_x86_ops->get_enable_apicv()
```cpp
static bool vmx_get_enable_apicv(struct kvm_vcpu *vcpu)
{
    return enable_apicv;
}
```

可见`enable_apicv`代表当前host支不支持 virtual apic, 关于
`enable_apicv` 细节部分，请见附录。

### kvm_create_lapic

```cpp
int kvm_create_lapic(struct kvm_vcpu *vcpu, int timer_advance_ns)
{
    struct kvm_lapic *apic;

    ASSERT(vcpu != NULL);
    apic_debug("apic_init %d\n", vcpu->vcpu_id);

    apic = kzalloc(sizeof(*apic), GFP_KERNEL_ACCOUNT);
    if (!apic)
        goto nomem;

    vcpu->arch.apic = apic;
	//====================(1)=====================
    apic->regs = (void *)get_zeroed_page(GFP_KERNEL_ACCOUNT);
    if (!apic->regs) {
        printk(KERN_ERR "malloc apic regs error for vcpu %x\n",
        ¦   ¦  vcpu->vcpu_id);
        goto nomem_free_apic;
    }
    apic->vcpu = vcpu;

    hrtimer_init(&apic->lapic_timer.timer, CLOCK_MONOTONIC,
        ¦   ¦HRTIMER_MODE_ABS_PINNED);
    apic->lapic_timer.timer.function = apic_timer_fn;
    if (timer_advance_ns == -1) {
        apic->lapic_timer.timer_advance_ns = 1000;
        apic->lapic_timer.timer_advance_adjust_done = false;
    } else {
        apic->lapic_timer.timer_advance_ns = timer_advance_ns;
        apic->lapic_timer.timer_advance_adjust_done = true;
    }


    /*
    ¦* APIC is created enabled. This will prevent kvm_lapic_set_base from
    ¦* thinking that APIC satet has changed.
    ¦*/
    vcpu->arch.apic_base = MSR_IA32_APICBASE_ENABLE;
    static_key_slow_inc(&apic_sw_disabled.key); /* sw disabled at reset */
    kvm_iodevice_init(&apic->dev, &apic_mmio_ops);

    return 0;
nomem_free_apic:
    kfree(apic);
nomem:
    return -ENOMEM;
}
```
> 1. 申请了一个新页，初始化为0, 并将它赋给vcpu->arch.apic->regs

## 2分支
上面可以看到, 在`kvm_create_lapic`时，并未初始化, virtual apic register,
这些register更新实际上在`kvm_lapic_reset`中去做, 该函数定义如下:
```cpp
void kvm_lapic_reset(struct kvm_vcpu *vcpu, bool init_event)
```
其中第二个参数`init_event`表示是否是init event导致的 vcpu reset。
其调用函数为`kvm_vcpu_reset`, 该函数会在两个地方被调用

* kvm_apic_accept_events
```cpp
void kvm_apic_accept_events(struct kvm_vcpu *vcpu)
{
	...
	if (test_bit(KVM_APIC_INIT, &pe)) {
	    kvm_vcpu_reset(vcpu, true);
	    if (kvm_vcpu_is_bsp(apic->vcpu))
	        vcpu->arch.mp_state = KVM_MP_STATE_RUNNABLE;      
	    else
	        vcpu->arch.mp_state = KVM_MP_STATE_INIT_RECEIVED; 
	}
	...
}
```
这个是发生在处理apic 的事件，如果是INIT 事件的华，则调用。
* kvm_arch_vcpu_setup(此处)
```cpp
int kvm_arch_vcpu_setup(struct kvm_vcpu *vcpu)
{
	vcpu->arch.arch_capabilities = kvm_get_arch_capabilities();
	vcpu->arch.msr_platform_info = MSR_PLATFORM_INFO_CPUID_FAULT;
	kvm_vcpu_mtrr_init(vcpu);
	vcpu_load(vcpu);
	kvm_vcpu_reset(vcpu, false);
	kvm_init_mmu(vcpu, false);
	vcpu_put(vcpu);
	return 0;
}
```
本分支为`init_event`false, 该参数会有什么影响呢，请看`kvm_lapic_reset`

### kvm_lapic_reset

我们先看开头一小部分
```cpp
void kvm_lapic_reset(struct kvm_vcpu *vcpu, bool init_event)
{
    struct kvm_lapic *apic = vcpu->arch.apic;
    int i;

    if (!apic)
        return;

    apic_debug("%s\n", __func__);

    /* Stop the timer in case it's a reset to an active apic */
	//==============(1)=======================
    hrtimer_cancel(&apic->lapic_timer.timer);

    if (!init_event) {
		//=============(2)=======================
        kvm_lapic_set_base(vcpu, APIC_DEFAULT_PHYS_BASE |
        						MSR_IA32_APICBASE_ENABLE);
        kvm_apic_set_xapic_id(apic, vcpu->vcpu_id);
    }
    kvm_apic_set_version(apic->vcpu);
	...
}
```
1. 这里考虑到了从active apic reset进入该分支，所以要stop 掉timer
2. 如果不是init event进入的说明为kvm 创建初始化vcpu进入该分支，
则需要设置下 base 和 xapic id。
* base指的是**APIC-access address**, 见Intel-sdm 29.4
```cpp
void kvm_lapic_set_base(struct kvm_vcpu *vcpu, u64 value)
{
    u64 old_value = vcpu->arch.apic_base;
    struct kvm_lapic *apic = vcpu->arch.apic;

    if (!apic)
        value |= MSR_IA32_APICBASE_BSP;

    vcpu->arch.apic_base = value;

    if ((old_value ^ value) & MSR_IA32_APICBASE_ENABLE)
        kvm_update_cpuid(vcpu);

    if (!apic)
        return;

    /* update jump label if enable bit changes */
    if ((old_value ^ value) & MSR_IA32_APICBASE_ENABLE) {
        if (value & MSR_IA32_APICBASE_ENABLE) {
            kvm_apic_set_xapic_id(apic, vcpu->vcpu_id);
            static_key_slow_dec_deferred(&apic_hw_disabled);
        } else {
            static_key_slow_inc(&apic_hw_disabled.key);
            recalculate_apic_map(vcpu->kvm);
        }
    }

    if (((old_value ^ value) & X2APIC_ENABLE) && (value & X2APIC_ENABLE))
        kvm_apic_set_x2apic_id(apic, vcpu->vcpu_id);

    if ((old_value ^ value) & (MSR_IA32_APICBASE_ENABLE | X2APIC_ENABLE))
        kvm_x86_ops->set_virtual_apic_mode(vcpu);

    apic->base_address = apic->vcpu->arch.apic_base &
            	MSR_IA32_APICBASE_BASE;

    if ((value & MSR_IA32_APICBASE_ENABLE) &&
        apic->base_address != APIC_DEFAULT_PHYS_BASE)
        pr_warn_once("APIC base relocation is unsupported by KVM");

    /* with FSB delivery interrupt, we can restart APIC functionality */
    apic_debug("apic base msr is 0x%016" PRIx64 ", and base address is "
           "0x%lx.\n", apic->vcpu->arch.apic_base, apic->base_address);

}
```
* id指的是是设置virtual apic的 reg (APIC_ID)
```cpp
static inline void kvm_apic_set_xapic_id(struct kvm_lapic *apic, u8 id)
{
    kvm_lapic_set_reg(apic, APIC_ID, id << 24);
    recalculate_apic_map(apic->vcpu->kvm);
}
```

接下来的部分
```cpp
void kvm_lapic_reset(struct kvm_vcpu *vcpu, bool init_event)
{
	...
	//===================(1)============================
	for (i = 0; i < KVM_APIC_LVT_NUM; i++)
    	kvm_lapic_set_reg(apic, APIC_LVTT + 0x10 * i, APIC_LVT_MASKED);
	//===================(2)============================
	apic_update_lvtt(apic);
	if (kvm_vcpu_is_reset_bsp(vcpu) &&
	¦   kvm_check_has_quirk(vcpu->kvm, KVM_X86_QUIRK_LINT0_REENABLED))
	    kvm_lapic_set_reg(apic, APIC_LVT0,
	        ¦   ¦SET_APIC_DELIVERY_MODE(0, APIC_MODE_EXTINT));
	apic_manage_nmi_watchdog(apic, kvm_lapic_get_reg(apic, APIC_LVT0));
	
	kvm_lapic_set_reg(apic, APIC_DFR, 0xffffffffU);
	apic_set_spiv(apic, 0xff);
	kvm_lapic_set_reg(apic, APIC_TASKPRI, 0);
	if (!apic_x2apic_mode(apic))
	    kvm_apic_set_ldr(apic, 0);
	kvm_lapic_set_reg(apic, APIC_ESR, 0);
	kvm_lapic_set_reg(apic, APIC_ICR, 0);
	kvm_lapic_set_reg(apic, APIC_ICR2, 0);
	kvm_lapic_set_reg(apic, APIC_TDCR, 0);
	kvm_lapic_set_reg(apic, APIC_TMICT, 0);
	for (i = 0; i < 8; i++) {
	    kvm_lapic_set_reg(apic, APIC_IRR + 0x10 * i, 0);
	    kvm_lapic_set_reg(apic, APIC_ISR + 0x10 * i, 0);
	    kvm_lapic_set_reg(apic, APIC_TMR + 0x10 * i, 0);
	}
	...
}
```
1. set mask -> 1 to inhibit reception of interrupt
2. 更新local timer

最后部分

```cpp
void kvm_lapic_reset(struct kvm_vcpu *vcpu, bool init_event)
{
	apic->irr_pending = vcpu->arch.apicv_active;
	apic->isr_count = vcpu->arch.apicv_active ? 1 : 0;
	apic->highest_isr_cache = -1;
	update_divide_count(apic);
	atomic_set(&apic->lapic_timer.pending, 0);
	if (kvm_vcpu_is_bsp(vcpu))
	    kvm_lapic_set_base(vcpu,
	            vcpu->arch.apic_base | MSR_IA32_APICBASE_BSP);
	vcpu->arch.pv_eoi.msr_val = 0;
	apic_update_ppr(apic);
	if (vcpu->arch.apicv_active) {
		//======================(1)=========================
	    kvm_x86_ops->apicv_post_state_restore(vcpu);
		//======================(2)=========================
	    kvm_x86_ops->hwapic_irr_update(vcpu, -1);
	    kvm_x86_ops->hwapic_isr_update(vcpu, -1);
	}
	
	vcpu->arch.apic_arb_prio = 0;
	vcpu->arch.apic_attention = 0;
	
	apic_debug("%s: vcpu=%p, id=0x%x, base_msr="
	    	"0x%016" PRIx64 ", base_address=0x%0lx.\n", __func__,
	    	vcpu, kvm_lapic_get_reg(apic, APIC_ID),
	    	vcpu->arch.apic_base, apic->base_address);
}
```
1. 重置 post interrupt descripter
```cpp
static void vmx_apicv_post_state_restore(struct kvm_vcpu *vcpu)
{                                                              
    struct vcpu_vmx *vmx = to_vmx(vcpu);                       
	//clear on
    pi_clear_on(&vmx->pi_desc);
    memset(vmx->pi_desc.pir, 0, sizeof(vmx->pi_desc.pir));
}
```
2. 更新RVI, SVI/Guest Interrupt Status (这个竟然不是只读的)

### vmx_vcpu_reset
```cpp
static void vmx_vcpu_reset(struct kvm_vcpu *vcpu, bool init_event)
{
	...
	//==================(1)=======================
	if (cpu_has_vmx_tpr_shadow() && !init_event) {
		vmcs_write64(VIRTUAL_APIC_PAGE_ADDR, 0);
    	if (cpu_need_tpr_shadow(vcpu))
    	    vmcs_write64(VIRTUAL_APIC_PAGE_ADDR,
					__pa(vcpu->arch.apic->regs));
    	vmcs_write32(TPR_THRESHOLD, 0);
	}

	//==================(2)=======================
	kvm_make_request(KVM_REQ_APIC_PAGE_RELOAD, vcpu);
	...
}
```
1. 先看是否支持TPR SHADOW,，如果支持的话，先将
`VIRTUAL_APIC_PAGE_ADDR`字段清0, 再判断 是否需要TPR SHADOW
的功能，如果需要，则将`vcpu->arch.apic->regs`写入
`VIRTUAL_APIC_PAGE_ADDR`字段

2. 在这里，创建了一个 KVM_REQ_APIC_PAGE_RELOAD的 request，
该request会在 `vcpu_enter_guest`函数中处理

```cpp
static int vcpu_enter_guest(struct kvm_vcpu *vcpu)    
{
	...
	if (kvm_request_pending(vcpu)) {
	...
		if (kvm_check_request(KVM_REQ_APIC_PAGE_RELOAD, vcpu))  
		    kvm_vcpu_reload_apic_access_page(vcpu);
		}
	...
	}
	...
}
```

`kvm_vcpu_reload_apic_access_page`:

```cpp
void kvm_vcpu_reload_apic_access_page(struct kvm_vcpu *vcpu)
{
    struct page *page = NULL;                                            

    if (!lapic_in_kernel(vcpu))                                          
        return;                                                          

    if (!kvm_x86_ops->set_apic_access_page_addr)                         
        return;                                                          
	//==========================(1)===============================
    page = gfn_to_page(vcpu->kvm, APIC_DEFAULT_PHYS_BASE >> PAGE_SHIFT); 
    if (is_error_page(page))                                             
        return;                                                          
    kvm_x86_ops->set_apic_access_page_addr(vcpu, page_to_phys(page));    

    /*                                                                   
    ¦* Do not pin apic access page in memory, the MMU notifier           
    ¦* will call us again if it is migrated or swapped out.              
    ¦*/                                                                  
    put_page(page);                                                      
}                                                                   
```

`APIC-access page`是一个 物理地址(hpa), 所以这里要将gpa->hpa, 然后将hpa写入
VMCS中`APIC-access page`字段，如下:

`vmx_set_apic_access_page_addr`:

```cpp
static void vmx_set_apic_access_page_addr(struct kvm_vcpu *vcpu, hpa_t hpa) 
{                                                                           
    if (!is_guest_mode(vcpu)) {                                             
        vmcs_write64(APIC_ACCESS_ADDR, hpa);                                
        vmx_flush_tlb(vcpu, true);                                          
    }                                                                       
}                                                                           
```
# 附录

##  enable_apicv

### 定义
```cpp
bool __read_mostly enable_apicv = 1;
module_param(enable_apicv, bool, S_IRUGO);
```
`enable_apicv`是一个kvm_intel的模块参数，默认值
为1, 该值还需要根据host上的硬件对该功能的实现
情况来最终确定能否为1, 请看下面:

### 变量初始化堆栈
```
module_init(vmx_init)	//module: kvm-intel
	kvm_init
		kvm_arch_hardware_setup
			kvm_x86_ops->hardware_setup  (arch/x86/kvm/vmx/vmx.c:hardware_setup)
```

### hardware_setup()
```
static __init int hardware_setup(void)
{
	...
	if (!cpu_has_vmx_apicv()) {
		enable_apicv = 0;
    	kvm_x86_ops->sync_pir_to_irr = NULL;
	}
	...
}
static inline bool cpu_has_vmx_apicv(void)
{
    return cpu_has_vmx_apic_register_virt() &&
        cpu_has_vmx_virtual_intr_delivery() &&
        cpu_has_vmx_posted_intr();
}

static inline bool cpu_has_vmx_apic_register_virt(void)
{
    return vmcs_config.cpu_based_2nd_exec_ctrl &
        SECONDARY_EXEC_APIC_REGISTER_VIRT;
}

static inline bool cpu_has_vmx_virtual_intr_delivery(void)
{
    return vmcs_config.cpu_based_2nd_exec_ctrl &
        SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY;
}

static inline bool cpu_has_vmx_posted_intr(void)
{
    return IS_ENABLED(CONFIG_X86_LOCAL_APIC) &&
        vmcs_config.pin_based_exec_ctrl & PIN_BASED_POSTED_INTR;
}
```
由上可见`enable_apicv`为1的条件为: 支持全部下面条件
* APIC-register virtualization
* Virtual-interrupt delivery
* Process posted interrupts

> 关于`vmcs_config` 的细节，请见<<<<>>>>章节


