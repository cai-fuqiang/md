## 添加kernel cmdline 增加调试
```
rd.debug systemd.debug-shell console=tty0 console=ttyS0,115200n8
```
通过将`console=ttyS0`, 将systemd debug信息打印到串口

## 复现后日志提取
执行下面命令:
```
[root@11-244-8-3 10:43:46 wfq]# cat console.log |grep -E  -i 'finish|start'
...
         Starting dracut pre-mount hook...
[  OK  ] Finished dracut pre-mount hook.
         Starting Virtual Console Setup...
         Starting Virtual Console Setup...
         Starting File System Check on /dev…0777-6866-433e-9ef0-907081da38e2...
[  OK  ] Finished Virtual Console Setup.
[  OK  ] Finished File System Check on /dev…b30777-6866-433e-9ef0-907081da38e2.
         Starting dracut pre-pivot and cleanup hook...
[  OK  ] Finished dracut pre-pivot and cleanup hook.
```

可以发现最后一个启动的服务已经完成了, 并没有阻塞的任务.

## 查看正常启动的虚拟机
```
[  OK  ] Finished dracut initqueue hook.
         Starting dracut pre-mount hook...
[  OK  ] Finished dracut pre-mount hook.
         Starting Virtual Console Setup...
         Starting Virtual Console Setup...
         Starting File System Check on /dev…0777-6866-433e-9ef0-907081da38e2...
[  OK  ] Finished Virtual Console Setup.
[  OK  ] Finished File System Check on /dev…b30777-6866-433e-9ef0-907081da38e2.
         Starting Mountpoints Configured in the Real Root...
[  OK  ] Finished Mountpoints Configured in the Real Root.
         Starting dracut pre-pivot and cleanup hook...
[  OK  ] Finished dracut pre-pivot and cleanup hook.
         Starting Cleaning Up and Shutting Down Daemons...
         Starting Cleanup udev Database...
[  OK  ] Finished Cleaning Up and Shutting Down Daemons.
[  OK  ] Finished Cleanup udev Database.
         Starting Switch Root...
```

可以看到在`pre-pivot`其后，又执行了`Cleaning Up and Shutting Down Daemons`服务.

## 查看cleaning up...相关服务
```
[root@11-244-8-100 10:58:25 system]# cat initrd-cleanup.service
[Unit]
Description=Cleaning Up and Shutting Down Daemons
DefaultDependencies=no
AssertPathExists=/etc/initrd-release
OnFailure=emergency.target
OnFailureJobMode=replace-irreversibly
After=initrd-root-fs.target initrd-fs.target initrd.target

[Service]
Type=oneshot
ExecStart=systemctl --no-block isolate initrd-switch-root.target
```

systemctl show 查看其依赖:
```
[root@11-244-8-100 11:03:18 system]# systemctl show initrd-cleanup |grep -i -E 'want|after|before'
RemainAfterExit=no
WantedBy=initrd-switch-root.target
Before=initrd-switch-root.target
After=dracut-pre-pivot.service initrd-fs.target system.slice initrd-root-fs.target systemd-journald.socket initrd.target
```

我们在console log中查看after 这些服务状态:
* dracut-pre-pivot.service
  ```
  [root@11-244-8-3 11:04:24 wfq]# cat console.log |grep 'dracut pre-pivot and cleanup hook'
         Starting dracut pre-pivot and cleanup hook...
  [  OK  ] Finished dracut pre-pivot and cleanup hook.
  ```
* initrd-fs.target
  ```
  initrd-fs.target:Description=Initrd File Systems
  ...
  [root@11-244-8-3 11:04:23 wfq]# cat console.log |grep 'Initrd File Systems'
  [  OK  ] Reached target Initrd File Systems.
  ```
* system.slice
  ```
  system.slice changed dead -> active
  ```
* initrd-root-fs.target: 

  + 异常环境:
    ```
    [root@11-244-8-3 11:09:49 wfq]# cat console.log |grep 'Initrd Root File System'
    [root@11-244-8-3 11:10:38 wfq]#
    ```
  + 正常环境
    ```
    [root@11-244-8-100 11:11:31 i-4v0vppmc5n]# cat console.log |grep 'Initrd Root File System'
    [  OK  ] Reached target Initrd Root File System.
    [  OK  ] Stopped target Initrd Root File System.
    ```
