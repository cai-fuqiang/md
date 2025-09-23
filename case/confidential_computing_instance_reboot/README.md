## 机密计算机型重启失败

查看重启失败日志
```
//====(1)===
118548@[2025-09-22 17:37:22.845679] virtio_console_qga_chr_read qga cmd: {"execute":"guest-shutdown","arguments":{"mode":"powerdown"}}
...

//====(2)===
118548@[2025-09-22 17:37:32.859708] qmp_dispatch qmp_cmd_name: system_powerdown, arguments: {}
118548@[2025-09-22 17:37:32.859772] qemu_system_powerdown_request
//====(3)===
{"timestamp": {"seconds": 1758533852, "microseconds": 859801}, "event": "POWERDOWN"}
domain is power down by outside operation
2025-09-22T09:37:42.867769Z qemu-system-x86_64: terminating on signal 15 from pid 14261 (/usr/sbin/libvirtd)
{"timestamp": {"seconds": 1758533862, "microseconds": 867812}, "event": "SHUTDOWN", "data": {"guest": false, "reason": "host-signal"}}
118548@[2025-09-22 17:37:42.867864] main_loop_should_exit_quit guest system shutdown reason:4,(vmm shutdown) vmm will exit too
...
//====(4)===
2025-09-22 09:37:43.068+0000: shutting down, reason=destroyed
//====(5)===
2025-09-22 09:37:44.175+0000: starting up libvirt version: 8.5.0, package: 26.ef8fbbe.24.03LTS (Unknown, 2025-06-30-10:37:00, A06-R08-I137-169-66Z6GM2.JCLOUD.COM), qemu version: 6.2.0qemu-6.2.0.121-34a2e4e72.24.03LTS, kernel: 6.6.0-jdcloud7.sp1.131d3847b.x86_64, hostname: 11-244-8-3.jdstack.local

...
//====(6)===
2025-09-22T09:39:29.644657Z qemu-system-x86_64: cpus are not resettable, terminating
{"timestamp": {"seconds": 1758533969, "microseconds": 644745}, "event": "SHUTDOWN", "data": {"guest": true, "reason": "guest-reset"}}
                                                                                            ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
124213@[2025-09-22 17:39:29.644800] runstate_set current_run_state 9 (running) new_state 11 (shutdown)
124213@[2025-09-22 17:39:29.644882] vm_state_notify running 0 reason 11 (shutdown)
...

{"timestamp": {"seconds": 1758533969, "microseconds": 661036}, "event": "STOP"}
124213@[2025-09-22 17:39:29.661135] main_loop_should_exit_shutdown guest system shutdown reason:7,(vmm shutdown) but vmm keep alive
2025-09-22T09:39:29.661177Z qemu-system-x86_64: terminating on signal 15 from pid 14261 (/usr/sbin/libvirtd)
{"timestamp": {"seconds": 1758533969, "microseconds": 661182}, "event": "SHUTDOWN", "data": {"guest": false, "reason": "host-signal"}}
124213@[2025-09-22 17:39:29.661201] main_loop_should_exit_quit guest system shutdown reason:4,(vmm shutdown) vmm will exit too
124213@[2025-09-22 17:39:29.661208] migrate_fd_cancel
//====(7)====
2025-09-22 09:39:29.863+0000: shutting down, reason=shutdown
```

发生顺序大概如下:
1. `17:37:22`控制面发起重启动做，qemu这边首先收到的是gga `mode powerdown`(关机)
2. 过了10秒之后，发现qemu通过qga 还没有关机，于是给qemu又发送qmp, `system_powerdown` 执行关机
3. qemu确认收到 powerdown, 触发event
4. 这里表明，qemu是被强制powerdown的
5. qemu 又被libvirt拉起

6. 在此之前从未收到qmp/qga，可以看到该虚拟机是在guest中被重启的。但是该cpu不支持
   重启，所以关机.
7. 成功关机

由此可见`libvirt/控制面`逻辑:

1. 首先通过qga关机
2. 如果过10s还未关机，通过 powerdown 的方式关机
3. 待qemu关机后，再将qemu拉起

## guest 重启触发关机代码

