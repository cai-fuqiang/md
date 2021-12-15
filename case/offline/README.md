# struct psci_ops
## 36 host
```
crash> p psci_ops
psci_ops = $1 = {
  get_version = 0xffff000010761218 <psci_get_version>,
  cpu_suspend = 0xffff000010761250 <psci_cpu_suspend>,
  cpu_off = 0xffff0000107612c0 <psci_cpu_off>,
  cpu_on = 0xffff000010761330 <psci_cpu_on>,
  migrate = 0xffff0000107613a0 <psci_migrate>,
  affinity_info = 0xffff000010761410 <psci_affinity_info>,
  migrate_info_type = 0xffff000010761458 <psci_migrate_info_type>,
  conduit = PSCI_CONDUIT_SMC,
  smccc_version = SMCCC_VERSION_1_1
}
```

## qemu guest
```
crash> p psci_ops
psci_ops = $4 = {
  get_version = 0xffff00001075e070 <psci_get_version>,
  cpu_suspend = 0xffff00001075e0a8 <psci_cpu_suspend>,
  cpu_off = 0xffff00001075e118 <psci_cpu_off>,
  cpu_on = 0xffff00001075e188 <psci_cpu_on>,
  migrate = 0xffff00001075e1f8 <psci_migrate>,
  affinity_info = 0xffff00001075e268 <psci_affinity_info>,
  migrate_info_type = 0xffff00001075e2b0 <psci_migrate_info_type>,
  conduit = PSCI_CONDUIT_HVC,
  smccc_version = SMCCC_VERSION_1_1
}

```

# pci_function_id
## qemu
```
0xffff000011eee7a8 <psci_function_id>:  0xc4000001      0xc4000003      0x84000002      0xc4000005
```
## 36 host
```
0xffff000011eee8d0 <psci_function_id>:  0xc4000001      0xc4000003      0x84000002      0xc4000005
```

# invoke_psci_fn
```
invoke_psci_fn = $6 = (psci_fn *) 0xffff00001075e4a0 <__invoke_psci_fn_hvc>
```
# sizeof(struct arm_smccc_res)
```
crash> p sizeof(struct arm_smccc_res)
$1 = 32
(0x20)
```
# dis __invoke_psci_fn_hvc
```
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
0xffff000010761680 <__invoke_psci_fn_hvc+56>:   mov     x1, x21
0xffff000010761684 <__invoke_psci_fn_hvc+60>:   mov     x0, x20
0xffff000010761688 <__invoke_psci_fn_hvc+64>:   adrp    x19, 0xffff000011453000 <page_wait_table+5312>
0xffff00001076168c <__invoke_psci_fn_hvc+68>:   add     x19, x19, #0x708
0xffff000010761690 <__invoke_psci_fn_hvc+72>:   mov     x3, x23
0xffff000010761694 <__invoke_psci_fn_hvc+76>:   mov     x2, x22
0xffff000010761698 <__invoke_psci_fn_hvc+80>:   ldr     x4, [x19]
0xffff00001076169c <__invoke_psci_fn_hvc+84>:   str     x4, [sp,#120]
0xffff0000107616a0 <__invoke_psci_fn_hvc+88>:   mov     x4, #0x0                        // #0
0xffff0000107616a4 <__invoke_psci_fn_hvc+92>:   mov     x7, #0x0                        // #0
0xffff0000107616a8 <__invoke_psci_fn_hvc+96>:   mov     x6, #0x0                        // #0
0xffff0000107616ac <__invoke_psci_fn_hvc+100>:  mov     x5, #0x0                        // #0
0xffff0000107616b0 <__invoke_psci_fn_hvc+104>:  bl      0xffff00001009528c <__arm_smccc_hvc>
0xffff0000107616b4 <__invoke_psci_fn_hvc+108>:  ldr     x0, [sp,#120]
0xffff0000107616b8 <__invoke_psci_fn_hvc+112>:  ldr     x1, [x19]
0xffff0000107616bc <__invoke_psci_fn_hvc+116>:  eor     x1, x0, x1
0xffff0000107616c0 <__invoke_psci_fn_hvc+120>:  ldr     x0, [sp,#88]
0xffff0000107616c4 <__invoke_psci_fn_hvc+124>:  cbnz    x1, 0xffff0000107616e0 <__invoke_psci_fn_hvc+152>
0xffff0000107616c8 <__invoke_psci_fn_hvc+128>:  ldp     x29, x30, [sp,#16]
0xffff0000107616cc <__invoke_psci_fn_hvc+132>:  ldp     x19, x20, [sp,#32]
0xffff0000107616d0 <__invoke_psci_fn_hvc+136>:  ldp     x21, x22, [sp,#48]
0xffff0000107616d4 <__invoke_psci_fn_hvc+140>:  ldr     x23, [sp,#64]
0xffff0000107616d8 <__invoke_psci_fn_hvc+144>:  add     sp, sp, #0x80
0xffff0000107616dc <__invoke_psci_fn_hvc+148>:  ret
0xffff0000107616e0 <__invoke_psci_fn_hvc+152>:  bl      0xffff0000100e38d8 <__stack_chk_fail>
```

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
