```
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/fs/overlayfs/readdir.c: 69
0xffff00000a75ba60 <ovl_cache_entry_find_link.constprop.11>:    stp     x29, x30, [sp,#-80]!
0xffff00000a75ba64 <ovl_cache_entry_find_link.constprop.11+4>:  mov     x29, sp
0xffff00000a75ba68 <ovl_cache_entry_find_link.constprop.11+8>:  stp     x19, x20, [sp,#16]
0xffff00000a75ba6c <ovl_cache_entry_find_link.constprop.11+12>: stp     x21, x22, [sp,#32]
0xffff00000a75ba70 <ovl_cache_entry_find_link.constprop.11+16>: stp     x23, x24, [sp,#48]
0xffff00000a75ba74 <ovl_cache_entry_find_link.constprop.11+20>: str     x25, [sp,#64]
0xffff00000a75ba78 <ovl_cache_entry_find_link.constprop.11+24>: mov     x25, x2		//***link
0xffff00000a75ba7c <ovl_cache_entry_find_link.constprop.11+28>: sxtw    x21, w1		//len
0xffff00000a75ba80 <ovl_cache_entry_find_link.constprop.11+32>: mov     x22, x0		//name
0xffff00000a75ba84 <ovl_cache_entry_find_link.constprop.11+36>: mov     x23, x3		//parent
0xffff00000a75ba88 <ovl_cache_entry_find_link.constprop.11+40>: mov     x0, x30
0xffff00000a75ba8c <ovl_cache_entry_find_link.constprop.11+44>: nop
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/fs/overlayfs/readdir.c: 74
0xffff00000a75ba90 <ovl_cache_entry_find_link.constprop.11+48>: ldr     x20, [x25]	//new_wp = *link
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/fs/overlayfs/readdir.c: 76
0xffff00000a75ba94 <ovl_cache_entry_find_link.constprop.11+52>: mov     x24, x21        //x24 = len
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/fs/overlayfs/readdir.c: 80
0xffff00000a75ba98 <ovl_cache_entry_find_link.constprop.11+56>: ldr     x19, [x20]	//x19 为 *newp
0xffff00000a75ba9c <ovl_cache_entry_find_link.constprop.11+60>: mov     x2, x21         //len
0xffff00000a75baa0 <ovl_cache_entry_find_link.constprop.11+64>: mov     x0, x22         //name
0xffff00000a75baa4 <ovl_cache_entry_find_link.constprop.11+68>: add     x1, x19, #0x22  //*newp + 0x22 = name
0xffff00000a75baa8 <ovl_cache_entry_find_link.constprop.11+72>: cbz     x19, 0xffff00000a75bad4 <ovl_cache_entry_find_link.constprop.11+116> //是0跳转
0xffff00000a75baac <ovl_cache_entry_find_link.constprop.11+76>: str     x19, [x23]	//*parent = *newp, x19为*newp, 也就是struct rb_node *类型
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/fs/overlayfs/readdir.c: 82
0xffff00000a75bab0 <ovl_cache_entry_find_link.constprop.11+80>: bl      0xffff00001093d904 <__efistub_strncmp>
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/fs/overlayfs/readdir.c: 85
0xffff00000a75bab4 <ovl_cache_entry_find_link.constprop.11+84>: cmp     w0, #0x0        // x0为compare的值
0xffff00000a75bab8 <ovl_cache_entry_find_link.constprop.11+88>: b.le    0xffff00000a75baf4 <ovl_cache_entry_find_link.constprop.11+148> //小于等于跳转
0xffff00000a75babc <ovl_cache_entry_find_link.constprop.11+92>: add     x20, x19, #0x8	//x19+8 rb_node.rb_right,  x20为
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/fs/overlayfs/readdir.c: 80
0xffff00000a75bac0 <ovl_cache_entry_find_link.constprop.11+96>: mov     x2, x21			//len
0xffff00000a75bac4 <ovl_cache_entry_find_link.constprop.11+100>:        mov     x0, x22		//name
0xffff00000a75bac8 <ovl_cache_entry_find_link.constprop.11+104>:        ldr     x19, [x20]	//x20为rb_right, 获取rb_right 值
0xffff00000a75bacc <ovl_cache_entry_find_link.constprop.11+108>:        add     x1, x19, #0x22	//rb_right + 0x22, name
0xffff00000a75bad0 <ovl_cache_entry_find_link.constprop.11+112>:        cbnz    x19, 0xffff00000a75baac <ovl_cache_entry_find_link.constprop.11+76> //再次进行name比较
0xffff00000a75bad4 <ovl_cache_entry_find_link.constprop.11+116>:        str     x20, [x25]			//[x25] = *link, *link = newp
0xffff00000a75bad8 <ovl_cache_entry_find_link.constprop.11+120>:        mov     w0, #0x0                        // #0
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.el8_1.5es.14.aarch64/fs/overlayfs/readdir.c: 92
0xffff00000a75badc <ovl_cache_entry_find_link.constprop.11+124>:        ldp     x19, x20, [sp,#16]
0xffff00000a75bae0 <ovl_cache_entry_find_link.constprop.11+128>:        ldp     x21, x22, [sp,#32]
0xffff00000a75bae4 <ovl_cache_entry_find_link.constprop.11+132>:        ldp     x23, x24, [sp,#48]
0xffff00000a75bae8 <ovl_cache_entry_find_link.constprop.11+136>:        ldr     x25, [sp,#64]
0xffff00000a75baec <ovl_cache_entry_find_link.constprop.11+140>:        ldp     x29, x30, [sp],#80
0xffff00000a75baf0 <ovl_cache_entry_find_link.constprop.11+144>:        ret
```

