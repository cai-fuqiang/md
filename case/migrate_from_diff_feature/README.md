## migrate from diff model

### 测试环境
||目的节点|源节点|
|----|----|----|
|cpu|Intel(R) Xeon(R) CPU E5-2640 v3 @ 2.60GHz|INTEL(R) XEON(R) PLATINUM 8538C Unknown CPU @ 2.6GHz|

### 直接使用 `-host` 参数测试
dst qemu cmd:
```sh
/usr/bin/qemu-system-x86_64 -cpu host --no-reboot -nodefaults  -machine accel=kvm  \
-nographic -monitor stdio -serial tcp:localhost:12347,server,nowait \
-incoming tcp:10.226.134.73:5555
```
src qemu cmd:
```sh
qemu-system-x86_64 -cpu host --no-reboot -nodefaults  \
-machine accel=kvm -nographic -monitor stdio  \
-serial tcp:localhost:12347,server,nowait
```
执行迁移命令:
```
(qemu) migrate tcp:10.226.134.73:5555
## 执行成功

目的端:
(qemu) info status
VM status: running

源端:
(qemu) info status
VM status: paused (postmigrate)
```

### 在同一个节点使用cpuid kvm-units-test测试
执行下面脚本:
```sh
VMX_OP=""
TSC_ADJ=""
LOG_FILE=""

test_one() {
        /usr/local/bin/qemu-system-x86_64 -cpu Haswell-noTSX,vmx=$VMX_OP,tsc_adjust=$TSC_ADJ --no-reboot -nodefaults -device pc-testdev -device isa-debug-exit,iobase=0xf4,iosize=0x4 -vnc none -serial stdio -device pci-testdev -machine accel=kvm -kernel x86/cpuid.flat > $LOG_FILE
}
export VMX_OP=on
export TSC_ADJ=on
export LOG_FILE=vmx_on_tsc_on.txt
test_one
export VMX_OP=off
export TSC_ADJ=off
export LOG_FILE=vmx_off_tsc_off.txt
test_one
```
比较两个cpuid:
```diff
[root@A06-R08-I134-73-919XB72 cpuid.flat]# diff -Naru vmx_on_tsc_on.txt vmx_off_tsc_off.txt
--- vmx_on_tsc_on.txt   2025-07-30 20:07:12.993688548 +0800
+++ vmx_off_tsc_off.txt 2025-07-30 20:07:18.164688684 +0800
@@ -1,6 +1,6 @@
    Leaf      Subleaf       EAX        EBX        ECX        EDX
 00000000 00000000: 0000000d 756e6547 6c65746e 49656e69
-00000001 00000000: 000306c1 00000800 f7fa3223 078bfbff
+00000001 00000000: 000306c1 00000800 f7fa3203 078bfbff
 00000002 00000000: 00000001 00000000 0000004d 002c307d
 00000003 00000000: 00000000 00000000 00000000 00000000
 00000004 00000000: 00000121 01c0003f 0000003f 00000001
@@ -9,7 +9,7 @@
 00000004 00000003: 00000163 03c0003f 00003fff 00000006
 00000005 00000000: 00000000 00000000 00000003 00000000
 00000006 00000000: 00000004 00000000 00000000 00000000
-00000007 00000000: 00000000 000007ab 00000000 00000000
+00000007 00000000: 00000000 000007a9 00000000 00000000
 00000008 00000000: 00000000 00000000 00000000 00000000
 00000009 00000000: 00000000 00000000 00000000 00000000
 0000000a 00000000: 00000000 00000000 00000000 00000000
```
对照intel spec 发现确实差别在`vmx` 以及`tsc_adjust`, 尝试执行热迁移命令:

目的端:
```
[root@A06-R08-I134-73-919XB72 cpuid.flat]# cat dst.sh
## feature 多的作为源端
/usr/local/bin/qemu-system-x86_64 -cpu Haswell-noTSX,vmx=off,tsc_adjust=off -machine accel=kvm \
        -monitor stdio \
        -serial tcp:localhost:66667,server,nowait  \
        -nographic \
        -incoming tcp:localhost:5555
```
源端:
```
## feature 多的作为源端
/usr/local/bin/qemu-system-x86_64 -cpu Haswell-noTSX,vmx=on,tsc_adjust=on -machine accel=kvm \
        -monitor stdio \
        -serial tcp:localhost:66668,server,nowait  \
        -nographic
```

