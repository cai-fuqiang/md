# 查看coredump堆栈
安装systemd-debuginfo-219-73.el7_8.6.5es.aarch64.rpm, glibc-debuginfo-2.17-292.el7.centos.es.aarch64.rpm
等debuginfo 包，查看异常堆栈:
```
gdb ./core.systemd.63108.node-1.domain.tld.1640145986
...
#0  0x0000ffff9c185488 in kill () at ../sysdeps/unix/syscall-template.S:81
#1  0x0000aaaab152747c in crash (sig=6) at src/core/main.c:172
#2  <signal handler called>
#3  0x0000ffff9c185228 in __GI_raise (sig=sig@entry=6) at ../nptl/sysdeps/unix/sysv/linux/raise.c:55
#4  0x0000ffff9c1868a0 in __GI_abort () at abort.c:90
#5  0x0000ffff9c1c516c in __libc_message (do_abort=<optimized out>, fmt=fmt@entry=0xffff9c28a620 "*** Error in `%s': %s: 0x%s ***\n") at ../sysdeps/unix/sysv/linux/libc_fatal.c:196
#6  0x0000ffff9c1cd67c in malloc_printerr (ar_ptr=0xffff9c2d0530 <main_arena>, ptr=<optimized out>, str=0xffff9c28a710 "free(): invalid next size (normal)", action=3) at malloc.c:4967
#7  _int_free (av=0xffff9c2d0530 <main_arena>, p=<optimized out>, have_lock=0) at malloc.c:3843
#8  0x0000aaaab15e1620 in mount_setup_unit (set_flags=true, fstype=0xaaaabfecfcf0 "tmpfs", options=<optimized out>,
    where=0xaaaac002ce90 "/var/lib/kubelet/pods/03a4f6ea-d8c9-45b3-9b93-108244abb801/volumes/kubernetes.io~secret/ceph-bootstrap-mds-keyring", what=0xaaaac0208600 "tmpfs", m=0xaaaabfaf0d80)
    at src/core/mount.c:1558
#9  mount_load_proc_self_mountinfo (m=m@entry=0xaaaabfaf0d80, set_flags=set_flags@entry=true) at src/core/mount.c:1628
#10 0x0000aaaab15e1ca8 in mount_dispatch_io (source=<optimized out>, fd=<optimized out>, revents=<optimized out>, userdata=0xaaaabfaf0d80) at src/core/mount.c:1771
#11 0x0000aaaab15c8d2c in source_dispatch (s=s@entry=0xaaaabfaffc70) at src/libsystemd/sd-event/sd-event.c:2115
#12 0x0000aaaab15c9bcc in sd_event_dispatch (e=0xaaaabfaf1440) at src/libsystemd/sd-event/sd-event.c:2472
#13 0x0000aaaab15c9d94 in sd_event_run (e=<optimized out>, timeout=<optimized out>) at src/libsystemd/sd-event/sd-event.c:2501
#14 0x0000aaaab152fa9c in manager_loop (m=0xaaaabfaf0d80) at src/core/manager.c:2316
#15 0x0000aaaab1523d5c in main (argc=5, argv=0xffffcb8b06b8) at src/core/main.c:1783
```

# 查看相关代码
经查看src.rpm包中代码，和debuginfo包中的二进制信息对不上。
所以这里我们通过gdb`l`命令查看相关代码

## 查看`mount_setup_unit`代码

```
				....
1550            free(p->what);
1551            p->what = w;
1552            w = NULL;
1553
1554            free(p->options);
1555            p->options = o;
1556            o = NULL;
1557
1558            free(p->fstype);			<===== 造成异常调用
(gdb)
1559            p->fstype = f;
1560            f = NULL;
1561
				....
