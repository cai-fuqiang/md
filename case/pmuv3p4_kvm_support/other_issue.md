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

**使用-machine virt,gic-version=3** 可以规避该问题, 由于启用虚拟机都带该选项，该问题不再纠结

***

##  回到该问题

本地复现了该问题，使用virsh 命令启动虚拟机:

```
[root@11-244-8-51 17:13:00 ~]# virsh start i-zd0tggqip8

```
发现会卡住, 查看qemu进程:
```
[root@11-244-8-51 17:13:12 ~]# ps aux |grep qemu
root     3447969  0.0  0.0 229568 52544 ?        Sl   16:54   0:00 /usr/libexec/qemu-kvm -S -no-user-config -nodefaults -nographic -machine none,accel=kvm:tcg -qmp unix:/var/lib/libvirt/qemu/qmp-3PFRH3/qmp.monitor,server,nowait -pidfile /var/lib/libvirt/qemu/qmp-3PFRH3/qmp.pid -daemonize
root     3447971  0.0  0.0      0     0 ?        Zs   16:54   0:00 [qemu-kvm] <defunct>
root     3447973  0.0  0.0 237824 24640 ?        Sl   16:54   0:00 /usr/libexec/qemu-kvm -S -no-user-config -nodefaults -nographic -machine none,accel=kvm:tcg -qmp unix:/var/lib/libvirt/qemu/qmp-3PFRH3/qmp.monitor,server,nowait -pidfile /var/lib/libvirt/qemu/qmp-3PFRH3/qmp.pid -daemonize
```
只有上面这些.

