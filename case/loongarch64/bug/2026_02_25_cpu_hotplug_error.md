## cpu 热插拔失败

执行`virsh` 命令`virsh setvcpus i-dv8eouzael 4 --live`

qemu log:

```
365622@[2026-02-25 19:50:50.047452] qmp_dispatch qmp_cmd_name: query-cpus-fast, arguments: {}
qmp_cmd_name: device_add, arguments: {"core-id": 0, "thread-id": 0, "node-id": 0, "socket-id": 3, "driver": "la464-loongarch-cpu", "id": "vcpu3"}
365622@[2026-02-25 19:50:50.098333] qmp_dispatch qmp_cmd_name: device_add, arguments: {"core-id": 0, "thread-id": 0, "node-id": 0, "socket-id": 3, "driver": "la464-loongarch-cpu", "id": "vcpu3"}
add qdev la464-loongarch-cpu:vcpu3 success
add qdev la464-loongarch-cpu:vcpu3 success
{"timestamp": {"seconds": 1772020250, "microseconds": 129638}, "event": "ACPI_DEVICE_OST", "data": {"info": {"device": "vcpu3", "source": 1, "status": 0, "slot": "3", "slot-type": "CPU"}}}
qmp_cmd_name: query-hotpluggable-cpus, arguments: {}
365622@[2026-02-25 19:50:50.179776] qmp_dispatch qmp_cmd_name: query-hotpluggable-cpus, arguments: {}
qmp_cmd_name: query-cpus-fast, arguments: {}
365622@[2026-02-25 19:50:50.182243] qmp_dispatch qmp_cmd_name: query-cpus-fast, arguments: {}
```

并未报错.

guest dmesg中有报错:

```
loongarch-test login: [   51.488809][  T802] ACPI: Changing CPU present bit is not supported
```

## 环境信息(openeuler 2403 sp2)

* kernel: 6.6.0-117.0.0.122
* qemu: 8.2.0-37
* edk2-ovmf: edk2-ovmf-loongarch64-202308-23
* libvirt: libvirt-9.10.0-16

## 参考链接

[libvirt doc: setvcpus](https://www.libvirt.org/manpages/virsh.html#setvcpus)
