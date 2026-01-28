## 显卡透传测试
### 镜像信息
使用`openEuler-24.03-LTS-SP1-x86_64.qcow2` cloud-image 作为虚拟机镜像测试:

### 环境配置
1. 配置kenrel cmdline(将显卡先绑定到pci-stub 方便换绑; 配置iommu)
   ```sh
   pci-stub.ids=10de:2901 intel_iommu=on iommu=pt
   ```

2. 重启后，执行下面脚本换绑:
   ```sh
   modprobe vfio-pci
   modprobe vfio
   unbind_dev()
   {
           BDF=$1
           echo $BDF > /sys/bus/pci/drivers/pci-stub/unbind 
   }

   unbind_dev 0000:18:00.0
   unbind_dev 0000:3a:00.0
   unbind_dev 0000:4b:00.0
   unbind_dev 0000:5c:00.0
   unbind_dev 0000:9a:00.0
   unbind_dev 0000:ba:00.0
   unbind_dev 0000:ca:00.0
   unbind_dev 0000:da:00.0

   echo '10de 2901' > /sys/bus/pci/drivers/vfio-pci/new_id
   ```

### 虚拟机启动(UEFI)
换绑后，使用下面命令启动虚拟机:
```sh
IMG=openEuler-24.03-LTS-SP1-x86_64.qcow2

/usr/bin/qemu-system-x86_64 -name guest=centos8 \
    -enable-kvm \
     -m 16384 \
     -bios /usr/share/edk2/ovmf/OVMF.fd \
     -smp 8          \
     -nodefaults -boot strict=on     \
     -name wang_rocky,debug-threads=on \
     -cpu host       \
     -machine pc \
     -boot strict=on \
     -qmp tcp:localhost:9999,server,nowait \
     -device virtio-scsi-pci \
     -drive file=$(pwd)/${IMG} \
     -object iothread,id=iothread0 \
     -msg timestamp=on \
     -nographic   \
     -device pci-bridge,id=bridge0,chassis_nr=3,addr=0x14  \
     -usb \
     -serial telnet:localhost:66668,server,nowait -gdb tcp::1237 \
     -device vfio-pci,host=0000:18:00.0,bus=pci.0 \
     -device virtio-net-pci,mac=50:54:11:00:10:46,netdev=internet,id=internet-dev,bus=pci.0    \
     -netdev user,id=internet,hostfwd=tcp::55557-:22    \
     -net nic,model=virtio -net tap,ifname=tap2,script=no,downscript=no   \
     -monitor stdio
```

使用上面命令启动虚拟机后，串口卡死，根据下面寄存器信息来看，疑似卡在了bios.

```sh
(qemu) info registers 
RAX=0000000000000000 RBX=0000000000000004 RCX=00000000bf538498 RDX=0000000000000000
RSI=0000000000000001 RDI=00000000bfe582a0 RBP=00000000bfe599e0 RSP=00000000bfe42508
R8 =00000000bfe426d8 R9 =00000000bfe426e8 R10=00000000becb4618 R11=0000000000000000
R12=00000000bf5384e0 R13=0000000000000100 R14=00000000bfe59ae0 R15=00000000bf538498
RIP=00000000bf5344e1 RFL=00000202 [-------] CPL=0 II=0 A20=1 SMM=0 HLT=1
```

如果使用`-device vfio-pci,host=0000:18:00.0,bus=bridge0`, 将显卡绑定到`pcibridge`
虚拟机可以启动，但是会遇到下面错误:

```
[  103.455825][ T1194] NVRM: This PCI I/O region assigned to your NVIDIA device is invalid:
[  103.455825][ T1194] NVRM: BAR0 is 0M @ 0x0 (PCI:0000:01:00.0)
```

查看显卡bar 空间 :
```
  Bus  1, device   0, function 0:
    3D controller: PCI device 10de:2901
      PCI subsystem 10de:1999
      IRQ 0, pin A
      BAR0: 64 bit prefetchable memory at 0xffffffffffffffff [0x03fffffe].
      BAR2: 64 bit prefetchable memory at 0xffffffffffffffff [0x3ffffffffe].
      BAR4: 64 bit prefetchable memory at 0xffffffffffffffff [0x01fffffe].
```
发现这些bar空间没有被正确初始化.

查找资料，在<sup>2</sup>中发现有人遇到该问题, 提到有两种解决方式:
* 使用 `sea-bios`
* 增加一个参数
  ```
  -fw_cfg name=opt/ovmf/X-PciMmio64Mb,string=xxxxx
  ```
由于b200显卡bar2大小为256G。

我这边将`string`设置为 512G(`524288`)

发现虚拟机可以正常启动. 安装显卡驱动后，可以正常驱动

随便执行一些命令:
```
[root@localhost ~]# nvidia-smi 
Wed Jan 28 06:57:03 2026       
+-----------------------------------------------------------------------------------------+
| NVIDIA-SMI 580.126.09             Driver Version: 580.126.09     CUDA Version: 13.0     |
+-----------------------------------------+------------------------+----------------------+
| GPU  Name                 Persistence-M | Bus-Id          Disp.A | Volatile Uncorr. ECC |
| Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
|                                         |                        |               MIG M. |
|=========================================+========================+======================|
|   0  NVIDIA B200                    Off |   00000000:00:15.0 Off |                    0 |
| N/A   28C    P0            198W / 1000W |       0MiB / 183359MiB |      3%      Default |
|                                         |                        |             Disabled |
+-----------------------------------------+------------------------+----------------------+

+-----------------------------------------------------------------------------------------+
| Processes:                                                                              |
|  GPU   GI   CI              PID   Type   Process name                        GPU Memory |
|        ID   ID                                                               Usage      |
|=========================================================================================|
|  No running processes found                                                             |
+-----------------------------------------------------------------------------------------+
[root@localhost ~]# nvidia-smi -pm 1
Enabled Legacy persistence mode for GPU 00000000:00:15.0.
All done.
[root@localhost ~]# nvidia-smi 
Wed Jan 28 06:58:40 2026       
+-----------------------------------------------------------------------------------------+
| NVIDIA-SMI 580.126.09             Driver Version: 580.126.09     CUDA Version: 13.0     |
+-----------------------------------------+------------------------+----------------------+
| GPU  Name                 Persistence-M | Bus-Id          Disp.A | Volatile Uncorr. ECC |
| Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
|                                         |                        |               MIG M. |
|=========================================+========================+======================|
|   0  NVIDIA B200                    On  |   00000000:00:15.0 Off |                    0 |
| N/A   28C    P0            195W / 1000W |       0MiB / 183359MiB |      0%      Default |
|                                         |                        |             Disabled |
+-----------------------------------------+------------------------+----------------------+

+-----------------------------------------------------------------------------------------+
| Processes:                                                                              |
|  GPU   GI   CI              PID   Type   Process name                        GPU Memory |
|        ID   ID                                                               Usage      |
|=========================================================================================|
|  No running processes found                                                             |
```

### 虚拟机启动(seaBIOS)

可以正常启动。

## 结论
* BIOS镜像可以正常启动
* UEFI镜像需要增加`-fw_cfg name=opt/ovmf/X-PciMmio64Mb,string=xxxxx` qemu
  cmdline才能正常驱动

## 参考链接

1. [GPU passthrough with virt-manager, QEMU, and KVM](https://wiki.gentoo.org/wiki/GPU_passthrough_with_virt-manager,_QEMU,_and_KVM)
2. [ PCI Passthrough BAR 0 errors (debugging)](https://forum.proxmox.com/threads/pci-passthrough-bar-0-errors-debugging.164990/)
