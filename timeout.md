# 模拟环境负载高, dracut-shutdown执行情况

## /usr/lib/dracut/usr/lib/dracut/dracut-initramfs-restore 原本脚本
```
#!/bin/bash

set -e
set -x

# do some sanity checks first
[ -e /run/initramfs/bin/sh ] && exit 0
[ -e /run/initramfs/.need_shutdown ] || exit 0

# SIGTERM signal is received upon forced shutdown: ignore the signal
# We want to remain alive to be able to trap unpacking errors to avoid
# switching root to an incompletely unpacked initramfs
trap 'echo "Received SIGTERM signal, ignoring!" >&2' TERM

KERNEL_VERSION="$(uname -r)"

[[ $dracutbasedir ]] || dracutbasedir=/usr/lib/dracut
SKIP="$dracutbasedir/skipcpio"
[[ -x $SKIP ]] || SKIP=cat

[[ -f /etc/machine-id ]] && read MACHINE_ID < /etc/machine-id

mount -o ro /boot &>/dev/null || true

if [[ $MACHINE_ID ]] && [[ -d /boot/${MACHINE_ID} || -L /boot/${MACHINE_ID} ]] ; then
    IMG="/boot/${MACHINE_ID}/${KERNEL_VERSION}/initrd"
fi
[[ -f $IMG ]] || IMG="/boot/initramfs-${KERNEL_VERSION}.img"

cd /run/initramfs

[ -f .need_shutdown -a -f "$IMG" ] || exit 1

if $SKIP "$IMG" | zcat | cpio -id --no-absolute-filenames --quiet >/dev/null |sleep 100000; then
    rm -f -- .need_shutdown
elif $SKIP "$IMG"| xzcat | cpio -id --no-absolute-filenames --quiet >/dev/null|sleep 9999; then
    rm -f -- .need_shutdown
else
    # something failed, so we clean up
    echo "Unpacking of $IMG to /run/initramfs failed" >&2
    rm -f -- /run/initramfs/shutdown
    exit 1
fi

if [[ -d squash ]]; then
    unsquashfs -no-xattrs -f -d . squash-root.img >/dev/null
    if [ $? -ne 0 ]; then
        echo "Squash module is enabled for this initramfs but failed to unpack squash-root.img" >&2
        rm -f -- /run/initramfs/shutdown
        exit 1
    fi
fi

exit 0
```

## 修改脚本
```
[root@localhost dracut]# diff -Naru dracut-initramfs-restore.org dracut-initramfs-restore
--- dracut-initramfs-restore.org        2022-05-27 17:54:20.787423069 +0800
+++ dracut-initramfs-restore    2022-05-27 17:53:20.686835954 +0800
@@ -1,6 +1,7 @@
 #!/bin/bash

 set -e
+set -x

 # do some sanity checks first
 [ -e /run/initramfs/bin/sh ] && exit 0
@@ -30,13 +31,7 @@

 [ -f .need_shutdown -a -f "$IMG" ] || exit 1

-if $SKIP "$IMG" | zcat | cpio -id --no-absolute-filenames --quiet >/dev/null; then
-    rm -f -- .need_shutdown
-elif $SKIP "$IMG" | xzcat | cpio -id --no-absolute-filenames --quiet >/dev/null; then
-    rm -f -- .need_shutdown
-elif $SKIP "$IMG" | lz4 -d -c | cpio -id --no-absolute-filenames --quiet >/dev/null; then
-    rm -f -- .need_shutdown
-elif $SKIP "$IMG" | zstd -d -c | cpio -id --no-absolute-filenames --quiet >/dev/null; then
#=====(1)============
+if $SKIP "$IMG" | zcat | cpio -id --no-absolute-filenames --quiet >/dev/null |sleep 100000; then
     rm -f -- .need_shutdown
 else
     # something failed, so we clean up
```

在脚本(1) 处，管道末尾加入sleep 100000, 模拟命令执行变慢的情况。
并把其他的elif的语句删除，做测试。

## 第一次测试过程
执行`systemctl stop dracut-shutdown`, 该过程会执行`/usr/lib/dracut/dracut-initramfs-restore`
脚本 , `strace`跟踪 `bash dracut-initramfs-restore`, 和`sleep 100000`进程

