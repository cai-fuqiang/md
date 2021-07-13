# 寄存器信息

## struct kvm_vcpu 
```
ffff88043ea73280
```

## struct kvm_vcpu.aarch.mmu.root_hpa ffff88043ea73280
```
87064576
```

## 堆栈信息
vmx_set_cr3 堆栈信息

```
0xffff88003e5ebc70:     0x0000000000000000              0x0000000000000000
0xffff88003e5ebc80:     0x000000000916619e              0x0000000000000000
0xffff88003e5ebc90:     0x000ffffffffff000              0xffff88043ea73280
0xffff88003e5ebca0:     0xffff88003e5ebcd8              0xffffffffc07980a9
0xffff88003e5ebcb0:     0x0000000000000001              0xffff88043ec8aef8
0xffff88003e5ebcc0:     0xffff88003e5ebcfc              0xffff88043ea73280
0xffff88003e5ebcd0:     0xffff88043ea73280              0xffff88003e5ebd48
0xffff88003e5ebce0:     0x0000000000006c14              0x0000000000000000  (rbx)
0xffff88003e5ebcf0:     0xffff88043ea73280              0x0000000000000000
0xffff88003e5ebd00:     0xffff88003e5ebd48              0xffffffffc09a9833  (pc)
```

## 获取偏移

```
crash> x/1xg ((struct kvm_vcpu *)0)->arch.mmu.set_cr3
gdb: invalid user virtual address: 348  type: "gdb_readmem_callback"
Cannot access memory at address 0x348
gdb: gdb request failed: x/1xg
```

## vmx_set_cr3 汇编代码

```
/usr/src/debug/kernel-3.10.0-693.el7/linux-3.10.0-693.el7.x86_64/arch/x86/kvm/vmx.c: 3750
0xffffffffc07a2190 <vmx_set_cr3>:       nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc07a2195 <vmx_set_cr3+5>:     push   %rbp
0xffffffffc07a2196 <vmx_set_cr3+6>:     mov    %rsp,%rbp
0xffffffffc07a2199 <vmx_set_cr3+9>:     push   %r13
0xffffffffc07a219b <vmx_set_cr3+11>:    push   %r12
0xffffffffc07a219d <vmx_set_cr3+13>:    mov    %rsi,%r12
0xffffffffc07a21a0 <vmx_set_cr3+16>:    push   %rbx     -->压入rbx
0xffffffffc07a21a1 <vmx_set_cr3+17>:    mov    %rdi,%rbx
0xffffffffc07a21a4 <vmx_set_cr3+20>:    sub    $0x18,%rsp
```

## 堆栈

```
0xffff88003e5ebcb0:     0x0000000000000001              0xffff88043ec8aef8        
0xffff88003e5ebcc0:     0xffff88003e5ebcfc              0xffff88043ea73280        
0xffff88003e5ebcd0:     0xffff88043ea73280              0xffff88003e5ebd48        
0xffff88003e5ebce0:     0x0000000000006c14              0x0000000000000000
0xffff88003e5ebcf0:     0xffff88043ea73280              0x0000000000000000        
0xffff88003e5ebd00:     0xffff88003e5ebd48              0xffffffffc09a9833
   ffff88003e5ebcf0: ffff88043ea73280(-0x58) 		0000000000000000(-0x50)
   ffff88003e5ebd00: ffff88003e5ebd48(-0x48) 		ffffffffc09a9833(-0x40)
#1 [ffff88003e5ebd08] kvm_mmu_load at ffffffffc09a9833 [kvm]
   ffff88003e5ebd10: ffff88003e5ebd48(-0x38)		000000000916619e(-0x30)
   ffff88003e5ebd20: ffff88043ea73280(rbx)(rsp) 	0000000000000000(-0x20)
   ffff88003e5ebd30: 0000000000000001 				0000000000000000(-0x20)
   ffff88003e5ebd40: 0000000000000001 				ffff88003e5ebdc8(rbp寄存器地址)
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

## kvm_mmmu_load
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
0
```
