## 测试热迁移
源端启动命令:
```
qemu-system-loongarch64 -cpu la464 -accel kvm -m 1G --nographic --serial tcp:localhost:6666,server,nowait -monitor stdio -smp 4
```

目的端启动命令:
```
qemu-system-loongarch64 -cpu la464 -accel kvm -m  1G --nographic --incoming tcp:localhost:12345 -smp 4
```

源端monitor 执行
```
migrate tcp:localhost:12345
```

目的端打印:
```
runstate_set current_run_state 1 (inmigrate) new_state 9 (running)
vm_state_notify running 1 reason 9 (running)
virtio_set_status vdev 0x555572c62c40 val 0
migrate_set_state new state completed
```

热迁移执行成功, 在源端monitor执行`info migrate`
```
(qemu) info migrate
globals:
store-global-state: on
only-migratable: off
send-configuration: on
send-section-footer: on
decompress-error-check: on
clear-bitmap-shift: 18
Migration status: completed
total time: 774 ms
downtime: 2 ms
setup: 4 ms
transferred ram: 2651 kbytes
throughput: 28.21 mbps
remaining ram: 0 kbytes
total ram: 1081648 kbytes
duplicate: 270364 pages
skipped: 0 pages
normal: 48 pages
normal bytes: 192 kbytes
dirty sync count: 3
page size: 4 kbytes
multifd bytes: 0 kbytes
pages-per-second: 323080
precopy ram: 2568 kbytes
```

## virsh dump
虚拟机环境信息

* **kernel**: 6.6.0-97.0.0.102.oe2403sp2.loongarch64
* **kernel cmdline**: 增加 onkaslr

dump后，使用crash 调试该转储文件
```
invalid
crash: invalid kernel virtual address: ccccccccccccccd4  type: "first vmlist addr"

Errors like the one above typically occur when the kernel and memory source
do not match.  These are the files being used:

      KERNEL: /usr/lib/debug/lib/modules/6.6.0-97.0.0.102.oe2403sp2.loongarch64/vmlinux
    DUMPFILE: vm.dump
```

## host kexec-tools
物理机环境信息
* **kernel** : 6.6.0-104.0.0.110.loongarch64
* **kexec-tools 版本** : kexec-tools-2.0.26-12.oe2403sp2.loongarch64

`kdump`服务启动失败:
```
[root@localhost kdump]# kdumpctl start
grep: unrecognized option '--mount'
Usage: grep [OPTION]... PATTERNS [FILE]...
grep: Try 'grep --help' for more information.
warning: stray \ before -
Error: /boot/vmlinux-6.6.0-104.0.0.110.loongarch64 not found.
Starting kdump: [FAILED]
```
查看`kdumpctl`脚本，发现由`/etc/sysconfig/kdump`定义`KDUMP_IMG`前缀:
```
KDUMP_IMG="vmlinux"
```

将其修改为`vmlinuz`后，在执行`kdumpctl start` 命令,
发现报如下错误:
```sh
kexec_load failed: Cannot assign requested address
entry       = 0x15c9000 flags = 0x1020001
nr_segments = 4
segment[0].buf   = 0x7fffef48c010
segment[0].bufsz = 0x322bc00
segment[0].mem   = 0x200000
segment[0].memsz = 0x3344000
segment[1].buf   = 0x7fffed2f0010
segment[1].bufsz = 0x2189e00
segment[1].mem   = 0xb0040000
segment[1].memsz = 0x218c000
segment[2].buf   = 0x555557934090
segment[2].bufsz = 0x200
segment[2].mem   = 0xb21cc000
segment[2].memsz = 0x4000
segment[3].buf   = 0x5555800d1d40
segment[3].bufsz = 0x2000
segment[3].mem   = 0xecdfc000
segment[3].memsz = 0x4000
kexec: failed to load kdump kernel
Starting kdump: [FAILED]
```
dmesg中无报错.
<!--
调试，找到kexec命令cmdline
```
/sbin/kexec -p '--command-line=BOOT_IMAGE=/vmlinuz-6.6.0-104.0.0.110.loongarch64 \
 ro video=efifb:off cgroup_disable=files apparmor=0 console=tty0 \
 console=ttyS0,115200 loongarch_iommu=on iommu=pt  \
 vfio_iommu_type1.allow_unsafe_interrupts=1 nokaslr nr_cpus=1 \
 init 3 irqpoll reset_devices cgroup_disable=memory \
 udev.children-max=2 panic=10 novmcoredd'  \
 --initrd=/boot/initramfs-6.6.0-104.0.0.110.loongarch64kdump.img /boot/vmlinuz-6.6.0-104.0.0.110.loongarch64
kexec_load failed: Cannot assign requested address
```
-->