先看下libvirt堆栈：
```
Thread 8 (LWP 3417432):
#0  0x0000fffe17081eec in poll () at /usr/lib64/libc.so.6
#1  0x0000fffe180d986c in poll (__timeout=-1, __nfds=2, __fds=<optimized out>) at /usr/include/bits/poll2.h:46
#2  virCommandProcessIO (cmd=cmd@entry=0xfffdec015b90) at ../../src/util/vircommand.c:2272
#3  0x0000fffe180dd7a8 in virCommandRun (cmd=0xfffdec015b90, exitstatus=exitstatus@entry=0xfffe05cdde08) at ../../src/util/vircommand.c:2493
#4  0x0000fffde69c59d0 in qemuProcessQMPLaunch (proc=0xfffdec012c00) at ../../src/qemu/qemu_process.c:8541
#5  qemuProcessQMPStart (proc=proc@entry=0xfffdec012c00) at ../../src/qemu/qemu_process.c:8637
#6  0x0000fffde6962f08 in virQEMUCapsInitQMPSingle (qemuCaps=qemuCaps@entry=0xfffdec015a90, libDir=libDir@entry=0xfffdb8121790 "/var/lib/libvirt/qemu", runUid=runUid@e--Type <RET> for more, q to quit, c to continue without paging--
ntry=0, runGid=runGid@entry=0, onlyTCG=onlyTCG@entry=false) at ../../src/qemu/qemu_capabilities.c:5156
#7  0x0000fffde69630bc in virQEMUCapsInitQMP (runGid=0, runUid=0, libDir=0xfffdb8121790 "/var/lib/libvirt/qemu", qemuCaps=0xfffdec015a90) at ../../src/qemu/qemu_capabilities.c:5179
#8  virQEMUCapsNewForBinaryInternal (hostArch=VIR_ARCH_AARCH64, binary=0xfffdec00cec0 "/usr/libexec/qemu-kvm", libDir=0xfffdb8121790 "/var/lib/libvirt/qemu", runUid=0, runGid=0, microcodeVersion=0, kernelVersion=0xfffdb814e140 "4.19.90-89.27.v2401.fortest.ky10.aarch64 #1 SMP Thu Dec 11 21:29:38 CST 2025") at ../../src/qemu/qemu_capabilities.c:5229
#9  0x0000fffe18155d20 in virFileCacheNewData (name=0xfffdec00cec0 "/usr/libexec/qemu-kvm", cache=0xfffdb8121620) at ../../src/util/virfilecache.c:205
#10 virFileCacheValidate (cache=cache@entry=0xfffdb8121620, name=name@entry=0xfffdec00cec0 "/usr/libexec/qemu-kvm", data=0xfffe05cde0e0, data@entry=0xfffe05cde100) at ../../src/util/virfilecache.c:274
#11 0x0000fffe18155ff0 in virFileCacheLookup (cache=cache@entry=0xfffdb8121620, name=name@entry=0xfffdec00cec0 "/usr/libexec/qemu-kvm") at ../../src/util/virfilecache.c:307
#12 0x0000fffde6963498 in virQEMUCapsCacheLookup (cache=cache@entry=0xfffdb8121620, binary=binary@entry=0xfffdec00cec0 "/usr/libexec/qemu-kvm") at ../../src/qemu/qemu_capabilities.c:5390
#13 0x0000fffde6963584 in virQEMUCapsInitGuest (guestarch=VIR_ARCH_AARCH64, hostarch=VIR_ARCH_AARCH64, cache=0xfffdb8121620, caps=0xfffdec00cf90) at ../../src/qemu/qemu_capabilities.c:885
#14 virQEMUCapsInit (cache=0xfffdb8121620) at ../../src/qemu/qemu_capabilities.c:1107
#15 0x0000fffde69b3bb8 in virQEMUDriverCreateCapabilities (driver=0xaaab0edfcc50, driver@entry=0xfffdb813e830) at ../../src/qemu/qemu_conf.c:1310
#16 0x0000fffde69b3ecc in virQEMUDriverGetCapabilities (driver=driver@entry=0xfffdb813e830, refresh=refresh@entry=false) at ../../src/qemu/qemu_conf.c:1373
#17 0x0000fffde6a172f4 in qemuConnectGetVersion (conn=<optimized out>, version=0xfffe05cde330) at ../../src/qemu/qemu_driver.c:1683
#18 0x0000fffe182b1248 in virConnectGetVersion (conn=0xfffdf40060f0, hvVer=0xfffe05cde330, hvVer@entry=0xfffe05cde370) at ../../src/libvirt-host.c:195
#19 0x0000aaaad17a5f4c in remoteDispatchConnectGetVersion (server=0xaaab0ed71590, msg=0xaaab0edfcc50, ret=0xfffdec00cea0, rerr=0xfffe05cde430, client=0xaaab0edfc6e0) a--Type <RET> for more, q to quit, c to continue without paging--
t ./remote/remote_daemon_dispatch_stubs.h:1265
#20 remoteDispatchConnectGetVersionHelper (server=0xaaab0ed71590, client=0xaaab0edfc6e0, msg=0xaaab0edfcc50, rerr=0xfffe05cde430, args=0x0, ret=0xfffdec00cea0) at ./remote/remote_daemon_dispatch_stubs.h:1247
#21 0x0000fffe1822fd64 in virNetServerProgramDispatchCall (msg=0xaaab0edfcc50, client=0xaaab0edfc6e0, server=0x0, prog=0xaaab0edd3d20) at ../../src/rpc/virnetserverprogram.c:430
#22 virNetServerProgramDispatch (prog=0xaaab0edd3d20, server=0x0, server@entry=0xaaab0ed71590, client=0xaaab0edfc6e0, msg=0xaaab0edfcc50) at ../../src/rpc/virnetserverprogram.c:302
#23 0x0000fffe182352dc in virNetServerProcessMsg (msg=<optimized out>, prog=<optimized out>, client=<optimized out>, srv=0xaaab0ed71590) at ../../src/rpc/virnetserver.c:137
#24 virNetServerHandleJob (jobOpaque=0xaaab0edeb160, opaque=0xaaab0ed71590) at ../../src/rpc/virnetserver.c:154
#25 0x0000fffe181484e4 in virThreadPoolWorker (opaque=<optimized out>) at ../../src/util/virthreadpool.c:163
#26 0x0000fffe18147a5c in virThreadHelper (data=<optimized out>) at ../../src/util/virthread.c:233
#27 0x0000fffe171b87a0 in  () at /usr/lib64/libpthread.so.0
#28 0x0000fffe1708bcbc in  () at /usr/lib64/libc.so.6
```
发现有个栈阻塞在调用qemu命令，查看其cmdline:
```
(gdb) p cmd->args[0]
$2 = 0xfffdec014c00 "/usr/libexec/qemu-kvm"
(gdb) p cmd->args[1]
$3 = 0xfffdec005de0 "-S"
(gdb) p cmd->args[2]
$4 = 0xfffdec00cf00 "-no-user-config"
(gdb) p cmd->args[3]
$5 = 0xfffdec012fa0 "-nodefaults"
(gdb) p cmd->args[4]
$6 = 0xfffdec0136a0 "-nographic"
(gdb) p cmd->args[5]
$7 = 0xfffdec007e00 "-machine"
(gdb) p cmd->args[6]
$8 = 0xfffdec015440 "none,accel=kvm:tcg"
(gdb) p cmd->args[7]
$9 = 0xfffdec013d00 "-qmp"
(gdb) p cmd->args[8]
$10 = 0xfffdec014340 "unix:/var/lib/libvirt/qemu/qmp-3PFRH3/qmp.monitor,server,nowait"
(gdb) p cmd->args[9]
$11 = 0xfffdec013d20 "-pidfile"
(gdb) p cmd->args[10]
$12 = 0xfffdec00ca00 "/var/lib/libvirt/qemu/qmp-3PFRH3/qmp.pid"
(gdb) p cmd->args[11]
$13 = 0xfffdec013d40 "-daemonize"
```
可以发现，libvirt是再等这些`qemu-kvm`进程返回.

编写脚本:
```
[root@11-244-8-51 17:17:36 ~]# cat qemu_kvm.sh
/usr/libexec/qemu-kvm \
-S \
-no-user-config \
-nodefaults \
-nographic \
-name "wangfuqiang" \
-machine none,accel=kvm:tcg \
-qmp unix:/tmp/qmp.monitor,server,nowait \
-pidfile /tmp/qmp.pid \
-daemonize \
```