* bash进程
```
[root@localhost ~]# strace -p 1022
strace: Process 1022 attached
wait4(-1, 0x7fff6492a3a0, 0, NULL)      = ? ERESTARTSYS (To be restarted if SA_RESTART is set)
--- SIGTERM {si_signo=SIGTERM, si_code=SI_USER, si_pid=1, si_uid=0} ---
--- SIGCONT {si_signo=SIGCONT, si_code=SI_USER, si_pid=1, si_uid=0} ---
rt_sigreturn({mask=[CHLD]})             = -1 EINTR (Interrupted system call)
wait4(-1, [{WIFSIGNALED(s) && WTERMSIG(s) == SIGTERM}], 0, NULL) = 1028
rt_sigaction(SIGINT, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7f0038c14400}, {sa_handler=0x55a69a976c60, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7f0038c14400}, 8) = 0
openat(AT_FDCWD, "/usr/share/locale/locale.alias", O_RDONLY|O_CLOEXEC) = 5
fstat(5, {st_mode=S_IFREG|0644, st_size=2997, ...}) = 0
read(5, "# Locale name alias data base.\n#"..., 4096) = 2997
read(5, "", 4096)                       = 0
close(5)                                = 0
openat(AT_FDCWD, "/usr/share/locale/en_US.UTF-8/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en_US.utf8/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en_US/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en.UTF-8/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en.utf8/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
write(2, "Terminated\n", 11)            = 11
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [CHLD], 8) = 0
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
rt_sigprocmask(SIG_BLOCK, NULL, [CHLD], 8) = 0
write(2, "++ echo 'Received SIGTERM signal"..., 45) = 45
fcntl(1, F_GETFD)                       = 0
fcntl(1, F_DUPFD, 10)                   = 10
fcntl(1, F_GETFD)                       = 0
fcntl(10, F_SETFD, FD_CLOEXEC)          = 0
dup2(2, 1)                              = 1
fcntl(2, F_GETFD)                       = 0
fstat(1, {st_mode=S_IFSOCK|0777, st_size=0, ...}) = 0
write(1, "Received SIGTERM signal, ignorin"..., 35) = 35
dup2(10, 1)                             = 1
fcntl(10, F_GETFD)                      = 0x1 (flags FD_CLOEXEC)
close(10)                               = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [CHLD], 8) = 0
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
close(5)                                = -1 EBADF (Bad file descriptor)
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=1025, si_uid=0, si_status=0, si_utime=40, si_stime=19} ---
wait4(-1, 0x7fff6492a010, WNOHANG, NULL) = -1 ECHILD (No child processes)
rt_sigreturn({mask=[]})                 = 0
write(2, "+ echo 'Unpacking of /boot/initr"..., 93) = 93
fcntl(1, F_GETFD)                       = 0
fcntl(1, F_DUPFD, 10)                   = 10
fcntl(1, F_GETFD)                       = 0
fcntl(10, F_SETFD, FD_CLOEXEC)          = 0
dup2(2, 1)                              = 1
fcntl(2, F_GETFD)                       = 0
write(1, "Unpacking of /boot/initramfs-4.1"..., 84) = 84
dup2(10, 1)                             = 1
fcntl(10, F_GETFD)                      = 0x1 (flags FD_CLOEXEC)
close(10)                               = 0
write(2, "+ rm -f -- /run/initramfs/shutdo"..., 35) = 35
stat(".", {st_mode=S_IFDIR|0755, st_size=440, ...}) = 0
stat("/usr/local/sbin/rm", 0x7fff6492a2e0) = -1 ENOENT (No such file or directory)
stat("/usr/local/bin/rm", 0x7fff6492a2e0) = -1 ENOENT (No such file or directory)
stat("/usr/sbin/rm", 0x7fff6492a2e0)    = -1 ENOENT (No such file or directory)
stat("/usr/bin/rm", {st_mode=S_IFREG|0755, st_size=72216, ...}) = 0
stat("/usr/bin/rm", {st_mode=S_IFREG|0755, st_size=72216, ...}) = 0
geteuid()                               = 0
getegid()                               = 0
getuid()                                = 0
getgid()                                = 0
access("/usr/bin/rm", X_OK)             = 0
stat("/usr/bin/rm", {st_mode=S_IFREG|0755, st_size=72216, ...}) = 0
geteuid()                               = 0
getegid()                               = 0
getuid()                                = 0
getgid()                                = 0
access("/usr/bin/rm", R_OK)             = 0
stat("/usr/bin/rm", {st_mode=S_IFREG|0755, st_size=72216, ...}) = 0
stat("/usr/bin/rm", {st_mode=S_IFREG|0755, st_size=72216, ...}) = 0
geteuid()                               = 0
getegid()                               = 0
getuid()                                = 0
getgid()                                = 0
access("/usr/bin/rm", X_OK)             = 0
stat("/usr/bin/rm", {st_mode=S_IFREG|0755, st_size=72216, ...}) = 0
geteuid()                               = 0
getegid()                               = 0
getuid()                                = 0
getgid()                                = 0
access("/usr/bin/rm", R_OK)             = 0
rt_sigprocmask(SIG_BLOCK, [INT CHLD], [], 8) = 0
clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f00395f5a10) = 1359
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [], 8) = 0
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [], 8) = 0
rt_sigaction(SIGINT, {sa_handler=0x55a69a976c60, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7f0038c14400}, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7f0038c14400}, 8) = 0
wait4(-1, [{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0, NULL) = 1359
rt_sigaction(SIGINT, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7f0038c14400}, {sa_handler=0x55a69a976c60, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7f0038c14400}, 8) = 0
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=1359, si_uid=0, si_status=0, si_utime=0, si_stime=0} ---
wait4(-1, 0x7fff64929e50, WNOHANG, NULL) = -1 ECHILD (No child processes)
rt_sigreturn({mask=[]})                 = 0
write(2, "+ exit 1\n", 9)               = 9
rt_sigprocmask(SIG_BLOCK, [CHLD], [], 8) = 0
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
close(4)                                = 0
exit_group(1)                           = ?
+++ exited with 1 +++
```

