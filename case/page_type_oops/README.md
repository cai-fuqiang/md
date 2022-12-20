# bt
```
 #8 [ffff0000879cfcb0] el1_ia at ffff301964682ecc
     PC: ffff30196488de78  [page_mapped+136]
     LR: ffff30196488de5c  [page_mapped+108]
     SP: ffff0000879cfcc0  PSTATE: 80000005
    X29: ffff0000879cfcc0  X28: ffffbdae845e8600  X27: 0000ffffc8870108
    X26: ffff3019652d3000  X25: ffff0000879cfe68  X24: ffff7fe000000000
    X23: 0000ffffc8800008  X22: 0000000000000000  X21: 31fffff000008000
    X20: ffff7fef65d80800  X19: ffff7fef65d80800  X18: 000000000000ffff
    X17: 0000000000000000  X16: 0000000000000000  X15: 000000006ffffeff
    X14: 0000000000000000  X13: 0000ffff85ba3e48  X12: 0000000000000040
    X11: 0000000000000008  X10: 0101010101010101   X9: 0000000000000002
     X8: 0000000000000043   X7: 0000000400000000   X6: 0000000000000018
     X5: ffff301966491fe0   X4: 0000000000000001   X3: 0000000000002000
     X2: 0000000000002000   X1: 0000000000001fe1   X0: ffff7fef65e00000
 #8 [ffff0000879cfcb0] el1_ia at ffff301964682ecc
     PC: ffff30196488de78  [page_mapped+136]
     LR: ffff30196488de5c  [page_mapped+108]
     SP: ffff0000879cfcc0  PSTATE: 80000005
    X29: ffff0000879cfcc0  X28: ffffbdae845e8600  X27: 0000ffffc8870108
    X26: ffff3019652d3000  X25: ffff0000879cfe68  X24: ffff7fe000000000
    X23: 0000ffffc8800008  X22: 0000000000000000  X21: 31fffff000008000
    X20: ffff7fef65d80800  X19: ffff7fef65d80800  X18: 000000000000ffff
    X17: 0000000000000000  X16: 0000000000000000  X15: 000000006ffffeff
    X14: 0000000000000000  X13: 0000ffff85ba3e48  X12: 0000000000000040
    X11: 0000000000000008  X10: 0101010101010101   X9: 0000000000000002
     X8: 0000000000000043   X7: 0000000400000000   X6: 0000000000000018
     X5: ffff301966491fe0   X4: 0000000000000001   X3: 0000000000002000
     X2: 0000000000002000   X1: 0000000000001fe1   X0: ffff7fef65e00000
 #9 [ffff0000879cfcc0] page_mapped at ffff30196488de74
#10 [ffff0000879cfce0] stable_page_flags at ffff3019649a2798
#11 [ffff0000879cfd10] kpageflags_read at ffff3019649a28bc
#12 [ffff0000879cfd70] proc_reg_read at ffff30196498fdb8
#13 [ffff0000879cfdb0] __vfs_read at ffff30196490fbdc
#14 [ffff0000879cfde0] vfs_read at ffff30196490fcbc
#15 [ffff0000879cfe20] ksys_pread64 at ffff301964910520
#16 [ffff0000879cfe70] __arm64_sys_pread64 at ffff301964910584
#17 [ffff0000879cfe90] el0_svc_handler at ffff301964695374
#18 [ffff0000879cfff0] el0_svc at ffff301964683784
```
# struct page -o
```
struct page {
   [0] unsigned long flags;
       union {
           struct {
   [8]         struct list_head lru;
  [24]         struct address_space *mapping;
 
```

