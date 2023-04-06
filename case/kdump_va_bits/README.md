# 问题现象
在kernel 版本为 为`4.18.0-372.19.1.es8_2.aarch64`, `kexec-tools`版本为`2.0.20-46`
时，触发panic，会出现无法生成`vmcore`和`vmcore-dmesg.txt`的问题，报错信息如下:
```
Mar 24 15:42:10 node-1.domain.tld kdump[687]: Kdump is using the default log level(3).
Mar 24 15:42:10 node-1.domain.tld kdump[714]: saving to /sysroot//var/crash/127.0.0.1-2023-03-24-15:42:10/
Mar 24 15:42:10 node-1.domain.tld kdump[718]: saving vmcore-dmesg.txt to /sysroot//var/crash/127.0.0.1-2023-03-24-15:42:10/
Mar 24 15:42:10 node-1.domain.tld kdump.sh[674]: Missing the log_buf symbol
Mar 24 15:42:10 node-1.domain.tld kdump[721]: saving vmcore-dmesg.txt failed
Mar 24 15:42:10 node-1.domain.tld kdump[723]: saving vmcore
Mar 24 15:42:10 node-1.domain.tld kdump.sh[674]: calculate_plat_config: PAGE SIZE 0x10000 and VA Bits 47 not supported
Mar 24 15:42:10 node-1.domain.tld kdump.sh[674]: get_machdep_info_arm64: Can't determine platform config values
Mar 24 15:42:10 node-1.domain.tld kdump.sh[674]: makedumpfile Failed.
Mar 24 15:42:10 node-1.domain.tld kdump[726]: saving vmcore failed, _exitcode:1
```
# 问题分析
## 代码分析
### 报错代码
报错函数的打印如下:
```cpp
static int calculate_plat_config(void)
{
        /* derive pgtable_level as per arch/arm64/Kconfig */
        if ((PAGESIZE() == SZ_16K && va_bits == 36) ||
                        (PAGESIZE() == SZ_64K && va_bits == 42)) {
                pgtable_level = 2;
        } else if ((PAGESIZE() == SZ_64K && va_bits == 48) ||
                        (PAGESIZE() == SZ_4K && va_bits == 39) ||
                        (PAGESIZE() == SZ_16K && va_bits == 47)) {
                pgtable_level = 3;
        } else if ((PAGESIZE() != SZ_64K && va_bits == 48)) {
                pgtable_level = 4;
        } else {
                ERRMSG("PAGE SIZE %#lx and VA Bits %d not supported\n", //<-----报错打印
                                PAGESIZE(), va_bits);
                return FALSE;
        }

        return TRUE;
}
```
可见，会将va_bits和PAGESIZE()结合做合法性检验，如果满足上面的这些条件，
则会认为这个vmcore中的信息不对(和真实硬件允许的配置对不上)，从而报错。
（我们会在附1中，讲述下 arm64 VA_BITS和PAGESIZE之间的关系)

