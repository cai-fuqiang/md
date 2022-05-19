# virsh dump
参照东旭之前处理的case 
https://easystack.atlassian.net/browse/EAS-87665

想通过`virsh dump`命令搞到vmcore， 然后执行dmesg命令获取
更多的日志。

在libvirt pod中执行下面命令：
```
virsh dump --domain instance-00000004 --verbose --memory --format=kernel-zlib /root/dump.zlib
```
但是报错：
```
error: command 'dump' doesn't support option --memory
```
查看virsh dump 命令help
```
  NAME
    dump - dump the core of a domain to a file for analysis

  SYNOPSIS
    dump <domain> <file> [--live] [--crash] [--bypass-cache] [--reset] [--verbose] [--memory-only] [--format <string>]

  DESCRIPTION
    Core dump a domain.

  OPTIONS
    [--domain] <string>  domain name, id or uuid
    [--file] <string>  where to dump the core
    --live           perform a live core dump if supported
    --crash          crash the domain after core dump
    --bypass-cache   avoid file system cache when dumping
    --reset          reset the domain after core dump
    --verbose        display the progress of dump
    --memory-only    dump domain's memory only
    --format <string>  specify the format of memory-only dump
```
并结合命令执行情况, 得出下面的结论: 
* 该环境上的这个命令没有`--memory`参数，只有`--memory-only`参数.
* --format参数，只支持`'kdump-zlib', 'kdump-lzo', 'kdump-snappy' , 'elf'`参数
执行

```
virsh dump --domain instance-00000004 --verbose --memory-only --format=kdump-zlib /root/dump.zlib
```
命令执行成功。

但是在执行`crash vmcore /root/dump.zlib`命令时失败
失败信息：

```
WARNING: kimage_voffset cannot be determined from the dumpfile.
       Using default value of 0.  If this is not correct, then try
       using the command line option: --machdep kimage_voffset=<addr>
GNU gdb (GDB) 7.6
Copyright (C) 2013 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "aarch64-unknown-linux-gnu"...

crash: vmlinux and dump.zlib do not match!

```

看起来像是文件格式的问题，查看dump.zlib文件和正常kernel 崩溃产生的vmcore的前100个字节：

* 通过`virsh dump`导出的vmcore
```
0000000: 6d61 6b65 6475 6d70 6669 6c65 0000 0000  makedumpfile....
0000010: 0000 0000 0000 0001 0000 0000 0000 0001  ................
0000020: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000030: 0000                                     ..
```

* kernel崩溃产生的vmcore

```
0000000: 4b44 554d 5020 2020 0600 0000 4c69 6e75  KDUMP   ....Linu
0000010: 7800 0000 0000 0000 0000 0000 0000 0000  x...............
0000020: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000030: 0000                                     ..
```

请教东旭，了解到x86 `virsh dump`命令可以正常执行，arm环境会有问题


void
xfs_error_report(
    const char      *tag,
    int         level,
    struct xfs_mount    *mp,
    const char      *filename,
    int         linenum,
    xfs_failaddr_t      failaddr)
{
    if (level <= xfs_error_level) {
		//会打印下面一条，并且会打印dump堆栈	
        xfs_alert_tag(mp, XFS_PTAG_ERROR_REPORT,
        "Internal error %s at line %d of file %s.  Caller %pS",
                tag, linenum, filename, failaddr);

        xfs_stack_trace();
    }
}
```

