## 大页支持

目前`loongarch64` 仅支持32MB.

代码如下(以openeuler上游代码为例):

初始化代码:

```sh
hugetlb_init
## 这里会现将`HUGETLB_PAGE_ORDER` 加到hstate
|-> hugetlb_add_hstate(HUGETLB_PAGE_ORDER);
```

而如果我们配置了`hugepagesz=xxx`, 则会走到
```sh
hugepagesz_setup
## 先判断该size 在架构上是否合法 ==(1)==
|-> if !(arch_hugetlb_valid_size(size))
    ## 不合法直接报错
    |-> pr_err("HugeTLB: unsupported hugepagesz=%s\n", s);
    |-> return 1
## 在 hstate 中查找该size的对应的hstate是否已经初始化.

|-> h = size_to_hstate(size);
|-> if h
    |-> if (!parsed_default_hugepagesz ||  h != &default_hstate ||
              default_hstate.max_huge_pages)
        |-> pr_warn("HugeTLB: hugepagesz=%s specified twice, ignoring\n", s);
        |-> return 1
    |-> parsed_hstate = h
    |-> parsed_valid_hugepagesz = true
|-> hugetlb_add_hstate(ilog2(size) - PAGE_SHIFT)
|-> parsed_valid_hugepagesz = true
```
1. `arch_hugetlb_valid_size()`(loongarch64没有定义)
   ```cpp
   //===FILE mm/hugetlb.c===
   bool __init __attribute((weak)) arch_hugetlb_valid_size(unsigned long size)
   {
        return size == HPAGE_SIZE;
   }
   ```
   那说明`loongarch` 只支持一个 `hugepagesz`

   `HPAGE_SHIFT` 定义比较奇怪:
   ```cpp
   #define HPAGE_SHIFT  (PAGE_SHIFT + PAGE_SHIFT - 3)
   ```
   `PAGE_SHIFT * 2 - 3`

   而在openeuler 2403 sp2 配置中, `PAGE_SIZE` 为
   ```sh
   [root@localhost ~]# cat /boot/config-6.6.0-97.0.0.102.oe2403sp2.loongarch64 |grep PAGE |grep SI
   CONFIG_PAGE_SIZE_16KB=y
   ```
   16KB, 根据下面定义`PAGE_SHIFT 为 14`
   ```cpp
   #ifdef CONFIG_PAGE_SIZE_4KB
   #define PAGE_SHIFT  12
   #endif
   #ifdef CONFIG_PAGE_SIZE_16KB
   #define PAGE_SHIFT  14
   #endif
   #ifdef CONFIG_PAGE_SIZE_64KB
   #define PAGE_SHIFT  16
   #endif
   ```

   `HPAGE_SHIFT = 14 * 2 - 3 = 25`

   **大页大小为32M**
2. 这里简单介绍下这快逻辑
   + 如果`parsed_default_hugepagesz` 不为 0，但是还是找到了hstate，
     说明之前通过`hugepagesz` 配置过该size，属于重复设置.
   + 如果`h != &default_hstate`, 同上，只不过是配置了`default_hugepagesz`,
     这里发现两者不想等，说明还是`hugepagesz` 之前配置过.
   + `default_hstate.max_huge_pages` ??? **TODO**

所以需要将虚拟机大页修改为:

```xml
<currentMemory unit='KiB'>16777216</currentMemory>
<memoryBacking>
  <hugepages>
    <page size='32' unit='MiB'/>
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  </hugepages>
</memoryBacking>
```
kernel cmdline修改为:
```
hugepagesz=32MB hugepages=xxxx
```
包含OVS 和 虚拟机大页.

## cpu model 修改

loongarch64 不支持 `host-passthrough`, 需要修改成`la464`

```xml
  <cpu mode='custom' match='exact' check='none'>
    <model fallback='forbid'>la464</model>
    <numa>
      <cell id='0' cpus='0-7' memory='16777216' unit='KiB' memAccess='shared'/>
    </numa>
  </cpu>
```

## 其他修改

修改架构以及BIOS路径
```xml
<domain>
 <metadata>
   <arch>loongarch64</arch>
         ^^^^^^^^^^^
 </metadata>
 <os>
  <type arch='loongarch64' machine='virt'>hvm</type>
              ^^^^^^^^^^^           ^^^^
  <loader readonly='yes' type='pflash'>/usr/share/edk2/loongarch64/QEMU_EFI-silent-pflash.raw</loader>
                                       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  <nvram>/var/lib/libvirt/qemu/nvram/wang_VARS.fd</nvram>
  <smbios mode='sysinfo'/>
 </os>
 <devices>
   <emulator>/usr/bin/qemu-system-loongarch64</emulator>
 </devices>
</domain>
```
