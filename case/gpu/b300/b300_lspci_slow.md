# 问题现象

在虚拟机中执行`nvidia-smi`, 会发现某个GPU掉卡
```
root@b300-instance-test8:~# nvidia-smi  |grep 0000
Unable to determine the device handle for GPU2: 0000:F5:09.0: Unknown Error
|   0  NVIDIA B300 SXM6 AC            On  |   00000000:F1:09.0 Off |                    0 |
|   1  NVIDIA B300 SXM6 AC            On  |   00000000:F3:09.0 Off |                    0 |
|   3  NVIDIA B300 SXM6 AC            On  |   00000000:F7:09.0 Off |                    0 |
|   4  NVIDIA B300 SXM6 AC            On  |   00000000:F9:09.0 Off |                    0 |
|   5  NVIDIA B300 SXM6 AC            On  |   00000000:FB:09.0 Off |                    0 |
|   6  NVIDIA B300 SXM6 AC            On  |   00000000:FD:09.0 Off |                    0 |
|   7  NVIDIA B300 SXM6 AC            On  |   00000000:FF:09.0 Off |                    0 |
```

此时, `F5:09.0` 掉卡。

# 问题调查

## 查看host上相应hostdev映射

虚拟机中topo
```
 +-[0000:f4]---00.0-[f5]--+-09.0  NVIDIA Corporation Device 3182
 |                        +-0a.0  Mellanox Technologies CX8 Family [ConnectX-8]
 |                        \-0b.0  Mellanox Technologies CX8 Family [ConnectX-8]
```
查看`f4:00.0`(244) 的 `pci-expander-bus`

```
    <controller type='pci' index='7' model='pci-expander-bus'>
      <model name='pxb'/>
      <target busNr='244'>
        <node>0</node>
      </target>
      <alias name='pci.7'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x08' function='0x0'/>
    </controller>

    <hostdev mode='subsystem' type='pci' managed='yes'>
      <driver name='vfio'/>
      <source>
        <address domain='0x0000' bus='0xbd' slot='0x00' function='0x0'/>
      </source>
      <alias name='hostdev2'/>
      <address type='pci' domain='0x0000' bus='0x07' slot='0x09' function='0x0'/>
    </hostdev>
```

可以看到GPU 为`bd:00.0`, 

## 在host查看所有GPU的配置空间
执行下面脚本查看
```
BDFS=(1a:00.0
40:00.0
62:00.0
73:00.0
9a:00.0
bd:00.0
df:00.0
f0:00.0)
for BDF in ${BDFS[@]}
do
	echo ========${BDF}=============

	time lspci -vvv -s $BDF > /dev/null
done
```

结果发现，只有`bd:00.0` 上特别卡
```
========1a:00.0=============

real	0m0.009s
user	0m0.006s
sys	0m0.003s
========40:00.0=============

real	0m0.009s
user	0m0.004s
sys	0m0.005s
========62:00.0=============

real	0m0.009s
user	0m0.005s
sys	0m0.004s
========73:00.0=============

real	0m0.009s
user	0m0.004s
sys	0m0.004s
========9a:00.0=============

real	0m0.008s
user	0m0.004s
sys	0m0.004s
========bd:00.0=============

real	0m12.894s
user	0m3.115s
sys	0m9.779s
========df:00.0=============

real	0m0.009s
user	0m0.005s
sys	0m0.004s
========f0:00.0=============

real	0m0.009s
user	0m0.005s
sys	0m0.004s
```

## host上该gpu的相关topo
查看该gpu在host上的topo关系:
```
 +-[0000:b7]-+-00.0
 |           +-00.1
 |           +-00.2
 |           +-00.4
 |           \-02.0-[b8-bd]----00.0-[b9]--+-00.0-[ba]--+-00.0
 |                                        |            \-00.1
 |                                        \-02.0-[bb]----00.0-[bc]----00.0-[bd]----00.0
```

查看整个链路上`lspci` 是否卡顿

未了避免lspci命令带来影响，我们直接读取配置空间:
```
for dev in b7:02.0 b8:00.0 b9:02.0 bb:00.0 bc:00.0 bd:00.0; do
    echo "===== $dev config ====="

    dd  if=/sys/bus/pci/devices/0000:$dev/config \
        of=/dev/null bs=64 count=1 status=none
done
```

结果:
```
===== b7:02.0 config =====

real	0m0.001s
user	0m0.001s
sys	0m0.000s
===== b8:00.0 config =====

real	0m12.886s
user	0m0.000s
sys	0m12.886s
===== b9:02.0 config =====

real	0m12.889s
user	0m0.712s
sys	0m12.176s
===== bb:00.0 config =====

real	0m12.890s
user	0m0.000s
sys	0m12.888s
===== bc:00.0 config =====

real	0m12.887s
user	0m0.000s
sys	0m12.887s
===== bd:00.0 config =====
```
可以发现整个链路都存在过慢

## 进一步获取配置空间

查看`bd:00.0`, 以及`b8:00.0` 的配置空间内容:
```
[root@10-75-4-155 11:12:29 wangfuqiang49]# lspci -xx -s bd:00.0
bd:00.0 3D controller: NVIDIA Corporation Device 3182 (rev a1)
00: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
10: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
20: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
30: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff

[root@10-75-4-155 11:13:35 wangfuqiang49]# lspci -xx -s b8:00.0
b8:00.0 PCI bridge: Mellanox Technologies ConnectX/BlueField Family mlx5Gen PCIe Bridge [PCIe Bridge]
00: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
10: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
20: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
30: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
```

可以发现都是全F。

## 查看`b7:02.0`AER cap
```
        Capabilities: [100 v2] Advanced Error Reporting
                UESta:  DLP- SDES- TLP- FCP- CmpltTO+ CmpltAbrt- UnxCmplt- RxOF- MalfTLP- ECRC- UnsupReq- ACSViol-
                UEMsk:  DLP+ SDES+ TLP- FCP+ CmpltTO+ CmpltAbrt+ UnxCmplt+ RxOF+ MalfTLP+ ECRC+ UnsupReq+ ACSViol+
                UESvrt: DLP- SDES- TLP- FCP- CmpltTO- CmpltAbrt- UnxCmplt- RxOF- MalfTLP- ECRC- UnsupReq- ACSViol-
                CESta:  RxErr- BadTLP- BadDLLP- Rollover- Timeout- AdvNonFatalErr-
                CEMsk:  RxErr+ BadTLP+ BadDLLP+ Rollover+ Timeout+ AdvNonFatalErr+
                AERCap: First Error Pointer: 00, ECRCGenCap+ ECRCGenEn- ECRCChkCap+ ECRCChkEn-
                        MultHdrRecCap- MultHdrRecEn- TLPPfxPres- HdrLogCap+
                HeaderLog: 60000010 ba0100ff 00000001 9330b080
                RootCmd: CERptEn- NFERptEn- FERptEn-
                RootSta: CERcvd- MultCERcvd- UERcvd- MultUERcvd-
                         FirstFatal- NonFatalMsg- FatalMsg- IntMsg 0
```
可以发现有UE. 报告  `CmpltTO`, 说明PCIe链路不正常