执行该脚本，发现卡住, 查看进程信息:
```
root     3491402  1.2  0.0 229568 52544 pts/3    Sl+  17:20   0:00 /usr/libexec/qemu-kvm -S -no-user-config -nodefaults -nographic -name wangfuqiang -machine none,accel=kvm:tcg -qmp unix:/tmp/qmp.monitor,server,nowait -pidfile /tmp/qmp.pid -daemonize
root     3491406  0.0  0.0 237824 24768 ?        Sl   17:20   0:00 /usr/libexec/qemu-kvm -S -no-user-config -nodefaults -nographic -name wangfuqiang -machine none,accel=kvm:tcg -qmp unix:/tmp/qmp.monitor,server,nowait -pidfile /tmp/qmp.pid -daemonize
root     3491555  0.0  0.0 214080  1536 pts/2    S+   17:20   0:00 grep --color=auto wangfuqiang

[root@11-244-8-51 17:22:31 ~]# ps -o comm,pid,ppid -ax |grep qemu-kvm |grep -E '3491402|3491406'
qemu-kvm        3491402 3491401
qemu-kvm        3491406       1
[root@11-244-8-51 17:22:45 ~]# ps aux |grep 3491401
root     3491401  0.0  0.0 214592  1728 pts/3    S+   17:20   0:00 sh qemu_kvm.sh
```

那就是`qemu_kvm.sh(3491401)->qemu-kvm(3491402)`, `3491402`启动`3491406(vm进程)`后，`3491406` 变为
`daemonize`.

查看阻塞堆栈
* 3491402
  ```
  Thread 2 (LWP 3491403):
  #0  0x0000fffeb99d7e10 in syscall () from /usr/lib64/libc.so.6
  #1  0x0000aaaab2a1ffc8 in qemu_futex_wait (val=<optimized out>, f=<optimized out>) at /usr/src/debug/qemu-4.1.0.30-3253dce3.ky10.aarch64/include/qemu/futex.h:29
  #2  qemu_event_wait (ev=ev@entry=0xaaaab3065838 <rcu_call_ready_event>) at util/qemu-thread-posix.c:442
  #3  0x0000aaaab2a32214 in call_rcu_thread (opaque=<optimized out>) at util/rcu.c:260
  #4  0x0000aaaab2a1f15c in qemu_thread_start (args=0xaaaae0cd57a0) at util/qemu-thread-posix.c:502
  #5  0x0000fffeb9a987a0 in ?? () from /usr/lib64/libpthread.so.0
  #6  0x0000fffeb99dbcbc in ?? () from /usr/lib64/libc.so.6
  
  Thread 1 (LWP 3491402):
  #0  0x0000fffeb9aa23c4 in read () from /usr/lib64/libpthread.so.0
  #1  0x0000aaaab26f630c in read (__nbytes=1, __buf=0xffffd2fe15ff, __fd=<optimized out>) at /usr/include/bits/unistd.h:44
  #2  os_daemonize () at os-posix.c:272
  #3  0x0000aaaab24868b4 in main (argc=<optimized out>, argv=<optimized out>, envp=<optimized out>) at vl.c:3998
  ```
* 3491406:
  ```
  (gdb) thread apply all bt
  
  Thread 3 (LWP 3491408):
  #0  0x0000fffeb99d7e10 in syscall () from /usr/lib64/libc.so.6
  #1  0x0000fffeba175f94 in base::internal::SpinLockDelay(int volatile*, int, int) () from /usr/lib64/libtcmalloc.so.4
  #2  0x0000fffeba175e30 in SpinLock::SlowLock() () from /usr/lib64/libtcmalloc.so.4
  #3  0x0000fffeba164514 in tcmalloc::CentralFreeList::InsertRange(void*, void*, int) () from /usr/lib64/libtcmalloc.so.4
  #4  0x0000fffebae2aa14 in g_free () from /usr/lib64/libglib-2.0.so.0
  #5  0x0000aaaab2a1f14c in qemu_thread_start (args=0xaaaae0f32c40) at util/qemu-thread-posix.c:499
  #6  0x0000fffeb9a987a0 in ?? () from /usr/lib64/libpthread.so.0
  #7  0x0000fffeb99dbcbc in ?? () from /usr/lib64/libc.so.6
  
  Thread 2 (LWP 3491407):
  #0  0x0000fffeb99d7e10 in syscall () from /usr/lib64/libc.so.6
  #1  0x0000fffeba175f94 in base::internal::SpinLockDelay(int volatile*, int, int) () from /usr/lib64/libtcmalloc.so.4
  #2  0x0000fffeba175e30 in SpinLock::SlowLock() () from /usr/lib64/libtcmalloc.so.4
  #3  0x0000fffeba164514 in tcmalloc::CentralFreeList::InsertRange(void*, void*, int) () from /usr/lib64/libtcmalloc.so.4
  #4  0x0000fffebae2aa14 in g_free () from /usr/lib64/libglib-2.0.so.0
  #5  0x0000aaaab2a1f14c in qemu_thread_start (args=0xaaaae0f32c20) at util/qemu-thread-posix.c:499
  #6  0x0000fffeb9a987a0 in ?? () from /usr/lib64/libpthread.so.0
  #7  0x0000fffeb99dbcbc in ?? () from /usr/lib64/libc.so.6
  
  Thread 1 (LWP 3491406):
  #0  0x0000fffeb99d7e10 in syscall () from /usr/lib64/libc.so.6
  #1  0x0000fffeba175f94 in base::internal::SpinLockDelay(int volatile*, int, int) () from /usr/lib64/libtcmalloc.so.4
  #2  0x0000fffeba175e30 in SpinLock::SlowLock() () from /usr/lib64/libtcmalloc.so.4
  #3  0x0000fffeba164a6c in tcmalloc::CentralFreeList::RemoveRange(void**, void**, int) () from /usr/lib64/libtcmalloc.so.4
  #4  0x0000fffeba1682b8 in tcmalloc::ThreadCache::FetchFromCentralCache(unsigned int, int, void* (*)(unsigned long)) () from /usr/lib64/libtcmalloc.so.4
  #5  0x0000fffeba179708 in tc_realloc () from /usr/lib64/libtcmalloc.so.4
  #6  0x0000fffebae2a9bc in g_realloc () from /usr/lib64/libglib-2.0.so.0
  #7  0x0000fffebadf0928 in ?? () from /usr/lib64/libglib-2.0.so.0
  #8  0x0000fffebadf20e0 in g_ptr_array_add () from /usr/lib64/libglib-2.0.so.0
  #9  0x0000fffebae47a58 in g_strsplit () from /usr/lib64/libglib-2.0.so.0
  #10 0x0000aaaab2929710 in container_get (root=0xaaaae0dea800, path=<optimized out>) at qom/container.c:34
  #11 0x0000aaaab2486a24 in main (argc=<optimized out>, argv=<optimized out>, envp=<optimized out>) at vl.c:4039
  ```