如果在guest中执行reboot，qemu会打印:
```
2025-09-22T09:39:29.644657Z qemu-system-x86_64: cpus are not resettable, terminating
```
该打印代码:
```cpp
void qemu_system_reset_request(ShutdownCause reason)
{
    if (reboot_action == REBOOT_ACTION_SHUTDOWN &&
        reason != SHUTDOWN_CAUSE_SUBSYSTEM_RESET) {
        shutdown_requested = reason;
    } else if (!cpus_are_resettable()) {
        error_report("cpus are not resettable, terminating"); //<--- this
        shutdown_requested = reason;
    } else {
        reset_requested = reason;
    }
    cpu_stop_current();
    qemu_notify_event();
}
```
相关代码:
```cpp
bool cpus_are_resettable(void)
{
    if (cpus_accel->cpus_are_resettable) {
        return cpus_accel->cpus_are_resettable();
    }
    return true;
}
static void kvm_accel_ops_class_init(ObjectClass *oc, void *data)
{
    ...
    ops->cpus_are_resettable = kvm_cpus_are_resettable;
    ...
}
```
相关代码:
```sh
kvm_cpus_are_resettable(void)
    |-> !kvm_enabled() || kvm_cpu_check_are_resettable();
         |-> kvm_arch_cpu_check_are_resettable(); ## kvm_cpu_check_are_resettable()
             |-> !sev_es_enabled()
                 |-> return sev_enabled() && (sev_guest->policy & SEV_POLICY_ES); 
                     \-> !!sev_guest ## sev_enabled()
```
gdb 调试csv3和sev-es虚拟机:

gdb 调试csv3:
```
(gdb) p sev_guest
$1 = (SevGuestState *) 0x5591f9f36980
(gdb) p sev_guest->policy
$2 = 69
## 69 = 64+4+1
```

gdb 调试`sev-es`
```
(gdb) p sev_guest
$1 = (SevGuestState *) 0x562d91f08850
(gdb) p sev_guest->policy
$2 = 5
```

可以发现都有, 主要是因为无论是csv3还是sev-es其都会在下面流程中初始化:
```sh
kvm_arch_init
|-> sev_kvm_init
    |-> SevGuestState *sev = (SevGuestState *)object_dynamic_cast(OBJECT(cgs), TYPE_SEV_GUEST);
```
而这两者的配置均为:
```
  <launchSecurity type='sev'>
    <policy>0x0005</policy>
  </launchSecurity>
```
在qemu cmdline中的体现:
```
-object {"qom-type":"sev-guest","id":"lsec0","cbitpos":51,"reduced-phys-bits":1,"policy":5}
```

其在代码中的type
```cpp
#define TYPE_SEV_GUEST "sev-guest"

/* sev guest info */
static const TypeInfo sev_guest_info = {
    .parent = TYPE_CONFIDENTIAL_GUEST_SUPPORT,
    .name = TYPE_SEV_GUEST,
    .instance_size = sizeof(SevGuestState),
    .instance_finalize = sev_guest_finalize,
    .class_init = sev_guest_class_init,
    .instance_init = sev_guest_instance_init,
    .interfaces = (InterfaceInfo[]) {
        { TYPE_USER_CREATABLE },
        { }
    }
};
static void
sev_guest_instance_init(Object *obj)
{
    SevGuestState *sev = SEV_GUEST(obj);

    sev->sev_device = g_strdup(DEFAULT_SEV_DEVICE);
    sev->policy = DEFAULT_GUEST_POLICY;
    ## policy
    object_property_add_uint32_ptr(obj, "policy", &sev->policy,
                                   OBJ_PROP_FLAG_READWRITE);
    object_property_add_uint32_ptr(obj, "handle", &sev->handle,
                                   OBJ_PROP_FLAG_READWRITE);
    object_property_add_uint32_ptr(obj, "cbitpos", &sev->cbitpos,
                                   OBJ_PROP_FLAG_READWRITE);
    object_property_add_uint32_ptr(obj, "reduced-phys-bits",
                                   &sev->reduced_phys_bits,
                                   OBJ_PROP_FLAG_READWRITE);
}
```