### va_bits 获取
代码路径:
```
main
  create_dumpfile
    initial
      get_machdep_info (#define get_machdep_info() get_machdep_info_arm64())
         get_versiondep_info_arm64
```
我们主要来看下`get_versiondep_info_arm64()`是怎么获取
```cpp
int
get_versiondep_info_arm64(void)
{
        ulong _stext;
	//===============(1)=================
        _stext = get_stext_symbol();
        if (!_stext) {
                ERRMSG("Can't get the symbol of _stext.\n");
                return FALSE;
        }
	//===============(2)==================
        /* Derive va_bits as per arch/arm64/Kconfig */
        if ((_stext & PAGE_OFFSET_36) == PAGE_OFFSET_36) {
                va_bits = 36;
        } else if ((_stext & PAGE_OFFSET_39) == PAGE_OFFSET_39) {
                va_bits = 39;
        } else if ((_stext & PAGE_OFFSET_42) == PAGE_OFFSET_42) {
                va_bits = 42;
        } else if ((_stext & PAGE_OFFSET_47) == PAGE_OFFSET_47) {
                va_bits = 47;
        } else if ((_stext & PAGE_OFFSET_48) == PAGE_OFFSET_48) {
                va_bits = 48;
        } else {
                ERRMSG("Cannot find a proper _stext for calculating VA_BITS\n");
                return FALSE;
        }

        info->page_offset = (0xffffffffffffffffUL) << (va_bits - 1);

        DEBUG_MSG("va_bits      : %d\n", va_bits);
        DEBUG_MSG("page_offset  : %lx\n", info->page_offset);

        return TRUE;
}
#define PAGE_OFFSET_36          ((0xffffffffffffffffUL) << 36)
```
1. 获取`_stext` symbol的地址(从/proc/kallsyms获取)
2. 看看`_stext`地址是否满足PAGE_OFFSET_36，然后再看`39, 42, 47, 48`从小到大，
为什么要这样做呢，假如说一个地址`0xffff fff0 0000 0000`, 这个地址是36 bits, 
但是这个地址按照上面的逻辑计算，`address & PAGE_OFFSET_39`也会是真，但如果
反过来，一个地址为`0xffff 0000 0000 0000`, 这个地址是48 bits, 但是`address &
PAGE_OFFSET_36`，就会是false，所以要按照这样的方式计算。

> NOTE
>
> 我们之前分析过x86中的有效地址，假如说为虚拟地址为48bits, 合法的地址空间为
> [0x0~ 0x0000 7fff ffff ffff] 和[0xffff 8000 0000 0000], 但是看似arm64
> 不满足这一要求, 关于这部分，也会在附1 中说明

在环境中查看`_stext`的地址，为:
```
ffff800010081000 T _stext
```
通过上面的计算，满足`PAGE_OFFSET_47`, 而kernel的配置为:
```
CONFIG_ARM64_VA_BITS=48
```
所以，是makedumpfile工具获取的有问题。

那我们来看下最新的代码有没有什么改动:

### upstream va_bits 获取代码
```cpp
int
get_versiondep_info_arm64(void)
{
	//================(1)========================
        if (NUMBER(VA_BITS) != NOT_FOUND_NUMBER) {
                va_bits = NUMBER(VA_BITS);
                DEBUG_MSG("va_bits      : %d (vmcoreinfo)\n", va_bits);
        } else if (get_va_bits_from_stext_arm64() == FALSE) {
                ERRMSG("Can't determine va_bits.\n");
                return FALSE;
        }
	...
}
```
1. 这里先通过`NUMBER(VA_BITS)`
2. 如果获取不到, 则会调用`get_va_bits_from_stext_arm64()`获取

```cpp
static int
get_va_bits_from_stext_arm64(void)
{
        ulong _stext;
	
        _stext = get_stext_symbol();
        if (!_stext) {
                ERRMSG("Can't get the symbol of _stext.\n");
                return FALSE;
        }

        /*
        ¦* Derive va_bits as per arch/arm64/Kconfig. Note that this is a
        ¦* best case approximation at the moment, as there can be
        ¦* inconsistencies in this calculation (for e.g., for 52-bit
        ¦* kernel VA case, the 48th bit is set in * the _stext symbol).
        ¦*/
	//========================(1)===============================
        if ((_stext & PAGE_OFFSET_48) == PAGE_OFFSET_48) {
                va_bits = 48;
        } else if ((_stext & PAGE_OFFSET_47) == PAGE_OFFSET_47) {
                va_bits = 47;
        } else if ((_stext & PAGE_OFFSET_42) == PAGE_OFFSET_42) {
                va_bits = 42;
        } else if ((_stext & PAGE_OFFSET_39) == PAGE_OFFSET_39) {
                va_bits = 39;
        } else if ((_stext & PAGE_OFFSET_36) == PAGE_OFFSET_36) {
                va_bits = 36;
        } else {
                ERRMSG("Cannot find a proper _stext for calculating VA_BITS\n");
                return FALSE;
        }

        DEBUG_MSG("va_bits       : %d (guess from _stext)\n", va_bits);

        return TRUE;
}
```
> NOTE
>
> 这个地方修改了顺序，从有小到大，变成了有大到小，这里提到，在52-bit 情况下，可能获取到的_stext 
> symbol是48th的。（这部分改动没太看懂，不知道为什么要调换顺序)

