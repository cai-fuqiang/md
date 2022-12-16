# kvm_vcpu_kick
```
commit 6b7e2d0991489559a1df4500d77f7b76c4607ed0
Author: Xiao Guangrong <xiaoguangrong@cn.fujitsu.com>
Date:   Wed Jan 12 15:40:31 2011 +0800

    KVM: Add "exiting guest mode" state

commit 32f8840064d88cc3f6e85203aec7b6b57bebcb97
Author: Marcelo Tosatti <mtosatti@redhat.com>
Date:   Thu May 7 17:55:12 2009 -0300

    KVM: use smp_send_reschedule in kvm_vcpu_kick
```


# vmx_deliver_posted_interrupt
```
commit b95234c840045b7c72380fd14c59416af28fcb02
Author: Paolo Bonzini <pbonzini@redhat.com>
Date:   Mon Dec 19 13:57:33 2016 +0100

    kvm: x86: do not use KVM_REQ_EVENT for APICv interrupt injection

commit bf9f6ac8d74969690df1485b33b7c238ca9f2269
Author: Feng Wu <feng.wu@intel.com>
Date:   Fri Sep 18 22:29:55 2015 +0800

    KVM: Update Posted-Interrupts Descriptor when vCPU is blocked


commit a20ed54d6e470bf0d28921b7aadb6ca0da0ff0c3
Author: Yang Zhang <yang.z.zhang@Intel.com>
Date:   Thu Apr 11 19:25:15 2013 +0800

    KVM: VMX: Add the deliver posted interrupt algorithm
```


# 支持post interrupt Descriptor
```
commit a547c6db4d2f16ba5ce8e7054bffad6acc248d40
Author: Yang Zhang <yang.z.zhang@Intel.com>
Date:   Thu Apr 11 19:25:10 2013 +0800

    KVM: VMX: Enable acknowledge interupt on vmexit

...

commit 5a71785dde307f6ac80e83c0ad3fd694912010a1
Author: Yang Zhang <yang.z.zhang@Intel.com>
Date:   Thu Apr 11 19:25:16 2013 +0800

    KVM: VMX: Use posted interrupt to deliver virtual interrupt
```

# virtual interrupt delivery
```
mail-list:
https://lore.kernel.org/all/1358331672-32384-1-git-send-email-yang.z.zhang@intel.com/

83d4c286931c9d28c5be21bac3c73a2332cab681	[1/3]
    x86, apicv: add APICv register virtualization support

8d14695f9542e9e0195d6e41ddaa52c32322adf5	[2/3]
    x86, apicv: add virtual x2apic support

c7c9c56ca26f7b9458711b2d78b60b60e0d38ba7	[3/3]
    x86, apicv: add virtual interrupt delivery support
```
