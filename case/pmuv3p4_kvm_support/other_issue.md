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
## gdb 调试带PMU
```
(gdb) bt
#0  0x0000fffff68f6614 in raise () from /usr/lib64/libc.so.6
#1  0x0000fffff68f7a80 in abort () from /usr/lib64/libc.so.6
#2  0x0000aaaaaaf3591c in kvm_arm_pmu_set_irq (cs=cs@entry=0xaaaaacc00000, irq=<optimized out>, irq@entry=23)
    at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/target/arm/kvm64.c:444
#3  0x0000aaaaaaebd2dc in virt_cpu_init_pmu (cpu=cpu@entry=0xaaaaacc00000, vms=0xaaaaac680000) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/hw/arm/virt.c:617
#4  0x0000aaaaaaec09e8 in fdt_add_pmu_nodes (vms=0xaaaaac680000) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/hw/arm/virt.c:632
#5  machvirt_init (machine=0xaaaaab38e000 <__func__.43377+8>) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/hw/arm/virt.c:1841
#6  0x0000aaaaab0711e0 in machine_run_board_init (machine=0xaaaaac680000) at hw/core/machine.c:1132
#7  0x0000aaaaaad899b4 in main (argc=<optimized out>, argv=<optimized out>, envp=<optimized out>) at vl.c:4484
(gdb) f 2
#2  0x0000aaaaaaf3591c in kvm_arm_pmu_set_irq (cs=cs@entry=0xaaaaacc00000, irq=<optimized out>, irq@entry=23)
    at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/target/arm/kvm64.c:444
444	        abort();

```

代码为:
```sh
machvirt_init
=> fdt_add_pmu_nodes
   => CPU_FOREACH(cpu)
      => if !arm_feature(&armcpu->env, ARM_FEATURE_PMU)
         \-> return
      => if kvm_enabled()
         \=> if kvm_irqchip_in_kernel()
             \=> kvm_arm_pmu_set_irq(cpu, PPI(VIRTUAL_PMU_IRQ));
                 |=> struct kvm_device_attr attr = {
                          .group = KVM_ARM_VCPU_PMU_V3_CTRL,
                          .addr = (intptr_t)&irq,
                          .attr = KVM_ARM_VCPU_PMU_V3_IRQ,
                     } 
                 |=> kvm_arm_pmu_set_attr(cs, &attr);
         |=> kvm_arm_pmu_init()
             => struct kvm_device_attr attr = {
                    .group = KVM_ARM_VCPU_PMU_V3_CTRL,
                    .attr = KVM_ARM_VCPU_PMU_V3_INIT,
                };
             => kvm_arm_pmu_set_attr(cs, &attr)
   ## 下面构造fdt node
   => armcpu = ARM_CPU(qemu_get_cpu(0));
   => qemu_fdt_add_subnode(vms->fdt, "/pmu");
   => if arm_feature(&armcpu->env, ARM_FEATURE_V8)
      => const char compat[] = "arm,armv8-pmuv3";
      => qemu_fdt_setprop(vms->fdt, "/pmu", "compatible", ...)
      => qemu_fdt_setprop_cells(vms->fdt, "/pmu", "interrupts", ...)
```
大概的逻辑是:
1. 先设置PMU IRQ vector
2. 调用` KVM_ARM_VCPU_PMU_V3_INIT` 初始化PMU
3. 在fdt 中新增 pmu node

kvm 中会调用`kvm_arm_pmu_v3_set_attr` 执行具体的attr, 使用function_graph tracer
调试:

```
[root@43-66-4-72 20:07:14 tracing]# echo kvm_arm_pmu_v3_set_attr > set_graph_function
[root@43-66-4-72 20:07:33 tracing]# echo function_graph > current_tracer 
[root@43-66-4-72 20:10:06 tracing]# cat trace
# tracer: function_graph
#
# CPU  DURATION                  FUNCTION CALLS
# |     |   |                     |   |   |   |
 230)   2.770 us    |  kvm_arm_pmu_v3_set_attr();
 ------------------------------------------
 230) qemu-kv-712267 => qemu-kv-712819
```

## gdb 调试 不带 PMU

在此处打断点:
```cpp
int kvm_set_irq(KVMState *s, int irq, int level)
{
    struct kvm_irq_level event;
    int ret;

    assert(kvm_async_interrupts_enabled());

    event.level = level;
    event.irq = irq;
    ret = kvm_vm_ioctl(s, s->irq_set_ioctl, &event);
    if (ret < 0) {
        perror("kvm_set_irq");   //here
        abort();
    }

    return (s->irq_set_ioctl == KVM_IRQ_LINE) ? 1 : event.status;
}
```
调试断点:
```
Thread 4 (LWP 1515341):
#0  kvm_set_irq (s=0xaaaaac7b0000, irq=<optimized out>, level=<optimized out>) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/accel/kvm/kvm-all.c:1272
#1  0x0000aaaaaaf203bc in write_cpustate_to_list (cpu=cpu@entry=0xaaaaacc00000, kvm_sync=kvm_sync@entry=true) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/target/arm/helper.c:323
#2  0x0000aaaaaaf36578 in kvm_arch_put_registers (cs=0xaaaaacc00000, level=43690) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/target/arm/kvm64.c:912
#3  0x0000aaaaaadf4114 in do_kvm_cpu_synchronize_post_init (cpu=0xaaaaacc00000, arg=...) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/accel/kvm/kvm-all.c:2199
#4  0x0000aaaaaaffd350 in process_queued_cpu_work (cpu=0xaaaaacc00000) at cpus-common.c:342
#5  0x0000aaaaaadd4b50 in qemu_wait_io_event (cpu=cpu@entry=0xaaaaacc00000) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/cpus.c:1254
#6  0x0000aaaaaadd4ea0 in qemu_kvm_cpu_thread_fn (arg=0xaaaaacc00000) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/cpus.c:1290
#7  0x0000aaaaab31f15c in qemu_thread_start (args=0xaaaaac6540e0) at util/qemu-thread-posix.c:502
#8  0x0000fffff6a587a0 in ?? () from /usr/lib64/libpthread.so.0
#9  0x0000fffff699bcbc in ?? () from /usr/lib64/libc.so.6

(gdb) f 1
#1  0x0000aaaaaaf203bc in write_cpustate_to_list (cpu=cpu@entry=0xaaaaacc00000, kvm_sync=kvm_sync@entry=true)
    at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/target/arm/helper.c:323
323	                write_raw_cp_reg(&cpu->env, ri, oldval);
(gdb) p *ri
$10 = {name = 0xaaaaac1a69b0 "PMINTENCLR_EL1", cp = 19 '\023', crn = 9 '\t', crm = 14 '\016', opc0 = 3 '\003', opc1 = 0 '\000', opc2 = 2 '\002', state = 1, type = 96, 
  access = 252, secure = 2, opaque = 0x0, resetvalue = 0, fieldoffset = 1168, bank_fieldoffsets = {0, 0}, accessfn = 0xaaaaaaf18f38 <access_tpm>, readfn = 0x0, 
  writefn = 0xaaaaaaf160c8 <pmintenclr_write>, raw_readfn = 0x0, raw_writefn = 0x0, resetfn = 0x0}
```

发现同样是`PMU`相关寄存器.
