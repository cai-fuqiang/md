# 获取offset

## 获取PC寄存器
```
virsh # qemu-monitor-command --domain instance-00004c93 --hmp 'info registers'
 PC=ffffc01b0ea0321c X00=ffff30c8fa64c598 X01=ffff70adea540000
```

## 获取指令
```
virsh # qemu-monitor-command --domain instance-00004c93 --hmp 'x/10i 0xffffc01b0ea0321c'
0xffffc01b0ea0321c:  a94d6ffa  ldp      x26, x27, [sp, #0xd0]
0xffffc01b0ea03220:  a94e77fc  ldp      x28, x29, [sp, #0xe0]
0xffffc01b0ea03224:  f9407bfe  ldr      x30, [sp, #0xf0]
0xffffc01b0ea03228:  910503ff  add      sp, sp, #0x140
0xffffc01b0ea0322c:  d69f03e0  eret
0xffffc01b0ea03230:  d503379f  dsb      nsh
0xffffc01b0ea03234:  d5033fdf  isb
0xffffc01b0ea03238:  d503201f  nop
0xffffc01b0ea0323c:  a94e77fc  ldp      x28, x29, [sp, #0xe0]
0xffffc01b0ea03240:  f9407bfe  ldr      x30, [sp, #0xf0]

virsh # qemu-monitor-command --domain instance-00004c93 --hmp 'x/1xg 0xffffc01b0ea03220'
ffffc01b0ea03220: 0xf9407bfea94e77fc
```

## 在另一个虚拟机上search 上面指令值
```
[root@node-2 ~]# cat /proc/kallsyms |grep -E ' _text| _end'
ffff800010080000 T _text
ffff800013430000 B _end
crash> search -s ffff800010080000 -e ffff800013430000 0xa94b5ff6a94a57f4
ffff800010082f60: a94b5ff6a94a57f4
ffff800010083098: a94b5ff6a94a57f4
ffff800010083210: a94b5ff6a94a57f4
ffff800010083888: a94b5ff6a94a57f4
ffff80001009fe00: a94b5ff6a94a57f4
```

kaslr 在做偏移的时候,是有对其的(好像是2M), 所以,该地址肯定是`ffff800010083210`,
对照下:
```
virsh # qemu-monitor-command --domain instance-00004c93 --hmp 'x/10xg 0xffffc01b0ea03210'
ffffc01b0ea03210: 0xa94b5ff6a94a57f4 0xa94d6ffaa94c67f8
ffffc01b0ea03220: 0xf9407bfea94e77fc 0xd69f03e0910503ff
ffffc01b0ea03230: 0xd5033fdfd503379f 0xd503201fd503201f
ffffc01b0ea03240: 0xa9010fe2a90007e0 0xa9031fe6a90217e4
ffffc01b0ea03250: 0xa9052feaa90427e8 0xa9073feea90637ec

0xffff800010083210 <el1_irq+272>:       0xa94b5ff6a94a57f4      0xa94d6ffaa94c67f8
0xffff800010083220 <el1_irq+288>:       0xf9407bfea94e77fc      0xd69f03e0910503ff
0xffff800010083230 <el1_irq+304>:       0xd5033fdfd503379f      0xd503201fd503201f
0xffff800010083240 <el0_sync>:  0xa9010fe2a90007e0      0xa9031fe6a90217e4
0xffff800010083250 <el0_sync+16>:       0xa9052feaa90427e8      0xa9073feea90637ec
```

可以发现其值完全一样.

计算offset
```
crash> p (char *)(0xffffc01b0ea03210-0xffff800010083210)
$1 = 0x401afe980000 <Address 0x401afe980000 out of bounds>
```

# 获取kimage_voffset
```
crash> p &kimage_voffset
$2 = (u64 *) 0xffff800010e8c6f0 <kimage_voffset>
crash> p (char *)(0xffff800010e8c6f0+0x401afe980000)
$3 = 0xffffc01b0f80c6f0 p: invalid kernel virtual address: ffffc01b0f80c6f0  type: "gdb_readmem_callback"
p: invalid kernel virtual address: ffffc01b0f80c6f0  type: "gdb_readmem_callback"
<Address 0xffffc01b0f80c6f0 out of bounds>
virsh # qemu-monitor-command --domain instance-00004c93 --hmp 'x/1xg 0xffffc01b0f80c6f0'
ffffc01b0f80c6f0: 0xffffc014b0a00000
```

