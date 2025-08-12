## 检查SRIOV feature
```
root@ib02:/sys/class/net/ibs5/device# mlxconfig -d /dev/mst/mt4123_pciconf0 q |grep -E -i 'SR|VF'
        PF_NUM_OF_VF_VALID                          False(0)
        VF_VPD_ENABLE                               False(0)
        STRICT_VF_MSIX_NUM                          False(0)
        VF_NODNIC_ENABLE                            False(0)
        NUM_OF_VFS                                  0
        SRIOV_EN                                    False(0)
        ^^^^^^^^
        VF_LOG_BAR_SIZE                             1
        NUM_VF_MSIX                                 11
        SRIOV_IB_ROUTING_MODE_P1                    LID(1)
        DYNAMIC_VF_MSIX_TABLE                       False(0)
```

未开启sr-iov

修改mlxconfg
```
root@ib02:/sys/class/net/ibs5/device# mlxconfig -d /dev/mst/mt4123_pciconf0 set SRIOV_EN=1 NUM_OF_VFS=16

Device #1:
----------

Device type:        ConnectX6
Name:               MCX653105A-HDA_Ax
Description:        ConnectX-6 VPI adapter card; HDR IB (200Gb/s) and 200GbE; single-port QSFP56; PCIe4.0 x16; tall bracket; ROHS R6
Device:             /dev/mst/mt4123_pciconf0

Configurations:                                          Next Boot       New
        SRIOV_EN                                    False(0)             True(1)
        NUM_OF_VFS                                  0                    16

 Apply new Configuration? (y/n) [n] : y
Applying... Done!
-I- Please reboot machine to load new configurations.
```


修改`/etc/default/grub` 增加iommu配置.
```
GRUB_CMDLINE_LINUX="intel_iommu=on iommu=pt"
```
重新生成grub.cfg
```
cd /boot/grub
cp grub.cfg grub.cfg.bak
grub-mkconfig -o /boot/grub/grub.cfg
```

## SR_IOV pci config

申请8个sr-iov
```
root@ib02:~# echo 8 >  /sys/class/infiniband/mlx5_0/device/sriov_numvfs
root@ib02:~# cat /sys/class/infiniband/mlx5_0/device/mlx5_num_vfs
8
```
执行`lspci`命令:
```
18:00.0 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6]
18:00.1 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function]
18:00.2 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function]
18:00.3 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function]
18:00.4 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function]
18:00.5 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function]
18:00.6 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function]
18:00.7 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function]
18:01.0 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function]
```

可以看到8个VF

ibstat:
```
CA 'mlx5_10'
        CA type: MT4124
--
CA 'mlx5_3'
        CA type: MT4124
--
CA 'mlx5_4'
        CA type: MT4124
--
CA 'mlx5_5'
        CA type: MT4124
--
CA 'mlx5_6'
        CA type: MT4124
--
CA 'mlx5_7'
        CA type: MT4124
--
CA 'mlx5_8'
        CA type: MT4124
--
CA 'mlx5_9'
        CA type: MT4124
```
VF 0 对应 `mlx5_3`

## 解绑VF 并VFIO透传启动虚拟机
1. 解绑设备
```
echo 0000:18:00.1 > /sys/bus/pci/drivers/mlx5_core/unbind
```
2. 查看venderid,deviceid
```
root@ib02:~# lspci -nn -s 18:00.1
18:00.1 Infiniband controller [0207]: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function] [15b3:101c]
```
3. 绑定vfio
```
root@ib02:cd /sys/bus/pci/drivers/vfio-pci/
root@ib02:/sys/bus/pci/drivers/vfio-pci# echo 15b3 101c > new_id
```
3. 查看当前设备驱动 
```
root@ib02:/sys/bus/pci/drivers/vfio-pci# lspci -vvv -s 18:00.1 |grep Kernel
        Kernel driver in use: vfio-pci
        Kernel modules: mlx5_core
```

可以发现已经绑定vfio

4. host 执行ibstat
```
root@ib02:~/wangfuqiang49# ibstat |grep -B 1 MT4124
CA 'mlx5_10'
        CA type: MT4124
--
CA 'mlx5_4'
        CA type: MT4124
--
CA 'mlx5_5'
        CA type: MT4124
--
CA 'mlx5_6'
        CA type: MT4124
--
CA 'mlx5_7'
        CA type: MT4124
--
CA 'mlx5_8'
        CA type: MT4124
--
CA 'mlx5_9'
        CA type: MT4124
```