可见bash进程收到信号后，会将错误输出重定位，然后将`Received SIGTERM 
signal, ignorin`输出到标准输出（实际上是错误输出), 然后看起来像是等
子进程退出，之后脚本忽略该信号继续运行.

* sleep进程
```
[root@localhost ~]# strace -p 1028
strace: Process 1028 attached
restart_syscall(<... resuming interrupted nanosleep ...>) = ? ERESTART_RESTARTBLOCK (Interrupted by signal)
--- SIGTERM {si_signo=SIGTERM, si_code=SI_USER, si_pid=1, si_uid=0} ---
+++ killed by SIGTERM +++
```
sleep 收到信号后直接退出。

可见脚本中的trap的作用，可以让`dracut-initramfs-restore`遇到信号后不退出，但是子进程会退出，
继续走下面的流程. 最终会走到else处，打印`Unpacking of $IMG to /run/initramfs failed"`
并将`rm -f --/run/initramfs/shutdown`

这样看来不会有问题，即便是cpio等相关命令过慢，导致超时，但是该脚本会将
`/run/initramfs/shutdown`删除， 就不会遇到之前的panic现象。

但是上面测试删除了if分支，假如那几个分支也执行过慢，会有什么现象呢，

## 再次测试
修改脚本如下:
```
[root@localhost dracut]# diff -Naru dracut-initramfs-restore.org dracut-initramfs-restore
--- dracut-initramfs-restore.org        2022-05-27 17:54:20.787423069 +0800
+++ dracut-initramfs-restore    2022-05-27 18:42:52.146981013 +0800
@@ -1,6 +1,7 @@
 #!/bin/bash

 set -e
+set -x

 # do some sanity checks first
 [ -e /run/initramfs/bin/sh ] && exit 0
@@ -30,13 +31,9 @@

 [ -f .need_shutdown -a -f "$IMG" ] || exit 1

-if $SKIP "$IMG" | zcat | cpio -id --no-absolute-filenames --quiet >/dev/null; then
+if $SKIP "$IMG" | zcat | cpio -id --no-absolute-filenames --quiet >/dev/null |sleep 100000; then
     rm -f -- .need_shutdown
-elif $SKIP "$IMG" | xzcat | cpio -id --no-absolute-filenames --quiet >/dev/null; then
-    rm -f -- .need_shutdown
-elif $SKIP "$IMG" | lz4 -d -c | cpio -id --no-absolute-filenames --quiet >/dev/null; then
-    rm -f -- .need_shutdown
-elif $SKIP "$IMG" | zstd -d -c | cpio -id --no-absolute-filenames --quiet >/dev/null; then
+elif $SKIP "$IMG" | xzcat | cpio -id --no-absolute-filenames --quiet >/dev/null |sleep 9999; then
     rm -f -- .need_shutdown
 else
     # something failed, so we clean up
```

