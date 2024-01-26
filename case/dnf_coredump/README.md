# coredump分析
## 查看产生coredump的进程
因为dnf是一个python脚本，这里怀疑是python进程，使用
file命令正好可以看到当时进程的cmdline, 如下:
```
[root@localhost python36]# file core.dnf.60305.node-31.domain.tld.1684334980
core.dnf.60305.node-31.domain.tld.1684334980: ELF 64-bit LSB core file, x86-64, version 1 (SYSV), SVR4-style, from '/usr/bin/python3.6 /usr/bin/dnf shell -y -y --allowerasing --best', real uid: 0, effective uid: 0, real gid: 0, effective gid: 0, execfn: '/usr/bin/dnf', platform: 'x86_64'
```
## 安装debuginfo包进行调试
相关debuginfo包链接:
```
http://koji.easystack.io/kojifiles/packages/python3/3.6.8/39.es8.rocky.0/x86_64/python3-debuginfo-3.6.8-39.es8.rocky.0.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/libdb/5.3.28/42.es8/x86_64/libdb-debuginfo-5.3.28-42.es8.x86_64.rpm
```
执行下面gdb命令
```
[root@localhost python36]# gdb core.dnf.60305.node-31.domain.tld.1684334980
...
Reading symbols from /usr/libexec/platform-python3.6...Reading symbols from /usr/lib/debug/usr/libexec/platform-python3.6-3.6.8-39.es8.rocky.0.x86_64.debug...done.
...
(gdb) bt
#0  0x00007fbeb9eb70a8 in __lock_allocobj (part_id=<optimized out>, lt=0x1) at ../../src/lock/lock_alloc.incl:48
#1  0x00007fbeb9eb70a8 in __lock_getobj (lt=0x1, ndx=<optimized out>, create=<optimized out>, retp=<optimized out>, obj=<optimized out>, obj=<optimized out>) at ../../src/lock/lock.c:1504
#2  0x00007fbeae74d3f0 in  ()
#3  0x0000000000000000 in  ()
```

可以看到这里提供的栈信息有限，只能打印出上面两层栈(libdb)，和python相关的栈给不出，
没有什么参考价值。

查看 `__lock_getobj()`定义：
```cpp
static int
__lock_getobj(lt, obj, ndx, create, retp)
    DB_LOCKTAB *lt;
    const DBT *obj;
    u_int32_t ndx;
    int create;
    DB_LOCKOBJ **retp;
```

可见 `lt`为`0x1`，这里感觉是一个非法指针。

查看出问题的指令:
```
(gdb) x/1xg 0x00007fbeb9eb70a8
0x7fbeb9eb70a8 <__lock_getobj+408>:     0x0000000000000000
(gdb) x/1i 0x00007fbeb9eb70a8
=> 0x7fbeb9eb70a8 <__lock_getobj+408>:  add    %al,(%rax)
(gdb) x/10xg 0x00007fbeb9eb70a8
0x7fbeb9eb70a8 <__lock_getobj+408>:     0x0000000000000000      0x0000000000000000
0x7fbeb9eb70b8 <__lock_getobj+424>:     0x0000000000000000      0x0000000000000000
0x7fbeb9eb70c8 <__lock_getobj+440>:     0x0000000000000000      0x0000000000000000
0x7fbeb9eb70d8 <__lock_getobj+456>:     0x0000000000000000      0x0000000000000000
0x7fbeb9eb70e8 <__lock_getobj+472>:     0x0000000000000000      0x0000000000000000
```
可以看到相关代码段的内存都被初始化为0.

