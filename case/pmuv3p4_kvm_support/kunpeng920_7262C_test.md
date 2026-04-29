## 环境信息
|env entry|value|
|----|----|
|cpu|kunpeng 920 7262C|
|内核版本|4.19.90-89.11.v2401.ky10.aarch64|
|qemu|qemu-4.1.0|
|guest img|openEuler-24.03-LTS-SP2-aarch64.qcow2|


## 启动虚拟机测试 

qemu 启动参数如下:
```sh
qemu-kvm \
  -machine virt,accel=kvm,gic-version=3 \
  -cpu host,pmu=on \
  -m 4096 \
  -smp 4 \
  -drive if=pflash,format=raw,file=/usr/share/edk2/aarch64/QEMU_EFI-pflash.raw,readonly=on \
  -drive if=pflash,format=raw,file=./vars.fd \
  -drive file=/root/aarch64/openEuler-24.03-LTS-SP2-aarch64.qcow2,if=virtio,format=qcow2 \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device virtio-net-device,netdev=net0 \
  -nographic
```

执行上述命令虚拟机可以正常启动.

怀疑是本地物理机不支持PMUv3p4. 编写kernel modules 做测试:

* [read bfr0.c](./read_mrs/read_dfr0.c)
* [Makefile](./read_mrs/Makefile)

测试结果 :
```
[2579650.570734] SYS_ID_AA64DFR0_EL1 = 0x0000000110305408
```

其中`ID_AA64DFR0_EL1.PMUVer` 为:
* `bit [11-8]`: 0b0100 手册中的解释为: `PMUv3 for Armv8.1.`
