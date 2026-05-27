## 问题现象
```
[root@localhost ~]# /sbin/kexec -d -p --command-line='BOOT_IMAGE=/vmlinuz-6.6.52-1.10.1.lns23.jd.loongarch64 console=ttyS0,115200 console=tty0 earlycon=uart,mmio,0x1fe001e0 ro resume=/dev/mapper/server-swap rhgb LANG=en_US.UTF-8 cgroup.memory=nokmem vfio_iommu_type1.allow_unsafe_interrupts=1 nokaslr init 3 irqpoll reset_devices cgroup_disable=memory udev.children-max=2 panic=10 novmcoredd nokaslr crashkernel=1G@high' --initrd=/boot/initramfs-6.6.52-1.10.1.lns23.jd.loongarch64kdump.img /boot/vmlinuz-6.6.52-1.10.1.lns23.jd.loongarch64
arch_process_options:239: command_line: BOOT_IMAGE=/vmlinuz-6.6.52-1.10.1.lns23.jd.loongarch64 console=ttyS0,115200 console=tty0 earlycon=uart,mmio,0x1fe001e0 ro resume=/dev/mapper/server-swap rhgb LANG=en_US.UTF-8 cgroup.memory=nokmem vfio_iommu_type1.allow_unsafe_interrupts=1 nokaslr init 3 irqpoll reset_devices cgroup_disable=memory udev.children-max=2 panic=10 novmcoredd nokaslr crashkernel=1G@high
arch_process_options:241: initrd: /boot/initramfs-6.6.52-1.10.1.lns23.jd.loongarch64kdump.img
Try gzip decompression.
Try LZMA decompression.
kernel: 0x7fffec000010 kernel_size: 0x33abc18
MEMORY RANGES:
0000000000000000-000000000d880000 (0)
0000000000000000-0000000000200000 (1)
00000000036b7670-0000000007c04000 (1)
0000000008000000-0000000008038000 (1)
0000000008040000-0000000008078000 (1)
0000000008080000-00000000080b8000 (1)
00000000080c0000-00000000080f8000 (1)
0000000008100000-0000000008138000 (1)
0000000008140000-0000000008178000 (1)
0000000008180000-00000000081b8000 (1)
00000000081c0000-00000000081f8000 (1)
0000000008200000-0000000008238000 (1)
0000000008240000-0000000008278000 (1)
0000000008280000-00000000082b8000 (1)
00000000082c0000-00000000082f8000 (1)
0000000008300000-0000000008338000 (1)
0000000008340000-0000000008378000 (1)
0000000008380000-00000000083b8000 (1)
00000000083c0000-00000000083f8000 (1)
0000000008400000-0000000008438000 (1)
0000000008440000-0000000008478000 (1)
0000000008480000-00000000084b8000 (1)
00000000084c0000-00000000084f8000 (1)
0000000008500000-0000000008538000 (1)
0000000008540000-0000000008578000 (1)
0000000008580000-00000000085b8000 (1)
00000000085c0000-00000000085f8000 (1)
0000000008600000-0000000008638000 (1)
0000000008640000-0000000008678000 (1)
0000000008680000-00000000086b8000 (1)
00000000086c0000-00000000086f8000 (1)
0000000008700000-0000000008738000 (1)
0000000008740000-0000000008778000 (1)
0000000008780000-00000000087b8000 (1)
00000000087c0000-00000000087f8000 (1)
0000000008800000-0000000008838000 (1)
0000000008840000-0000000008878000 (1)
0000000008880000-00000000088b8000 (1)
00000000088c0000-00000000088f8000 (1)
0000000008900000-0000000008938000 (1)
0000000008940000-0000000008978000 (1)
0000000008980000-00000000089b8000 (1)
00000000089c0000-00000000089f8000 (1)
0000000008a00000-0000000008a38000 (1)
0000000008a40000-0000000008a78000 (1)
0000000008a80000-0000000008ab8000 (1)
0000000008ac0000-0000000008af8000 (1)
0000000008b00000-0000000008b38000 (1)
0000000008b40000-0000000008b78000 (1)
0000000008b80000-0000000008bb8000 (1)
0000000008bc0000-0000000008bf8000 (1)
0000000008c00000-0000000008c38000 (1)
0000000008c40000-0000000008c78000 (1)
0000000008c80000-0000000008cb8000 (1)
0000000008cc0000-0000000008cf8000 (1)
0000000008d00000-0000000008d38000 (1)
0000000008d40000-0000000008d78000 (1)
0000000008d80000-0000000008db8000 (1)
0000000008dc0000-0000000008df8000 (1)
0000000008e00000-0000000008e38000 (1)
0000000008e40000-0000000008e78000 (1)
0000000008e80000-0000000008eb8000 (1)
0000000008ec0000-0000000008ef8000 (1)
0000000008f00000-0000000008f38000 (1)
elf_loongarch_load: kernel_segment: 00000000c1000000
elf_loongarch_load: image_size:     00000000034c0000
elf_loongarch_load: text_offset:    0000000000200000
elf_loongarch_load: phys_offset:    0000000000000000
elf_loongarch_load: PE format:      no
Reserved memory range
00000000c0000000-00000000ffffffff (0)
Coredump memory ranges
0000000000000000-000000000d87ffff (0)
000000000d890000-000000000dc2ffff (0)
000000000de50000-000000000deaffff (0)
000000000dfd0000-000000000e0affff (0)
000000000e220000-000000000e23ffff (0)
000000000e420000-000000000f49ffff (0)
000000000f4f0000-000000000fffffff (0)
0000000080000000-00000000bfffffff (0)
0000000100000000-000000046fffffff (0)
kernel symbol _text vaddr = 9000000000200000
get_crash_notes_per_cpu: crash_notes addr = 8031800, size = 512
Elf header: p_type = 4, p_offset = 0x8031800 p_paddr = 0x8031800 p_vaddr = 0x0 p_filesz = 0x200 p_memsz = 0x200
get_crash_notes_per_cpu: crash_notes addr = 8071800, size = 512
Elf header: p_type = 4, p_offset = 0x8071800 p_paddr = 0x8071800 p_vaddr = 0x0 p_filesz = 0x200 p_memsz = 0x200
get_crash_notes_per_cpu: crash_notes addr = 80b1800, size = 512
Elf header: p_type = 4, p_offset = 0x80b1800 p_paddr = 0x80b1800 p_vaddr = 0x0 p_filesz = 0x200 p_memsz = 0x200
get_crash_notes_per_cpu: crash_notes addr = 80f1800, size = 512
Elf header: p_type = 4, p_offset = 0x80f1800 p_paddr = 0x80f1800 p_vaddr = 0x0 p_filesz = 0x200 p_memsz = 0x200
get_crash_notes_per_cpu: crash_notes addr = 8131800, size = 512
Elf header: p_type = 4, p_offset = 0x8131800 p_paddr = 0x8131800 p_vaddr = 0x0 p_filesz = 0x200 p_memsz = 0x200
get_crash_notes_per_cpu: crash_notes addr = 8171800, size = 512
Elf header: p_type = 4, p_offset = 0x8171800 p_paddr = 0x8171800 p_vaddr = 0x0 p_filesz = 0x200 p_memsz = 0x200
get_crash_notes_per_cpu: crash_notes addr = 81b1800, size = 512
Elf header: p_type = 4, p_offset = 0x81b1800 p_paddr = 0x81b1800 p_vaddr = 0x0 p_filesz = 0x200 p_memsz = 0x200
get_crash_notes_per_cpu: crash_notes addr = 81f1800, size = 512
Elf header: p_type = 4, p_offset = 0x81f1800 p_paddr = 0x81f1800 p_vaddr = 0x0 p_filesz = 0x200 p_memsz = 0x200
vmcoreinfo header: p_type = 4, p_offset = 0x101048000 p_paddr = 0x101048000 p_vaddr = 0x0 p_filesz = 0x4024 p_memsz = 0x4024
Kernel text Elf header: p_type = 1, p_offset = 0x200000 p_paddr = 0x200000 p_vaddr = 0x9000000000200000 p_filesz = 0x339b200 p_memsz = 0x339b200
Elf header: p_type = 1, p_offset = 0x0 p_paddr = 0x0 p_vaddr = 0x9000000000000000 p_filesz = 0xd880000 p_memsz = 0xd880000
Elf header: p_type = 1, p_offset = 0xd890000 p_paddr = 0xd890000 p_vaddr = 0x900000000d890000 p_filesz = 0x3a0000 p_memsz = 0x3a0000
Elf header: p_type = 1, p_offset = 0xde50000 p_paddr = 0xde50000 p_vaddr = 0x900000000de50000 p_filesz = 0x60000 p_memsz = 0x60000
Elf header: p_type = 1, p_offset = 0xdfd0000 p_paddr = 0xdfd0000 p_vaddr = 0x900000000dfd0000 p_filesz = 0xe0000 p_memsz = 0xe0000
Elf header: p_type = 1, p_offset = 0xe220000 p_paddr = 0xe220000 p_vaddr = 0x900000000e220000 p_filesz = 0x20000 p_memsz = 0x20000
Elf header: p_type = 1, p_offset = 0xe420000 p_paddr = 0xe420000 p_vaddr = 0x900000000e420000 p_filesz = 0x1080000 p_memsz = 0x1080000
Elf header: p_type = 1, p_offset = 0xf4f0000 p_paddr = 0xf4f0000 p_vaddr = 0x900000000f4f0000 p_filesz = 0xb10000 p_memsz = 0xb10000
Elf header: p_type = 1, p_offset = 0x80000000 p_paddr = 0x80000000 p_vaddr = 0x9000000080000000 p_filesz = 0x40000000 p_memsz = 0x40000000
Elf header: p_type = 1, p_offset = 0x100000000 p_paddr = 0x100000000 p_vaddr = 0x9000000100000000 p_filesz = 0x370000000 p_memsz = 0x370000000
Could not find a free area of memory of 0x4000 bytes...
locate_hole failed
```

