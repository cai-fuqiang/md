# 问题现象
![message](pic/message.png)
可以看到主要的出错日志为:
![main](pic/main.png)
```
systemd[1]: Created slice User Slice of UID 0
systemd-logind[1]: New session 25477 of user root.
systemd[1]: Started Session 25477 of user root
systemd[1]: Started Session 25477 of user root
systemd[1]: user-runtime-dir@0.service: Succeeded.
systemd[1]: user-runtime-dir@0.service: Start request repeated too quickly.
systemd[1]: user-runtime-dir@0.service: Failed with result 'start-limit-hit'.
systemd[1]: Failed to start /run/user/0 mount wrapper.
systemd[1]: Dependency failed for user Manager for UID 0.
```
# 代码流程
## 服务依赖
### user@.sevice
```
[Unit]
Description=User Manager for UID %i
After=systemd-user-sessions.service
After=user-runtime-dir@%i.service
Requires=user-runtime-dir@%i.service

[Service]
User=%i
PAMName=systemd-user
Type=notify
ExecStart=-/usr/lib/systemd/systemd --user
Slice=user-%i.slice
KillMode=mixed
Delegate=pids memory
TasksMax=infinity
TimeoutStopSec=120s
```
### user-runtime-dir@%i.service
```
[Unit]
Description=/run/user/%i mount wrapper
StopWhenUnneeded=yes

[Service]
ExecStart=/usr/lib/systemd/systemd-user-runtime-dir start %i
ExecStop=/usr/lib/systemd/systemd-user-runtime-dir stop %i
RemainAfterExit=true
```

## systemd-logind
```
int session_start(Session *s, sd_bus_message *properties) {
	...
	log_struct(s->class == SESSION_BACKGROUND ? LOG_DEBUG : LOG_INFO,
		"MESSAGE_ID=" SD_MESSAGE_SESSION_START_STR,
		"SESSION_ID=%s", s->id,
		"USER_ID=%s", s->user->name,
		"LEADER="PID_FMT, s->leader,
		LOG_MESSAGE("New session %s of user %s.", s->id, s->user->name));
	...

}
static int method_create_session(sd_bus_message *message, void *userdata, sd_bus_error *error) {
	...
	r = session_start(session, message);
	...
}
const sd_bus_vtable manager_vtable[] = {
	...
	SD_BUS_METHOD("CreateSession", "uusssssussbssa(sv)", "soshusub", method_create_session, 0),
	...
};
```
可以看到，该代码流程可以通过`dbus method CreateSession` 调用到。
```

```