```

根据之前调试经验，如果free函数发生异常可能存在两种情况:
1. free了一个被已经被free的内存
2. 存在malloc数组越界

## 查看调用`__libc_message`代码

```
(gdb) l malloc.c:4967
4962          buf[sizeof (buf) - 1] = '\0';
4963          char *cp = _itoa_word ((uintptr_t) ptr, &buf[sizeof (buf) - 1], 16, 0);
4964          while (cp > buf)
4965            *--cp = '0';
4966
4967          __libc_message (action & 2, "*** Error in `%s': %s: 0x%s ***\n",
4968                          __libc_argv[0] ?: "<unknown>", str, cp);
4969        }
```
这里我们可以获取下libc的打印，通过以下gdb命令:

```
(gdb) f 6
#6  0x0000ffff9c1cd67c in malloc_printerr (ar_ptr=0xffff9c2d0530 <main_arena>, ptr=<optimized out>, str=0xffff9c28a710 "free(): invalid next size (normal)", action=3) at malloc.c:4967
4967          __libc_message (action & 2, "*** Error in `%s': %s: 0x%s ***\n",
(gdb) p __libc_argv[0]
$7 = 0xffffcb8bff9c "/usr/lib/systemd/systemd"
(gdb) p str
$8 = 0xffff9c28a710 "free(): invalid next size (normal)"
(gdb) p cp
$9 = <optimized out>
(gdb) p buf
$10 = "0000aaaac01b9260"
```

这里cp变量被优化了(实际上就是没有保存在栈中，以寄存器的方式存储的），但是可以通过buf变量获取
所以这里打印的信息为:

```
*** Error in `/usr/lib/systemd/systemd': free(): invalid next size (normal): 0x0000aaaac01b9260 ***\n
```

我们再次看下`mount_setup_unit`堆栈

```
(gdb) f 8
#8  0x0000aaaab15e1620 in mount_setup_unit (set_flags=true, fstype=0xaaaabfecfcf0 "tmpfs", options=<optimized out>,
    where=0xaaaac002ce90 "/var/lib/kubelet/pods/03a4f6ea-d8c9-45b3-9b93-108244abb801/volumes/kubernetes.io~secret/ceph-bootstrap-mds-keyring", what=0xaaaac0208600 "tmpfs", m=0xaaaabfaf0d80)
    at src/core/mount.c:1558
1558            free(p->fstype);
(gdb) p p->fstype
$11 = 0xaaaac01b9260 "tmpfs"
```

所以果然是`p->fstype`有问题。


# 分析free出问题堆栈

查看_int_free相关汇编代码
```
   0x0000ffff9c1ccf68 <+120>:   add     x24, x19, x20
3942        }
3943
3944        nextchunk = chunk_at_offset(p, size);
```

```
3968        nextsize = chunksize(nextchunk);
   0x0000ffff9c1ccf88 <+152>:   and     x25, x0, #0xfffffffffffffff8

3969        if (__builtin_expect (nextchunk->size <= 2 * SIZE_SZ, 0)
   0x0000ffff9c1ccf84 <+148>:   cmp     x0, #0x10
   0x0000ffff9c1ccf8c <+156>:   b.ls    0xffff9c1cdb04 <_int_free+3092>

3970            || __builtin_expect (nextsize >= av->system_mem, 0))
   0x0000ffff9c1ccf90 <+160>:   ldr     x0, [x21,#2176]
   0x0000ffff9c1ccf94 <+164>:   cmp     x25, x0
   0x0000ffff9c1ccf98 <+168>:   b.cs    0xffff9c1cdb04 <_int_free+3092>

3971          {
3972            errstr = "free(): invalid next size (normal)";
   0x0000ffff9c1cdb04 <+3092>:  adrp    x20, 0xffff9c28a000
   0x0000ffff9c1cdb08 <+3096>:  add     x19, x19, #0x10
   0x0000ffff9c1cdb0c <+3100>:  add     x20, x20, #0x710
   0x0000ffff9c1cdb10 <+3104>:  b       0xffff9c1cd590 <_int_free+1696>

3973            goto errout;
3974          }
```

```
3971          {
3972            errstr = "free(): invalid next size (normal)";
	0x0000ffff9c1cdb04 <+3092>:	adrp	x20, 0xffff9c28a000
	0x0000ffff9c1cdb08 <+3096>:	add	x19, x19, #0x10
	0x0000ffff9c1cdb0c <+3100>:	add	x20, x20, #0x710
	0x0000ffff9c1cdb10 <+3104>:	b	0xffff9c1cd590 <_int_free+1696>
3973            goto errout;
3974          }

--------------------------------

3840        errout:
3841          if (have_lock || locked)
   0x0000ffff9c1cd590 <+1696>:	orr	w27, w27, w23
   0x0000ffff9c1cd594 <+1700>:	cbnz	w27, 0xffff9c1cd690 <_int_free+1952>

3842            (void)mutex_unlock(&av->mutex);
   0x0000ffff9c1cd690 <+1952>:	mov	w1, #0x0                   	// #0
   0x0000ffff9c1cd694 <+1956>:	ldxr	w0, [x21]
   0x0000ffff9c1cd698 <+1960>:	stlxr	w2, w1, [x21]
   0x0000ffff9c1cd69c <+1964>:	cbnz	w2, 0xffff9c1cd694 <_int_free+1956>
   0x0000ffff9c1cd6a0 <+1968>:	cmp	w0, #0x1
   0x0000ffff9c1cd6a4 <+1972>:	b.le	0xffff9c1cd598 <_int_free+1704>
   0x0000ffff9c1cd6a8 <+1976>:	mov	x0, x21
   0x0000ffff9c1cd6ac <+1980>:	mov	x1, #0x81                  	// #129
   0x0000ffff9c1cd6b0 <+1984>:	mov	x2, #0x1                   	// #1
   0x0000ffff9c1cd6b4 <+1988>:	mov	x3, #0x0                   	// #0
   0x0000ffff9c1cd6b8 <+1992>:	mov	x8, #0x62                  	// #98
   0x0000ffff9c1cd6bc <+1996>:	svc	#0x0
   0x0000ffff9c1cd6c0 <+2000>:	b	0xffff9c1cd598 <_int_free+1704>

3843          malloc_printerr (check_action, errstr, chunk2mem(p), av);
   0x0000ffff9c1cd598 <+1704>:  adrp    x0, 0xffff9c2d0000
   0x0000ffff9c1cd59c <+1708>:  add     x0, x0, #0xe8
   0x0000ffff9c1cd5a0 <+1712>:  ldr     w22, [x0,#88]

```