# dis -l page_mapped
```
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/mm/util.c: 468
0xffff30196488ddf0 <page_mapped>:       stp     x29, x30, [sp,#-32]!
0xffff30196488ddf4 <page_mapped+4>:     mov     x29, sp
0xffff30196488ddf8 <page_mapped+8>:     str     x19, [sp,#16]
0xffff30196488ddfc <page_mapped+12>:    mov     x19, x0
0xffff30196488de00 <page_mapped+16>:    mov     x0, x30
0xffff30196488de04 <page_mapped+20>:    nop
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/./include/asm-generic/bitops/non-atomic.h: 106
0xffff30196488de08 <page_mapped+24>:    ldr     x0, [x19]			//page->flags
0xffff30196488de0c <page_mapped+28>:    tst     w0, #0x8000			//1->15   PG_head
0xffff30196488de10 <page_mapped+32>:    b.eq    0xffff30196488de34		//line 472: return;
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/./include/linux/compiler.h: 188
0xffff30196488de14 <page_mapped+36>:    ldr     x0, [x19,#8]
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/./include/linux/page-flags.h: 151
0xffff30196488de18 <page_mapped+40>:    sub     x1, x0, #0x1
0xffff30196488de1c <page_mapped+44>:    tst     x0, #0x1
0xffff30196488de20 <page_mapped+48>:    csel    x19, x1, x19, ne
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/./include/linux/compiler.h: 188
0xffff30196488de24 <page_mapped+52>:    ldr     w0, [x19,#84]
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/mm/util.c: 474
0xffff30196488de28 <page_mapped+56>:    tbnz    w0, #31, 0xffff30196488de54
0xffff30196488de2c <page_mapped+60>:    mov     w0, #0x1                        // #1
0xffff30196488de30 <page_mapped+64>:    b       0xffff30196488de48
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/./include/linux/compiler.h: 188
0xffff30196488de34 <page_mapped+68>:    ldr     x0, [x19,#8]
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/./include/linux/page-flags.h: 158
0xffff30196488de38 <page_mapped+72>:    tbnz    w0, #0, 0xffff30196488de14
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/./include/linux/compiler.h: 188
0xffff30196488de3c <page_mapped+76>:    ldr     w0, [x19,#48]
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/mm/util.c: 472
0xffff30196488de40 <page_mapped+80>:    mvn     w0, w0
0xffff30196488de44 <page_mapped+84>:    lsr     w0, w0, #31
0xffff30196488de48 <page_mapped+88>:    ldr     x19, [sp,#16]
0xffff30196488de4c <page_mapped+92>:    ldp     x29, x30, [sp],#32
0xffff30196488de50 <page_mapped+96>:    ret
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/mm/util.c: 476
0xffff30196488de54 <page_mapped+100>:   mov     x0, x19
0xffff30196488de58 <page_mapped+104>:   bl      0xffff3019648cce88
0xffff30196488de5c <page_mapped+108>:   mov     w1, w0
0xffff30196488de60 <page_mapped+112>:   mov     w0, #0x0                        // #0
0xffff30196488de64 <page_mapped+116>:   cbnz    w1, 0xffff30196488de48
0xffff30196488de68 <page_mapped+120>:   mov     x0, x19
0xffff30196488de6c <page_mapped+124>:   mov     w4, #0x1                        // #1
0xffff30196488de70 <page_mapped+128>:   mov     w3, #0x2000                     // #8192
0xffff30196488de74 <page_mapped+132>:   b       0xffff30196488de84
0xffff30196488de78 <page_mapped+136>:   ldr     w2, [x0,#48]
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/mm/util.c: 479
0xffff30196488de7c <page_mapped+140>:   add     x0, x0, #0x40
0xffff30196488de80 <page_mapped+144>:   tbz     w2, #31, 0xffff30196488de2c
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.aarch64/./include/asm-generic/bitops/non-atomic.h: 106
0xffff30196488de84 <page_mapped+148>:   ldr     x2, [x19]
0xffff30196488de88 <page_mapped+152>:   tst     x2, #0x8000
0xffff30196488de8c <page_mapped+156>:   csel    w2, w4, w3, eq
0xffff30196488de90 <page_mapped+160>:   cmp     w1, w2
0xffff30196488de94 <page_mapped+164>:   add     w1, w1, #0x1
0xffff30196488de98 <page_mapped+168>:   b.lt    0xffff30196488de78
0xffff30196488de9c <page_mapped+172>:   mov     w0, #0x0                        // #0
0xffff30196488dea0 <page_mapped+176>:   b       0xffff30196488de48
0xffff30196488dea4 <page_mapped+180>:   nop
```