qemu进程死锁了?

## 分析第一个进程

堆栈为:
```sh
main
=> os_daemonize()
   => pipe(fds);
   => pid = fork();
   ## 父进程
   => if pid > 0
      => close(fds[1]);
         ## 估计是卡在这里了
         => len = read(fds[0], &status, 1);
         ## 读到1 byte并且status==0 ，认为子进程创建的没有问题返回0(父进程退出)
         => exit(len == 1 && status == 0 ? 0 : 1);
   ## 下面是子进程流程
   => close(fds[0]);
   => daemon_pipe = fds[1];
   ## 这个的作用是, 设置增加  `FD_CLDEXEC` 给这个管道描述符，防止该进程exec
   ## 导致父进程一直等待
   => qemu_set_cloexec(daemon_pipe);
   ## 下面的流程是变daemon的常规操作
   => setsid();
   => pid = fork()
   ## 该进程exit
   => if (pid > 0) exit(0)
   ## 子进程继续运行(变daemon了)
   => umask(027);
   => signal(xxxx)
```

那该管道什么时候写入呢?
```sh
main
=> os_daemonize()
=> 各种初始化 
=> os_setup_post
   => len = write(daemon_pipe, &status, 1);
## 执行main_loop()
=> main_loop()
```
可以看到在执行`main_loop()`之前, 才会调用`os_setup_post()`, 比较靠后了。

而子进程在比较靠前的流程中因`g_realloc`卡死了:
```sh
main
=> object_property_add_child(container_get(OBJECT(current_machine),
            "/unattached"), "sysbus", OBJECT(sysbus_get_default()),
            NULL);
```

## 使用tracepoint syscall::futex 调试

1. 打开trace event
   ```
   echo 0 > /sys/kernel/debug/tracing/tracing_on
   echo '' > /sys/kernel/debug/tracing/trace
   echo 1 > /sys/kernel/debug/tracing/events/syscalls/sys_enter_futex/enable
   echo 1 > /sys/kernel/debug/tracing/events/syscalls/sys_exit_futex/enable
   echo 1 > /sys/kernel/debug/tracing/tracing_on
   ```
2. 启动qemu_kvm进程
3. 过滤
   ```
    cat /sys/kernel/debug/tracing/trace_pipe|grep qemu-kvm2 > futex_log.txt
   ```
5. 可以发现
   ```
   qemu-kvm2-147733 [079] ....  5066.151067: sys_futex(uaddr: ffffad0a5838, op: 80, val: 2, utime: ffffa239b928, uaddr2: 0, val3: 0)
   qemu-kvm2-147733 [079] ....  5066.151151: sys_futex -> 0xffffffffffffff92
   qemu-kvm2-147733 [079] ....  5066.151156: sys_futex(uaddr: ffffad0a5838, op: 80, val: 2, utime: ffffa239b928, uaddr2: 0, val3: 0)
   qemu-kvm2-147734 [079] ....  5066.151168: sys_futex(uaddr: ffffad0a5838, op: 80, val: 2, utime: ffffa1b8b928, uaddr2: 0, val3: 0)
   qemu-kvm2-147732 [078] ....  5066.151359: sys_futex(uaddr: ffffad0a5838, op: 80, val: 2, utime: fffff2b36ae8, uaddr2: 0, val3: 0)
   qemu-kvm2-147732 [078] ....  5066.152618: sys_futex -> 0xffffffffffffff92
   qemu-kvm2-147732 [078] ....  5066.152623: sys_futex(uaddr: ffffad0a5838, op: 80, val: 2, utime: fffff2b36ae8, uaddr2: 0, val3: 0)
   qemu-kvm2-147729 [076] ....  5066.154355: sys_futex(uaddr: aaab1e105838, op: 0, val: ffffffff, utime: 0, uaddr2: 0, val3: 0)
   qemu-kvm2-147733 [079] ....  5066.155163: sys_futex -> 0xffffffffffffff92
   ```
   qemu的三个线程只执行过 `WAIT`, 但是没有执行过`WAKEUP`