在源端迁移后，并不报错，迁移成功:
```
(qemu) migrate tcp:localhost:5555
(qemu)
(qemu)
```

## 使用上个篇章的命令行参数启动虚拟机

在源端
```
vmx=on,tsc_adjust=on
```
目的端
```
vmx=off,tsc_adjust=off
```
统计cpuid对比如下:

```diff
[root@A06-R08-I134-73-919XB72 wangfuqiang49]# diff -Naru cpuid_src.txt cpuid_dst.txt
--- cpuid_src.txt       2025-07-30 20:27:53.979721216 +0800
+++ cpuid_dst.txt       2025-07-30 20:28:34.242722276 +0800
@@ -1,5 +1,5 @@
 00000000 00000000: 0000000d 756e6547 6c65746e 49656e69
-00000001 00000000: 000306c1 05080800 fffa3223 178bfbff
+00000001 00000000: 000306c1 05080800 fffa3203 178bfbff
 00000002 00000000: 00000001 00000000 0000004d 002c307d
 00000003 00000000: 00000000 00000000 00000000 00000000
 00000004 00000000: 1c000121 01c0003f 0000003f 00000001
@@ -8,7 +8,7 @@
 00000004 00000003: 1c01c163 03c0003f 00003fff 00000006
 00000005 00000000: 00000000 00000000 00000003 00000000
 00000006 00000000: 00000004 00000000 00000000 00000000
-00000007 00000000: 00000000 000007ab 00000000 00000000
+00000007 00000000: 00000000 000007a9 00000000 00000000
 00000008 00000000: 00000000 00000000 00000000 00000000
 00000009 00000000: 00000000 00000000 00000000 00000000
 0000000a 00000000: 00000000 00000000 00000000 00000000
```
可见，迁移完后cpu feature 少了:
```
vmx(CPUID.0x1.ECX[5])
IA32_TSC_ADJUST support(CPUID.0x7.EBX[1])

```

***

使用如下的cpu feature测试,

源端:
```

```
目的端:
```
vmx=off,tsc_adjust=off,vme=on
```

源端:
```
vmx=on,tsc_adjust=on,vme=off
```
测试结果
```diff
--- cpuid_src.txt       2025-07-30 21:10:28.131788453 +0800
+++ cpuid_dst.txt       2025-07-30 21:11:03.831789393 +0800
@@ -1,5 +1,5 @@
 00000000 00000000: 0000000d 756e6547 6c65746e 49656e69
-00000001 00000000: 000306c1 00000800 fffa3223 078bfbfd
+00000001 00000000: 000306c1 00000800 fffa3203 078bfbff
 00000002 00000000: 00000001 00000000 0000004d 002c307d
 00000003 00000000: 00000000 00000000 00000000 00000000
 00000004 00000000: 00000121 01c0003f 0000003f 00000001
@@ -8,7 +8,7 @@
 00000004 00000003: 00000163 03c0003f 00003fff 00000006
 00000005 00000000: 00000000 00000000 00000003 00000000
 00000006 00000000: 00000004 00000000 00000000 00000000
-00000007 00000000: 00000000 000007ab 00000000 00000000
+00000007 00000000: 00000000 000007a9 00000000 00000000
 00000008 00000000: 00000000 00000000 00000000 00000000
 00000009 00000000: 00000000 00000000 00000000 00000000
 0000000a 00000000: 00000000 00000000 00000000 00000000
```

迁移后增加的feature:
```
vme(CPUID.0x1:EDX[2])
```
减少的feature:
```
vmx(CPUID.0x1.ECX[5])
IA32_TSC_ADJUST support(CPUID.0x7.EBX[1])
```

这样来看, cpu feature 在迁移完后，无脑选择目的端的.
