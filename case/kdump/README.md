# 问题现象
kdump服务启动失败
在执行`systemctl status kdump`
命令时
```
[root@node-1 ~]# systemctl status kdump
● kdump.service - Crash recovery kernel arming
   Loaded: loaded (/usr/lib/systemd/system/kdump.service; enabled; vendor preset: enabled)
   Active: failed (Result: exit-code) since Wed 2022-04-27 16:43:03 CST; 20min ago
  Process: 2480 ExecStart=/usr/bin/kdumpctl start (code=exited, status=1/FAILURE)
 Main PID: 2480 (code=exited, status=1/FAILURE)

Apr 27 16:43:02 node-1.domain.tld systemd[1]: Starting Crash recovery kernel arming...
Apr 27 16:43:03 node-1.domain.tld kdumpctl[2480]: kdump: kexec: failed to load kdump kernel
Apr 27 16:43:03 node-1.domain.tld kdumpctl[2480]: kdump: Starting kdump: [FAILED]
Apr 27 16:43:03 node-1.domain.tld systemd[1]: kdump.service: Main process exited, code=exited, status=1/FAILURE
Apr 27 16:43:03 node-1.domain.tld systemd[1]: kdump.service: Failed with result 'exit-code'.
Apr 27 16:43:03 node-1.domain.tld systemd[1]: Failed to start Crash recovery kernel arming.
```

# 问题分析

查看kdump unit文件
```
[root@node-1 ~]# cat /usr//lib/systemd/system/kdump.service
[Unit]
Description=Crash recovery kernel arming
After=network.target network-online.target remote-fs.target basic.target
DefaultDependencies=no
ConditionKernelCommandLine=crashkernel

[Service]
Type=oneshot
ExecStart=/usr/bin/kdumpctl start
ExecStop=/usr/bin/kdumpctl stop
ExecReload=/usr/bin/kdumpctl reload
RemainAfterExit=yes
StartLimitInterval=0

[Install]
WantedBy=multi-user.target
```
可以看到执行的命令为`/usr/bin/kdumpctl start`

单独执行该命令:
```
[root@node-1 ~]# /usr/bin/kdumpctl start
kdump: kexec: failed to load kdump kernel
kdump: Starting kdump: [FAILED]
```

查看kdumpctl文件,该文件是一个shell脚本
报错代码为:
```
load_kdump()
{
		...
        ddebug "$KEXEC $KEXEC_ARGS $standard_kexec_args --command-line=$KDUMP_COMMANDLINE --initrd=$TARGET_INITRD $KDUMP_KERNEL"

        # The '12' represents an intermediate temporary file descriptor
        # to store the standard error file descriptor '2', and later
        # restore the error file descriptor with the file descriptor '12'
        # and release it.
        exec 12>&2
        exec 2>> $KDUMP_LOG_PATH/kdump.log
        PS4='+ $(date "+%Y-%m-%d %H:%M:%S") ${BASH_SOURCE}@${LINENO}: '
        set -x

        $KEXEC $KEXEC_ARGS $standard_kexec_args \
                --command-line="$KDUMP_COMMANDLINE" \
                --initrd=$TARGET_INITRD $KDUMP_KERNEL

        ret=$?
        set +x
        exec 2>&12 12>&-

        remove_kdump_kernel_key

        if [ $ret == 0 ]; then
                dinfo "kexec: loaded kdump kernel"
                return 0
        else
                derror "kexec: failed to load kdump kernel"
                return 1
        fi
}
```

