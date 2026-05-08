## 现象
当使用较长的kernel cmdline 时，panic 后第二个内核引导成功，但是无法执行正确的save vmcore流程

> NOTE
>
> kexec 已经合入了 <sup>1</sup> 中的patch

当启动第二个内核后，可能会有一些错误，而导致第二个内核`makedumpfile` 流程会失败:

* [Cannot open /proc/vmcore: No such file or directory](./log/error_log.log)
* [第二个内核产生panic，找不到root](./log/error_log_panic.log)
* 正常启动
  + [log0](./log/success_0.log)
  + [log1](./log/success_1.log)

我们先分析第一个问题

## 代码流程

在第一个日志中，还有下面报错 :
```
[ 2026-03-05T16:56:28+08:00 ] [    0.721118][    T1] Warning: Core image elf header not found
[ 2026-03-05T16:56:28+08:00 ] [    0.721122][    T1] Kdump: vmcore not initialized
```

代码如下:

```sh
vmcore_init
=> elfcorehdr_alloc()  ## 除了 s390 其他架构都为空
=> parse_crash_elf_headers
   ## 先从 `elfcorehdr_addr` 获取 e_ident
   => addr = elfcorehdr_addr;
   ## 将elf header 读取到 e_ident 中
   => elfcorehdr_read(e_ident, EI_NIDENT, &addr);
   => if (memcmp(e_ident, ELFMAG, SELFMAG) != 0)
      => pr_warn("Warning: Core image elf header not found\n");
      => return -EINVAL
## 如果 parse_crash_elf_headers 执行成功, 创建"vmcore" proc 文件
=> proc_vmcore = proc_create("vmcore", S_IRUSR, NULL, &vmcore_proc_ops);
```

可以发现解析elfcore hdr 时，发现格式不对。直接报错:

个人怀疑是 `elfcorehdr_addr` 传入的不对。我们来看下相关日志中，关于 cmdline相关打印:

```
[ 2026-03-05T16:56:20+08:00 ] [    0.000000][    T0] Kernel command line: 
kexec BOOT_IMAGE=/vmlinuz-6.6.0-jdcloud8.0.0cf662f0e6.loongarch64 ro loongarch_iommu=on iommu=pt
isolcpus=40,52,51,43 spectre_v2=off nopti rdt=l3cat selinux=0 rd.lvm.lv=openeuler/root
video=efifb:off cgroup_disable=files apparmor=0 console=tty0 console=ttyS0,115200.allow_unsafe_interrupts=1
nokaslr.allow_unsafe_interrupts=1 nokaslr init 3 irqpoll reset_devices cgroup_disable=memory
udev.children-max=2 panic=10 novmcoredd nokaslr aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
initrd=2973040640,48939008 elfcorehdr=8K
```

而正常启动流程中cmdline如下:
```
[ 2026-03-05T17:03:45+08:00 ] [    0.000000][    T0] Kernel command line:
kexec BOOT_IMAGE=/vmlinuz-6.6.0-jdcloud8.0.0cf662f0e6.loongarch64 ro loongarch_iommu=on
iommu=pt isolcpus=40,52,51,43 spectre_v2=off nopti rdt=l3cat selinux=0 rd.lvm.lv=openeuler/root
video=efifb:off cgroup_disable=files apparmor=0 console=tty0
console=ttyS0,115200.allow_unsafe_interrupts=1 nokaslr.allow_unsafe_interrupts=1 nokaslr
init 3 irqpoll reset_devices cgroup_disable=memory udev.children-max=2
panic=10 novmcoredd nokaslr initrd=2973040640,48938496 elfcorehdr=8K@3882992K
mem=1048576K@2834432K vfio_i
```

cmdline 似乎被截断了.

> NOTE
>
> crash kernel panic 的cmdline 如下:
>
> ```
> [ 2026-03-06T05:03:38+08:00 ] [    0.000000][    T0] 
> Kernel command line: kexec BOOT_IMAGE=/vmlinuz-6.6.0-jdcloud8.0.0cf662f0e6.loongarch64 ro
> loongarch_iommu=on iommu=pt isolcpus=40,52,51,43 spectre_v2=off nopti rdt=l3cat selinux=0
> rd.lvm.lv=openeuler/root video=efifb:off cgroup_disable=files apparmor=0
> console=tty0 console=ttyS0,115200.allow_unsafe_interrupts=1
> nokaslr.allow_unsafe_interrupts=1 nokaslr init 3 irqpoll reset_devices
> cgroup_disable=memory udev.children-max=2 panic=10 novmcoredd nokaslr
> aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
> ```
> 可以看到这个截取的更厉害。

