我这边主要分析的是和CPU 1堆栈相关内容

# CPU 1堆栈分析
##  CPU 堆栈

```
crash> bt -c 1 -f
PID: 2757   TASK: ffff8803e0e40000  CPU: 1   COMMAND: "qemu-kvm"
 #0 [ffff88003e5ebce8] __schedule at ffffffff816a8f45
    ffff88003e5ebcf0: ffff88043ea73280 0000000000000000 
    ffff88003e5ebd00: ffff88003e5ebd48 ffffffffc09a9833 
 #1 [ffff88003e5ebd08] kvm_mmu_load at ffffffffc09a9833 [kvm]
    ffff88003e5ebd10: ffff88003e5ebd48 000000000916619e 
    ffff88003e5ebd20: ffff88043ea73280 0000000000000000 
    ffff88003e5ebd30: 0000000000000001 0000000000000000 
    ffff88003e5ebd40: 0000000000000001 ffff88003e5ebdc8 
    ffff88003e5ebd50: ffffffffc09920fb 
 #2 [ffff88003e5ebd50] vcpu_enter_guest at ffffffffc09920fb [kvm]
    ffff88003e5ebd58: ffff88003e5ebda8 ffffffffc099689c 
    ffff88003e5ebd68: 000000013e5ebfd8 000000000916619e 
    ffff88003e5ebd78: 00ff8803e0e40000 ffff88043ea73280 
    ffff88003e5ebd88: ffff8800ab5e0000 0000000000000000 
    ffff88003e5ebd98: 000000000916619e ffff88043ea73280 
    ffff88003e5ebda8: ffff88003e5ebfd8 ffff8803e0e40000 
    ffff88003e5ebdb8: ffff8803f9678048 0000000000000001 
    ffff88003e5ebdc8: ffff88003e5ebe10 ffffffffc0999e1d 
 #3 [ffff88003e5ebdd0] kvm_arch_vcpu_ioctl_run at ffffffffc0999e1d [kvm]
	...
 #4 [ffff88003e5ebe18] kvm_vcpu_ioctl at ffffffffc097f1f1 [kvm]
	...

 #5 [ffff88003e5ebeb0] do_vfs_ioctl at ffffffff812151cd
	...

 #6 [ffff88003e5ebf30] sys_ioctl at ffffffff81215471
	...

 #7 [ffff88003e5ebf80] system_call_fastpath at ffffffff816b4fc9
	...
```

从这个堆栈打印来看, 最终的调用顺序是`vcpu_enter_guest`->`kvm_mmu_load`->
`__schedule`。但是根据

```
#0 [ffff88003e5ebce8] __schedule at ffffffff816a8f45
       ^                    ^              ^
       |                    |              |
     堆栈地              调用者函数  调用者函数的返回地址
```
实际上就是`kvm_mmu_load`函数调用的函数，假设该函数为A(),
A()在调用某函数时(假设B())，会使用`call`指令，将A()的下一
条指令压入栈, 所以`ffff88003e5ebce8`栈地址处的内容应该是
`ffffffff816a8f45`, 但是通过x指令查看`ffff88003e5ebce8`地址
的内存，如下:

```
crash> x/1xg 0xffff88003e5ebce8
0xffff88003e5ebce8:     0x0000000000000000
```

所以，该堆栈#0 实际上是不正确的。

从下面的堆栈信息看，
```
 #1 [ffff88003e5ebd08] kvm_mmu_load at ffffffffc09a9833 [kvm]
```
`kvm_mmu_load`函数调用子函数的返回地址为`ffffffffc09a9833`
***
查看`kvm_mmu_load`反汇编代码
```
...
0xffffffffc09a95b2 <kvm_mmu_load+18>:   mov    %rdi,%rbx
...
0xffffffffc09a982d <kvm_mmu_load+653>:  callq  *0x348(%rbx)
0xffffffffc09a9833 <kvm_mmu_load+659>:  jmpq   0xffffffffc09a95c5 <kvm_mmu_load+37> <---调用子函数的返回地址
```

这里的rbx, 是赋值的rdi, 也就是`kvm_mmu_load`的第一个参数，如下面所示

```
int kvm_mmu_load(struct kvm_vcpu *vcpu)
```

经过推导(推导过程略)第一个参数的地址为`ffff88043ea73280`:
`0x348`这个偏移实际上是`struct kvm_vcpu.arch.mmu.set_cr3`

查看该成员

```
crash> struct kvm_vcpu.arch ffff88043ea73280 -o -p
struct kvm_vcpu {
  [ffff88043ea73490] struct kvm_vcpu_arch arch;
}
crash> struct kvm_vcpu_arch.mmu ffff88043ea73490 |head
  mmu = {
    set_cr3 = 0xffffffffc07a2190 <vmx_set_cr3>,
...
```
也就是说，实际上调用流程为`kvm_mmu_load->vmx_set_cr3`

