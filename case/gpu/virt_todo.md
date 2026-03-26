## HOST
### grub改动将nvidia 驱动绑定vfio
```sh
OPT='vfio-pci.ids=10de:2335,10de:22a3'
grep -qF "$OPT" /etc/default/grub || \
sudo sed -i -E "s|^GRUB_CMDLINE_LINUX=\"([^\"]*)\"$|GRUB_CMDLINE_LINUX=\"\1 $OPT\"|" /etc/default/grub
cp /boot/efi/EFI/openEuler/grub.cfg  /boot/efi/EFI/openEuler/grub.cfg.bak
grub2-mkconfig -o /boot/efi/EFI/openEuler/grub.cfg
```

### 开启acs

![enable_acs](./enable_acs.sh)

### 开启ats

> [!NOTE]
> 
> 目前考虑开启所有的IB卡


执行脚本:
```sh
mst start
for IBNAME in `ls /dev/mst/mt*`
do
  mlxconfig -d $IBNAME set ATS_ENABLED=1
done
```

### 重启物理节点


## service VM 搭建

1. 将`10.200.128.53`环境上的`/export/wangfuqiang49/vm_2` copy到目标节点相同路径:
2. `cd /export/wangfuqiang49/vm_2 && virsh define example.xml && virsh start service_vm`
3. 等虚拟机启动后，进入虚拟机确认`nvidia-fabricmanager`服务是否启动
   ```
   ssh root@localhost -p 55557
   systemctl status nvidia-fabricmanager.service 
   ```


## 启动虚拟机...

### 修改虚拟机xml增加expander-bus

> NOTE
> 
> 可以先不用关心这部分内容，后续会给出所有xml

路径
```
cd /xxxx/instances/$INSTANCE_ID/
## 修改xml文件
```

根据规格增加`expander-bus`, 每个gpu 需要一个 `expander-bus`, 一个
`expander-bus` xml如下:

```xml
<controller type='pci' index='2' model='pci-expander-bus'>
   <model name='pxb'/>
   <target busNr='254'>
     <node>0</node>
   </target>
   <alias name='pci.2'/>
   <address type='pci' domain='0x0000' bus='0x00' slot='0x03' function='0x0'/>
</controller>
```

需要注意, 每个bus的下面字段不一样:
* `controller index`: 依次+1
* `controller.alias name`: `pci.2` 依次加1， 下一个`pci.3`
* `<address type='pci' domain='0x0000' bus='0x00' slot='0x03' function='0x0'/>` slot 依次加1, 注意
  是16进制
* `controller target busNr`  依次减2, 下一个为 252
* `controller.target.node`在8卡下，需要4个`expander-bus`配置为1



### 获取 物理机topo
1. 执行`lstopo -sv > lstopo.txt`
2. 执行 [topo_by_numa.py](topo_by_numa.py) 脚本:
   ```
   python topo_by_numa.py --ignore-split-ib |jq '.[].[] | {gpus,mlx}'
   ```
   里面有显卡和IB卡的亲和性

如果有如下打印:
```
{
  "gpus": [
    "19:00.0"
  ],
  "mlx": [
    {
      "name": "mlx5_0",
      "bdf": "1a:00.0"
    },
    {
      "name": "mlx5_1",
      "bdf": "1b:00.0"
    }
  ]
}
```
一般来说第二个卡为存储IB, 需要手动确认。

将这两个设备透传到虚拟机中，修改xml:
```
  <hostdev mode='subsystem' type='pci' managed='yes'>
      <driver name='vfio'/>
      <source>
        <address domain='0x0000' bus='0x19' slot='0x00' function='0x0'/>
      </source>
      <alias name='hostdev0'/>
      <address type='pci' domain='0x0000' bus='0x2' slot='0x01' function='0x0'/>
    </hostdev>
    <hostdev mode='subsystem' type='pci' managed='yes'>
      <driver name='vfio'/>
      <source>
        <address domain='0x0000' bus='0x1a' slot='0x00' function='0x0'/>
      </source>
      <alias name='hostdev0'/>
      <address type='pci' domain='0x0000' bus='0x02' slot='0x02' function='0x0'/>
    </hostdev>
```

注意
1. `hostdev addres bus=0x2`, 0x2代表`expander bus index`
2. 对于8卡设备来说，物理机的numa拓扑要和虚拟机中对上，主要是显卡ib卡链接的bus(`expander-bus`)
   `controller target busNr` 要和物理机相同

为了简单起见:
我整理了所有的1卡和8卡的xml, 但是如果使用这些xml需要保证物理机topo相同，可以
在不同的物理节点执行`lstopo -sv` 输出比对。

* 1 卡
  + NUMA 0 亲和虚拟机
    + [1](./xml_temp/1_gpu_node0_1.xml)
    + [2](./xml_temp/1_gpu_node0_2.xml)
    + [3](./xml_temp/1_gpu_node0_3.xml)
    + [4](./xml_temp/1_gpu_node0_4.xml)
  + NUMA 1 亲和虚拟机
    + [1](./xml_temp/1_gpu_node1_1.xml)
    + [2](./xml_temp/1_gpu_node1_2.xml)
    + [3](./xml_temp/1_gpu_node1_3.xml)
    + [4](./xml_temp/1_gpu_node1_4.xml)
* 8 卡 : [xml](./xml_temp/8_gpu.xml)

## 启动虚拟前nvlink share准备

[参考]()
1. 进入service vm `ssh root@localhost -p 55557`
2. 执行`/root/fmpm -l |grep "isActive\|numGpus\|partitionId\|physicalId" | tac`
   输出和 [fmpm.txt](./fmpm.txt)  对比是否相同，如果不同联系 wangfuqiang49

gpu id和其对应的physical id 如下所示, 如果要开单卡虚拟机， 在启动虚拟机之前，
需要执行 `ssh root@localhost -p 55557 '/root/fmpm -a xxx'`

BDF 和 xxx 对应关系如下:

* db:00.0: 14
* 9b:00.0: 13
* cb:00.0: 12
* bb:00.0: 11
* 3b:00.0: 10
* 5d:00.0:  9
* 19:00.0:  8
* 4c:00.0:  7

销毁虚拟机 : `ssh root@localhost -p 55557 '/root/fmpm -a xxx`