`elfcorehdr_addr` 赋值在
```sh
static int __init setup_elfcorehdr(char *arg)
{
	char *end;
	if (!arg)
		return -EINVAL;
	elfcorehdr_addr = memparse(arg, &end);
	if (*end == '@') {
		elfcorehdr_size = elfcorehdr_addr;
		elfcorehdr_addr = memparse(end + 1, &end);
	}
	return end > arg ? 0 : -EINVAL;
}
early_param("elfcorehdr", setup_elfcorehdr);
```

`elfcorehhdr` 参数举例: `8K@3882992K`
* **8K**: elfcorehdr_size
* **3882992K**: elfcorehdr_addr

那很明显, `elfcorehdr` 被截断了, 获取不到`elfcorehdr_addr`.

## 被谁截断

那么问题来了，是被谁截断的呢?

**怀疑点1: 用户态**

调用下面bpftrace 脚本，获取`kexec_load` 执行时, cmdline segment  具体的值:

[bpftrace](./kexec_load.bpf)

输出如下:
```
=== do_kexec_load ===
  pid: 24540, comm: kexec
  entry: 0xae5cf000, nr_segments: 4, [CRASH]
  segment[2]: buf=0x5555581d8098 bufsz=4096 mem=0xb41fc000 memsz=16384
    [str @0]:    "kexec BOOT_IMAGE=/vmlinuz-6.6.0-jdcloud8.0.0cf662f0e6.loongarch64 ro loongarch_iommu=on iommu=pt iso"
    [str @100]:  "lcpus=40,52,51,43 spectre_v2=off nopti rdt=l3cat selinux=0 rd.lvm.lv=openeuler/root video=efifb:off "
    [str @200]:  "cgroup_disable=files apparmor=0 console=tty0 console=ttyS0,115200.allow_unsafe_interrupts=1 nokaslr."
    [str @300]:  "allow_unsafe_interrupts=1 nokaslr init 3 irqpoll reset_devices cgroup_disable=memory udev.children-m"
    [str @400]:  "ax=2 panic=10 novmcoredd nokaslr aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    [str @500]:  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa initrd=2973040640,48938496 elfcorehdr=8K@3882992K "
    [str @600]:  "mem=1048576K@2834432K"
    [str @700]:  ""
    [str @800]:  ""
    [str @800]:  ""
    [str @1000]: ""
    [str @1200]: ""
    [str @1400]: ""
    [str @1600]: ""
    [str @1800]: ""
    [str @2000]: ""
    [str @2200]: ""
    [str @2400]: ""
    [str @2600]: ""
    [str @2800]: ""
    [str @3000]: ""
    [str @3200]: ""
    [str @3400]: ""
    [str @3600]: ""
    [str @3800]: ""
```

可以发现没有什么问题，cmdline 非常完成.

**怀疑点2 : 内核态**

内核态流程:

```sh
kexec_load
=> do_kexec_load
   => machine_kexec_prepare
      ## 后续我没有看懂 怎么查找的 cmdline 的 segment index 我们直接看copy动作
      => copy_from_user(cmdline_ptr, kimage->segment[i].buf, COMMAND_LINE_SIZE))
				 => kimage->arch.cmdline_ptr = (unsigned long)cmdline_ptr;
```

而`COMMAND_LINE_SIZE` loongarch 在该内核版本没有明确定义，使用的是`include/uapi/asm-generic/setup.h`:
```cpp
#define COMMAND_LINE_SIZE	512
```

## 上游改动
```
From f7794a4d92ade518c813de69a01b27ca6d8d86f3 Mon Sep 17 00:00:00 2001
From: Ming Wang <wangming01@loongson.cn>
Date: Wed, 20 Aug 2025 22:23:16 +0800
Subject: [PATCH] LoongArch: Increase COMMAND_LINE_SIZE up to 4096
```

合入该改动后，再次测试，问题不再复现。

openeuler也已经合入该patch.

## 参考链接

1. [loongarch64: kexec support large cmdline (> 512)](https://gitcode.com/cai-fuqiang/kexec-tools_9436/commit/f51b3711ef84def6f1bede35baafa16db38a7e9e?ref=openEuler-24.03-LTS-SP4)

2. [\[OLK-6.6\]\[LoongArch\] Loongarch 下 命令行参数长度不足](https://gitcode.com/openeuler/kernel/issues/8609)