## 如下修改代码, 让子进程, 父进程在fork()后STOP
```diff
 void os_daemonize(void)
 {
+    pid_t pid_tmp;
     if (daemonize) {
         pid_t pid;
         int fds[2];
@@ -288,6 +289,10 @@ void os_daemonize(void)

         pid = fork();
         if (pid > 0) {
+           pid_tmp = getpid();
+            printf("[%d] daemon father stop...\n", pid_tmp);
+           kill(pid_tmp, SIGSTOP);
+            printf("[%d] daemon father stop end\n", pid_tmp);
             exit(0);
         } else if (pid < 0) {
             exit(1);
@@ -297,6 +302,10 @@ void os_daemonize(void)
         signal(SIGTSTP, SIG_IGN);
         signal(SIGTTOU, SIG_IGN);
         signal(SIGTTIN, SIG_IGN);
+       pid_tmp = getpid();
+       printf("[%d]: child process begin exec, STOP...\n", pid_tmp);
+       kill(pid_tmp, SIGSTOP);
+       printf("[%d]: child process STOP END\n", pid_tmp);
     }
 }
```
使父子进程fork()前后均调用`kill()`, 然后使用gdb调试两者堆栈:

父进程
```
Thread 2 (LWP 346664):
#0  0x0000fffe7dd72c58 in nanosleep () from /usr/lib64/libpthread.so.0
#1  0x0000fffe7f121b1c in g_usleep () from /usr/lib64/libglib-2.0.so.0
#2  0x0000aaac35311ee4 in call_rcu_thread (opaque=<optimized out>) at util/rcu.c:252
#3  0x0000aaac352fee6c in qemu_thread_start (args=<optimized out>) at util/qemu-thread-posix.c:502
#4  0x0000fffe7dd687a0 in ?? () from /usr/lib64/libpthread.so.0
#5  0x0000fffe7dcabcbc in ?? () from /usr/lib64/libc.so.6

Thread 1 (LWP 346663):
#0  0x0000fffe7dc06948 in kill () from /usr/lib64/libc.so.6
#1  0x0000aaac34fd6370 in os_daemonize () at os-posix.c:294
#2  0x0000aaac34d66934 in main (argc=<optimized out>, argv=<optimized out>, envp=<optimized out>) at vl.c:3998
```

子进程:
```
Thread 2 (LWP 346666):
#0  0x0000fffe7dca7e10 in syscall () from /usr/lib64/libc.so.6
#1  0x0000fffe7e445f94 in base::internal::SpinLockDelay (w=0xfffe7e575838 <tcmalloc::Static::central_cache_+2432>, value=2, loop=<optimized out>) at ./src/base/spinlock_linux-inl.h:84
#2  0x0000fffe7e445e30 in SpinLock::SlowLock (this=0xfffe7e575838 <tcmalloc::Static::central_cache_+2432>) at src/base/spinlock.cc:118
#3  0x0000fffe7e434514 in SpinLock::Lock (this=0xfffe7e575838 <tcmalloc::Static::central_cache_+2432>) at src/base/spinlock.h:71
#4  SpinLockHolder::SpinLockHolder (l=0xfffe7e575838 <tcmalloc::Static::central_cache_+2432>, this=<synthetic pointer>) at src/base/spinlock.h:133
#5  tcmalloc::CentralFreeList::InsertRange (this=0xfffe7e575838 <tcmalloc::Static::central_cache_+2432>, start=0xaaac3d955820, end=0xaaac3d955820, N=1) at src/central_freelist.cc:230
#6  0x0000fffe7f0faa14 in g_free () from /usr/lib64/libglib-2.0.so.0
#7  0x0000aaac352fee34 in qemu_thread_start (args=0xaaac3de02c00) at util/qemu-thread-posix.c:499
#8  0x0000fffe7dd687a0 in ?? () from /usr/lib64/libpthread.so.0
#9  0x0000fffe7dcabcbc in ?? () from /usr/lib64/libc.so.6

Thread 1 (LWP 346665):
#0  0x0000fffe7dc06948 in kill () from /usr/lib64/libc.so.6
#1  0x0000aaac34fd632c in os_daemonize () at os-posix.c:307
#2  0x0000aaac34d66934 in main (argc=<optimized out>, argv=<optimized out>, envp=<optimized out>) at vl.c:3998
```

