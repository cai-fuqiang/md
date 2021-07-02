# 正常异常节点message信息对比
晓阳已经复现了该问题，取得了现场的message 信息，下面
结合正常环境的 message 信息和异常现场的message信息分析。

## 正常节点
执行`hostnamectl set-hostname ${HOSTNAME}`模拟现场环境命令执行。
查看message信息

```
dbus接收到请求
Jul  1 16:35:12 node-4 dbus[2005]: [system] Activating via systemd: service name='org.freedesktop.hostname1' unit='dbus-org.freedesktop.hostname1.service'
systemd 启动服务
Jul  1 16:35:12 node-4 systemd: Starting Hostname Service...
dbus 激活服务
Jul  1 16:35:13 node-4 dbus[2005]: [system] Successfully activated service 'org.freedesktop.hostname1'
systemd 启动服务成功
Jul  1 16:35:13 node-4 systemd: Started Hostname Service.
hostnamed修改hostname
Jul  1 16:35:13 node-4 systemd-hostnamed: Changed static host name to 'node-4.domain.tld.1'
Jul  1 16:35:13 node-4 systemd-hostnamed: Changed host name to 'node-4.domain.tld.1'
```

## 复现问题后message信息提取

```
08:26 有和xfs相关的stack打印，表示该请求已经超过120s未执行

Jun 30 18:08:26 node-4 kernel: INFO: task kworker/10:1:150 blocked for more than 120 seconds.
Jun 30 18:08:26 node-4 kernel:      Tainted: G           OE    --------- -  - 4.18.0-147.5.1.el8_1.5es.14.aarch64 #1
Jun 30 18:08:26 node-4 kernel: "echo 0 > /proc/sys/kernel/hung_task_timeout_secs" disables this message.
Jun 30 18:08:26 node-4 kernel: kworker/10:1    D    0   150      2 0x00000028
Jun 30 18:08:26 node-4 kernel: Workqueue: xfs-sync/dm-1 xfs_log_worker [xfs]
Jun 30 18:08:26 node-4 kernel: Call trace:
Jun 30 18:08:26 node-4 kernel: __switch_to+0x6c/0x90
Jun 30 18:08:26 node-4 kernel: __schedule+0x270/0x8a0
Jun 30 18:08:26 node-4 kernel: schedule+0x30/0x78
Jun 30 18:08:26 node-4 kernel: schedule_timeout+0x2d4/0x420
Jun 30 18:08:26 node-4 kernel: wait_for_common+0x1a8/0x210
Jun 30 18:08:26 node-4 kernel: wait_for_completion+0x28/0x38
Jun 30 18:08:26 node-4 kernel: flush_work+0x118/0x238
Jun 30 18:08:26 node-4 kernel: xlog_cil_force_lsn+0x78/0x228 [xfs]
Jun 30 18:08:26 node-4 kernel: xfs_log_force+0xb8/0x340 [xfs]
Jun 30 18:08:26 node-4 kernel: xfs_log_worker+0x40/0x150 [xfs]
Jun 30 18:08:26 node-4 kernel: process_one_work+0x1ac/0x3e8
Jun 30 18:08:26 node-4 kernel: worker_thread+0x44/0x448
Jun 30 18:08:26 node-4 kernel: kthread+0x130/0x138
Jun 30 18:08:26 node-4 kernel: ret_from_fork+0x10/0x18

....
10:12 dbus接受到 请求
Jun 30 18:10:12 node-4 dbus[1231]: [system] Activating via systemd: service name='org.freedesktop.hostname1' unit='dbus-org.freedesktop.hostname1.service'
启动hostnamed 
Jun 30 18:10:12 node-4 systemd: Starting Hostname Service...
dbus 请求超时, 超时时间大概是25s
Jun 30 18:10:37 node-4 dbus[1231]: [system] Failed to activate service 'org.freedesktop.hostname1': timed out

...

10:51 hostnamed 启动起来了, 启动的时间为39s
Jun 30 18:10:51 node-4 systemd: Started Hostname Service.

... 
12:32 还会有一些io阻塞
Jun 30 18:12:32 node-4 kernel: INFO: task kworker/10:1:150 blocked for more than 120 seconds.
Jun 30 18:12:32 node-4 kernel:      Tainted: G           OE    --------- -  - 4.18.0-147.5.1.el8_1.5es.14.aarch64 #1
Jun 30 18:12:32 node-4 kernel: "echo 0 > /proc/sys/kernel/hung_task_timeout_secs" disables this message.
Jun 30 18:12:32 node-4 kernel: kworker/10:1    D    0   150      2 0x00000028
Jun 30 18:12:32 node-4 kernel: Workqueue: xfs-sync/dm-1 xfs_log_worker [xfs]
Jun 30 18:12:32 node-4 kernel: Call trace:
Jun 30 18:12:32 node-4 kernel: __switch_to+0x6c/0x90
Jun 30 18:12:32 node-4 kernel: __schedule+0x270/0x8a0
Jun 30 18:12:32 node-4 kernel: schedule+0x30/0x78
Jun 30 18:12:32 node-4 kernel: schedule_timeout+0x2d4/0x420
Jun 30 18:12:32 node-4 kernel: wait_for_common+0x1a8/0x210
Jun 30 18:12:32 node-4 kernel: wait_for_completion+0x28/0x38
Jun 30 18:12:32 node-4 kernel: flush_work+0x118/0x238
Jun 30 18:12:32 node-4 kernel: xlog_cil_force_lsn+0x78/0x228 [xfs]
Jun 30 18:12:32 node-4 kernel: xfs_log_force+0xb8/0x340 [xfs]
Jun 30 18:12:32 node-4 kernel: xfs_log_worker+0x40/0x150 [xfs]
Jun 30 18:12:32 node-4 kernel: process_one_work+0x1ac/0x3e8
Jun 30 18:12:32 node-4 kernel: worker_thread+0x44/0x448
Jun 30 18:12:32 node-4 kernel: kthread+0x130/0x138
Jun 30 18:12:32 node-4 kernel: ret_from_fork+0x10/0x18


13:49 重启网络，重启网络后，不再有xfs相关io stack打印
Jun 30 18:13:49 node-4 systemd: Starting LSB: Bring up/down networking...
Jun 30 18:13:49 node-4 network: Bringing up loopback interface:  [  OK  ]
Jun 30 18:13:49 node-4 network: Bringing up interface br-mgmt:  RTNETLINK answers: File exists
Jun 30 18:13:49 node-4 network: [  OK  ]
Jun 30 18:13:49 node-4 network: Bringing up interface br-roller:  [  OK  ]
Jun 30 18:13:50 node-4 network: Bringing up interface br-storagepub:  [  OK  ]
Jun 30 18:13:50 node-4 network: Bringing up interface enp1s0:  [  OK  ]
Jun 30 18:13:50 node-4 network: Bringing up interface enp2s0:  [  OK  ]
Jun 30 18:13:50 node-4 network: Bringing up interface enp3s0:  [  OK  ]
```

