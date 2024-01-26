# kexec-tools问题
在启用`crashkernel=128M,low crashkernel=1G, high`内核cmdline后,kdump服务起不来
```
~/.../kexec-tools# systemctl status kdump
● kdump.service - Crash recovery kernel arming
   Loaded: loaded (/usr/lib/systemd/system/kdump.service; enabled; vendor preset: enabled)
   Active: failed (Result: exit-code) since Thu 2023-11-16 23:57:40 CST; 3s ago
  Process: 59212 ExecStart=/usr/bin/kdumpctl start (code=exited, status=1/FAILURE)
 Main PID: 59212 (code=exited, status=1/FAILURE)

Nov 16 23:57:39 node-3.domain.tld dracut[59553]: *** Stripping files done ***
Nov 16 23:57:39 node-3.domain.tld dracut[59553]: *** Squashing the files inside the initramfs ***
Nov 16 23:57:40 node-3.domain.tld dracut[59553]: *** Squashing the files inside the initramfs done ***
Nov 16 23:57:40 node-3.domain.tld dracut[59553]: *** Creating image file '/boot/initramfs-4.18.0-372.19.1.el8_3.wang3.aarch64kdump.img' ***
Nov 16 23:57:40 node-3.domain.tld dracut[59553]: *** Creating initramfs image file '/boot/initramfs-4.18.0-372.19.1.el8_3.wang3.aarch64kdump.img' done ***
Nov 16 23:57:40 node-3.domain.tld kdumpctl[59212]: kdump: kexec: failed to load kdump kernel
Nov 16 23:57:40 node-3.domain.tld kdumpctl[59212]: kdump: Starting kdump: [FAILED]
Nov 16 23:57:40 node-3.domain.tld systemd[1]: kdump.service: Main process exited, code=exited, status=1/FAILURE
Nov 16 23:57:40 node-3.domain.tld systemd[1]: kdump.service: Failed with result 'exit-code'.
Nov 16 23:57:40 node-3.domain.tld systemd[1]: Failed to start Crash recovery kernel arming.
```

修改`kdumpctl`脚本加了一些调试,执行`/usr/bin/kdumpctl restart`, 出现下面报错
```
~/.../kexec-tools# /usr/bin/kdumpctl restart
kdump: kexec: unloaded kdump kernel
kdump: Stopping kdump: [OK]
+ /sbin/kexec -s -d -p '--command-line=BOOT_IMAGE=(hd0,gpt3)/vmlinuz-4.18.0-372.19.1.el8_3.wang3.aarch64 ro transparent_hugepage=never cgroup.memory=nokmem iommu.passthrough=1 ixgbe.allow_unsupported_sfp=1 biosdevname=0 rootdelay=90 nomodeset intel_idle.max_cstate=0 processor.max_cstate=0 net.ifnames=1 console=tty0 selinux=0 memblock=debug efi=debug irqpoll nr_cpus=1 reset_devices cgroup_disable=memory udev.children-max=2 panic=10 swiotlb=noforce novmcoredd sbsa_gwdt.pretimeout=0' --initrd=/boot/initramfs-4.18.0-372.19.1.el8_3.wang3.aarch64kdump.img /boot/vmlinuz-4.18.0-372.19.1.el8_3.wang3.aarch64
realloc(): invalid pointer
/usr/bin/kdumpctl: line 672: 66179 Aborted                 (core dumped) $KEXEC $KEXEC_ARGS $standard_kexec_args --command-line="$KDUMP_COMMANDLINE" --initrd=$TARGET_INITRD $KDUMP_KERNEL
+ ret=134
+ set +x
```

发现为kexec工具出发abort了, (有coredump可以看, 但是比较简单,我们暂不分析), 
我们直接看代码, 代码流程为:

