# shrink_slab
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./arch/arm64/include/asm/jump_label.h: 31
0xffff8000102d0944 <shrink_slab+292>:   nop
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./include/linux/memcontrol.h: 923
0xffff8000102d0948 <shrink_slab+296>:   ldr     w0, [x21,#124]      //=====>758: memcg->css.flags 
                                                                    //x21是 memcg
0xffff8000102d094c <shrink_slab+300>:   tbz     w0, #1, 0xffff8000102d0a9c <shrink_slab+636>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 761
0xffff8000102d0950 <shrink_slab+304>:   adrp    x20, 0xffff800011f74000 <event_oom_score_adj_update+128>
0xffff8000102d0954 <shrink_slab+308>:   add     x20, x20, #0x618
0xffff8000102d0958 <shrink_slab+312>:   mov     x0, x20
0xffff8000102d095c <shrink_slab+316>:   bl      0xffff800010160d98 <down_read_trylock>
0xffff8000102d0960 <shrink_slab+320>:   cbz     w0, 0xffff8000102d0a9c <shrink_slab+636>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 764
0xffff8000102d0964 <shrink_slab+324>:   add     x0, x21, w23, sxtw #3           //x23是nid,  这里我的理解是, 先用 memcg + nid * 8, 8 代表一个指针的大小, shift 3
0xffff8000102d0968 <shrink_slab+328>:   stp     x27, x28, [sp,#80]

//4120 正好是, memcg->nodeinfo成员, 再加上x0的话, 就得到了 mem_cgroup->nodeinfo[nid]
struct mem_cgroup {
  [4120] struct mem_cgroup_per_node *nodeinfo[];
}

0xffff8000102d096c <shrink_slab+332>:   ldr     x0, [x0,#4120]                  //x0为mem_cgroup_per_node

struct mem_cgroup_per_node {
  [840] struct shrinker_info *shrinker_info;
}

0xffff8000102d0970 <shrink_slab+336>:   ldr     x28, [x0,#840]      //x28为shrinker_info
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
0xffff8000102d09a0 <shrink_slab+384>:   add     x27, x27, #0x878
0xffff8000102d09a4 <shrink_slab+388>:   mov     x19, #0x0                       // #0
0xffff8000102d09a8 <shrink_slab+392>:   str     x0, [sp,#104]
0xffff8000102d09ac <shrink_slab+396>:   add     x0, x20, #0x8
0xffff8000102d09b0 <shrink_slab+400>:   str     x0, [sp,#120]
0xffff8000102d09b4 <shrink_slab+404>:   b       0xffff8000102d09f0 <shrink_slab+464>
0xffff8000102d09b8 <shrink_slab+408>:   add     x19, x19, x0
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./include/linux/compiler.h: 276
0xffff8000102d09bc <shrink_slab+412>:   ldr     x0, [x20,#8]
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/./include/linux/list.h: 282
0xffff8000102d09c0 <shrink_slab+416>:   ldr     x1, [sp,#120]
0xffff8000102d09c4 <shrink_slab+420>:   cmp     x0, x1
0xffff8000102d09c8 <shrink_slab+424>:   b.ne    0xffff8000102d0adc <shrink_slab+700>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 768
0xffff8000102d09cc <shrink_slab+428>:   ldrsw   x1, [x27]
0xffff8000102d09d0 <shrink_slab+432>:   add     w2, w22, #0x1

crash> struct shrinker_info -o
struct shrinker_info {
   [0] struct callback_head rcu;
  [16] atomic_long_t *nr_deferred;
  [24] unsigned long *map;
}
0xffff8000102d09d4 <shrink_slab+436>:   ldr     x0, [x28,#24]       -->shrinker_info->map
0xffff8000102d09d8 <shrink_slab+440>:   sxtw    x2, w2
0xffff8000102d09dc <shrink_slab+444>:   bl      0xffff8000105004c0 <find_next_bit>
0xffff8000102d09e0 <shrink_slab+448>:   mov     w22, w0
0xffff8000102d09e4 <shrink_slab+452>:   ldr     w1, [x27]
0xffff8000102d09e8 <shrink_slab+456>:   cmp     w0, w1
0xffff8000102d09ec <shrink_slab+460>:   b.ge    0xffff8000102d0ab0 <shrink_slab+656>
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 769
0xffff8000102d09f0 <shrink_slab+464>:   add     x2, sp, #0x24
0xffff8000102d09f4 <shrink_slab+468>:   str     w24, [sp,#128]
0xffff8000102d09f8 <shrink_slab+472>:   ldr     x0, [sp,#104]
0xffff8000102d09fc <shrink_slab+476>:   str     xzr, [sp,#148]
0xffff8000102d0a00 <shrink_slab+480>:   stp     xzr, xzr, [x2,#96]
0xffff8000102d0a04 <shrink_slab+484>:   sxtw    x1, w22                 //shrinker_nr_max
0xffff8000102d0a08 <shrink_slab+488>:   stp     w23, wzr, [sp,#152]
0xffff8000102d0a0c <shrink_slab+492>:   str     x21, [sp,#160]
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 776
0xffff8000102d0a10 <shrink_slab+496>:   bl      0xffff800010a92ef8 <idr_find>
0xffff8000102d0a14 <shrink_slab+500>:   mov     x3, x0
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_8.aarch64/mm/vmscan.c: 777
0xffff8000102d0a18 <shrink_slab+504>:   cbz     x0, 0xffff8000102d0ac0 <shrink_slab+672>
0xffff8000102d0a1c <shrink_slab+508>:   ldr     x0, [x0,#32]            //shrinker->flags

//那到这里, x28 : shrinker_info
// x21 : memcg
// x23 : nid
0xffff8000102d0a20 <shrink_slab+512>:   and     x1, x0, #0x1
0xffff8000102d0a24 <shrink_slab+516>:   str     x1, [sp,#112]
```