经上面两个信息打印对比，发现以下几点:
* dbus在激活服务时，超时时间为25s
* dbus 在收到hostname相关请求后，会激活hostname相关服务，systemd会启动systemd-hostnamed服务, 启动服务成功后，dbus认为激活服务成功。
* systemd-hostnamed服务会去修改kernel hostname 和static hostname
* 异常节点中，启动systemd-hostnamed服务过慢，达到dbus超时时间，这是hostnamectl 返回错误的主要原因.
* 异常节点中，blk层会出现阻塞的情况，不知道是IO负载高，还是网络问题，需要进一步定位。

# 模拟现场环境
## 模拟思路
因为目前从现场来看，是dbus激活服务时，出现了超时，激活的服务实际上是`systemd-hostnamed`服务，
模拟的话需要让systemd-hostnamed服务拉起超时25s异常。这里使用`systemtap`延时`systemd-hostnamed`
服务的`main`函数。

* `echo 0 > /proc/sys/kernel/softlockup_panic`

在执行stp脚本前需要执行, 因为执行该脚本
会造成短时间的kernel softlock_up。

* `hostname.stp`脚本

```
probe process("/usr/lib/systemd/systemd-hostnamed").function("main")
{
        mdelay(30000); 		//延时30s
        printf("the function method_set_static_hostname is called");
        printf("1.execname(%s) pid(%d) pp(%s) \n", execname(), pid(), pp());
}
```
* 执行systemtap脚本
* `hostnamectl set-hostname ${HOSTNAME}`