# 获取 phys_offset -- memstart_addr
```
crash> p &memstart_addr
$4 = (s64 *) 0xffff800010e8c6d8 <memstart_addr>
crash> p (char *)(0xffff800010e8c6d8+0x401afe980000)
$5 = 0xffffc01b0f80c6d8 p: invalid kernel virtual address: ffffc01b0f80c6d8  type: "gdb_readmem_callback"
p: invalid kernel virtual address: ffffc01b0f80c6d8  type: "gdb_readmem_callback"
<Address 0xffffc01b0f80c6d8 out of bounds>
virsh # qemu-monitor-command --domain instance-00004c93 --hmp 'x/1xg 0xffffc01b0f80c6d8'
ffffc01b0f80c6d8: 0xffffcf4740000000
```

# 获取kaslr
```cpp
static inline unsigned long kaslr_offset(void)
{
        return kimage_vaddr - KIMAGE_VADDR;
}
```

所以需要获取 `kimage_vaddr`
```
crash> p &kimage_vaddr
$6 = (u64 *) 0xffff800010aba000 <kimage_vaddr>
crash> p (char *)(0xffff800010aba000+0x401afe980000)
$7 = 0xffffc01b0f43a000 p: invalid kernel virtual address: ffffc01b0f43a000  type: "gdb_readmem_callback"
p: invalid kernel virtual address: ffffc01b0f43a000  type: "gdb_readmem_callback"
<Address 0xffffc01b0f43a000 out of bounds>
virsh # qemu-monitor-command --domain instance-00004c93 --hmp 'x/1xg 0xffffc01b0f43a000'
ffffc01b0f43a000: 0xffffc01b0e980000

crash> p (char *)(0xffffc01b0e980000-0xffff800010000000)
$1 = 0x401afe980000 <Address 0x401afe980000 out of bounds>
```

# 执行 crash 命令
```

[root@node-1 wangfuqiang]# cat crash.sh
crash instance-00004c93-0109.vmcore usr/lib/debug/lib/modules/4.18.0-372.19.1.es8_8.aarch64/vmlinux -m vabits_actual=48 -m kimage_voffset=0xffffc014b0a00000 --kaslr 0x401afe980000 -m phys_offset=0xffffcf4740000000
[root@node-1 wangfuqiang]# sh crash.sh

crash 7.2.9-2.es8
Copyright (C) 2002-2020  Red Hat, Inc.
Copyright (C) 2004, 2005, 2006, 2010  IBM Corporation
Copyright (C) 1999-2006  Hewlett-Packard Co
Copyright (C) 2005, 2006, 2011, 2012  Fujitsu Limited
Copyright (C) 2006, 2007  VA Linux Systems Japan K.K.
Copyright (C) 2005, 2011  NEC Corporation
Copyright (C) 1999, 2002, 2007  Silicon Graphics, Inc.
Copyright (C) 1999, 2000, 2001, 2002  Mission Critical Linux, Inc.
This program is free software, covered by the GNU General Public License,
and you are welcome to change it and/or distribute copies of it under
certain conditions.  Enter "help copying" to see the conditions.
This program has absolutely no warranty.  Enter "help warranty" for details.

WARNING: ignoring --machdep option: vabits_actual=48
NOTE: setting kimage_voffset to: 0xffffc014b0a00000

NOTE: setting phys_offset to: 0xffffcf4740000000

GNU gdb (GDB) 7.6
Copyright (C) 2013 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "aarch64-unknown-linux-gnu"...

WARNING: kernel relocated [67219433MB]: patching 94279 gdb minimal_symbol values

crash: invalid kernel virtual address: ffff30c8fa5c0058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fa660058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fa700058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fa7a0058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fa840058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fa8e0058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fa980058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8faa20058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8faac0058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fab60058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fac00058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8faca0058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fad40058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fade0058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8fae80058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8faf20058  type: "IRQ stack pointer"
crash: invalid kernel virtual address: ffff30c8ff7ca380  type: "memory section root table"
```

# 调试node-6: instance-00004cd5

