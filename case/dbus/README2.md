`sd_bus_add_match`
https://www.freedesktop.org/software/systemd/man/sd_bus_add_match_async.html

```
static int bus_setup_api(Manager *m, sd_bus *bus)
        r = sd_bus_add_match(
                        bus,
                        NULL,
                        "type='signal',"
                        "sender='org.freedesktop.DBus',"
                        "path='/org/freedesktop/DBus',"
                        "interface='org.freedesktop.DBus',"
                        "member='NameOwnerChanged'",
                        signal_name_owner_changed, m);
```


# systemd 调用clone流程

```
 0xffff9e06f630 : clone+0x30/0x50 [/usr/lib64/libc-2.17.so]
 0xffff9e126d78 [/usr/lib64/libpthread-2.17.so+0x6d78/0x40000]
 0xffff9e06f630 : clone+0x30/0x50 [/usr/lib64/libc-2.17.so]
 0xffff9e126d78 [/usr/lib64/libpthread-2.17.so+0x6d78/0x40000]
 0xffff9e03ab3c : __fork+0xd0/0x2a4 [/usr/lib64/libc-2.17.so]
 0xaaaae87e5188 : exec_spawn+0x124/0x5e8 [/usr/lib/systemd/systemd]
 0xaaaae87ceb94 : service_spawn+0x3ec/0x7a4 [/usr/lib/systemd/systemd]
 0xaaaae87d3248 : service_enter_start+0xcc/0x290 [/usr/lib/systemd/systemd]
 0xaaaae87d576c : service_start+0x230/0x288 [/usr/lib/systemd/systemd]
 0xaaaae885feb8 : unit_start+0x1a8/0x2d4 [/usr/lib/systemd/systemd]
 0xaaaae8865f68 : job_perform_on_unit+0xcc/0xf0 [/usr/lib/systemd/systemd]
 0xaaaae8867bdc : job_run_and_invalidate+0x284/0x3f8 [/usr/lib/systemd/systemd]
 0xaaaae87b7ce4 : manager_dispatch_run_queue+0x40/0x198 [/usr/lib/systemd/systemd]
 0xaaaae8858c54 : source_dispatch+0x13c/0x344 [/usr/lib/systemd/systemd]
 0xaaaae8859bcc : sd_event_dispatch+0x6c/0x1c8 [/usr/lib/systemd/systemd]
 0xaaaae87bfa9c : manager_loop+0x4e8/0x650 [/usr/lib/systemd/systemd]
 0xaaaae87b3d5c : main+0x19ac/0x3e18 [/usr/lib/systemd/systemd]
 0xffff9dfb1714 : __libc_start_main+0xf0/0x1cc [/usr/lib64/libc-2.17.so]
 0xaaaae87b6200 : _start+0x38/0x3c [/usr/lib/systemd/systemd]
```


```
[SYSTEMD call].execname(systemd) (1) pp(process("/usr/lib/systemd/systemd").function("exec_spawn@src/core/execute.c:1929"))
the command path is "/usr/lib/systemd/systemd-hostnamed"
[KERNEL] clone a process


[SYSTEMD call EXEC END]!!!!!!!!!!!!!!!!
[SYSTEMD call].execname(systemd) pid(1) pp(process("/usr/lib/systemd/systemd").function("unit_get_status_message_format@src/core/unit.c:1352"))
the type is 0
[SYSTEMD call].execname(systemd) pid(1) pp(process("/usr/lib/systemd/systemd").statement("unit_status_log_starting_stopping_reloading@src/core/unit.c:1419"))
the buf is "Starting Hostname Service..."


[SYSTEMD call].execname(systemd) pid(1) pp(process("/usr/lib/systemd/systemd").function("unit_get_status_message_format@src/core/unit.c:1352"))
the type is 0
[KERNEL] exec a process
[MAIN].execname(systemd-hostnam) pid(43557) pp(process("/usr/lib/systemd/systemd-hostnamed").function("main@src/hostname/hostnamed.c:669"))
[DBUS call].execname(dbus-daemon) pid(3536) pp(process("/usr/bin/dbus-daemon").function("bus_driver_send_service_owner_changed@../../bus/driver.c:220"))
the service name is ":1.257"
[DBUS call].execname(dbus-daemon) pid(3536) pp(process("/usr/bin/dbus-daemon").function("bus_driver_send_service_owner_changed@../../bus/driver.c:220"))
the service name is "org.freedesktop.hostname1"
[SYSTEMD call].execname(systemd) pid(1) pp(process("/usr/lib/systemd/systemd").function("job_get_status_message_format@src/core/job.c:642"))
the JobType is 0
the JobResult is 0
[SYSTEMD call].execname(systemd) pid(1) pp(process("/usr/lib/systemd/systemd").statement("job_log_status_message@src/core/job.c:796"))
the buf is "Started Hostname Service."


[SYSTEMD call].execname(systemd) pid(1) pp(process("/usr/lib/systemd/systemd").function("job_get_status_message_format@src/core/job.c:642"))
the JobType is 0
the JobResult is 0
[DBUS call].execname(dbus-daemon) pid(3536) pp(process("/usr/bin/dbus-daemon").function("bus_driver_send_service_owner_changed@../../bus/driver.c:220"))
the service name is ":1.256"

```