在环境中查找相关库（不带调试信息）， 查看`__lock_getobj+408`相关指令
```
[root@localhost python36]# gdb /usr/lib64/libdb-5.3.so
(gdb) disassemble __lock_getobj
   ...
   0x00000000000df462 <+402>:   je     0xdf48f <__lock_getobj+447>
   0x00000000000df464 <+404>:   mov    0x18(%rax),%rcx
   0x00000000000df468 <+408>:   mov    0x20(%rax),%rsi
   0x00000000000df46c <+412>:   cmp    $0xffffffffffffffff,%rcx
   ...
```
可以看到这里是一个`mov`指令，但是根据shawn提供的message日志，其中
里面kernel报错:
```
traps: dnf[60305] traps divide error ip:....
```
查看kernel相关代码如下:
```cpp
DO_ERROR(X86_TRAP_DE,     SIGFPE,  FPE_INTDIV,   IP, "divide error",        divide_error)


#define DO_ERROR(trapnr, signr, sicode, addr, str, name)                   \
dotraplinkage void do_##name(struct pt_regs *regs, long error_code)        \
{                                                                          \
        do_error_trap(regs, error_code, str, trapnr, signr, sicode, addr); \
}

/*
 * 相关调用栈，这里不再列举代码:
 * do_error_trap
 *    do_trap
 *       show_signal
 */
static void show_signal(struct task_struct *tsk, int signr,
                        const char *type, const char *desc,
                        struct pt_regs *regs, long error_code)
{
    if (show_unhandled_signals && unhandled_signal(tsk, signr) &&
¦   printk_ratelimit()) {
        pr_info("%s[%d] %s%s ip:%lx sp:%lx error:%lx",
                tsk->comm, task_pid_nr(tsk), type, desc,
                regs->ip, regs->sp, error_code);
        print_vma_addr(KERN_CONT " in ", regs->ip);
        pr_cont("\n");
}
```
关于idt entry的定义:
```cpp
static const __initconst struct idt_data def_idts[] = {
    INTG(X86_TRAP_DE,               divide_error),
}
```

可以看到为`#DE`的异常。
关于`#DE`异常，在intel sdm中搜索了下，主要是跟除法相关指令(eg: div)有关。
mov指令按道理不能触发。


## 做个实验看下正常dnf 进程的相关偏移
执行`dnf remove vim`，使其阻塞, 然后使用gdb attach该进程:
```
[root@localhost ~]# ps aux |grep vim
root      439348 16.4  0.0 574236 74856 pts/2    S+   08:06   0:01 /usr/bin/python3.6 /usr/bin/dnf remove vim
root      439350  0.0  0.0   9204  1192 pts/3    S+   08:06   0:00 grep --color=auto vim
[root@localhost ~]# gdb -p 439348
...
(gdb)  x/1i 0x00007fbeb9eb70a8
   0x7fbeb9eb70a8:      Cannot access memory at address 0x7fbeb9eb70a8
(gdb) p __Lock_getobj
(gdb) p &__lock_getobj
$2 = (int (*)(DB_LOCKTAB *, u_int32_t, int, DB_LOCKOBJ **, const DBT *, const DBT *)) 0x7fd0abb372d0 <__lock_getobj>
```
可以发现折这个符号的地址变了。
ldd查看动态连接库
```
[root@localhost ~]# ls -l  /usr/bin/python3.6
lrwxrwxrwx. 1 root root 31 Mar 14 08:07 /usr/bin/python3.6 -> /usr/libexec/platform-python3.6
[root@localhost ~]# ldd /usr/libexec/platform-python3.6
        linux-vdso.so.1 (0x00007ffe20bd1000)
        libcrypto.so.1.1 => /lib64/libcrypto.so.1.1 (0x00007f713c761000)
        libpython3.6m.so.1.0 => /lib64/libpython3.6m.so.1.0 (0x00007f713c21e000)
        libpthread.so.0 => /lib64/libpthread.so.0 (0x00007f713bffe000)
        libdl.so.2 => /lib64/libdl.so.2 (0x00007f713bdfa000)
        libutil.so.1 => /lib64/libutil.so.1 (0x00007f713bbf6000)
        libm.so.6 => /lib64/libm.so.6 (0x00007f713b874000)
        libc.so.6 => /lib64/libc.so.6 (0x00007f713b4af000)
        libz.so.1 => /lib64/libz.so.1 (0x00007f713b298000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f713ce4a000)
```
可以发现并没有这个库，所以也就是说`libdb`可能是`ldopen`方式连接进来的。
# 暂时结论
* 目前通过看`coredump`发现python层面的调试信息并不完整（目前主要是想通过
coredump获取python层面的栈信息，但是现在无法获得，而且从堆栈来看，栈的
层级数量比较少，不像是落在了业务代码中）。
* 关于libdb 库的api触发了异常，如果栈可信的话，可能是由于参数传了一个非法
 指针导致, 但是为什么有这个传递，还需要结合python栈分析（但是现在获取不到）
* 另外，coredump中获取的相关信息(这里涉及动态库），和kernel dmesg中信息对不上，
这个原因未知（个人感觉kernel处理异常的流程应该没有问题，结合上面的分析，
可能是coredump保存的时候出现了问题。