```cpp
int main()
{
        ...
        if (do_load &&
            ((kexec_flags & KEXEC_ON_CRASH) ||
             (kexec_file_flags & KEXEC_FILE_ON_CRASH)) &a
             //该函数
            !is_crashkernel_mem_reserved()) {
                die("Memory for crashkernel is not reserved\n"
                    "Please reserve memory by passing"
                    "\"crashkernel=X@Y\" parameter to kernel\n"
                    "Then try to loading kdump kernel\n");
        }
        ...
}


int is_crashkernel_mem_reserved(void)
{
        //如果没有获取过,获取一下
        if (!usablemem_rgns.size)
                kexec_iomem_for_each_line(NULL, iomem_range_callback, NULL);

        return crash_reserved_mem.start != crash_reserved_mem.end;
}


int kexec_iomem_for_each_line(char *match,
                              int (*callback)(void *data,
                                              int nr,
                                              char *str,
                                              unsigned long long base,
                                              unsigned long long length),
                              void *data)
{
        //这里返回 "/proc/iomem"
        const char *iomem = proc_iomem();
        char line[MAX_LINE];
        FILE *fp;
        unsigned long long start, end, size;
        char *str;
        int consumed;
        int count;
        int nr = 0, ret;
        //只读打开
        fp = fopen(iomem, "r");
        if (!fp)
                die("Cannot open %s\n", iomem);
        //获取每一行
        while(fgets(line, sizeof(line), fp) != 0) {
                //解析 start en , 以及str的位置, 
                //例如 78000000-7fffffff : Crash kernel, 
                //consumed 返回Crash kernel 所在的位置
                count = sscanf(line, "%llx-%llx : %n", &start, &end, &consumed);
                if (count != 2)
                        continue;
                str = line + consumed;
                size = end - start + 1;
                if (!match || memcmp(str, match, strlen(match)) == 0) {
                        if (callback) {
                                //匹配,则调用callback
                                ret = callback(data, nr, str, start, size);
                                if (ret < 0)
                                        break;
                                else if (ret == 0)
                                        nr++;
                        }
                }
        }

        fclose(fp);

        return nr;
}

static int iomem_range_callback(void *UNUSED(data), int UNUSED(nr),
                                char *str, unsigned long long base,
                                unsigned long long length)
{
        //为这个路径
        if (strncmp(str, CRASH_KERNEL, strlen(CRASH_KERNEL)) == 0)
                return mem_regions_alloc_and_add(&usablemem_rgns,
                                                base, length, RANGE_RAM);
        else if (strncmp(str, SYSTEM_RAM, strlen(SYSTEM_RAM)) == 0)
                return mem_regions_alloc_and_add(&system_memory_rgns,
                                                base, length, RANGE_RAM);
        else if (strncmp(str, KERNEL_CODE, strlen(KERNEL_CODE)) == 0)
                elf_info.kern_paddr_start = base;
        else if (strncmp(str, KERNEL_DATA, strlen(KERNEL_DATA)) == 0)
                elf_info.kern_size = base + length - elf_info.kern_paddr_start;

        return 0;
}

int mem_regions_alloc_and_add(struct memory_ranges *ranges,
                              unsigned long long base,
                              unsigned long long length, int type)
{
        void *new_ranges;
        //#define KEXEC_MEMORY_RANGES 16
        //如果 ranges 的size > max_size, 则调用realloc 重新分配一个数组,
        //大小为原来的max_size 之上 +  KEXEC_MEMORY_RANGES(16)
        if (ranges->size >= ranges->max_size) {
                new_ranges = realloc(ranges->ranges,
                                sizeof(struct memory_range) *
                                (ranges->max_size + KEXEC_MEMORY_RANGES));
                if (!new_ranges)
                        return -1;

                ranges->ranges = new_ranges;
                ranges->max_size += KEXEC_MEMORY_RANGES;
        }

        return mem_regions_add(ranges, base, length, type);
}
```

而`usablemem_rgns`为
```cpp
struct memory_ranges usablemem_rgns = {
        .size = 0,
        .max_size = 1,
        .ranges = &crash_reserved_mem,
};
```
是一个数据段的变量(非malloc出来的), 所以realloc会有问题.

其根本原因是, 当前`kexec-tools`不支持两个`Crash kernel`区间
```
~# cat /proc/iomem |grep Crash
78000000-7fffffff : Crash kernel
  205fc0000000-205fffffffff : Crash kernel
```

> NOTE
>
> 该变量只有在kexec 不加-s参数时, 才会用到,主要的流程是,
> kexec初始化 segment, dtb, elfheader时候会用,在添加 -s 参数时, kexec会调用
> `kexec_file_load()`函数, 有kernel  初始化segment, dtb, elfheader, 所以
> 该成员的数据有问题,也没有关系.
>
> 该calloc() 是由
>
> ```
> kexec-tools-2.0.20-kexec-add-variant-helper-functions-for-handling-memo.patch
> kexec-tools-2.0.20-arm64-kdump-deal-with-a-lot-of-resource-entries-in-p.patch
> ```
> 引入, 我们看下合入patch之前的代码
>
> ```cpp
>  static int iomem_range_callback(void *UNUSED(data), int UNUSED(nr),
> @@ -90,11 +85,11 @@ static int iomem_range_callback(void *UNUSED(data), int UNUSED(nr),
>                                 unsigned long long length)
>  {
>         if (strncmp(str, CRASH_KERNEL, strlen(CRASH_KERNEL)) == 0)
> -               return mem_regions_add(&usablemem_rgns,
> -                                      base, length, RANGE_RAM);
> +               return mem_regions_alloc_and_add(&usablemem_rgns,
> +                                               base, length, RANGE_RAM);
>         else if (strncmp(str, SYSTEM_RAM, strlen(SYSTEM_RAM)) == 0)
> -               return mem_regions_add(&system_memory_rgns,
> -                                      base, length, RANGE_RAM);
> +               return mem_regions_alloc_and_add(&system_memory_rgns,
> +                                               base, length, RANGE_RAM);
>         else if (strncmp(str, KERNEL_CODE, strlen(KERNEL_CODE)) == 0)
>                 elf_info.kern_paddr_start = base;
>         else if (strncmp(str, KERNEL_DATA, strlen(KERNEL_DATA)) == 0)
>
> int mem_regions_add(struct memory_ranges *ranges, unsigned long long base,
>                     unsigned long long length, int type)
> {
>         struct memory_range *range;
> 
>         if (ranges->size >= ranges->max_size)
>                 return -1;
> 
>         range = ranges->ranges + ranges->size++;
>         range->start = base;
>         range->end = base + length - 1;
>         range->type = type;
> 
>         return 0;
> }
> ```
>
> 可以看到, 多次调用时, 则会覆盖之前的值, 对 `kexec -s ` 没有影响

# 解决方法
上游 patch 解决了这个问题

https://git.kernel.org/pub/scm/utils/kernel/kexec/kexec-tools.git/commit/kexec?id=b5a34a20984c4ad27cc5054d9957af8130b42a50

经讨论,决定升级`kexec-tools`工具版本解决.
在红帽官网找到下一个版本的kexec-tools: `2.0.24-6.el8`, 经查看代码+测试,发现可以满足要求