但是又不像调用的`vmx_set_cr3`函数原因主要有以下两条:
* vmx_set_cr3函数中，没有可以阻塞住的流程(死循环，或者锁)
* 根据下面的`vmx_set_cr3`堆栈和代码来看:

堆栈:
```
0xffff88003e5ebcb0:     0x0000000000000001              0xffff88043ec8aef8
0xffff88003e5ebcc0:     0xffff88003e5ebcfc              0xffff88043ea73280(上一层的pc)
0xffff88003e5ebcd0:     0xffff88043ea73280(-0x18)       0xffff88003e5ebd48(-0x10)
0xffff88003e5ebce0:     0x0000000000006c14(-0x8)        0x0000000000000000(rbx)
   ffff88003e5ebcf0: ffff88043ea73280(r12)   0000000000000000(r13)
   ffff88003e5ebd00: ffff88003e5ebd48(rbp)   ffffffffc09a9833(pc)
#1 [ffff88003e5ebd08] kvm_mmu_load at ffffffffc09a9833 [kvm]
   ffff88003e5ebd10: ffff88003e5ebd48            000000000916619e
   ffff88003e5ebd20: ffff88043ea73280            0000000000000000
   ffff88003e5ebd30: 0000000000000001            0000000000000000
   ffff88003e5ebd40: 0000000000000001            ffff88003e5ebdc8
   ffff88003e5ebd50: ffffffffc09920fb

```
代码:
```
0xffffffffc07a2190 <vmx_set_cr3>:   nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc07a2195 <vmx_set_cr3+5>: push   %rbp
0xffffffffc07a2196 <vmx_set_cr3+6>: mov    %rsp,%rbp
0xffffffffc07a2199 <vmx_set_cr3+9>: push   %r13
0xffffffffc07a219b <vmx_set_cr3+11>:    push   %r12
0xffffffffc07a219d <vmx_set_cr3+13>:    mov    %rsi,%r12
0xffffffffc07a21a0 <vmx_set_cr3+16>:    push   %rbx
0xffffffffc07a21a1 <vmx_set_cr3+17>:    mov    %rdi,%rbx
0xffffffffc07a21a4 <vmx_set_cr3+20>:    sub    $0x18,%rsp
```
从这里可以看出, rbx寄存器压入栈的值为:0x0000000000000000, 而上一层函数
`vcpu_enter_guest`是将`struct kvm_vcpu`的地址存放在rbx，所以这个地方显示的不正确.

* 根据`vcpu_enter_guest`堆栈看
```
#2 [ffff88003e5ebd50] vcpu_enter_guest at ffffffffc09920fb [kvm]
```

汇编代码:
```
dis -l vcpu_enter_guest
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/x86.c: 6703
0xffffffffc09920eb <vcpu_enter_guest+635>:  mov    0x4eafe(%rip),%rax        # 0xffffffffc09e0bf0 <kvm_x86_ops>
0xffffffffc09920f2 <vcpu_enter_guest+642>:  mov    %rbx,%rdi
0xffffffffc09920f5 <vcpu_enter_guest+645>:  callq  *0x178(%rax)
```

通过 `dis -l`可以得到`%rax`存放地址所代表全局变量,实际上是`kvm_x86_ops`
通过crash命令查看偏移代表的函数:

```
crash> p 0x178
$4 = 376
crash> struct kvm_x86_ops -o
...
[376] void (*run)(struct kvm_vcpu *);
...

crash> x/1xg 0xffffffffc09e0bf0
0xffffffffc09e0bf0 <kvm_x86_ops>:       0xffffffffc07b7140
crash> struct kvm_x86_ops.run 0xffffffffc07b7140
  run = 0xffffffffc079e8c0 <vmx_vcpu_run>
```

从上面来看, `vcpu_enter_guest`->`vmx_vcpu_run`

## 堆栈

