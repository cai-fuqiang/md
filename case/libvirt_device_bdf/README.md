## HOW to define pci device bus

在xml中定义一个pci设备时，可以通过address 命令指定
* domain 
* bus
* slot
* function

以vhost-user为例:
```
<interface type='vhostuser'>
  ...
  <address type='pci' domain='0x0000' bus='0x02' slot='0x03' function='0x0'/>
  ...
</interface>
```

但是我们知道, 一个pci 拓扑，硬件唯一能指定的就是start bus，和整个的tree，而
拓扑中可以定义`device number`, `function number`. bus number 是在software 进行
枚举时，自己划定的。（根据tree的遍历方式可以有不同的结果)

那么这里定义的bus，是搞啥的。

每个pci设备的位置需要锚定一个bridge. 这里的`bus`实际上是指定的bridge index.

我们做如下实验:
xml中定义bridge

定义两个bridge:
* `0000:1f:00.0`: index = 1
* `0000:1e:00.0`: index = 2
```
    <controller type='pci' index='1' model='pci-bridge'>
      <model name='pci-bridge'/>
      <target chassisNr='1'/>
      <alias name='pci.1'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x1f' function='0x0'/>
    </controller>
    <controller type='pci' index='2' model='pci-bridge'>
      <model name='pci-bridge'/>
      <target chassisNr='2'/>
      <alias name='pci.2'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x1e' function='0x0'/>
    </controller>
```
然后在添加一个设备:
```
<interface type='vhostuser'>
  ...
  <address type='pci' domain='0x0000' bus='0x02' slot='0x03' function='0x0'/>
</interface>
```

将bus 定义为`2`, 我们知道, 按照pci 枚举的方式
* `0000:1e:00.0`: bus 1 (index 2)
* `0000:1f:00.0`: bus 2 (index 1)

vhostuser设备指定`bus=0x02`, 到底是挂在`1e:00.0(bus 1, index 2)`, 还是`1f:00.0(bus 2 index 1`
呢?

执行下面的命令查看, pci详细信息:
```
virsh qemu-monitor-command wang_disk_attach --hmp 'info pci'
  Bus  0, device  30, function 0:
    PCI bridge: PCI device 1b36:0001
      IRQ 10, pin A
      BUS 0.
      secondary bus 1.
      subordinate bus 1.
      IO range [0x1000, 0x1fff]
      memory range [0xc0200000, 0xc03fffff]
      prefetchable memory range [0x380000000000, 0x3807ffffffff]
      BAR0: 64 bit memory at 0x381000009000 [0x3810000090ff].
      id "pci.2"
  Bus  1, device   3, function 0:
    Ethernet controller: PCI device 1af4:1000
      PCI subsystem 1af4:0001
      IRQ 10, pin A
      BAR0: I/O at 0x1000 [0x103f].
      BAR1: 32 bit memory at 0xc0200000 [0xc0200fff].
      BAR4: 64 bit prefetchable memory at 0x380000000000 [0x380000003fff].
      id "net0"
  Bus  0, device  31, function 0:
    PCI bridge: PCI device 1b36:0001
      IRQ 11, pin A
      BUS 0.
      secondary bus 2.
      subordinate bus 2.
      IO range [0xf000, 0x0fff]
      memory range [0xc0000000, 0xc01fffff]
      prefetchable memory range [0x380800000000, 0x380fffffffff]
      BAR0: 64 bit memory at 0x381000008000 [0x3810000080ff].
      id "pci.1"
```
可以看到pci bus分配和想象的一样, 另外vhostuser device 挂载了`index=1`的设备上。

**所以，配置的bus 表示的是 所要挂载bridge 的index**

## pcie-expander-bus

这个设备比较特殊，该桥可以不链接到 host bridge所在的pci 拓扑上。

举例添加如下设备:
```
    <controller type='pci' index='3' model='pci-expander-bus'>
      <model name='pxb'/>
      <target busNr='254'>
        <node>0</node>
      </target>
      <alias name='pci.3'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x1d' function='0x0'/>
    </controller>
```
再次查看pci 信息
```

  Bus  0, device  29, function 0:
    Host bridge: PCI device 1b36:0009
      PCI subsystem 1af4:1100
      id "pci.3"
  Bus 254, device   0, function 0:
    PCI bridge: PCI device 1b36:0001
      BUS 254.
      secondary bus 255.
      subordinate bus 255.
      IO range [0xf000, 0x0fff]
      memory range [0xfff00000, 0x000fffff]
      prefetchable memory range [0xfff00000, 0x000fffff]
      id "pci.3"
```

可以发现，多出来一个host-bridge, 另外, 在bus 0 的tree之外，新搞出来一个
`bus 254`, 然后在`bus 254`上链接了一个刚刚定义的bridge.(index =3)
```
-+-[0000:00]-+-00.0
 |           +-01.0
 |           +-01.1
 |           +-01.2
 |           +-01.3
 |           +-02.0
 |           +-04.0
 |           +-1d.0
 |           +-1e.0-[01]----03.0
 |           \-1f.0-[02]--
 \-[0000:fe]---00.0-[ff]--
```
其实，这个bridge是链接到了host bridge `00:29.0`. 但是这个位置是虚的。host bridge
是所有pci层级的顶端。从upstream 路由到host bridge，不需要经过任何的pci 层级，
在 root complex 中就已经找到了。

这样的bridge有什么好处呢?

可以自定义pcie 拓扑numa, 我们做如下实验:

1. 给cpu 内存划分numa 0, 1:
   ```
     <cpu mode='host-passthrough' check='none' migratable='on'>
       <topology sockets='1' dies='1' cores='32' threads='1'/>
       <numa>
         <cell id='0' cpus='0-15' memory='16777216' unit='KiB' memAccess='shared'/>
         <cell id='1' cpus='16-31' memory='16777216' unit='KiB' memAccess='shared'/>
       </numa>
     </cpu>
   ```
2. 将bridge 指定numa: `pci-expander-bus`
   ```
       <controller type='pci' index='3' model='pci-expander-bus'>
         <model name='pxb'/>
         <target busNr='254'>
           <node>1</node>
         </target>
         <alias name='pci.3'/>
         <address type='pci' domain='0x0000' bus='0x00' slot='0x1d' function='0x0'/>
       </controller>
   ```
启动虚拟机，我们来看下:
```
[root@localhost ~]# lspci |grep fe:00.0
fe:00.0 PCI bridge: Red Hat, Inc. QEMU PCI-PCI bridge
[root@localhost ~]# lspci -vvv -s  fe:00.0 |grep -i node
        NUMA node: 1
```
可以看到numa node 为1。

在模拟设备的时候意义不大，但是在透传场景下，可以把pci拓扑传入。（适用于传入多个
设备，并且多个设备存在跨numa 的场景)


## 参考链接
1. [\[libvirt DOC\]: PCI addresses in domain XML and guest OS](https://www.libvirt.org/pci-addresses.html#pcie-expander-bus)
2. [\[libvirt DOC\]:Domain XML format](https://libvirt.org/formatdomain.html)