* bash进程
```
[root@localhost ~]# strace -p 1363
strace: Process 1363 attached
wait4(-1, 0x7ffe5e9e5710, 0, NULL)      = ? ERESTARTSYS (To be restarted if SA_RESTART is set)
--- SIGTERM {si_signo=SIGTERM, si_code=SI_USER, si_pid=1, si_uid=0} ---
--- SIGCONT {si_signo=SIGCONT, si_code=SI_USER, si_pid=1, si_uid=0} ---
rt_sigreturn({mask=[CHLD]})             = -1 EINTR (Interrupted system call)
wait4(-1, [{WIFSIGNALED(s) && WTERMSIG(s) == SIGTERM}], 0, NULL) = 1369
rt_sigaction(SIGINT, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7efc16c21400}, {sa_handler=0x55e9d4c84c60, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7efc16c21400}, 8) = 0
openat(AT_FDCWD, "/usr/share/locale/locale.alias", O_RDONLY|O_CLOEXEC) = 5
fstat(5, {st_mode=S_IFREG|0644, st_size=2997, ...}) = 0
read(5, "# Locale name alias data base.\n#"..., 4096) = 2997
read(5, "", 4096)                       = 0
close(5)                                = 0
openat(AT_FDCWD, "/usr/share/locale/en_US.UTF-8/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en_US.utf8/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en_US/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en.UTF-8/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en.utf8/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/usr/share/locale/en/LC_MESSAGES/libc.mo", O_RDONLY) = -1 ENOENT (No such file or directory)
write(2, "Terminated\n", 11)            = 11
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [CHLD], 8) = 0
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
rt_sigprocmask(SIG_BLOCK, NULL, [CHLD], 8) = 0
write(2, "++ echo 'Received SIGTERM signal"..., 45) = 45
fcntl(1, F_GETFD)                       = 0
fcntl(1, F_DUPFD, 10)                   = 10
fcntl(1, F_GETFD)                       = 0
fcntl(10, F_SETFD, FD_CLOEXEC)          = 0
dup2(2, 1)                              = 1
fcntl(2, F_GETFD)                       = 0
fstat(1, {st_mode=S_IFSOCK|0777, st_size=0, ...}) = 0
write(1, "Received SIGTERM signal, ignorin"..., 35) = 35
dup2(10, 1)                             = 1
fcntl(10, F_GETFD)                      = 0x1 (flags FD_CLOEXEC)
close(10)                               = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [CHLD], 8) = 0
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
close(5)                                = -1 EBADF (Bad file descriptor)
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=1366, si_uid=0, si_status=0, si_utime=41, si_stime=17} ---
wait4(-1, 0x7ffe5e9e5390, WNOHANG, NULL) = -1 ECHILD (No child processes)
rt_sigreturn({mask=[]})                 = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [], 8) = 0
pipe([5, 6])                            = 0
rt_sigprocmask(SIG_BLOCK, [INT CHLD], [CHLD], 8) = 0
clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7efc17602a10) = 1383
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
close(6)                                = 0
close(6)                                = -1 EBADF (Bad file descriptor)
pipe([6, 7])                            = 0
rt_sigprocmask(SIG_BLOCK, [INT CHLD], [CHLD], 8) = 0
clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7efc17602a10) = 1384
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
close(5)                                = 0
close(7)                                = 0
close(5)                                = -1 EBADF (Bad file descriptor)
close(7)                                = -1 EBADF (Bad file descriptor)
pipe([5, 7])                            = 0
rt_sigprocmask(SIG_BLOCK, [INT CHLD], [CHLD], 8) = 0
clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7efc17602a10) = 1385
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
close(6)                                = 0
close(7)                                = 0
close(6)                                = -1 EBADF (Bad file descriptor)
close(7)                                = -1 EBADF (Bad file descriptor)
rt_sigprocmask(SIG_BLOCK, [INT CHLD], [CHLD], 8) = 0
clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7efc17602a10) = 1386
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
close(5)                                = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [CHLD], 8) = 0
rt_sigprocmask(SIG_SETMASK, [CHLD], NULL, 8) = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [CHLD], 8) = 0
rt_sigaction(SIGINT, {sa_handler=0x55e9d4c84c60, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7efc16c21400}, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x7efc16c21400}, 8) = 0
wait4(-1, [{WIFEXITED(s) && WEXITSTATUS(s) == 1}], 0, NULL) = 1384
wait4(-1, [{WIFEXITED(s) && WEXITSTATUS(s) == 2}], 0, NULL) = 1385
wait4(-1, [{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0, NULL) = 1383
wait4(-1,
 <unfinished ...>)            = ?
+++ killed by SIGKILL +++
```