## 合入patch
```
commit 677dd2f8bcf765f6b6fdc5b07ec4df706b670f85
Author: Ming Wang <wangming01@loongson.cn>
Date:   Wed Mar 4 09:19:14 2026 +0800

    LoongArch: Increase MAX_MEMORY_RANGES to 1024

    On some LoongArch systems, especially multi-socket machines like 3D6000,
    the current limit of MAX_MEMORY_RANGES (64) is too small to accommodate
    all memory range entries in /proc/iomem.

    When the limit is reached, get_memory_ranges() stops parsing, which causes
    kexec to fail with "locate_hole failed" because it cannot correctly identify
    the crash kernel reserved memory or necessary RAM segments.

    Increase MAX_MEMORY_RANGES to 1024 to support systems with complex
    memory layouts.

    Signed-off-by: Ming Wang <wangming01@loongson.cn>
    Signed-off-by: Simon Horman <horms@kernel.org>

diff --git a/kexec/arch/loongarch/kexec-loongarch.h b/kexec/arch/loongarch/kexec-loongarch.h
index 2e65eb0..895809c 100644
--- a/kexec/arch/loongarch/kexec-loongarch.h
+++ b/kexec/arch/loongarch/kexec-loongarch.h
@@ -8,7 +8,7 @@
 #define BOOT_BLOCK_VERSION 17
 #define BOOT_BLOCK_LAST_COMP_VERSION 16

-#define MAX_MEMORY_RANGES 64
+#define MAX_MEMORY_RANGES 1024
 #define MAX_LINE 160
```