***

获取打印信息

* message信息

```
[root@node-4 stap]# tail -f /var/log/messages |grep -E 'hostname|Hostname'
Jul  1 17:30:05 node-4 dbus[3451]: [system] Activating via systemd: service name='org.freedesktop.hostname1' unit='dbus-org.freedesktop.hostname1.service'
Jul  1 17:30:05 node-4 systemd: Starting Hostname Service...



Jul  1 17:30:30 node-4 dbus[3451]: [system] Failed to activate service 'org.freedesktop.hostname1': timed out

Message from syslogd@node-4 at Jul  1 17:30:30 ...
 kernel:watchdog: BUG: soft lockup - CPU#32 stuck for 22s! [systemd-hostnam:55848]
Jul  1 17:30:36 node-4 systemd: Started Hostname Service.
```

* `hostnamectl`执行错误打印

```
[root@node-4 ~]# hostnamectl set-hostname node-4.domain.tld.111
Could not set property: Failed to activate service 'org.freedesktop.hostname1': timed out
```

得到类似于现场的打印信息


# 其他调试

```
#0  pending_activation_timed_out (data=0xaaaac579f5a0) at ../../bus/activation.c:1469
#1  0x0000ffffb09bcdec in dbus_timeout_handle (timeout=<optimized out>) at ../../dbus/dbus-timeout.c:474
#2  0x0000aaaaac871f40 in _dbus_loop_iterate (loop=loop@entry=0xaaaac57705f0, block=block@entry=1) at ../../dbus/dbus-mainloop.c:741
#3  0x0000aaaaac872194 in _dbus_loop_run (loop=0xaaaac57705f0) at ../../dbus/dbus-mainloop.c:883
#4  0x0000aaaaac859e38 in main (argc=<optimized out>, argv=<optimized out>) at ../../bus/main.c:684
```

