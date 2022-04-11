# 问题现象
执行`reboot`命令,理论上`systemd-logind`会发出
```
["/org/freedesktop/login1", 
"org.freedesktop.login1.Manager",
"PrepareForShutdown"]
```
dbus signal, 但是在测试过程中,发现未触发该流程

# 问题原因
`reboot`命令在系统中为`systemctl`软连接
```
$ ls -l /usr/sbin/reboot
lrwxrwxrwx 1 root root 14 3月   7 10:47 /usr/sbin/reboot -> /bin/systemctl
```
`reboot`命令执行过程如下:
```mermaid
graph TD
subgraph 直接调用reboot命令触发systemctl

end
```
