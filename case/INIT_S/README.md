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
`0x348`这个参数的