```
#0  bus_activation_activate_service (activation=activation@entry=0xaaaac5783250, connection=connection@entry=0xaaaac57ae770,
    transaction=transaction@entry=0xaaaac5797160, auto_activation=auto_activation@entry=0, activation_message=activation_message@entry=0xaaaac57aeb80,
        service_name=0xaaaac579b044 "org.freedesktop.PolicyKit1", error=error@entry=0xffffe3ff7228) at ../../bus/activation.c:1722
#1  0x0000aaaaac8682e4 in bus_driver_handle_activate_service (connection=0xaaaac57ae770, transaction=0xaaaac5797160, message=0xaaaac57aeb80, error=0xffffe3ff7228)
    at ../../bus/driver.c:1003
#2  0x0000aaaaac869d2c in bus_driver_handle_message (connection=connection@entry=0xaaaac57ae770, transaction=transaction@entry=0xaaaac5797160,
    message=message@entry=0xaaaac57aeb80, error=error@entry=0xffffe3ff7228) at ../../bus/driver.c:2757
#3  0x0000aaaaac8668f0 in bus_dispatch (message=0xaaaac57aeb80, connection=0xaaaac57ae770) at ../../bus/dispatch.c:390
#4  bus_dispatch_message_filter (connection=0xaaaac57ae770, message=0xaaaac57aeb80, user_data=<optimized out>) at ../../bus/dispatch.c:545
#5  0x0000ffffb09a7304 in dbus_connection_dispatch (connection=connection@entry=0xaaaac57ae770) at ../../dbus/dbus-connection.c:4705
#6  0x0000aaaaac871be0 in _dbus_loop_dispatch (loop=loop@entry=0xaaaac57705f0) at ../../dbus/dbus-mainloop.c:531
#7  0x0000aaaaac871e20 in _dbus_loop_iterate (loop=loop@entry=0xaaaac57705f0, block=block@entry=1) at ../../dbus/dbus-mainloop.c:857
#8  0x0000aaaaac872194 in _dbus_loop_run (loop=0xaaaac57705f0) at ../../dbus/dbus-mainloop.c:883
#9  0x0000aaaaac859e38 in main (argc=<optimized out>, argv=<optimized out>) at ../../bus/main.c:684


#0  bus_activation_activate_service (activation=0xaaaac5783250, connection=connection@entry=0xaaaac579c760, transaction=transaction@entry=0xaaaac57b65c0,
    auto_activation=auto_activation@entry=1, activation_message=activation_message@entry=0xaaaac57b67a0,
        service_name=service_name@entry=0xaaaac57b69d8 "org.freedesktop.hostname1", error=error@entry=0xffffe3ff7228) at ../../bus/activation.c:1722
#1  0x0000aaaaac866a00 in bus_dispatch (message=0xaaaac57b67a0, connection=0xaaaac579c760) at ../../bus/dispatch.c:435
#2  bus_dispatch_message_filter (connection=0xaaaac579c760, message=0xaaaac57b67a0, user_data=<optimized out>) at ../../bus/dispatch.c:545
#3  0x0000ffffb09a7304 in dbus_connection_dispatch (connection=connection@entry=0xaaaac579c760) at ../../dbus/dbus-connection.c:4705
#4  0x0000aaaaac871be0 in _dbus_loop_dispatch (loop=loop@entry=0xaaaac57705f0) at ../../dbus/dbus-mainloop.c:531
#5  0x0000aaaaac871e20 in _dbus_loop_iterate (loop=loop@entry=0xaaaac57705f0, block=block@entry=1) at ../../dbus/dbus-mainloop.c:857
#6  0x0000aaaaac872194 in _dbus_loop_run (loop=0xaaaac57705f0) at ../../dbus/dbus-mainloop.c:883
#7  0x0000aaaaac859e38 in main (argc=<optimized out>, argv=<optimized out>) at ../../bus/main.c:684

```