该改动的patch如下, 

```
commit 67d0e1d68f28c567a704fd6b9b8fd696ad3df183
Author: Kazuhito Hagio <k-hagio-ab@nec.com>
Date:   Fri Jan 29 11:40:24 2021 +0900

[PATCH 2/3] arm64: Make use of NUMBER(VA_BITS) in vmcoreinfo

Make use of the NUMBER(VA_BITS) in vmcoreinfo, which was added by
kernel commit 20a166243328 (Linux 4.12 and later kernels), as the
current way of guessing VA_BITS does not work on Linux 5.4 and
later kernels.
```

主要是为了引入`NUMBER(VA_BITS)`, 这里提到了kernel的一个commit，我们来看下:

### KERNEL NUMBER(VA_BITS) code
```cpp
commit 20a166243328c14a0c24bd8c7919223ab4174917
Author: AKASHI Takahiro <takahiro.akashi@linaro.org>
Date:   Mon Apr 3 11:24:37 2017 +0900

    arm64: kdump: add VMCOREINFO's for user-space tools

    In addition to common VMCOREINFO's defined in
    crash_save_vmcoreinfo_init(), we need to know, for crash utility,
      - kimage_voffset
      - PHYS_OFFSET
    to examine the contents of a dump file (/proc/vmcore) correctly
    due to the introduction of KASLR (CONFIG_RANDOMIZE_BASE) in v4.6.

      - VA_BITS
    is also required for makedumpfile command.

    arch_crash_save_vmcoreinfo() appends them to the dump file.
    More VMCOREINFO's may be added later.

diff --git a/arch/arm64/kernel/machine_kexec.c b/arch/arm64/kernel/machine_kexec.c
index 779a80046066..481f54a866c5 100644
--- a/arch/arm64/kernel/machine_kexec.c
+++ b/arch/arm64/kernel/machine_kexec.c
@@ -18,6 +18,7 @@

 #include <asm/cacheflush.h>
 #include <asm/cpu_ops.h>
+#include <asm/memory.h>
 #include <asm/mmu.h>
 #include <asm/mmu_context.h>
 #include <asm/page.h>
@@ -351,3 +352,13 @@ void crash_free_reserved_phys_range(unsigned long begin, unsigned long end)
        }
 }
 #endif /* CONFIG_HIBERNATION */
+
+void arch_crash_save_vmcoreinfo(void)
+{
+       VMCOREINFO_NUMBER(VA_BITS);
+       /* Please note VMCOREINFO_NUMBER() uses "%d", not "%x" */
+       vmcoreinfo_append_str("NUMBER(kimage_voffset)=0x%llx\n",
+                                               kimage_voffset);
+       vmcoreinfo_append_str("NUMBER(PHYS_OFFSET)=0x%llx\n",
+                                               PHYS_OFFSET);
+}
```
`VMCOREINFO_NUMBER()`的定义:
```
#define VMCOREINFO_NUMBER(name) \
        vmcoreinfo_append_str("NUMBER(%s)=%ld\n", #name, (long)name)
```

大概意思是为了支持KASLR, 所以需要将一些信息传出去, 有`kimage_offset`,
`PHYS_OFFSET`, 以及上面提到的`VA_BITS`

而`VA_BITS`的定义为:
```cpp
#define VA_BITS                 (CONFIG_ARM64_VA_BITS)
```

可以看到该信息是存在了vmcore中。

我们回过头来看下, makedumpfile 关于NUMBER(VA_BITS)的一些代码:

### makedumpfile NUMBER(VA_BITS)
```cpp
#define NUMBER(X)               (number_table.X)
int
read_vmcoreinfo(void)
{
	...
	READ_NUMBER("VA_BITS", VA_BITS);
	...

}
#define READ_NUMBER(str_number, number) \
do { \
        if (NUMBER(number) == NOT_FOUND_NUMBER) { \
                NUMBER(number) = read_vmcoreinfo_long(STR_NUMBER(str_number)); \
                if (NUMBER(number) == INVALID_STRUCTURE_DATA) \
                        return FALSE; \
        } \

#define STR_NUMBER(X)           "NUMBER("X")="
long
//=======================(3)=====================
read_vmcoreinfo_long(char *str_structure)
{
        long data = NOT_FOUND_LONG_VALUE;
        char buf[BUFSIZE_FGETS], *endp;
        unsigned int i;
	//=================(1)===================
        if (fseek(info->file_vmcoreinfo, 0, SEEK_SET) < 0) {
                ERRMSG("Can't seek the vmcoreinfo file(%s). %s\n",
                ¦   info->name_vmcoreinfo, strerror(errno));
                return INVALID_STRUCTURE_DATA;
        }
	//=================(2)===================
        while (fgets(buf, BUFSIZE_FGETS, info->file_vmcoreinfo)) {
                i = strlen(buf);
                if (!i)
                        break;
                if (buf[i - 1] == '\n')
                        buf[i - 1] = '\0';
		//==================(3)===============================
                if (strncmp(buf, str_structure, strlen(str_structure)) == 0) {
                        data = strtol(buf + strlen(str_structure), &endp, 10);
                        if (strlen(endp) != 0)
                                data = strtol(buf + strlen(str_structure), &endp, 16)
                        if ((data == LONG_MAX) || strlen(endp) != 0) {
                                ERRMSG("Invalid data in %s: %s",
                                ¦   info->name_vmcoreinfo, buf);
                                return INVALID_STRUCTURE_DATA;
                        }
                        break;
                }
        }
        return data;
}
```
1. `info->file_vmcoreinfo`并不是`/proc/vmcore`,而是copy到/tmp/下的一个副本（内容就是`/proc/vmcore`)
2. 读取该文件的每一行
3. 将获取的行数和`str_structure`进行比较，注意这里的大小就是`strlen(str_structure)`, 如果是上面路径的话，
该变量为`NUMBER("VA_BITS")=`, 所以正好匹配kernel那边的格式.


那现在来看，高版本的`makedumpfile` + 高版本的kernel, 可以通过从vmcore中获取`VA_BITS`相关信息, 不再通过
`_stext`符号来看了。那我们来再来分析下，`4.18.0-147 kernel` + `2.0.20-46 kexec-tools`为什么没有问题。

### kernel 4.18.0-147 && kernel 4.18.0-372
其实主要是`_stext`符号的地址改变了:
* 147 kernel
```
[root@node-2 ~]# cat /proc/kallsyms |grep _stext
ffff000010081000 T _stext
```
* 372 kernel
```
[root@node-1 ~]# cat /proc/kallsyms  |grep _stext
ffff800010081000 T _stext
```
可以看到372 kernel 的_stext symbol address， 第47 bits确实为1。

该改动在下面的patch引入:

### kernel patch : change _stext symbol address
commit 信息:
```
commit 14c127c957c1c6070647c171e72f06e0db275ebf
Author: Steve Capper <steve.capper@arm.com>
Date:   Wed Aug 7 16:55:14 2019 +0100

    arm64: mm: Flip kernel VA space

    In order to allow for a KASAN shadow that changes size at boot time, one
    must fix the KASAN_SHADOW_END for both 48 & 52-bit VAs and "grow" the
    start address. Also, it is highly desirable to maintain the same
    function addresses in the kernel .text between VA sizes. Both of these
    requirements necessitate us to flip the kernel address space halves s.t.
    the direct linear map occupies the lower addresses.

    This patch puts the direct linear map in the lower addresses of the
    kernel VA range and everything else in the higher ranges.

    We need to adjust:
     *) KASAN shadow region placement logic,
     *) KASAN_SHADOW_OFFSET computation logic,
     *) virt_to_phys, phys_to_virt checks,
     *) page table dumper.

    These are all small changes, that need to take place atomically, so they
    are bundled into this commit.

    As part of the re-arrangement, a guard region of 2MB (to preserve
    alignment for fixed map) is added after the vmemmap. Otherwise the
    vmemmap could intersect with IS_ERR pointers.
```
该commit 是为了支持KASAN shadow, 能让其在启动的时候去选择size(可能是下面场景:
配置了52-bits, 但是machine只支持48-bits， 这时候应该选择48-bits),
但是不同的va_bits, KASAN shadow 的size不同（kasan_shadow_area_size = va_size / 8), 
所以他想把 `KASAN_SHADOW_END`固定住，然后这样, kernel function address也固定住了, 
让 kasan shadow 低地址调整，那么就需要把 `KASAN_SHADOW_END` 拉高，继而拉高了
kernel function address。下面是部分patch diff:
```cpp
--- a/arch/arm64/Makefile
+++ b/arch/arm64/Makefile
@@ -130,7 +130,7 @@ KBUILD_AFLAGS += -DKASAN_SHADOW_SCALE_SHIFT=$(KASAN_SHADOW_SCALE_SHIFT)
 #                               - (1 << (64 - KASAN_SHADOW_SCALE_SHIFT))
 # in 32-bit arithmetic
 KASAN_SHADOW_OFFSET := $(shell printf "0x%08x00000000\n" $$(( \
-       (0xffffffff & (-1 << ($(CONFIG_ARM64_VA_BITS) - 32))) \
+       (0xffffffff & (-1 << ($(CONFIG_ARM64_VA_BITS) - 1 - 32))) \
        + (1 << ($(CONFIG_ARM64_VA_BITS) - 32 - $(KASAN_SHADOW_SCALE_SHIFT))) \
        - (1 << (64 - 32 - $(KASAN_SHADOW_SCALE_SHIFT))) )) )
diff --git a/arch/arm64/include/asm/memory.h b/arch/arm64/include/asm/memory.h
index 98fda92a2612..380594b1a0ba 100644
--- a/arch/arm64/include/asm/memory.h
+++ b/arch/arm64/include/asm/memory.h
@@ -38,9 +38,9 @@
  */
 #define VA_BITS                        (CONFIG_ARM64_VA_BITS)
 #define VA_START               (UL(0xffffffffffffffff) - \
-       (UL(1) << VA_BITS) + 1)
-#define PAGE_OFFSET            (UL(0xffffffffffffffff) - \
        (UL(1) << (VA_BITS - 1)) + 1)
+#define PAGE_OFFSET            (UL(0xffffffffffffffff) - \
+       (UL(1) << VA_BITS) + 1)

-#define VMEMMAP_START          (PAGE_OFFSET - VMEMMAP_SIZE)
+#define VMEMMAP_START          (-VMEMMAP_SIZE - SZ_2M)

-#define __is_lm_address(addr)  (!!((addr) & BIT(VA_BITS - 1)))
+#define __is_lm_address(addr)  (!((addr) & BIT(VA_BITS - 1)))

-#define VMALLOC_END            (PAGE_OFFSET - PUD_SIZE - VMEMMAP_SIZE - SZ_64K)
+#define VMALLOC_END            (- PUD_SIZE - VMEMMAP_SIZE - SZ_64K)

diff --git a/arch/arm64/mm/dump.c b/arch/arm64/mm/dump.c
index 82b3a7fdb4a6..beec87488e97 100644
--- a/arch/arm64/mm/dump.c
+++ b/arch/arm64/mm/dump.c
@@ -26,6 +26,8 @@
 #include <asm/ptdump.h>

 static const struct addr_marker address_markers[] = {
+       { PAGE_OFFSET,                  "Linear Mapping start" },
+       { VA_START,                     "Linear Mapping end" },
 #ifdef CONFIG_KASAN
        { KASAN_SHADOW_START,           "Kasan shadow start" },
        { KASAN_SHADOW_END,             "Kasan shadow end" },
@@ -42,7 +44,6 @@ static const struct addr_marker address_markers[] = {
        { VMEMMAP_START,                "vmemmap start" },
        { VMEMMAP_START + VMEMMAP_SIZE, "vmemmap end" },
 #endif
-       { PAGE_OFFSET,                  "Linear mapping" },
        { -1,                           NULL },
 };

```
这里`KASAN_SHADOW_START`是`kasan shadow`的起始地址。可以看到，在合入这个patch
之前，kernel 虚拟内存空间的开头部分，就是`kasan shadow`, 而patch合入后，
为`Linear Mapping start`, 这就给`kasan shadow`有了往低地址调整的空间。