```
0xffff88003e5ebcb0:     0x0000000000000001              0xffff88043ec8aef8
0xffff88003e5ebcc0:     0xffff88003e5ebcfc              0xffff88043ea73280        
0xffff88003e5ebcd0:     0xffff88043ea73280              0xffff88003e5ebd48        
0xffff88003e5ebce0:     0x0000000000006c14              0x0000000000000000(调用的下次压栈地址 rsp -0x38)
   ffff88003e5ebcf0: ffff88043ea73280(-0x58) 		0000000000000000(-0x50)
   ffff88003e5ebd00: ffff88003e5ebd48(-0x48) 		ffffffffc09a9833(-0x40)
#1 [ffff88003e5ebd08] kvm_mmu_load at ffffffffc09a9833 [kvm]
   ffff88003e5ebd10: ffff88003e5ebd48(-0x38)		000000000916619e(-0x30)
   ffff88003e5ebd20: ffff88043ea73280(rbx)(rsp) 	0000000000000000(-0x20)(r12)
   ffff88003e5ebd30: 0000000000000001(r13) 		0000000000000000(-0x20)(r14)
   ffff88003e5ebd40: 0000000000000001(r15)		ffff88003e5ebdc8(rbp寄存器地址)
   ffff88003e5ebd50: ffffffffc09920fb
#2 [ffff88003e5ebd50] vcpu_enter_guest at ffffffffc09920fb [kvm]
   ffff88003e5ebd58: ffff88003e5ebda8 ffffffffc099689c
   ffff88003e5ebd68: 000000013e5ebfd8 000000000916619e
   ffff88003e5ebd78: 00ff8803e0e40000 ffff88043ea73280
   ffff88003e5ebd88: ffff8800ab5e0000 0000000000000000
   ffff88003e5ebd98: 000000000916619e ffff88043ea73280
   ffff88003e5ebda8: ffff88003e5ebfd8 ffff8803e0e40000
   ffff88003e5ebdb8: ffff8803f9678048 0000000000000001
   ffff88003e5ebdc8: ffff88003e5ebe10 ffffffffc0999e1d
#3 [ffff88003e5ebdd0] kvm_arch_vcpu_ioctl_run at ffffffffc0999e1d [kvm]
   ffff88003e5ebdd8: fffffffe7ffbfeff 000000000916619e
   ffff88003e5ebde8: ffff88043ea73280 ffff8800ab5e1700
   ffff88003e5ebdf8: 0000000000000000 ffff88017a2b2500
   ffff88003e5ebe08: ffff8803e0e40000 ffff88003e5ebea8
   ffff88003e5ebe18: ffffffffc097f1f1
#4 [ffff88003e5ebe18] kvm_vcpu_ioctl at ffffffffc097f1f1 [kvm]
   ffff88003e5ebe20: 0000000000000000 000055d2495a6000
   ffff88003e5ebe30: ffff880401b38c80 000000000916619e
   ffff88003e5ebe40: 000000000916619e 000055d2495a6380
   ffff88003e5ebe50: 0000000000000081 0000000000000000
   ffff88003e5ebe60: 000055d2495a6380 ffff88003e5ebf58
   ffff88003e5ebe70: 0000004169359ca7 000000000916619e
   ffff88003e5ebe80: ffff88017a2b2500 0000000000000000
   ffff88003e5ebe90: ffff88017fbeee40 0000000000000000
   ffff88003e5ebea0: 0000000000000001 ffff88003e5ebf28
   ffff88003e5ebeb0: ffffffff812151cd
#5 [ffff88003e5ebeb0] do_vfs_ioctl at ffffffff812151cd
```

## kvm_mmu_load

```
0xffffffffc09a95a0 <kvm_mmu_load>:      nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc09a95a5 <kvm_mmu_load+5>:    push   %rbp
0xffffffffc09a95a6 <kvm_mmu_load+6>:    mov    %rsp,%rbp
0xffffffffc09a95a9 <kvm_mmu_load+9>:    push   %r15
0xffffffffc09a95ab <kvm_mmu_load+11>:   push   %r14
0xffffffffc09a95ad <kvm_mmu_load+13>:   push   %r13
0xffffffffc09a95af <kvm_mmu_load+15>:   push   %r12
0xffffffffc09a95b1 <kvm_mmu_load+17>:   push   %rbx
0xffffffffc09a95b2 <kvm_mmu_load+18>:   mov    %rdi,%rbx
0xffffffffc09a95b5 <kvm_mmu_load+21>:   sub    $0x10,%rsp
```


## msg

```
crash> struct vcpu_vmx.nested ffff88003e5ebd48
  nested = {
    vmxon = false,
    vmxon_ptr = 0,
    current_vmptr = 0,
    current_vmcs12_page = 0x0,
    current_vmcs12 = 0x0,
    cached_vmcs12 = 0x0,
    sync_shadow_vmcs = false,
    vmcs02_pool = {
      next = 0x0,
      prev = 0x0
    },
    vmcs02_num = 0,
    change_vmcs01_virtual_x2apic_mode = false,
    nested_run_pending = false,
    apic_access_page = 0x0,
    virtual_apic_page = 0x0,
    pi_desc_page = 0x0,
    pi_desc = 0x0,
    pi_pending = false,

crash> struct vcpu_vmx.soft_vnmi_blocked ffff88003e5ebd48
  soft_vnmi_blocked = 0

crash> struct vcpu_vmx.emulation_required ffff88003e5ebd48
  emulation_required = false
crash> struct vcpu_vmx.ple_window_dirty ffff88003e5ebd48
  ple_window_dirty = false

```