```
     0 dbus-daemon(3451):  -> bus_activation_activate_service
    11 dbus-daemon(3451):    -> bus_context_get_max_pending_activations
    17 dbus-daemon(3451):    <- bus_context_get_max_pending_activations
    21 dbus-daemon(3451):    -> bus_context_get_systemd_activation
    26 dbus-daemon(3451):    <- bus_context_get_systemd_activation
    30 dbus-daemon(3451):    -> activation_find_entry
    37 dbus-daemon(3451):      -> check_service_file
    44 dbus-daemon(3451):        -> _dbus_stat
    50 dbus-daemon(3451):          -> stat
    74 dbus-daemon(3451):          <- _dbus_stat
    78 dbus-daemon(3451):        <- check_service_file
    82 dbus-daemon(3451):        -> bus_context_get_registry
    86 dbus-daemon(3451):        <- bus_context_get_registry
    90 dbus-daemon(3451):        -> bus_registry_lookup
    95 dbus-daemon(3451):        <- bus_registry_lookup
   104 dbus-daemon(3451):        -> bus_transaction_send_from_driver
   112 dbus-daemon(3451):          -> bus_connection_is_active
   116 dbus-daemon(3451):            -> bus_connection_get_name
   128 dbus-daemon(3451):              -> bus_transaction_capture
   133 dbus-daemon(3451):                -> bus_context_get_connections
   138 dbus-daemon(3451):                <- bus_context_get_connections
   142 dbus-daemon(3451):              <- bus_transaction_capture
   145 dbus-daemon(3451):              -> bus_context_check_security_policy
   152 dbus-daemon(3451):                -> bus_connection_is_active
   157 dbus-daemon(3451):                <- bus_connection_is_active
   160 dbus-daemon(3451):                -> bus_connection_get_policy
   166 dbus-daemon(3451):                <- bus_connection_get_policy
   169 dbus-daemon(3451):                -> bus_client_policy_check_can_receive
   177 dbus-daemon(3451):                <- bus_client_policy_check_can_receive
   181 dbus-daemon(3451):              <- bus_context_check_security_policy
   184 dbus-daemon(3451):              -> bus_transaction_send
   190 dbus-daemon(3451):              <- bus_transaction_send
   193 dbus-daemon(3451):            <- bus_transaction_send_from_driver
   197 dbus-daemon(3451):          <- bus_activation_activate_service
 13218 dbus-daemon(3451):            -> bus_activation_activate_service
 13224 dbus-daemon(3451):              -> bus_context_get_max_pending_activations
 13230 dbus-daemon(3451):              <- bus_context_get_max_pending_activations
 13234 dbus-daemon(3451):              -> bus_context_get_systemd_activation
 13239 dbus-daemon(3451):              <- bus_context_get_systemd_activation
 13243 dbus-daemon(3451):              -> activation_find_entry
 13248 dbus-daemon(3451):                -> check_service_file
 13255 dbus-daemon(3451):                  -> _dbus_stat
 13260 dbus-daemon(3451):                    -> stat
 13279 dbus-daemon(3451):                    <- _dbus_stat
 13283 dbus-daemon(3451):                  <- check_service_file
 13287 dbus-daemon(3451):                  -> bus_context_get_activation_timeout
 13292 dbus-daemon(3451):                  <- bus_context_get_activation_timeout
 13296 dbus-daemon(3451):                  -> bus_context_get_loop
 13301 dbus-daemon(3451):                  <- bus_context_get_loop
 13304 dbus-daemon(3451):                  -> _dbus_loop_add_timeout
 13309 dbus-daemon(3451):                    -> timeout_callback_new
 13315 dbus-daemon(3451):                    <- _dbus_loop_add_timeout
 13320 dbus-daemon(3451):                    -> bus_pending_activation_unref
 13325 dbus-daemon(3451):                    <- bus_pending_activation_unref
 13329 dbus-daemon(3451):                    -> add_cancel_pending_to_transaction
 13332 dbus-daemon(3451):                      -> bus_transaction_add_cancel_hook
 13337 dbus-daemon(3451):                      <- bus_transaction_add_cancel_hook
 13341 dbus-daemon(3451):                      -> bus_pending_activation_ref
 13346 dbus-daemon(3451):                        -> bus_context_get_systemd_activation
 13352 dbus-daemon(3451):                        <- bus_context_get_systemd_activation
 13366 dbus-daemon(3451):                        -> bus_transaction_new
 13371 dbus-daemon(3451):                        <- bus_transaction_new
 13375 dbus-daemon(3451):                        -> bus_connection_get_registry
 13380 dbus-daemon(3451):                          -> bus_context_get_registry
 13385 dbus-daemon(3451):                          <- bus_context_get_registry
 13388 dbus-daemon(3451):                        <- bus_connection_get_registry
 13392 dbus-daemon(3451):                        -> bus_registry_lookup
 13397 dbus-daemon(3451):                        <- bus_registry_lookup
 13401 dbus-daemon(3451):                        -> bus_service_get_primary_owners_connection
 13406 dbus-daemon(3451):                          -> bus_service_get_primary_owner
 13411 dbus-daemon(3451):                          <- bus_service_get_primary_owners_connection
 13415 dbus-daemon(3451):                          -> bus_transaction_capture
 13420 dbus-daemon(3451):                            -> bus_context_get_connections
 13425 dbus-daemon(3451):                            <- bus_context_get_connections
 13428 dbus-daemon(3451):                          <- bus_transaction_capture
 13433 dbus-daemon(3451):                          -> bus_context_log
 13441 dbus-daemon(3451):                            -> _dbus_system_log
 13447 dbus-daemon(3451):                              -> _dbus_system_logv
 13453 dbus-daemon(3451):                                -> vsyslog
 13486 dbus-daemon(3451):                                <- _dbus_system_logv
 13490 dbus-daemon(3451):                              <- _dbus_system_log
 13493 dbus-daemon(3451):                            <- bus_context_log
 13497 dbus-daemon(3451):                            -> bus_dispatch_matches
 13502 dbus-daemon(3451):                              -> bus_transaction_get_context
 13506 dbus-daemon(3451):                              <- bus_transaction_get_context
 13510 dbus-daemon(3451):                              -> bus_context_check_security_policy
 13517 dbus-daemon(3451):                                -> bus_connection_is_active
 13522 dbus-daemon(3451):                                <- bus_connection_is_active
 13526 dbus-daemon(3451):                                -> bus_connection_get_policy
 13531 dbus-daemon(3451):                                <- bus_connection_get_policy
 13534 dbus-daemon(3451):                                -> bus_client_policy_check_can_receive
 13542 dbus-daemon(3451):                                <- bus_client_policy_check_can_receive
 13546 dbus-daemon(3451):                              <- bus_context_check_security_policy
 13550 dbus-daemon(3451):                              -> bus_transaction_send
 13555 dbus-daemon(3451):                              <- bus_transaction_send
 13559 dbus-daemon(3451):                              -> bus_context_get_connections
 13563 dbus-daemon(3451):                              <- bus_context_get_connections
 13567 dbus-daemon(3451):                              -> bus_context_get_matchmaker
 13571 dbus-daemon(3451):                              <- bus_context_get_matchmaker
 13574 dbus-daemon(3451):                              -> bus_matchmaker_get_recipients
 13579 dbus-daemon(3451):                                -> bus_connections_increment_stamp
 13585 dbus-daemon(3451):                                <- bus_connections_increment_stamp
 13589 dbus-daemon(3451):                                -> bus_connection_mark_stamp
 13593 dbus-daemon(3451):                                <- bus_connection_mark_stamp
 13597 dbus-daemon(3451):                                -> bus_matchmaker_get_rules
 13602 dbus-daemon(3451):                                <- bus_matchmaker_get_rules
 13606 dbus-daemon(3451):                                -> bus_matchmaker_get_rules
 13610 dbus-daemon(3451):                                  -> bus_matchmaker_get_rules
 13615 dbus-daemon(3451):                                  <- bus_matchmaker_get_rules
 13618 dbus-daemon(3451):                                  -> get_recipients_from_list
 13624 dbus-daemon(3451):                                  <- get_recipients_from_list
 13628 dbus-daemon(3451):                                  -> get_recipients_from_list
 13633 dbus-daemon(3451):                                  <- get_recipients_from_list
 13636 dbus-daemon(3451):                                  -> get_recipients_from_list
 13641 dbus-daemon(3451):                                  <- get_recipients_from_list
 13645 dbus-daemon(3451):                                  -> get_recipients_from_list
 13651 dbus-daemon(3451):                                    -> match_rule_matches
 13656 dbus-daemon(3451):                                    <- get_recipients_from_list
 13659 dbus-daemon(3451):                                  <- bus_matchmaker_get_recipients
 13663 dbus-daemon(3451):                                <- bus_dispatch_matches
 13667 dbus-daemon(3451):                                -> bus_transaction_execute_and_free
 13671 dbus-daemon(3451):                                  -> connection_execute_transaction
 13689 dbus-daemon(3451):                                    -> message_to_send_free
 13695 dbus-daemon(3451):                                    <- message_to_send_free
 13698 dbus-daemon(3451):                                    -> transaction_free
 13702 dbus-daemon(3451):                                      -> free_cancel_hooks
 13707 dbus-daemon(3451):                                      <- bus_transaction_execute_and_free
 13711 dbus-daemon(3451):                                    <- bus_activation_activate_service
```


