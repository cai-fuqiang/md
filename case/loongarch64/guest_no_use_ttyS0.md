## 虚拟机启动后没有反应

使用下面启动参数启动虚拟机:
```sh
IMG=openEuler-24.03-LTS-SP2-loongarch64.qcow2

qemu-system-loongarch64 -name guest=centos8 \
        -machine virt,accel=kvm \
        -m 4G \
        -smp 8          \
        -nodefaults -boot strict=on     \
        -name wang,debug-threads=on \
         -drive file=/usr/share/edk2/loongarch64/QEMU_EFI-silent-pflash.raw,if=pflash,format=raw,readonly=on \
        -boot strict=on \
        -qmp tcp:localhost:9999,server,nowait \
        -drive file=$(pwd)/${IMG} \
        -msg timestamp=on \
        -nographic   \
        -monitor stdio \
        -serial telnet:localhost:66668,server,nowait -gdb tcp::1237 \
```

启动后，在grub选择内核启动, 启动卡在如下界面:
```
Loading Linux 6.6.0-97.0.0.102.oe2403sp2.loongarch64 ...
Loading initial ramdisk ...
PROGRESS CODE: V03101019 I0
```

查看`registers`,  疑似卡在idle
```
PC=90000000002220ec

crash> dis 90000000002220ec
0x90000000002220ec <__arch_cpu_idle+12>:        jirl            $zero, $ra, 0
crash> dis __arch_cpu_idle
0x90000000002220e0 <__arch_cpu_idle>:   ori             $t0, $zero, 0x4
0x90000000002220e4 <__arch_cpu_idle+4>: csrxchg         $t0, $t0, 0x0
0x90000000002220e8 <__arch_cpu_idle+8>: idle            0x0
0x90000000002220ec <__arch_cpu_idle+12>:        jirl            $zero, $ra, 0
```

查看vcpu堆栈:
```
[root@localhost vm]# ps -o comm,pid,tid -p `pidof qemu-system-loongarch64` -T  |grep 'CPU 0'
CPU 0/KVM        147103  147109
[root@localhost vm]# cat /proc/147109/stack
[<0>] kvm_vcpu_block+0x48/0x90 [kvm]
[<0>] kvm_vcpu_halt+0x64/0x4f0 [kvm]
[<0>] kvm_emu_idle+0x8c/0xa8 [kvm]
[<0>] kvm_trap_handle_gspr+0x7c/0x2f0 [kvm]
[<0>] kvm_handle_gspr+0x18/0xa8 [kvm]
[<0>] kvm_handle_exit+0x20c/0x320 [kvm]
```

可以发现vcpu 确实处于idle

怀疑虚拟机正常，但是kernel cmdline 配置的不太对, 修改kernel cmdline
+ 去掉`quiet`
+ 增加`console=tty0 console=ttyS0,115200`

启动后, 虚拟机正常启动。