子进程刚刚进入就卡住了, 而卡住的位置:
```cpp
480 static void *qemu_thread_start(void *args)
481 {
482     QemuThreadArgs *qemu_thread_args = args;
483     void *(*start_routine)(void *) = qemu_thread_args->start_routine;
484     void *arg = qemu_thread_args->arg;
485     void *r;
486
487 #ifdef CONFIG_THREAD_SETNAME_BYTHREAD
488     /* Attempt to set the threads name; note that this is for debug, so
489      * we're not going to fail if we can't set it.
490      */
491     if (name_threads && qemu_thread_args->name) {
492 # if defined(CONFIG_PTHREAD_SETNAME_NP_W_TID)
493         pthread_setname_np(pthread_self(), qemu_thread_args->name);
494 # elif defined(CONFIG_PTHREAD_SETNAME_NP_WO_TID)
495         pthread_setname_np(qemu_thread_args->name);
496 # endif
497     }
498 #endif
499     g_free(qemu_thread_args->name);
500     g_free(qemu_thread_args);       //-->卡在这里面了
501     pthread_cleanup_push(qemu_thread_atexit_notify, NULL);
502     r = start_routine(arg);
503     pthread_cleanup_pop(1);
504     return r;
505 }
```
其中`start_routine`:
```sh
(gdb) p qemu_thread_args->start_routine
$5 = (void *(*)(void *)) 0xaaac35311e80 <call_rcu_thread>
```

那也就是说，父进程在fork()时，还未执行`start_routine()`, 在fork()之后，
父进程执行未卡住，但是子进程卡住了。。。

我们再增加调试信息, 让其在`fork()`之前STOP

## 继续增加调试信息
```
[464030][464031]: enter new thread
[464030][464031]: start gfree beg
[464030][464031]: start gfree end
[464030][464031]: start_routine
[464033][464033]: stop before fork

## kill -18 464033
## 第一个stop

## 父进程继续执行
[464033][464034]: enter new thread
[464033][464034]: start gfree beg
[464033][464033]: stop end before fork
[464033][464033]: start fork
[464033][464034]: start gfree end
[464033][464034]: start_routine
[464033][464033]: fork end

## 464673  是子进程,  子进程创建时, 父进程还没有执行到 
## start_routine, 子进程创建后，会继续执行

[464673][464673]: fork end
[464673][464674]: enter new thread

## 但是gfree 时，会因为futex卡住
[464673][464674]: start gfree beg
[464673][464673]: child process begin exec, STOP...

[464673][464673]: child process STOP END
[464673][466434]: enter new thread
[464673][466434]: start gfree beg
```

此时父进程还未执行到子进程, 父进程却卡在了 `gfree`. 
但是如果此时发送`SIGCONT`  给父进程, 父进程会继续执行, 并且
可以执行到`start_routine`. 子进程也最终会执行到gfree，
但是其会因为futex卡住。

## 其他
### 查看正常机器 domcapabilities 会启动哪些 cmdline 的qemu 
使用execsnoop抓取
1. 先清缓存
   ```
   [root@11-244-8-133 18:21:26 ~]# /bin/rm -rf /var/cache/libvirt/* 
   ```
2. 执行`execsnoop`脚本等待输出
3. 重启libvirtd
4. 观察`execsnoop`程序
   ```
   qemu-kvm         1603167 1603129   0 /usr/libexec/qemu-kvm -S -no-user-config
     -nodefaults -nographic -machine none,accel=kvm:tcg 
     -qmp unix:/var/lib/libvirt/qemu/qmp-D96SH3/qmp.monitor,server,nowait 
     -pidfile /var/lib/libvirt/qemu/qmp-D96SH3/qmp.pid -daemonize
   qemu-kvm         1603177 1603129   0 /usr/libexec/qemu-kvm -S -no-user-config 
     -nodefaults -nographic -machine none,accel=tcg 
     -qmp unix:/var/lib/libvirt/qemu/qmp-J7PVH3/qmp.monitor,server,nowait
     -pidfile /var/lib/libvirt/qemu/qmp-J7PVH3/qmp.pid -daemonize
   ```

   可以发现在这个过程中不会启动, `machine virt`(不带gic-version=3)的qemu

### 使用tcmalloc编译上游qemu

会在编译阶段卡住:
```
root     2714114  0.0  0.0 811776  9280 pts/1    Sl   16:18   0:00 /root/wangfuqiang/qemu-wang-upstream/build/qemu-keymap -f pc-bios/keymaps/mk -l mk
root     2714115  0.0  0.0 811776  9344 pts/1    Sl   16:18   0:00 /root/wangfuqiang/qemu-wang-upstream/build/qemu-keymap -f pc-bios/keymaps/nl -l nl
root     2714120  0.0  0.0 811776  9344 pts/1    Sl   16:18   0:00 /root/wangfuqiang/qemu-wang-upstream/build/qemu-keymap -f pc-bios/keymaps/no -l no
root     2714121  0.0  0.0 811776  9344 pts/1    Sl   16:18   0:00 /root/wangfuqiang/qemu-wang-upstream/build/qemu-keymap -f pc-bios/keymaps/pl -l pl
root     2714122  0.0  0.0 811776  9344 pts/1    Sl   16:18   0:00 /root/wangfuqiang/qemu-wang-upstream/build/qemu-keymap -f pc-bios/keymaps/pt -l pt
root     2714123  0.0  0.0 811776  9344 pts/1    Sl   16:18   0:00 /root/wangfuqiang/qemu-wang-upstream/build/qemu-keymap -f pc-bios/keymaps/pt-br -l br
root     2714124  0.0  0.0 811776  9344 pts/1    Sl   16:18   0:00 /root/wangfuqiang/qemu-wang-upstream/build/qemu-keymap -f pc-bios/keymaps/ru -l ru
root     2714125  0.0  0.0 811776  9280 pts/1    Sl   16:18   0:00 /root/wangfuqiang/qemu-wang-upstream/build/qemu-keymap -f pc-bios/keymaps/th -l th
```
(堆栈信息和下面相同)

