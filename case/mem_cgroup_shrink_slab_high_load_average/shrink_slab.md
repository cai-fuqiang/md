```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 855
0xffff8000102d0820 <shrink_slab>:       stp     x29, x30, [sp,#-176]!
0xffff8000102d0824 <shrink_slab+4>:     mov     x29, sp
0xffff8000102d0828 <shrink_slab+8>:     stp     x19, x20, [sp,#16]
0xffff8000102d082c <shrink_slab+12>:    stp     x21, x22, [sp,#32]
0xffff8000102d0830 <shrink_slab+16>:    stp     x23, x24, [sp,#48]
0xffff8000102d0834 <shrink_slab+20>:    stp     x25, x26, [sp,#64]
0xffff8000102d0838 <shrink_slab+24>:    adrp    x26, 0xffff800011b78000 <join_entries+14464>
0xffff8000102d083c <shrink_slab+28>:    mov     w24, w0
0xffff8000102d0840 <shrink_slab+32>:    add     x26, x26, #0x788
0xffff8000102d0844 <shrink_slab+36>:    mov     x0, x30
0xffff8000102d0848 <shrink_slab+40>:    mov     w23, w1
0xffff8000102d084c <shrink_slab+44>:    mov     x21, x2
0xffff8000102d0850 <shrink_slab+48>:    mov     w25, w3
0xffff8000102d0854 <shrink_slab+52>:    nop
0xffff8000102d0858 <shrink_slab+56>:    ldr     x0, [x26]
0xffff8000102d085c <shrink_slab+60>:    str     x0, [sp,#168]
0xffff8000102d0860 <shrink_slab+64>:    mov     x0, #0x0                        // #0
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./arch/arm64/include/asm/jump_label.h: 31
0xffff8000102d0864 <shrink_slab+68>:    nop
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./include/linux/memcontrol.h: 651
0xffff8000102d0868 <shrink_slab+72>:    adrp    x0, 0xffff800011b7c000 <mm_slots_hash+6424>
0xffff8000102d086c <shrink_slab+76>:    ldr     x0, [x0,#1768]
0xffff8000102d0870 <shrink_slab+80>:    cmp     x21, x0
0xffff8000102d0874 <shrink_slab+84>:    b.ne    0xffff8000102d0944 <shrink_slab+292>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 869
0xffff8000102d0878 <shrink_slab+88>:    adrp    x20, 0xffff800011f74000 <event_oom_score_adj_update+128>
0xffff8000102d087c <shrink_slab+92>:    add     x20, x20, #0x618
0xffff8000102d0880 <shrink_slab+96>:    mov     x0, x20
0xffff8000102d0884 <shrink_slab+100>:   mov     x19, #0x0                       // #0
0xffff8000102d0888 <shrink_slab+104>:   bl      0xffff800010160d98 <down_read_trylock>
0xffff8000102d088c <shrink_slab+108>:   cbz     w0, 0xffff8000102d0914 <shrink_slab+244>
0xffff8000102d0890 <shrink_slab+112>:   stp     x27, x28, [sp,#80]
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 872
0xffff8000102d0894 <shrink_slab+116>:   mov     x27, x20
0xffff8000102d0898 <shrink_slab+120>:   ldr     x22, [x27,#72]!
0xffff8000102d089c <shrink_slab+124>:   cmp     x22, x27
0xffff8000102d08a0 <shrink_slab+128>:   sub     x22, x22, #0x28
0xffff8000102d08a4 <shrink_slab+132>:   b.eq    0xffff8000102d0908 <shrink_slab+232>
0xffff8000102d08a8 <shrink_slab+136>:   add     x28, x20, #0x8
0xffff8000102d08ac <shrink_slab+140>:   b       0xffff8000102d08c0 <shrink_slab+160>
0xffff8000102d08b0 <shrink_slab+144>:   ldr     x22, [x22,#40]
0xffff8000102d08b4 <shrink_slab+148>:   cmp     x22, x27
0xffff8000102d08b8 <shrink_slab+152>:   sub     x22, x22, #0x28
0xffff8000102d08bc <shrink_slab+156>:   b.eq    0xffff8000102d0908 <shrink_slab+232>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 873
0xffff8000102d08c0 <shrink_slab+160>:   add     x3, sp, #0x24
0xffff8000102d08c4 <shrink_slab+164>:   str     w24, [sp,#128]
0xffff8000102d08c8 <shrink_slab+168>:   str     xzr, [sp,#148]
0xffff8000102d08cc <shrink_slab+172>:   mov     x1, x22
0xffff8000102d08d0 <shrink_slab+176>:   mov     w2, w25
0xffff8000102d08d4 <shrink_slab+180>:   add     x0, sp, #0x80
0xffff8000102d08d8 <shrink_slab+184>:   stp     xzr, xzr, [x3,#96]
0xffff8000102d08dc <shrink_slab+188>:   stp     w23, wzr, [sp,#152]
0xffff8000102d08e0 <shrink_slab+192>:   str     x21, [sp,#160]
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 879
0xffff8000102d08e4 <shrink_slab+196>:   bl      0xffff8000102cf460 <do_shrink_slab>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 880
0xffff8000102d08e8 <shrink_slab+200>:   ldr     x1, [x20,#8]
0xffff8000102d08ec <shrink_slab+204>:   cmn     x0, #0x2
0xffff8000102d08f0 <shrink_slab+208>:   add     x0, x19, x0
0xffff8000102d08f4 <shrink_slab+212>:   csel    x19, x0, x19, ne
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./include/linux/list.h: 282
0xffff8000102d08f8 <shrink_slab+216>:   cmp     x1, x28
0xffff8000102d08fc <shrink_slab+220>:   b.eq    0xffff8000102d08b0 <shrink_slab+144>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 889
0xffff8000102d0900 <shrink_slab+224>:   cmp     x19, #0x0
0xffff8000102d0904 <shrink_slab+228>:   csinc   x19, x19, xzr, ne
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 894
0xffff8000102d0908 <shrink_slab+232>:   mov     x0, x20
0xffff8000102d090c <shrink_slab+236>:   bl      0xffff8000101605b0 <up_read>
0xffff8000102d0910 <shrink_slab+240>:   ldp     x27, x28, [sp,#80]
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 896
0xffff8000102d0914 <shrink_slab+244>:   bl      0xffff800010aa8ef0 <_cond_resched>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 897
0xffff8000102d0918 <shrink_slab+248>:   mov     x0, x19
0xffff8000102d091c <shrink_slab+252>:   ldr     x2, [sp,#168]
0xffff8000102d0920 <shrink_slab+256>:   ldr     x1, [x26]
0xffff8000102d0924 <shrink_slab+260>:   eor     x1, x2, x1
0xffff8000102d0928 <shrink_slab+264>:   cbnz    x1, 0xffff8000102d0aec <shrink_slab+716>
0xffff8000102d092c <shrink_slab+268>:   ldp     x19, x20, [sp,#16]
0xffff8000102d0930 <shrink_slab+272>:   ldp     x21, x22, [sp,#32]
0xffff8000102d0934 <shrink_slab+276>:   ldp     x23, x24, [sp,#48]
0xffff8000102d0938 <shrink_slab+280>:   ldp     x25, x26, [sp,#64]
0xffff8000102d093c <shrink_slab+284>:   ldp     x29, x30, [sp],#176
0xffff8000102d0940 <shrink_slab+288>:   ret
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./arch/arm64/include/asm/jump_label.h: 31
0xffff8000102d0944 <shrink_slab+292>:   nop
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./include/linux/memcontrol.h: 923   //shrink_slab_memcg->mem_cgroup_online
0xffff8000102d0948 <shrink_slab+296>:   ldr     w0, [x21,#124]
0xffff8000102d094c <shrink_slab+300>:   tbz     w0, #1, 0xffff8000102d0a9c <shrink_slab+636>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 761
0xffff8000102d0950 <shrink_slab+304>:   adrp    x20, 0xffff800011f74000 <event_oom_score_adj_update+128>
0xffff8000102d0954 <shrink_slab+308>:   add     x20, x20, #0x618
0xffff8000102d0958 <shrink_slab+312>:   mov     x0, x20
0xffff8000102d095c <shrink_slab+316>:   bl      0xffff800010160d98 <down_read_trylock>
0xffff8000102d0960 <shrink_slab+320>:   cbz     w0, 0xffff8000102d0a9c <shrink_slab+636>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 764
0xffff8000102d0964 <shrink_slab+324>:   add     x0, x21, w23, sxtw #3
0xffff8000102d0968 <shrink_slab+328>:   stp     x27, x28, [sp,#80]
0xffff8000102d096c <shrink_slab+332>:   ldr     x0, [x0,#4120]
0xffff8000102d0970 <shrink_slab+336>:   ldr     x28, [x0,#840]
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 765
0xffff8000102d0974 <shrink_slab+340>:   cbz     x28, 0xffff8000102d0aac <shrink_slab+652>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 768
0xffff8000102d0978 <shrink_slab+344>:   adrp    x27, 0xffff80001233b000 <pmus_srcu+24688>
0xffff8000102d097c <shrink_slab+348>:   mov     x2, #0x0                        // #0
0xffff8000102d0980 <shrink_slab+352>:   ldr     x0, [x28,#24]
0xffff8000102d0984 <shrink_slab+356>:   ldrsw   x1, [x27,#2168]
0xffff8000102d0988 <shrink_slab+360>:   bl      0xffff8000105004c0 <find_next_bit>
0xffff8000102d098c <shrink_slab+364>:   mov     w22, w0
0xffff8000102d0990 <shrink_slab+368>:   ldr     w1, [x27,#2168]
0xffff8000102d0994 <shrink_slab+372>:   cmp     w0, w1
0xffff8000102d0998 <shrink_slab+376>:   b.ge    0xffff8000102d0aac <shrink_slab+652>
0xffff8000102d099c <shrink_slab+380>:   add     x0, x20, #0x28
```

ret
```
0xffff8000102d0a9c <shrink_slab+636>:   mov     x19, #0x0                       // #0
0xffff8000102d0aa0 <shrink_slab+640>:   b       0xffff8000102d0918 <shrink_slab+248>

```


```

```