所以，从上面看 x19为parent, 查看parent:
```

```

# 其他信息
```
crash> rb_read 0xffff808383f1dfa0
the rb_root is 0xffff808383f1dfa0, rb_root->rb_node is ffff808383f151a8
0:ovl_cache_entry addr = ffff808383f1c780
        node=ffff808383f1c7a8
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f1e4a9
1:ovl_cache_entry addr = ffff808383f1e480
        node=ffff808383f1e4a8
        node->rb_right=ffff808383f12828
        node->rb_left=ffff808383f1c7a8
        node->rb_parent_color=0xffff808383f1ed29
2:ovl_cache_entry addr = ffff808383f12800
        node=ffff808383f12828
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f1e4a9
3:ovl_cache_entry addr = ffff808383f1ed00
        node=ffff808383f1ed28
        node->rb_right=ffff808383f10ca8
        node->rb_left=ffff808383f1e4a8
        node->rb_parent_color=0xffff808383f151a9
4:ovl_cache_entry addr = ffff808383f1cf00
        node=ffff808383f1cf28
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f10ca9
5:ovl_cache_entry addr = ffff808383f10c80
        node=ffff808383f10ca8
        node->rb_right=ffff808383f149a8
        node->rb_left=ffff808383f1cf28
        node->rb_parent_color=0xffff808383f1ed29
6:ovl_cache_entry addr = ffff808383f14980
        node=ffff808383f149a8
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f10ca9
7:ovl_cache_entry addr = ffff808383f15180
        node=ffff808383f151a8
        node->rb_right=ffff808383f1ae28
        node->rb_left=ffff808383f1ed28
        node->rb_parent_color=0x1
8:ovl_cache_entry addr = ffff808383f16780
        node=ffff808383f167a8
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f1a8a9
9:ovl_cache_entry addr = ffff808383f1a880
        node=ffff808383f1a8a8
        node->rb_right=ffff808383f1e628
        node->rb_left=ffff808383f167a8
        node->rb_parent_color=0xffff808383f1ae29
10:ovl_cache_entry addr = ffff808383f1e600
        node=ffff808383f1e628
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f1a8a9
11:ovl_cache_entry addr = ffff808383f1ae00
        node=ffff808383f1ae28
        node->rb_right=ffff808383f196a8
        node->rb_left=ffff808383f1a8a8
        node->rb_parent_color=0xffff808383f151a9
12:ovl_cache_entry addr = ffff808383f19c00
        node=ffff808383f19c28
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f15d29
13:ovl_cache_entry addr = ffff808383f15d00
        node=ffff808383f15d28
        node->rb_right=ffff808383f11ea8
        node->rb_left=ffff808383f19c28
        node->rb_parent_color=0xffff808383f196a9
14:ovl_cache_entry addr = ffff808383f11e80
        node=ffff808383f11ea8
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f15d29
15:ovl_cache_entry addr = ffff808383f19680
        node=ffff808383f196a8
        node->rb_right=ffff808383f1aa28
        node->rb_left=ffff808383f15d28
        node->rb_parent_color=0xffff808383f1ae28
16:ovl_cache_entry addr = ffff808383f18880
        node=ffff808383f188a8
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f1aa29
17:ovl_cache_entry addr = ffff808383f1aa00
        node=ffff808383f1aa28
        node->rb_right=ffff808383f15928
        node->rb_left=ffff808383f188a8
        node->rb_parent_color=0xffff808383f196a9
18:ovl_cache_entry addr = ffff808383f15880
        node=ffff808383f158a8
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f15929
19:ovl_cache_entry addr = ffff808383f15900
        node=ffff808383f15928
        node->rb_right=ffff808383f172a5
        node->rb_left=ffff808383f158a8
        node->rb_parent_color=0xffff808383f1aa28
20:ovl_cache_entry addr = ffff808383f1d800
        node=ffff808383f1d828
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f172a5
21:ovl_cache_entry addr = ffff808383f1727c
        node=ffff808383f172a4
        node->rb_right=ffff808383f12ba8
        node->rb_left=ff808383f1d828ff
        node->rb_parent_color=0xff808383f15929ff
22:ovl_cache_entry addr = ffff808383f1cf80
        node=ffff808383f1cfa8
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f12ba8
23:ovl_cache_entry addr = ffff808383f12b80
        node=ffff808383f12ba8
        node->rb_right=ffff808383f191a8
        node->rb_left=ffff808383f1cfa8
        node->rb_parent_color=0xffff808383f172a5
24:ovl_cache_entry addr = ffff808383f19180
        node=ffff808383f191a8
        node->rb_right=0
        node->rb_left=0
        node->rb_parent_color=0xffff808383f12ba8
```