我们来看下调整后的虚拟内存布局:
(来自Documentation/arm64/memory.rst)
* 48-bits
```
  Start                 End                     Size            Use
  -----------------------------------------------------------------------
  0000000000000000      0000ffffffffffff         256TB          user
  ffff000000000000      ffff7fffffffffff         128TB          kernel logical memory map
 [ffff600000000000      ffff7fffffffffff]         32TB          [kasan shadow region]
  ffff800000000000      ffff800007ffffff         128MB          modules
  ffff800008000000      fffffbffefffffff         124TB          vmalloc
  fffffbfff0000000      fffffbfffdffffff         224MB          fixed mappings (top down)
  fffffbfffe000000      fffffbfffe7fffff           8MB          [guard region]
  fffffbfffe800000      fffffbffff7fffff          16MB          PCI I/O space
  fffffbffff800000      fffffbffffffffff           8MB          [guard region]
  fffffc0000000000      fffffdffffffffff           2TB          vmemmap
  fffffe0000000000      ffffffffffffffff           2TB          [guard region]
```

* 52-bits
```

  Start                 End                     Size            Use
  -----------------------------------------------------------------------
  0000000000000000      000fffffffffffff           4PB          user
  fff0000000000000      ffff7fffffffffff          ~4PB          kernel logical memory map
 [fffd800000000000      ffff7fffffffffff]        512TB          [kasan shadow region]
  ffff800000000000      ffff800007ffffff         128MB          modules
  ffff800008000000      fffffbffefffffff         124TB          vmalloc
  fffffbfff0000000      fffffbfffdffffff         224MB          fixed mappings (top down)
  fffffbfffe000000      fffffbfffe7fffff           8MB          [guard region]
  fffffbfffe800000      fffffbffff7fffff          16MB          PCI I/O space
  fffffbffff800000      fffffbffffffffff           8MB          [guard region]
  fffffc0000000000      ffffffdfffffffff          ~4TB          vmemmap
  ffffffe000000000      ffffffffffffffff         128GB          [guard region]
```
这样调整后 `kasan shadow region end`固定在了`0xffff7fffffffffff`, 而vmalloc
start 也固定在了`ffff800008000000`。

> NOTE :
> 
> `kernel stext`在 `vmalloc`区域。

# 代码分析结论
通过上面的分析, 可以发现，在新版本kernel下，48 va_bits kernel, 52 va_bits
kernel，有相同的`_stext symbol address`, 在通过该方式获取va_bits信息已经不行了，
需要通过从vmcore中读取`NUMBER(VA_BITS)=xxx`字符串的方式获取。

