## 现象
执行 qga 命令
```
virsh qemu-agent-command i-36lexrahpb '{"execute":"guest-ping"}'
Error：Guest agent is not responding: QEMU guest agent is not connected
```
libvirt中有打印:
```
Oct 20 21:49:55 11-241-212-41 journal[1839148]: Guest agent is not responding: QEMU guest agent is not connected
```

## 查看libvirt代码

报错代码:
```cpp
bool
qemuDomainAgentAvailable(virDomainObj *vm,
                         bool reportError)
{
    ...
    //====(1)====
    if (!priv->agent) {
        if (qemuFindAgentConfig(vm->def)) {
            if (reportError) {
                virReportError(VIR_ERR_AGENT_UNRESPONSIVE, "%s",
                               _("QEMU guest agent is not connected"));
            }
            return false;
        } else {
            if (reportError) {
                virReportError(VIR_ERR_ARGUMENT_UNSUPPORTED, "%s",
                               _("QEMU guest agent is not configured"));
            }
            return false;
        }
    }
    ...
}
```

只有当`priv->agent`为空时, 才会报错

查看`agent`变量赋值的代码:

```sh
qemuConnectAgent
=> agent = qemuAgentOpen(..)
...
=> priv->agent = agent;
```
## 编写bpf脚本
```sh
uprobe:/usr/lib64/libvirt/connection-driver/libvirt_driver_qemu.so:qemuAgentOpen
{
        $vm = arg0;
        printf("qemuAgentOpen: vm %lx\n", $vm);
}

uprobe:/usr/lib64/libvirt/connection-driver/libvirt_driver_qemu.so:qemuConnectAgent
{
        $vm = arg1;
        printf("qemuConnectAgent vm %lx\n", $vm);
}
```

执行该bpf脚本，并执行:
```
systemctl restart libvirtd
```

bpftrace打印:
```
qemuConnectAgent vm 7f25201782f0
qemuAgentOpen: vm 7f25201782f0
qemuConnectAgent vm 7f2520178210
qemuAgentOpen: vm 7f2520178210
qemuConnectAgent vm 7f2520178130
qemuAgentOpen: vm 7f2520178130
qemuConnectAgent vm 7f2520025660
qemuAgentOpen: vm 7f2520025660
qemuConnectAgent vm 7f25200d5330
qemuAgentOpen: vm 7f25200d5330
qemuConnectAgent vm 7f25200d55d0
qemuConnectAgent vm 7f2520025040
qemuAgentOpen: vm 7f2520025040
qemuAgentOpen: vm 7f25200d55d0
qemuConnectAgent vm 7f25200253c0
qemuAgentOpen: vm 7f25200253c0
qemuConnectAgent vm 7f2520025200
qemuAgentOpen: vm 7f2520025200
qemuConnectAgent vm 7f25200d5090
qemuAgentOpen: vm 7f25200d5090
qemuConnectAgent vm 7f25200d5410
qemuAgentOpen: vm 7f25200d5410
qemuConnectAgent vm 7f25200d5250
qemuAgentOpen: vm 7f25200d5250
qemuConnectAgent vm 7f25200d54f0
qemuAgentOpen: vm 7f25200d54f0
qemuConnectAgent vm 7f25200d5170
qemuAgentOpen: vm 7f25200d5170
qemuConnectAgent vm 7f2520178050
qemuAgentOpen: vm 7f2520178050
qemuConnectAgent vm 7f2520025120
qemuAgentOpen: vm 7f2520025120
qemuConnectAgent vm 7f25200254a0
qemuAgentOpen: vm 7f25200254a0
qemuConnectAgent vm 7f25201783d0
qemuAgentOpen: vm 7f25201783d0
//====有问题vm======
qemuConnectAgent vm 7f25200252e0
qemuConnectAgent vm 7f25200d56b0
qemuAgentOpen: vm 7f25200d56b0
qemuConnectAgent vm 7f2520025580
qemuAgentOpen: vm 7f2520025580
```

## 使用gdb调试有问题vm
```
## 一个channel
(gdb) p ((virDomainObj *)0x7f25200252e0)->def.nchannels
$2 = 1

## name type都没有问题
(gdb) p ((virDomainObj *)0x7f25200252e0)->def.channels[0]->target.name
$6 = 0x7f25200c65f0 "org.qemu.guest_agent.0"
(gdb) p ((virDomainObj *)0x7f25200252e0)->def.channels[0]->targetType
$7 = 2

## state
(gdb) p ((virDomainObj *)0x7f25200252e0)->def.channels[0].state
$2 = VIR_DOMAIN_CHR_DEVICE_STATE_DISCONNECTED
```

可以看到state 变为`disconnected`

## 查看libvirt xml
```
## 正常机器
[root@11-241-212-41 21:40:50 wangfuqiang49]# virsh dumpxml  i-tvys7xamgd |grep connect
      <target type='virtio' name='org.qemu.guest_agent.0' state='connected'/>

## 异常机器
[root@11-241-212-41 21:41:03 wangfuqiang49]# virsh dumpxml  i-36lexrahpb |grep conne
      <target type='virtio' name='org.qemu.guest_agent.0' state='disconnected'/>
```

可以发现异常机器xml变为`disconnected`