* systemd-journald.socket
  ```
  systemd-journald.socket: Changed dead -> listening
  systemd-journald.socket: Job 14 systemd-journald.socket/start finished, result=done
  systemd-journald.socket: Changed listening -> running
  ```
* initrd.target 
  ```
  [root@11-244-8-3 11:13:02 wfq]# cat console.log |grep -i 'Initrd Default Target'
  [  OK  ] Reached target Initrd Default Target.
  ```

**可以发现是 `initrd-root-fs.target`未执行成功导致**

## 查看 initrd-root-fs.target 日志
### 异常启动
```
[root@11-244-8-3 11:25:19 wfq]# cat console.log |grep -i -E 'initrd-root-fs'
unit_file_build_name_map: normal unit file: /usr/lib/systemd/system/initrd-root-fs.target
initrd-root-fs.target: Installed new job initrd-root-fs.target/start as 36
initrd-root-fs.target: starting held back, waiting for: dracut-pre-mount.service
dracut-mount.service: starting held back, waiting for: initrd-root-fs.target
initrd-parse-etc.service: starting held back, waiting for: initrd-root-fs.target
```
可以看到其在等待`dracut-pre-mount.service`

我们将该服务信息打印出来
```
unit_file_build_name_map: normal unit file: /usr/lib/systemd/system/initrd-root-fs.target
unit_file_build_name_map: normal unit file: /usr/lib/systemd/system/dracut-pre-mount.service
dracut-pre-mount.service: Installed new job dracut-pre-mount.service/start as 35
initrd-root-fs.target: Installed new job initrd-root-fs.target/start as 36
initrd-root-fs.target: starting held back, waiting for: dracut-pre-mount.service
dracut-mount.service: starting held back, waiting for: initrd-root-fs.target
dracut-pre-mount.service: starting held back, waiting for: dracut-initqueue.service
initrd-parse-etc.service: starting held back, waiting for: initrd-root-fs.target
...
[    3.228049] dracut-pre-mount[401]: ///lib/dracut/hooks/pre-mount/99-mount-virtiofs.sh@21(source): :
[    3.230076] dracut-pre-mount[401]: /bin/dracut-pre-mount@17(): export -p
[    3.232058] dracut-pre-mount[401]: /bin/dracut-pre-mount@19(): exit 0
```

可以看到其服务脚本执行完成了，并且返回正常值`0`

### 正常启动
```
unit_file_build_name_map: normal unit file: /usr/lib/systemd/system/initrd-root-fs.target
unit_file_build_name_map: normal unit file: /usr/lib/systemd/system/dracut-pre-mount.service
dracut-pre-mount.service: Installed new job dracut-pre-mount.service/start as 51
initrd-root-fs.target: Installed new job initrd-root-fs.target/start as 50
initrd-root-fs.target: starting held back, waiting for: dracut-pre-mount.service
sysroot.mount: starting held back, waiting for: dracut-pre-mount.service
dracut-mount.service: starting held back, waiting for: dracut-pre-mount.service
dracut-pre-mount.service: starting held back, waiting for: dracut-initqueue.service
initrd-parse-etc.service: starting held back, waiting for: initrd-root-fs.target
...

[    3.313439] dracut-pre-mount[402]: /bin/dracut-pre-mount@17(): export -p
[    3.318044] dracut-pre-mount[402]: /bin/dracut-pre-mount@19(): exit 0
unit_file_build_name_map: normal unit file: /usr/lib/systemd/system/initrd-root-fs.target
Linked unit file: /usr/lib/systemd/system/dracut-pre-mount.service → /usr/lib/dracut/modules.d/98dracut-systemd/dracut-pre-mount.service
initrd-root-fs.target changed dead -> active
initrd-root-fs.target: stopping held back, waiting for: initrd-switch-root.target
initrd-root-fs.target changed active -> dead
initrd-root-fs.target: Job 307 initrd-root-fs.target/stop finished, result=done
```
可以发现对比异常启动，正常启动在启动完`dracut-pre-mount.service`, `systemd` 意识
到该服务完成，并启动`initrd-root-fs`服务。

所以综上所属，直接原因时因为`initrd-root-fs`服务启动阻塞导致，而
`initrd-root-fs`被阻塞于`dracut-pre-mount.service`, 而在异常启动时，
`dracut-pre-mount.service`被正常执行完了，但是似乎并未被systemd识别到，导致
`initrd-root-fs`一直阻塞。