```
#0  0x0000ffffb073f804 in epoll_pwait () from /lib64/libc.so.6
#1  0x0000aaaaac875ec0 in socket_set_epoll_poll (set=<optimized out>, revents=0xffffe3ff73f8, max_events=<optimized out>, timeout_ms=<optimized out>)
    at ../../dbus/dbus-socket-set-epoll.c:300
#2  0x0000aaaaac871d74 in _dbus_socket_set_poll (timeout_ms=<optimized out>, max_events=64, revents=0xffffe3ff73f8, self=<optimized out>)
    at ../../dbus/dbus-socket-set.h:111
#3  _dbus_loop_iterate (loop=loop@entry=0xaaaac57705f0, block=block@entry=1) at ../../dbus/dbus-mainloop.c:658
#4  0x0000aaaaac872194 in _dbus_loop_run (loop=0xaaaac57705f0) at ../../dbus/dbus-mainloop.c:883
#5  0x0000aaaaac859e38 in main (argc=<optimized out>, argv=<optimized out>) at ../../bus/main.c:684

```


```
1.execname(systemd) pid(1) pp(process("/usr/lib/systemd/systemd").function("unit_get_status_message_format@src/core/unit.c:1352"))
the type is 0
 0xaaaab66abc74 : unit_get_status_message_format+0x0/0xc4 [/usr/lib/systemd/systemd]
 0xaaaab66af9a0 : unit_status_emit_starting_stopping_reloading+0x30/0x78 [/usr/lib/systemd/systemd]
 0xaaaab66b5f44 : job_perform_on_unit+0xa8/0xf0 [/usr/lib/systemd/systemd]
 0xaaaab66b7bdc : job_run_and_invalidate+0x284/0x3f8 [/usr/lib/systemd/systemd]
 0xaaaab6607ce4 : manager_dispatch_run_queue+0x40/0x198 [/usr/lib/systemd/systemd]
 0xaaaab66a8c54 : source_dispatch+0x13c/0x344 [/usr/lib/systemd/systemd]
 0xaaaab66a9bcc : sd_event_dispatch+0x6c/0x1c8 [/usr/lib/systemd/systemd]
 0xaaaab660fa9c : manager_loop+0x4e8/0x650 [/usr/lib/systemd/systemd]
 0xaaaab6603d5c : main+0x19ac/0x3e18 [/usr/lib/systemd/systemd]
 0xffffa90c1714 [/usr/lib64/libc-2.17.so+0x21714/0x190000]
1.execname(systemd) pid(1) pp(process("/usr/lib/systemd/systemd").function("job_get_status_message_format@src/core/job.c:642"))
the type is 0
 0xaaaab66b55f4 : job_get_status_message_format+0x0/0x15c [/usr/lib/systemd/systemd]
 0xaaaab66b57f4 : job_log_status_message+0xa4/0x608 [/usr/lib/systemd/systemd]
 0xaaaab66b716c : job_finish_and_invalidate+0x334/0x6bc [/usr/lib/systemd/systemd]
 0xaaaab66b4164 : unit_notify+0x314/0xa78 [/usr/lib/systemd/systemd]
 0xaaaab661fc4c : service_set_state+0x3bc/0x46c [/usr/lib/systemd/systemd]
 0xaaaab6623500 : service_bus_name_owner_change+0xf4/0x398 [/usr/lib/systemd/systemd]
 0xaaaab660cde8 : manager_dispatch_bus_name_owner_changed+0x50/0x9c [/usr/lib/systemd/systemd]
 0xaaaab6636cac : signal_name_owner_changed+0x80/0x118 [/usr/lib/systemd/systemd]
 0xaaaab66d4724 : bus_match_run+0x3d0/0x64c [/usr/lib/systemd/systemd]
 0xaaaab66d4778 : bus_match_run+0x424/0x64c [/usr/lib/systemd/systemd]
 0xaaaab66d4778 : bus_match_run+0x424/0x64c [/usr/lib/systemd/systemd]
 0xaaaab66d4418 : bus_match_run+0xc4/0x64c [/usr/lib/systemd/systemd]

```