并且执行qemu命令也会卡在`_start`阶段
```
#0  0x0000fffc4e207e10 in syscall () at /usr/lib64/libc.so.6
#1  0x0000fffc4e335f94 in base::internal::SpinLockDelay(int volatile*, int, int) (w=0xfffc4e48bf40 <tcmalloc::Static::pageheap_lock_>, value=2, loop=<optimized out>) at ./src/base/spinlock_linux-inl.h:84
#2  0x0000fffc4e335e30 in SpinLock::SlowLock() (this=0xfffc4e48bf40 <tcmalloc::Static::pageheap_lock_>) at src/base/spinlock.cc:118
#3  0x0000fffc4e3247ac in SpinLock::Lock() (this=<optimized out>) at src/base/spinlock.h:71
#4  SpinLockHolder::SpinLockHolder(SpinLock*) (l=<optimized out>, this=<synthetic pointer>) at src/base/spinlock.h:133
#5  tcmalloc::CentralFreeList::Populate() (this=this@entry=0xfffc4e478cf8 <tcmalloc::Static::central_cache_+81472>) at src/central_freelist.cc:325
#6  0x0000fffc4e3248b0 in tcmalloc::CentralFreeList::FetchFromOneSpansSafe(int, void**, void**) (this=0xfffc4e478cf8 <tcmalloc::Static::central_cache_+81472>, N=1, start=0xffffcc1f38e8, end=0xffffcc1f38f0) at src/central_freelist.cc:282
#7  0x0000fffc4e32496c in tcmalloc::CentralFreeList::RemoveRange(void**, void**, int) (this=0xfffc4e478cf8 <tcmalloc::Static::central_cache_+81472>, start=0xffffcc1f38e8, start@entry=0xffffcc1f3928, end=0xffffcc1f38f0, end@entry=0xffffcc1f3930, N=1) at src/central_freelist.cc:262
#8  0x0000fffc4e3282b8 in tcmalloc::ThreadCache::FetchFromCentralCache(unsigned int, int, void* (*)(unsigned long)) (this=this@entry=0xaaacc7871040, cl=<optimized out>, byte_size=32768, oom_handler=oom_handler@entry=0xfffc4e317510 <(anonymous namespace)::nop_oom_handler(size_t)>)
    at src/thread_cache.cc:126
#9  0x0000fffc4e3397a4 in tcmalloc::ThreadCache::Allocate(unsigned long, unsigned int, void* (*)(unsigned long)) (oom_handler=0xfffc4e317510 <(anonymous namespace)::nop_oom_handler(size_t)>, cl=<optimized out>, size=<optimized out>, this=<optimized out>) at src/thread_cache.h:380
#10 (anonymous namespace)::do_malloc (size=32768) at src/tcmalloc.cc:1386
#11 (anonymous namespace)::do_malloc_or_cpp_alloc (size=32768) at src/tcmalloc.cc:1394
#12 (anonymous namespace)::do_realloc_with_callback (invalid_get_size_fn=<optimized out>, invalid_free_fn=0xfffc4e317518 <(anonymous namespace)::InvalidFree(void*)>, new_size=32768, old_ptr=0xaaacc8118000) at src/tcmalloc.cc:1568
#13 (anonymous namespace)::do_realloc (new_size=32768, old_ptr=0xaaacc8118000) at src/tcmalloc.cc:1591
#14 tc_realloc(void*, size_t) (old_ptr=0xaaacc8118000, new_size=32768) at src/tcmalloc.cc:1990
#15 0x0000fffc4d3e8ce8 in _ULaarch64_dwarf_find_debug_frame () at /usr/lib64/libunwind.so.8
#16 0x0000fffc4d3e8f10 in  () at /usr/lib64/libunwind.so.8
#17 0x0000fffc4e248268 in dl_iterate_phdr () at /usr/lib64/libc.so.6
#18 0x0000fffc4d3e94a0 in  () at /usr/lib64/libunwind.so.8
#19 0x0000fffc4d3e62b0 in  () at /usr/lib64/libunwind.so.8
#20 0x0000fffc4d3e79ac in  () at /usr/lib64/libunwind.so.8
#21 0x0000fffc4d3e45e8 in _ULaarch64_step () at /usr/lib64/libunwind.so.8
#22 0x0000fffc4e336a90 in GetStackTrace_libunwind(void**, int, int) (result=<optimized out>, max_depth=30, skip_count=<optimized out>) at src/stacktrace_libunwind-inl.h:138
#23 0x0000fffc4e336e40 in GetStackTrace(void**, int, int) (result=result@entry=0xaaacc7850220, max_depth=max_depth@entry=30, skip_count=skip_count@entry=3) at src/stacktrace.cc:295
#24 0x0000fffc4e326464 in tcmalloc::RecordGrowth (growth=1048576) at src/page_heap.cc:641
#25 tcmalloc::PageHeap::GrowHeap(unsigned long) (this=0xfffc4e3647e0 <tcmalloc::Static::pageheap_>, n=<optimized out>) at src/page_heap.cc:667
#26 0x0000fffc4e326708 in tcmalloc::PageHeap::New(unsigned long) (this=0xfffc4e3647e0 <tcmalloc::Static::pageheap_>, n=n@entry=1) at src/page_heap.cc:154
#27 0x0000fffc4e324688 in tcmalloc::CentralFreeList::Populate() (this=this@entry=0xfffc4e46a438 <tcmalloc::Static::central_cache_+21888>) at src/central_freelist.cc:326
#28 0x0000fffc4e3248b0 in tcmalloc::CentralFreeList::FetchFromOneSpansSafe(int, void**, void**) (this=0xfffc4e46a438 <tcmalloc::Static::central_cache_+21888>, N=1, start=0xffffcc1f6aa8, end=0xffffcc1f6ab0) at src/central_freelist.cc:282
#29 0x0000fffc4e32496c in tcmalloc::CentralFreeList::RemoveRange(void**, void**, int) (this=0xfffc4e46a438 <tcmalloc::Static::central_cache_+21888>, start=0xffffcc1f6aa8, start@entry=0xffffcc1f6ae8, end=0xffffcc1f6ab0, end@entry=0xffffcc1f6af0, N=1) at src/central_freelist.cc:262
#30 0x0000fffc4e3282b8 in tcmalloc::ThreadCache::FetchFromCentralCache(unsigned int, int, void* (*)(unsigned long)) (this=this@entry=0xaaacc7871040, cl=<optimized out>, byte_size=288, oom_handler=oom_handler@entry=0xfffc4e317510 <(anonymous namespace)::nop_oom_handler(size_t)>)
    at src/thread_cache.cc:126
#31 0x0000fffc4e3397a4 in tcmalloc::ThreadCache::Allocate(unsigned long, unsigned int, void* (*)(unsigned long)) (oom_handler=0xfffc4e317510 <(anonymous namespace)::nop_oom_handler(size_t)>, cl=<optimized out>, size=<optimized out>, this=<optimized out>) at src/thread_cache.h:380
#32 (anonymous namespace)::do_malloc (size=288) at src/tcmalloc.cc:1386
#33 (anonymous namespace)::do_malloc_or_cpp_alloc (size=288) at src/tcmalloc.cc:1394
#34 (anonymous namespace)::do_realloc_with_callback (invalid_get_size_fn=<optimized out>, invalid_free_fn=0xfffc4e317518 <(anonymous namespace)::InvalidFree(void*)>, new_size=288, old_ptr=0xaaacc8220000) at src/tcmalloc.cc:1568
#35 (anonymous namespace)::do_realloc (new_size=288, old_ptr=0xaaacc8220000) at src/tcmalloc.cc:1591
#36 tc_realloc(void*, size_t) (old_ptr=0xaaacc8220000, new_size=288) at src/tcmalloc.cc:1990
#37 0x0000fffc4e82a9bc in g_realloc () at /usr/lib64/libglib-2.0.so.0
#38 0x0000aaac8c2ef4e8 in qemuio_add_command (ci=0xaaac8d3888a0 <write_cmd>) at ../qemu-io-cmds.c:48
#39 0x0000aaac8c2efd7c in init_qemuio_commands () at ../qemu-io-cmds.c:2787
#40 0x0000aaac8c4f84e8 in __libc_csu_init ()
#41 0x0000fffc4e153f28 in __libc_start_main () at /usr/lib64/libc.so.6
#42 0x0000aaac8b8304a4 in _start ()
```

## 相关patch
1. `rcu_disable_atfork()`

  ```
   commit 73c6e4013b4cd92d3d531bc22cc29e6036ef42e0
   Author: Paolo Bonzini <pbonzini@redhat.com>
   Date:   Wed Jan 27 08:49:21 2016 +0100
   
       rcu: completely disable pthread_atfork callbacks as soon as possible
   ```

## 参考链接
1. [性能打磨手记：记一段 Futex 机制的内核优化之旅](https://kernel.meizu.com/2024/03/15/Futex%E6%9C%BA%E5%88%B6%E7%9A%84%E5%86%85%E6%A0%B8%E4%BC%98%E5%8C%96/)

## tcmalloc

1. https://github.com/gperftools/gperftools/issues/499
2. https://github.com/gperftools/gperftools/commit/eeb7b84c20146c0e2e039ce72a2ea083a94ba80d