所以可以看到实际上是执行kexec相关命令执行失败导致报错.
debug该脚本,获取kexec完整命令
```
+ ddebug '/sbin/kexec -s -d -p --command-line=BOOT_IMAGE=(hd0,gpt3)/vmlinuz-4.18.0-147.5.1.es8_23.x86_64 
ro edd=off kvm.halt_poll_ns=400000 cgroup.memory=nokmem intel_iommu=on iommu=pt pci=realloc ixgbe.allow_unsupported_sfp=1 
rootdelay=90 nomodeset intel_idle.max_cstate=0 processor.max_cstate=0 biosdevname=0 net.ifnames=1 
irqpoll nr_cpus=1 reset_devices cgroup_disable=memory mce=off numa=off udev.children-max=2 panic=10 
rootflags=nofail acpi_no_memhotplug transparent_hugepage=never nokaslr novmcoredd hest_disable 
disable_cpu_apicid=0 --initrd=/boot/initramfs-4.18.0-147.5.1.es8_23.x86_64kdump.img 
/boot/vmlinuz-4.18.0-147.5.1.es8_23.x86_64'
+
```

去除`--command-line`参数, 简单执行该命令:
```
/sbin/kexec -s -d -p --initrd /boot/initramfs-4.18.0-147.5.1.es8_23.x86_64kdump.img /boot/vmlinuz-4.18.0-147.5.1.es8_23.x86_64
 Try gzip decompression.
 Try LZMA decompression.
 lzma_decompress_file: read on /boot/vmlinuz-4.18.0-147.5.1.es8_23.x86_64 of 65536 bytes failed
 kexec_file_load failed: Required key not available
```

`lzma_decompress_file`这行打印没有太大问题,(https://access.redhat.com/solutions/6116581)
后面的报错,在
https://access.redhat.com/solutions/5630371
提到,修改 `/etc/sysconfig/kdump`
将KEXEC_ARGS 中的 '-s' 参数删除,
删除后,kdump启动成功,并且在kernel panic时,可以产生vmcore

# 进一步调查
通过strace 跟踪kexec带-s 和不带-s参数的执行情况
通过strace获取到下面的信息:

* kexec -s
```
openat(AT_FDCWD, "/boot/vmlinuz-4.18.0-147.5.1.es8_23.x86_64", O_RDONLY) = 3
...
openat(AT_FDCWD, "/boot/initramfs-4.18.0-147.5.1.es8_23.x86_64kdump.img",
O_RDONLY) = 5
kexec_file_load(3, 5, 1, "\0", KEXEC_FILE_ON_CRASH) = -1 ENOKEY (Required key
not available)
```

`kexec_file_load()`函数

```
long kexec_file_load(int kernel_fd, int initrd_fd,
                           unsigned long cmdline_len, const char *cmdline,
                           unsigned long flags);
```
可以看到, 打开kernel 和initrd文件, 获取到文件描述符,传入到`kexec_file_load()`
函数,执行该函数后,返回错误.

* kexec 不带-s 参数
```
kexec_load(0xbfb54730, 6, [
{buf=0x7f96adbe4010, bufsz=33298432, mem=0xbae3e000, memsz=33300480}, 
{buf=0x7f96afbaa610, bufsz=8088448, mem=0xbce00000, memsz=45465600}, 
{buf=0x561a63caa0e0, bufsz=18023, mem=0xbfb4f000, memsz=20480}, 
{buf=0x561a63ca2e80, bufsz=28896, mem=0xbfb54000, memsz=36864}, 
{buf=0x561a63ca0c90, bufsz=2048, mem=0xbfb5d000, memsz=16384}, 
{buf=0x7f96b0f05010, bufsz=650240, mem=0xbfb61000, memsz=651264}], 
KEXEC_ARCH_X86_64|KEXEC_ON_CRASH) = 0
```
该函数参数:
```
long kexec_load(unsigned long entry, unsigned long nr_segments,
                struct kexec_segment *segments, unsigned long flags);
```

参数解释: (详见 man kexec_load)
* entry : kernel entry address
* nr_segment : segments中pointer的数量
* kexec_segment:
```
struct kexec_segment {
               void   *buf;        /* Buffer in user space */
               size_t  bufsz;      /* Buffer length in user space */
               void   *mem;        /* Physical address of kernel */
               size_t  memsz;      /* Physical address length */
           };

```
实际上就是一个个的kernel段,加载到虚拟机的不同物理地址.

实际上, 加不加`-g`参数,和调用的系统调用有关