## 启动虚拟机
使用下面qemu参数:
```
IMG=$(ls *.qcow2)
qemu-system-x86_64 -name guest=centos8 \
        -enable-kvm \
        -m 4096 \
        -smp 8          \
        -nodefaults -boot strict=on     \
        -name wang_rocky,debug-threads=on \
        -cpu host       \
        -boot strict=on \
        -qmp tcp:localhost:9999,server,nowait \
        -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd \
        -drive if=pflash,format=raw,file=/usr/share/OVMF/OVMF_VARS.fd \
        -drive file=$(pwd)/${IMG} \
        -object iothread,id=iothread0 \
        -msg timestamp=on \
        -netdev user,id=internet,hostfwd=tcp::55557-:22    \
        -nographic   \
        -device pci-bridge,id=bridge0,chassis_nr=3,addr=0x14 \
        -usb \
        -device vfio-pci,host=0000:18:00.1,bus=bridge0 \
        -serial tcp:localhost:6667,server,nowait \
        -monitor stdio
```
虚拟机启动后, 查看:
```
[root@localhost ~]# lspci |grep Inf
01:00.0 Infiniband controller: Mellanox Technologies MT28908 Family [ConnectX-6 Virtual Function]
[root@localhost ~]# dmesg |grep 01:00.0
[    0.528787] pci 0000:01:00.0: [15b3:101c] type 00 class 0x020700
[    0.530466] pci 0000:01:00.0: reg 0x10: [mem 0x800000000-0x8001fffff 64bit pref]
[    0.539391] pci 0000:01:00.0: enabling Extended Tags
[    0.540674] pci 0000:01:00.0: 0.000 Gb/s available PCIe bandwidth, limited by Unknown x0 link at 0000:00:14.0 (capable of 252.048 Gb/s with 16.0 GT/s PCIe x16 link)
[    3.292153] mlx5_core 0000:01:00.0: firmware version: 20.40.1000
```
ip a找不到ib卡:
```
[root@localhost ~]# ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host
       valid_lft forever preferred_lft forever
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether 00:11:22:33:44:55 brd ff:ff:ff:ff:ff:ff
    altname enp0s2
    altname ens2
    inet 10.0.2.15/24 brd 10.0.2.255 scope global dynamic noprefixroute eth0
       valid_lft 86293sec preferred_lft 86293sec
    inet6 fec0::211:22ff:fe33:4455/64 scope site dynamic noprefixroute
       valid_lft 86295sec preferred_lft 14295sec
    inet6 fe80::211:22ff:fe33:4455/64 scope link noprefixroute
       valid_lft forever preferred_lft forever
```
ib 卡并没有正确被初始化


## 安装最新官方doca驱动
```sh
rpm -ivh doca-host-3.0.0-058000_25.04_rhel94.x86_64.rpm
## 驱动包
yum install kmod-mlnx-ofa_kernel
## ibstat 所在的包
yum install infiniband-diags
```
安装驱动包后, 执行ibstat
```
[root@localhost ~]# ibstat
CA 'mlx5_0'
        CA type: MT4124
        Number of ports: 1
        Firmware version: 20.40.1000
        Hardware version: 0
        Node GUID: 0x0000000000000000
        System image GUID: 0x5c25730300843406
        Port 1:
                State: Down
                Physical state: LinkUp
                Rate: 200
                Base lid: 65535
                LMC: 0
                SM lid: 10
                Capability mask: 0xa651ec48
                Port GUID: 0x0000000000000000
                Link layer: InfiniBand
```

在host查看`ibstat`:
```
root@ib02:~# ibstat
CA 'mlx5_0'
        CA type: MT4123
        Number of ports: 1
        Firmware version: 20.40.1000
        Hardware version: 0
        Node GUID: 0x5c25730300843406
        System image GUID: 0x5c25730300843406
        Port 1:
                State: Active
                Physical state: LinkUp
                Rate: 200
                Base lid: 5
                LMC: 0
                SM lid: 10
                Capability mask: 0xa651e848
                Port GUID: 0x5c25730300843406
                Link layer: InfiniBand
```

## 修改nodeid, portid
`/sys/class/infiniband/mlx5_0/device/sriov/`路径下，有各个VF的配置文件:
```
root@ib02:/sys/kernel# tree /sys/class/infiniband/mlx5_0/device/sriov/
/sys/class/infiniband/mlx5_0/device/sriov/
├── 0
│   ├── node
│   ├── policy
│   └── port
├── 1
│   ├── node
│   ├── policy
│   └── port
├── 2
│   ├── node
│   ├── policy
│   └── port
├── 3
│   ├── node
│   ├── policy
│   └── port
├── 4
│   ├── node
│   ├── policy
│   └── port
├── 5
│   ├── node
│   ├── policy
│   └── port
├── 6
│   ├── node
│   ├── policy
│   └── port
└── 7
    ├── node
    ├── policy
    └── port
```
修改VF0
```
cd /sys/class/infiniband/mlx5_0/device/sriov/0
root@ib02:/sys/class/infiniband/mlx5_0/device/sriov/0# echo 00:11:22:33:44:55:02:01 > node
root@ib02:/sys/class/infiniband/mlx5_0/device/sriov/0# echo 00:00:00:00:00:00:00:44 > port
root@ib02:/sys/class/infiniband/mlx5_0/device/sriov/0# cat node
00:11:22:33:44:55:02:01
root@ib02:/sys/class/infiniband/mlx5_0/device/sriov/0# cat port
00:00:00:00:00:00:00:44
```
启动虚拟机查看nodeid, portid
```
[root@localhost ~]# ibstat
CA 'mlx5_0'
        CA type: MT4124
        Number of ports: 1
        Firmware version: 20.40.1000
        Hardware version: 0
        Node GUID: 0x0011223344550201
        System image GUID: 0x5c25730300843406
        Port 1:
                State: Down
                Physical state: LinkUp
                Rate: 200
                Base lid: 65535
                LMC: 0
                SM lid: 10
                Capability mask: 0xa651ec48
                Port GUID: 0x0000000000000044
                Link layer: InfiniBand
```

## 参考链接
1. [NVIDIA MLNX_OFED Documentation Rev 5.8-1.1.2.1 LTS --  Single Root IO Virtualization (SR-IOV)](https://docs.nvidia.com/networking/display/mlnxofedv581121lts/single+root+io+virtualization+(sr-iov))
2. [Savir RDMA 主页](https://www.zhihu.com/people/saviour-li)
