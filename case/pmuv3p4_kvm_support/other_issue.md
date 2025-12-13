## 启用新内核后异常现象

执行 `virsh domcapabilities` 命令会卡住
```
[root@43-66-4-72 11:27:25 qemu]# virsh domcapabilities


```
打算清空cap 缓存, 但是发现目录下为空:
```
[root@43-66-4-72 11:29:15 qemu]# ls /var/cache/libvirt/qemu/capabilities/
[root@43-66-4-72 11:29:31 qemu]# 
```

怀疑是 libvirtd 启动qemu出了问题, 查看qemu-kvm进程状态:
```
root        9354  0.0  0.0 229312 52416 ?        Sl   Dec12   0:00 /usr/libexec/qemu-kvm -S -no-user-config -nodefaults -nographic -machine none,accel=kvm:tcg -qmp unix:/var/lib/libvirt/qemu/qmp-BGQCH3/qmp.monitor,server,nowait -pidfile /var/lib/libvirt/qemu/qmp-BGQCH3/qmp.pid -daemonize
root        9356  0.0  0.0      0     0 ?        Zs   Dec12   0:00 [qemu-kvm] <defunct>
root        9358  0.0  0.0 237568 24576 ?        Sl   Dec12   0:06 /usr/libexec/qemu-kvm -S -no-user-config -nodefaults -nographic -machine none,accel=kvm:tcg -qmp unix:/var/lib/libvirt/qemu/qmp-BGQCH3/qmp.monitor,server,nowait -pidfile /var/lib/libvirt/qemu/qmp-BGQCH3/qmp.pid -daemonize
root     1353597  0.0  0.0 229248 52352 ?        Sl   10:51   0:00 /usr/libexec/qemu-kvm -S -no-user-config -nodefaults -nographic -machine none,accel=kvm:tcg -qmp unix:/var/lib/libvirt/qemu/qmp-POCFH3/qmp.monitor,server,nowait -pidfile /var/lib/libvirt/qemu/qmp-POCFH3/qmp.pid -daemonize
root     1353599  0.0  0.0      0     0 ?        Zs   10:51   0:00 [qemu-kvm] <defunct>
root     1353601  0.0  0.0 237504 24512 ?        Sl   10:51   0:00 /usr/libexec/qemu-kvm -S -no-user-config -nodefaults -nographic -machine none,accel=kvm:tcg -qmp unix:/var/lib/libvirt/qemu/qmp-POCFH3/qmp.monitor,server,nowait -pidfile /var/lib/libvirt/qemu/qmp-POCFH3/qmp.pid -daemonize
root     1402916  0.0  0.0 214080  1536 pts/2    S+   11:30   0:00 grep --color=auto qemu
```
可以发现有一些僵尸进程.



使用下面命令简单启动虚拟机，发现启动均失败:

* 带PMU
  ```
  [root@43-66-4-72 11:22:20 qemu]# qemu-kvm -cpu host -smp 2 -m 1G  -machine virt 2>&1|tail -10
  qemu-kvm: warning: Trying to enable KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2 but failed.  Falling back to the legacy mode. 
  1397470@[2025-12-13 11:26:26.712406] runstate_setcurrent_run_state 16 (preconfig) new_state 6 (prelaunch)
  1397470@[2025-12-13 11:26:26.720249] memory_region_add_subregion_commonname is system, subregion name is virt.flash0
  1397470@[2025-12-13 11:26:26.720331] memory_region_add_subregion_commonname is system, subregion name is virt.flash1
  1397470@[2025-12-13 11:26:26.725796] memory_region_add_subregion_commonname is system, subregion name is mach-virt.ram
  1397470@[2025-12-13 11:26:26.726292] memory_region_add_subregion_commonname is system, subregion name is gic_dist
  1397470@[2025-12-13 11:26:26.726326] memory_region_add_subregion_commonname is system, subregion name is gic_cpu
  1397470@[2025-12-13 11:26:26.726537] memory_region_add_subregion_commonname is system, subregion name is gicv2m
  qemu-kvm: PMU: KVM_SET_DEVICE_ATTR: Invalid argument
  qemu-kvm: failed to set irq for PMU
  Aborted
  ```

* 不带PMU
  ```
  [root@43-66-4-72 11:26:26 qemu]# qemu-kvm -cpu host,pmu=off -smp 2 -m 1G  -machine virt 2>&1|tail -10
  1397777@[2025-12-13 11:26:41.991303] memory_region_add_subregion_commonname is system, subregion name is virtio-mmio
  1397777@[2025-12-13 11:26:41.991372] memory_region_add_subregion_commonname is system, subregion name is virtio-mmio
  1397777@[2025-12-13 11:26:41.991442] memory_region_add_subregion_commonname is system, subregion name is virtio-mmio
  1397777@[2025-12-13 11:26:41.991548] memory_region_add_subregion_commonname is system, subregion name is virtio-mmio
  1397777@[2025-12-13 11:26:41.991626] memory_region_add_subregion_commonname is system, subregion name is virtio-mmio
  1397777@[2025-12-13 11:26:41.992535] memory_region_add_subregion_commonname is system, subregion name is fwcfg.ctl
  1397777@[2025-12-13 11:26:41.992601] memory_region_add_subregion_commonname is system, subregion name is fwcfg.data
  1397777@[2025-12-13 11:26:41.992661] memory_region_add_subregion_commonname is system, subregion name is fwcfg.dma
  1397777@[2025-12-13 11:26:41.993900] memory_region_add_subregion_commonname is system, subregion name is platform bus
  kvm_set_irq: No such device or address
  Aborted
  ```