# 纠结的问题
在`makedumpfile` commit `[PATCH 2/3] arm64: Make use of NUMBER(VA_BITS) in vmcoreinfo`, 
将判断的_stext的bits, 从`PAGE_OFFSET_xxx`从由低到高，改为有高到低，这样感觉不太对，
可能在低版本的kernel上会有问题(kernel 不支持 commit `arm64: kdump: add VMCOREINFO's 
for user-space tools`, 需要做进一步测试。
# kexec-tools版本
我们来看下，相关kexec-tools的版本支持情况:
|kexec-tools版本| 是否支持 通过NUMBER(VA_BITS)获取信息|
|----|----|
|2.0.20-60|N|
|2.0.20-63|Y|

# kernel版本
在4.18.0-147, 4.18.0-372 都已经支持patch`arm64: kdump: add VMCOREINFO's for user-space tools`

# 结论
建议将`kexec-tools`升级到`2.0.20-63`解决。

# 附
## 附 1. PS: VA_BITS && PAGESIZE
> NOTE
>
> 本文主要来自 arm64 sdm, `D8.1.9 Input address size configuration`

arm64 将VA region分为了 `lower VA range`和`upper VA ranges`

关于maximum VA size, 手册中这样描述:
> The maximum VA size is one of the following:
> * For a translation regime controlled by TCR_ELx, if the 4KB or 16KB 
> translation granule is used, then one of the following:
>	+ If the Effective value of TCR_ELx.DS is 0, then 48 bits.
>	+ If the Effective value of TCR_ELx.DS is 1, then 52 bits.
> * For any translation regime, if the 64KB translation granule
> is used, then one of the following:
>	+ If FEAT_LVA is not implemented, then 48 bits.
>	+ If FEAT_LVA is implemented, then 52 bits.
> 
> Only stage 1 address translation in the EL1&0 and EL2&0 translation
> regimes support two VA ranges.

前面主要是对page size和对应的va bits描述。

后面提到只有在stage 1 address translation 中可能会分两个VA range, stage 2
不会, 这里我觉得可能不会涉及这种场景，因为stage 1 的output address 一般不会
即可能为upper range, 又可能为lower range。(而且猜测用于translation的寄存器
应该是一个)。

假如说只支持single VA range，maximum VA range如下:
> * If the maximum VA size is 48 bits, then the maximum VA range is 0x0000000000000000 to 0x0000FFFFFFFFFFFF.
> * If the maximum VA size is 52 bits, then the maximum VA range is 0x0000000000000000 to 0x000FFFFFFFFFFFFF. 

从这里就可以看到和x86的va range不同之处，如果x86 是48-bits va_bits, lower
range情况下，第47-bits一定为0。

对于支持two VA range, 如下:
> If a translation regime supports two VA ranges, then all of the following apply:
> * IA bit[55] determines one of the following VA ranges:
>	+ If IA bit[55] is 0, the lower VA range is used.
> 	+ If IA bit[55] is 1, the upper VA range is used.
>
> * The maximum lower VA range is one of the following:
>
>	+ If the maximum VA size is 48 bits, then the maximum VA range
>	is 0x0000000000000000 to 0x0000FFFFFFFFFFFF.
>	+ If the maximum VA size is 52 bits, then the maximum VA range
>	is 0x0000000000000000 to 0x000FFFFFFFFFFFFF.
> 
> * The maximum upper VA range is one of the following:
>	+ If the maximum VA size is 48 bits, then the maximum VA 
>	range is 0xFFFF000000000000 to 0xFFFFFFFFFFFFFFFF.
>	+ If the maximum VA size is 52 bits, then the maximum VA 
>	range is 0xFFF0000000000000 to 0xFFFFFFFFFFFFFFFF.

从这里来看，arm64是通过 bit[55] 来判断是 lower range, 还是upper range,
而且这样的方式，会比x86寻址空间要多一些，是x86虚拟内存空间的2倍，假如说
是48bits，虚拟地址空间大小为 2 ^ 48 * 2 = 2 ^ 49, x86为 2 ^ 48

手册中还提到，如果upper VA range 小于 maximum, 高低值会固定在 `0xffff ffff ffff
ffff`, 调整低地址:

> If the configured size of the upper VA range is less than the maximum, then all of the following apply:
> * The range starting address is greater than the address used by the maximum VA size.
> * The range ending address is 0xFFFFFFFFFFFFFFFF.

48 & 52 bits 支持虚拟地址空间大小:
> A 48-bit VA range defines an address space of 256TB. A 52-bit VA range defines an address space of 4PB.