* sleep 100000进程
和上面一样，被SIGTEAM杀死

* sleep 9999 进程
```
[root@localhost ~]# strace -p 1386
strace: Process 1386 attached
restart_syscall(<... resuming interrupted nanosleep ...>
) = ?
+++ killed by SIGKILL +++

```
* systemctl status dracut-shutdown
```
● dracut-shutdown.service - Restore /run/initramfs on shutdown
   Loaded: loaded (/usr/lib/systemd/system/../../dracut/modules.d/98dracut-systemd/dracut-shutdown.service; static; vendor preset: disabled)
   Active: failed (Result: timeout) since Fri 2022-05-27 18:58:16 CST; 3min 8s ago
     Docs: man:dracut-shutdown.service(8)
  Process: 1363 ExecStop=/usr/lib/dracut/dracut-initramfs-restore (code=killed, signal=KILL)
  Process: 750 ExecStart=/bin/true (code=exited, status=0/SUCCESS)
 Main PID: 750 (code=exited, status=0/SUCCESS)

May 27 18:56:46 localhost.localdomain dracut-initramfs-restore[1363]: + cpio -id --no-absolute-filenames --quiet
May 27 18:56:46 localhost.localdomain dracut-initramfs-restore[1363]: + sleep 9999
May 27 18:56:46 localhost.localdomain dracut-initramfs-restore[1363]: xzcat: (stdin): File format not recognized
May 27 18:56:46 localhost.localdomain dracut-initramfs-restore[1363]: cpio: premature end of archive
May 27 18:58:16 localhost.localdomain systemd[1]: dracut-shutdown.service: State 'stop-sigterm' timed out. Killing.
May 27 18:58:16 localhost.localdomain systemd[1]: dracut-shutdown.service: Killing process 1363 (dracut-initramf) with signal SIGKILL.
May 27 18:58:16 localhost.localdomain systemd[1]: dracut-shutdown.service: Killing process 1386 (sleep) with signal SIGKILL.
May 27 18:58:16 localhost.localdomain systemd[1]: dracut-shutdown.service: Control process exited, code=killed status=9
May 27 18:58:16 localhost.localdomain systemd[1]: dracut-shutdown.service: Failed with result 'timeout'.
May 27 18:58:16 localhost.localdomain systemd[1]: Stopped Restore /run/initramfs on shutdown.
~
```

可以看到`/var/lib/dracut/dracut-initramfs-restore`脚本超时后，收到SIGTEAM信号会继续
执行，进入到第二个if分支，`sleep 9999`, 又过了**90s** , `sleep 9999`和
`dracut-initramfs-restore`相关进程被SIGKILL信号杀死。

这样来看，如果第一个分支执行过慢超时后，第二个分支也执行过慢，会导致被KILL信号杀，
就走不到最后的`rm -f /run/initramfs/shutdown`, 同时也不会删除`/run/initramfs/
.need_shutdown`, 这样就和case很像
