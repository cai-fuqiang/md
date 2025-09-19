## 堆栈
```
crash> bt
PID: 318    TASK: ffff92637d862100  CPU: 0   COMMAND: "dracut-pre-moun"
 #0 [ffff9263fd72b998] panic at ffffffff8b97a9c7
 #1 [ffff9263fd72ba18] oops_end at ffffffff8b98b7b5
 #2 [ffff9263fd72ba40] no_context at ffffffff8b275d14
 #3 [ffff9263fd72ba90] __bad_area_nosemaphore at ffffffff8b275fe2
 #4 [ffff9263fd72bae0] bad_area_nosemaphore at ffffffff8b276104
 #5 [ffff9263fd72baf0] __do_page_fault at ffffffff8b98e750
 #6 [ffff9263fd72bb60] trace_do_page_fault at ffffffff8b98ea26
 #7 [ffff9263fd72bba0] do_async_page_fault at ffffffff8b98dfa2
 #8 [ffff9263fd72bbc0] async_page_fault at ffffffff8b98a7a8
    [exception RIP: load_image_lzo+2086]
    RIP: ffffffff8b2fe476  RSP: ffff9263fd72bc70  RFLAGS: 00010246
    RAX: 0000000000000000  RBX: 0000000000021c6b  RCX: 0000000000000200
    RDX: 0000000000001000  RSI: ffffb3f1c0a07058  RDI: ffff9263fe142000
    RBP: ffff9263fd72bd78   R8: ffff9263fd6f9288   R9: 0000000000000001
    R10: 0000000000000001  R11: ffffb3f1c0a1c058  R12: 000000000000b000
    R13: ffff9263fd72bd88  R14: ffffb3f1c09fc058  R15: ffffb3f1c09fc000
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
 #9 [ffff9263fd72bd80] swsusp_read at ffffffff8b2ffb2e
#10 [ffff9263fd72be10] software_resume at ffffffff8b2fae0e
#11 [ffff9263fd72be38] resume_store at ffffffff8b2fafd5
#12 [ffff9263fd72be70] kobj_attr_store at ffffffff8b588612
#13 [ffff9263fd72be80] sysfs_kf_write at ffffffff8b4db0b2
#14 [ffff9263fd72be90] kernfs_fop_write at ffffffff8b4da69b
#15 [ffff9263fd72bec8] vfs_write at ffffffff8b44db50
#16 [ffff9263fd72bf08] sys_write at ffffffff8b44e92f
#17 [ffff9263fd72bf50] system_call_fastpath at ffffffff8b993f92
    RIP: 00007f8ec046aba0  RSP: 00007fffae7dca78  RFLAGS: 00010202
    RAX: 0000000000000001  RBX: 0000000000000006  RCX: 0000000000000b48
    RDX: 0000000000000006  RSI: 00007f8ec0d95000  RDI: 0000000000000001
    RBP: 00007f8ec0d95000   R8: 000000000000000a   R9: 00007f8ec0d92740
    R10: 00007fffae7dbe60  R11: 0000000000000246  R12: 00007f8ec0743400
    R13: 0000000000000006  R14: 0000000000000001  R15: 0000000000000000
    ORIG_RAX: 0000000000000001  CS: 0033  SS: 002b
```

出错指令:
```
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1316
0xffffffff8b2fe458 <load_image_lzo+2056>:       test   $0x2,%dil   ## rdi寄存器低8位
0xffffffff8b2fe45c <load_image_lzo+2060>:       jne    0xffffffff8b2fe510 <load_image_lzo+2240>
0xffffffff8b2fe462 <load_image_lzo+2066>:       test   $0x4,%dil
0xffffffff8b2fe466 <load_image_lzo+2070>:       jne    0xffffffff8b2fe530 <load_image_lzo+2272>
0xffffffff8b2fe46c <load_image_lzo+2076>:       mov    %edx,%ecx
0xffffffff8b2fe46e <load_image_lzo+2078>:       xor    %eax,%eax
0xffffffff8b2fe470 <load_image_lzo+2080>:       shr    $0x3,%ecx
0xffffffff8b2fe473 <load_image_lzo+2083>:       test   $0x4,%dl
0xffffffff8b2fe476 <load_image_lzo+2086>:       rep movsq %ds:(%rsi),%es:(%rdi) <---
```

dmesg 其他打印:
```
## 用一个thread 解压，共需要解压229471 pages
[    6.993806] PM: Using 1 thread(s) for decompression.
               PM: Loading and decompressing image data (229471 pages)...
[    8.566392] PM: Image loading progress:   0%
[    9.195822] PM: Image loading progress:  10%
[    9.479823] random: crng init done
[   10.721742] PM: Image loading progress:  20%
[   12.975760] PM: Image loading progress:  30%
[   17.266492] PM: Image loading progress:  40%
[   19.989404] PM: Image loading progress:  50%
## 解压到60% 挂了
[   21.148271] PM: Image loading progress:  60%
```

## 其他变量获取
```

## 获取page
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1089
0xffffffff8b2fdcc1 <load_image_lzo+113>:        callq  0xffffffff8b406720 <vmalloc>
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1090
0xffffffff8b2fdcc6 <load_image_lzo+118>:        test   %rax,%rax
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1089
0xffffffff8b2fdcc9 <load_image_lzo+121>:        mov    %rax,%rbx <---rbx 为page
...
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1238
0xffffffff8b2fe733 <load_image_lzo+2787>:       test   %eax,%eax
0xffffffff8b2fe735 <load_image_lzo+2789>:       jne    0xffffffff8b2fe7cc <load_image_lzo+2940>
0xffffffff8b2fe73b <load_image_lzo+2795>:       mov    -0x100(%rbp),%rax
0xffffffff8b2fe742 <load_image_lzo+2802>:       mov    %r12d,-0x74(%rbp) 
0xffffffff8b2fe746 <load_image_lzo+2806>:       mov    %rbx,-0xb0(%rbp) ---> 放入堆栈

## 获取data

/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1096
0xffffffff8b2fdcd2 <load_image_lzo+130>:        mov    -0x88(%rbp),%edi
0xffffffff8b2fdcd8 <load_image_lzo+136>:        imul   $0x43058,%rdi,%rdi
0xffffffff8b2fdcdf <load_image_lzo+143>:        callq  0xffffffff8b406720 <vmalloc>
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1097
0xffffffff8b2fdce4 <load_image_lzo+148>:        test   %rax,%rax
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1096
0xffffffff8b2fdce7 <load_image_lzo+151>:        mov    %rax,-0x80(%rbp)
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1097

## 获取 snapshot
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1407
0xffffffff8b2ffb10 <swsusp_read+464>:   testb  $0x2,0x0(%r13)
0xffffffff8b2ffb15 <swsusp_read+469>:   mov    0x1a8(%r14),%rdx --> header->pages, rdx: header
0xffffffff8b2ffb1c <swsusp_read+476>:   jne    0xffffffff8b2ffb60 <swsusp_read+544>
0xffffffff8b2ffb1e <swsusp_read+478>:   lea    -0x80(%rbp),%rsi --> snapshot
0xffffffff8b2ffb22 <swsusp_read+482>:   lea    -0x68(%rbp),%rdi --> handle
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1408
0xffffffff8b2ffb26 <swsusp_read+486>:   sub    $0x1,%edx
/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1407
0xffffffff8b2ffb29 <swsusp_read+489>:   callq  0xffffffff8b2fdc50 <load_image_lzo>

/usr/src/debug/kernel-3.10.0-1160.el7/linux-3.10.0-1160.el7.x86_64/kernel/power/swap.c: 1065
0xffffffff8b2fdc50 <load_image_lzo>:    nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff8b2fdc55 <load_image_lzo+5>:  push   %rbp
0xffffffff8b2fdc56 <load_image_lzo+6>:  mov    %rsp,%rbp
0xffffffff8b2fdc59 <load_image_lzo+9>:  push   %r15
0xffffffff8b2fdc5b <load_image_lzo+11>: push   %r14

    ffff9263fd72bd58: 0000000000000000 ffff9263fd72be1c
    ffff9263fd72bd68: ffff92637daca000[r14] ffff92637d8e1470
    ffff9263fd72bd78: ffff9263fd72be08[rbp old] ffffffff8b2ffb2e
    #9 [ffff9263fd72bd80] swsusp_read at ffffffff8b2ffb2e
```

* page                     : rbp(-0xb0) : (ffff9263fd72bcc8) -- ffffb3f100000000
* data(dec_data)           : rbp(-0x80) : (ffff9263fd72bcf8) -- ffffb3f1c09fc000
* header(swsusp_info)      : snapshot.buffer
* snapshot(snapshot_handle): rbp_old(-0x80): ffff9263fd72bd88
* handle(swap_map_handle)  : rbp_old(-0x68): ffff9263fd72bda0

```
struct snapshot_handle {
  cur = 138349,
  buffer = 0xffff9263fe142000,
  sync_read = 0
}
```
而panic是因为访问了`handle.buffer`指针指向的内存，发生page fault(not present)
执行vtop 查看具体映射:
```
crash> vtop 0xffff9263fe142000
VIRTUAL           PHYSICAL
ffff9263fe142000  be142000

PGD DIRECTORY: ffffffff8be10000
PAGE DIRECTORY: 1fe74067
   PUD: 1fe74c78 => 13ffff067
   PMD: 13fffff80 => 13fffe067
   PTE: 13fffea10 => 0

      PAGE       PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffe30302f85080 be142000                0        0  1 1fffff00000000
```

## orig bm
```
crash> p orig_bm
orig_bm = $110 = {
  blocks = {
    next = 0xffff92646cb9e620,
    prev = 0xffff92646cb9e008
  },
  p_list = 0xffff92646cb9e000,
  cur = {
    block = 0xffff92646cb9e288,
    bit = 24898
  }
}
crash> p *(orig_bm.cur.block)
$115 = {
  hook = {
    next = 0xffff92646cb9e260,
    prev = 0xffff92646cb9e2b0
  },
  start_pfn = 753665,
  end_pfn = 786433,
  data = 0xffff9263f744b000
}
crash> p orig_bm.cur.block.start_pfn+orig_bm.cur.bit-1
$119 = 778562
```
查看该page:
```
crash> eval 778562*4096
hexadecimal: be142000  (3114248KB)
crash> ptov be142000
VIRTUAL           PHYSICAL
ffff9263fe142000  be142000
^^^^^^^^^^^^^^^^
```
**可以发现这个就是异常地址!!!** 这样来说，copy的目的地就是suspend之前的
地址，没有用跳板，这样的pfn会在`forbidden_pages_map`, `free_pages_map`
中有标记，我们来确认下:

`forbidden_pages_map`:
```
crash> eval 778562
hexadecimal: be142
...

crash> forbidden_pages_map
forbidden_pages_map = $125 = (struct memory_bitmap *) 0xffff9263fd66fd80

crash> memory_bitmap.cur  0xffff9263fd66fd80
  cur = {
    block = 0xffff92637d62d288,
    bit = 24898
  }
crash> struct bm_block 0xffff92637d62d288 -x
struct bm_block {
  hook = {
    next = 0xffff92637d62d260,
    prev = 0xffff92637d62d2b0
  },
  start_pfn = 0xb8001,
  end_pfn = 0xc0001,
  data = 0xffff92637d621000
}
crash> p 0xbe142-0xb8001
$127 = 24897
crash> p 24897/8
$143 = 3112
crash> p (char *)(0xffff92637d621000+3112)
$144 = 0xffff92637d621c28 "\002"
crash> x/1xb 0xffff92637d621c28
0xffff92637d621c28:     0x02
```
可以发现这个bit是1.

**free_pages_map**

```
crash> p free_pages_map
free_pages_map = $145 = (struct memory_bitmap *) 0xffff9263fd66f480
crash> memory_bitmap.cur 0xffff9263fd66f480
  cur = {
    block = 0xffff9263fd6f9288,
    bit = 24898
  }
crash> struct bm_block 0xffff9263fd6f9288 -x
struct bm_block {
  hook = {
    next = 0xffff9263fd6f9260,
    prev = 0xffff9263fd6f92b0
  },
  start_pfn = 0xb8001,
  end_pfn = 0xc0001,
  data = 0xffff92637d524000
}
crash> p 0xbe142-0xb8001
$147 = 24897
crash> p 24897/8
$148 = 3112
crash> p (void *)(0xffff92637d524000+3112)
$150 = (void *) 0xffff92637d524c28
crash> x/1xb 0xffff92637d524c28
0xffff92637d524c28:     0xff
```
可以发现也是1， 说明猜想正确。

**那么问题来了, 为什么这个内存host没有建立映射呢???**

## memory model
我们来看下面两个咋算的:

**__pfn_to_page**:
```cpp
#define __pfn_to_page(pfn)  (vmemmap + (pfn))
#define vmemmap ((struct page *)VMEMMAP_START)
#define VMEMMAP_START   vmemmap_base
crash> p ((struct page *)vmemmap_base)+0xbe142
$155 = (struct page *) 0xffffe30302f85080
```

**page_to_pfn**
```cpp
#define pfn_to_virt(pfn)    __va((pfn) << PAGE_SHIFT)
#define __va(x)         ((void *)((unsigned long)(x)+PAGE_OFFSET))
#define PAGE_OFFSET     ((unsigned long)__PAGE_OFFSET)
#define __PAGE_OFFSET           page_offset_base

crash> p (void *)page_offset_base+(0xbe142*4096)
$160 = (void *) 0xffff9262fe142000
```
ptov输出:
```
crash> vtop ffff9263fe142000
VIRTUAL           PHYSICAL
ffff9263fe142000  be142000

PGD DIRECTORY: ffffffff8be10000
PAGE DIRECTORY: 1fe74067
   PUD: 1fe74c78 => 13ffff067
   PMD: 13fffff80 => 13fffe067
   PTE: 13fffea10 => 0

      PAGE       PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffe30302f85080 be142000                0        0  1 1fffff00000000
```
可以发现:
* `page`可以对上
* `va`也可以对上， 但是似乎依赖`page_offset_base`这个变量

这个变量是引入`kaslr`功能引入，每次启动的值都不一样！！这样就导致上次保存的pfn对
应的va，不一定在本次有映射。（因为pa变了，所以va也变了！)

## 查看该pfn
通过dmesg打印，可以发现，该PFN为 ACPI data
```
[    0.000000] BIOS-e820: [mem 0x0000000000900000-0x00000000be141fff] usable
[    0.000000] BIOS-e820: [mem 0x00000000be142000-0x00000000be142fff] ACPI data
[    0.000000] BIOS-e820: [mem 0x00000000be143000-0x00000000be1e7fff] usable
...
[    0.000000] efi: mem24: type=9, attr=0xf, range=[0x00000000be142000-0x00000000be143000) (0MB)
```

这个page 居然是 `ACPI data`! 我们来看下该page是否在memblock管理中:

```
crash> p memblock_memory_init_regions -x
  }, {
    base = 0x900000,
    size = 0xbd842000,
    flags = 0x0,
    nid = 0x0
  }, {
    base = 0xbe143000,
    size = 0xa5000,
    flags = 0x0,
    nid = 0x0
  }, {

crash> eval 0xbd842000+0x900000
hexadecimal: be142000  (3114248KB)
```
可以发现, 该地址空间并未在 `memblock`的管理中.

**Q: ACPI data 要不要save??**
**A: 必须得save, 因为这也是整个系统运行时的一部分(内核+用户态+bios)**

需要看下kernel 对saveable的过滤

## saveable pages

> 因为这里pfn < 4G, 我们这里不看highmem路径
```cpp
static unsigned int count_data_pages(void)
{
    struct zone *zone;
    unsigned long pfn, max_zone_pfn;
    unsigned int n = 0;

    for_each_populated_zone(zone) {
        if (is_highmem(zone))
            continue;

        mark_free_pages(zone);
        max_zone_pfn = zone->zone_start_pfn + zone->spanned_pages;
        for (pfn = zone->zone_start_pfn; pfn < max_zone_pfn; pfn++)
            if (saveable_page(zone, pfn))
                n++;
    }
    return n;
}
#define for_each_populated_zone(zone)               \
    for (zone = (first_online_pgdat())->node_zones; \
         zone;                  \
         zone = next_zone(zone))            \
        if (!populated_zone(zone))      \
            ; /* do nothing */      \
        else

```
crash 调试, 来调试下各个zone
```
crash> p node_states[1]
$10 = {
  bits = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
}
crash> p node_data[0]
$11 = (struct pglist_data *) 0xffff92647ffd7000
crash> struct pglist_data.node_zones 0xffff92647ffd7000 -o
struct pglist_data {
  [ffff92647ffd7000] struct zone node_zones[4];
}
crash> struct zone.zone_start_pfn,spanned_pages,present_pages ffff92647ffd7000 -x
  zone_start_pfn = 0x1
  spanned_pages = 0xfff
  present_pages = 0xea6
crash> struct zone.zone_start_pfn,spanned_pages,present_pages,name 0xffff92647ffd7800 -x
  zone_start_pfn = 0x1000
  spanned_pages = 0xff000
                  ^^^^^^^在这个zone里面，这个是DMA32 zone
  present_pages = 0xbe75e
  name = 0xffffffff8bc8e57a "DMA32"

crash> struct zone.zone_start_pfn,spanned_pages,present_pages 0xffff92647ffd8000 -x
  zone_start_pfn = 0x100000
  spanned_pages = 0x40000
  present_pages = 0x40000
```

查看判断是否是saveablepage的代码:
```cpp
static struct page *saveable_page(struct zone *zone, unsigned long pfn)
{
    struct page *page;

    if (!pfn_valid(pfn))
        return NULL;

    page = pfn_to_page(pfn);
    if (page_zone(page) != zone)
        return NULL;

    BUG_ON(PageHighMem(page));

    if (swsusp_page_is_forbidden(page) || swsusp_page_is_free(page))
        return NULL;

    if (PageOffline(page))
        return NULL;

    if (PageReserved(page)
        && (!kernel_page_present(page) || pfn_is_nosave(pfn)))
        return NULL;

    if (page_is_guard(page))
        return NULL;

    return page;
}
```
可以发现，也没啥，该page也不是`reserved` page, 所以该page 是可以 **saveable**
```
crash> kmem -p ffffe30302f85080
      PAGE       PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffe30302f85080 be142000                0        0  1 1fffff00000000
```

那么现在问题来了，为什么resume kernel 启动的时候，并未给`ACPI pages`建立映射。
但是却创建了page(struct page)

我们先查看相邻两个页的映射:
```
crash> ptov be141000
VIRTUAL           PHYSICAL
ffff9263fe141000  be141000
crash> vtop ffff9263fe141000
VIRTUAL           PHYSICAL
ffff9263fe141000  be141000

PGD DIRECTORY: ffffffff8be10000
PAGE DIRECTORY: 1fe74067
   PUD: 1fe74c78 => 13ffff067
   PMD: 13fffff80 => 13fffe067
   PTE: 13fffea08 => 80000000be141063
  PAGE: be141000

      PTE         PHYSICAL  FLAGS
80000000be141063  be141000  (PRESENT|RW|ACCESSED|DIRTY|NX)

      PAGE       PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffe30302f85040 be141000 ffff92646e1873e0      20b  1 1fffff00080030 dirty,lru,swapbacked

crash> ptov be143000
VIRTUAL           PHYSICAL
ffff9263fe143000  be143000
crash> vtop ffff9263fe143000
VIRTUAL           PHYSICAL
ffff9263fe143000  be143000

PGD DIRECTORY: ffffffff8be10000
PAGE DIRECTORY: 1fe74067
   PUD: 1fe74c78 => 13ffff067
   PMD: 13fffff80 => 13fffe067
   PTE: 13fffea18 => 80000000be143063
  PAGE: be143000

      PTE         PHYSICAL  FLAGS
80000000be143063  be143000  (PRESENT|RW|ACCESSED|DIRTY|NX)

      PAGE       PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffe30302f850c0 be143000 ffff92646e1873e0      195  1 1fffff00080030 dirty,lru,swapbacked
```

可以发现都有映射，我们在找一个`ACPI page`
```
crash> dmesg |grep BIOS-e820 |grep 'ACPI'
[    0.000000] BIOS-e820: [mem 0x0000000000800000-0x0000000000807fff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x000000000080b000-0x000000000080bfff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x0000000000810000-0x00000000008fffff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000be142000-0x00000000be142fff] ACPI data
[    0.000000] BIOS-e820: [mem 0x00000000be1e8000-0x00000000be1e8fff] ACPI data
[    0.000000] BIOS-e820: [mem 0x00000000bf36d000-0x00000000bf37efff] ACPI data
[    0.000000] BIOS-e820: [mem 0x00000000bf37f000-0x00000000bf3fefff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000bfb80000-0x00000000bfffffff] ACPI NVS
```
可以发现也没有:
```
crash> ptov be1e8000
VIRTUAL           PHYSICAL
ffff9263fe1e8000  be1e8000
crash>
crash> vtop ffff9263fe1e8000
VIRTUAL           PHYSICAL
ffff9263fe1e8000  be1e8000

PGD DIRECTORY: ffffffff8be10000
PAGE DIRECTORY: 1fe74067
   PUD: 1fe74c78 => 13ffff067
   PMD: 13fffff80 => 13fffe067
   PTE: 13fffef40 => 0

      PAGE       PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffe30302f87a00 be1e8000                0        0  1 1fffff00000000
```
## mapping kernel page

相关代码:
```
init_mem_mapping
|-> init_memory_mapping(0, ISA_END_ADDRESS);
|-> if (memblock_bottom_up()) 
    \-> memory_map_bottom_up(kernel_end, end);
    \-> memory_map_bottom_up(ISA_END_ADDRESS, kernel_end);
--> else
    \-> memory_map_top_down(ISA_END_ADDRESS, end);
```
关键看`memory_map_top_down()`
```cpp
memory_map_bottom_up
|-> while(start < map_end)
   |-> new_mapped_ram_size = init_range_memory_mapping(start, next);
       |-> for_each_mem_pfn_range(i, MAX_NUMNODES, &start_pfn, &end_pfn, NULL)
           |-> init_memory_mapping(start, end);
   |-> start = next;
```
而`for_each_mem_pfn_range`, 则是超找的`memblock.memory`. 上面提到过，该
`memblock`中，并没有触发异常的地址段.

## 那为什么save的时候没有问题，resume时候出问题了
我们来看save时的堆栈:
```
create_image
|-> save_processor_state();
## 在保存processor 状态之后
|-> swsusp_arch_suspend
    |-> swsusp_save
        ## 这里会申请copy pages
        |-> swsusp_alloc()
        ## 将orig_bm 中的内存，copy到 copy_bm的内存
        |-> copy_data_pages
            |-> for_each_mem_pfn_range()
                 ## 最后再重新标记saveable 内存
                 |-> for (pfn = zone->zone_start_pfn; pfn < max_zone_pfn; pfn++)
                     |-> if (page_is_saveable(zone, pfn)) 
                         \-> memory_bm_set_bit(orig_bm, pfn);
            ## 标记完后, copy到 copy bm 中的内存
            |-> for(;;)
                |-> pfn = memory_bm_next_pfn(orig_bm);
                |-> copy_data_page(memory_bm_next_pfn(copy_bm), pfn);
                    |-> safe_copy_page
```

copy用的函数是`safe_copy_page()`:
```cpp
static void safe_copy_page(void *dst, struct page *s_page)
{
    if (kernel_page_present(s_page)) {
        do_copy_page(dst, page_address(s_page));
    } else {
        kernel_map_pages(s_page, 1, 1);
        do_copy_page(dst, page_address(s_page));
        kernel_map_pages(s_page, 1, 0);
    }
}
```
其会查看该page的映射是否存在，不存在，`kernel_map_pages`建立
映射关系...

## restore memory safe && unsafe
(先略)

## 查看该page是不是被添加到伙伴系统

在前面的`kmem`的信息中可以看到，`be142000` page refcount 为1, 肯定是在伙伴系统中
分配出去了. (这个非常奇怪)
```
crash> kmem -p |grep -E ' 800000 | 80b000 | 810000 | be142000 | be1e8000 | bf36d000 | bf37f000 | bfb80000 '
ffffe30300020000   800000                0        0  1 fffff00000000
ffffe303000202c0   80b000                0        0  1 fffff00000000
ffffe30300020400   810000                0        0  1 fffff00000000
ffffe30302f85080 be142000                0        0  1 1fffff00000000
ffffe30302f87a00 be1e8000                0        0  1 1fffff00000000
ffffe30302fcdb40 bf36d000                0        0  1 1fffff00000000
ffffe30302fcdfc0 bf37f000                0        0  1 1fffff00000000
ffffe30302fee000 bfb80000                0        0  1 1fffff00000000
```
并且这几个`ACPI` memblock range都被分配出去了.


## 其他
nr_copy_pages, nr_meta_pages:
```
crash> p nr_copy_pages
nr_copy_pages = $10 = 229023
crash> p nr_meta_pages
nr_meta_pages = $11 = 448
```

e820
```
[    0.000000] reserve setup_data: [mem 0x0000000000000000-0x000000000009ffff] usable
[    0.000000] reserve setup_data: [mem 0x0000000000100000-0x00000000007fffff] usable
[    0.000000] reserve setup_data: [mem 0x0000000000800000-0x0000000000807fff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x0000000000808000-0x000000000080afff] usable
[    0.000000] reserve setup_data: [mem 0x000000000080b000-0x000000000080bfff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x000000000080c000-0x000000000080ffff] usable
[    0.000000] reserve setup_data: [mem 0x0000000000810000-0x00000000008fffff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x0000000000900000-0x00000000bd85d017] usable
[    0.000000] reserve setup_data: [mem 0x00000000bd85d018-0x00000000bd866a57] usable
[    0.000000] reserve setup_data: [mem 0x00000000bd866a58-0x00000000be141fff] usable
[    0.000000] reserve setup_data: [mem 0x00000000be142000-0x00000000be142fff] ACPI data
[    0.000000] reserve setup_data: [mem 0x00000000be143000-0x00000000be1e7fff] usable
[    0.000000] reserve setup_data: [mem 0x00000000be1e8000-0x00000000be1e8fff] ACPI data
[    0.000000] reserve setup_data: [mem 0x00000000be1e9000-0x00000000be674fff] usable
[    0.000000] reserve setup_data: [mem 0x00000000be675000-0x00000000be6bdfff] reserved
[    0.000000] reserve setup_data: [mem 0x00000000be6be000-0x00000000beb89fff] usable
[    0.000000] reserve setup_data: [mem 0x00000000beb8a000-0x00000000bebcafff] reserved
[    0.000000] reserve setup_data: [mem 0x00000000bebcb000-0x00000000bf0ecfff] usable
[    0.000000] reserve setup_data: [mem 0x00000000bf0ed000-0x00000000bf36cfff] reserved
[    0.000000] reserve setup_data: [mem 0x00000000bf36d000-0x00000000bf37efff] ACPI data
[    0.000000] reserve setup_data: [mem 0x00000000bf37f000-0x00000000bf3fefff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x00000000bf3ff000-0x00000000bfafbfff] usable
[    0.000000] reserve setup_data: [mem 0x00000000bfafc000-0x00000000bfb7ffff] reserved
[    0.000000] reserve setup_data: [mem 0x00000000bfb80000-0x00000000bfffffff] ACPI NVS
[    0.000000] reserve setup_data: [mem 0x00000000feffc000-0x00000000feffffff] reserved
[    0.000000] reserve setup_data: [mem 0x00000000ffc00000-0x00000000ffffffff] reserved
[    0.000000] reserve setup_data: [mem 0x0000000100000000-0x000000013fffffff] usable
```

查看`nosave_regions`
```
crash> p &nosave_regions
$54 = (struct list_head *) 0xffffffff8be5da50
crash> list nosave_region.list -H 0xffffffff8be5da50 -s nosave_region.start_pfn,end_pfn -x
ffff92647ff3c000
  start_pfn = 0xa0
  end_pfn = 0x100
ffff92647ff3a340
  start_pfn = 0x800
  end_pfn = 0x808
ffff92647ff3a300
  start_pfn = 0x80b
  end_pfn = 0x80c
ffff92647ff3a2c0
  start_pfn = 0x810
  end_pfn = 0x900
ffff92647ff3a280
  start_pfn = 0xbd85d
  end_pfn = 0xbd85e
ffff92647ff3a240
  start_pfn = 0xbd866
  end_pfn = 0xbd867
ffff92647ff3a200
  start_pfn = 0xbe142
  end_pfn = 0xbe143
ffff92647ff3a1c0
  start_pfn = 0xbe1e8
  end_pfn = 0xbe1e9
ffff92647ff3a180
  start_pfn = 0xbe675
  end_pfn = 0xbe6be
ffff92647ff3a140
  start_pfn = 0xbeb8a
  end_pfn = 0xbebcb
ffff92647ff3a100
  start_pfn = 0xbf0ed
  end_pfn = 0xbf3ff
ffff92647ff3a0c0
  start_pfn = 0xbfafc
  end_pfn = 0x100000
```
可以发现，问题的地址在`nosaveable page` 里面

查看快照之前的启动:
```
20250902-instance kernel: BIOS-e820: [mem 0x0000000000000000-0x000000000009ffff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x0000000000100000-0x00000000007fffff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x0000000000800000-0x0000000000807fff] ACPI NVS
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x0000000000808000-0x000000000080afff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x000000000080b000-0x000000000080bfff] ACPI NVS
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x000000000080c000-0x000000000080ffff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x0000000000810000-0x00000000008fffff] ACPI NVS
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x0000000000900000-0x00000000be128fff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000be129000-0x00000000be129fff] ACPI data
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000be12a000-0x00000000be1e7fff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000be1e8000-0x00000000be1e8fff] ACPI data
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000be1e9000-0x00000000be674fff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000be675000-0x00000000be6bdfff] reserved
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000be6be000-0x00000000beb89fff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000beb8a000-0x00000000bebcafff] reserved
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000bebcb000-0x00000000bf0ecfff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000bf0ed000-0x00000000bf36cfff] reserved
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000bf36d000-0x00000000bf37efff] ACPI data
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000bf37f000-0x00000000bf3fefff] ACPI NVS
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000bf3ff000-0x00000000bfafbfff] usable
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000bfafc000-0x00000000bfb7ffff] reserved
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000bfb80000-0x00000000bfffffff] ACPI NVS
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000feffc000-0x00000000feffffff] reserved
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x00000000ffc00000-0x00000000ffffffff] reserved
Sep 17 16:16:05 20250902-instance kernel: BIOS-e820: [mem 0x0000000100000000-0x000000013fffffff] usable
```

**可以发现整个的e820 变了, 出问题的地址段`0x00000000be142000-0x00000000be142fff` ,
在上次启动中是`usable` page. 所以导致了这个问题**

### 查看其他的bitmap
#### be1e8000 ~
```
crash> p 0xbe1e8-0xb8001
$24 = 25063
crash> p 25063/8
$25 = 3132
crash> p 25063%8
$26 = 7
crash> p (char *)(0xffff92637d621000+3132)
$27 = 0xffff92637d621c3c "\200"
crash> x/1xb 0xffff92637d621c3c
0xffff92637d621c3c:     0x80
crash> eval 0x80
     binary: 0000000000000000000000000000000000000000000000000000000010000000
```
#### bf36d000 ~ bf37efff ~ bf3fefff
```
crash> p 0xbf36d-0xb8001
$28 = 29548
crash> p 29548/8
$29 = 3693
crash> p 29548%8
$30 = 4
crash> p (char *)(0xffff92637d621000+3693)
$31 = 0xffff92637d621e6d "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377?"
crash> p 0x3ff-0x36d
$34 = 146
crash> x/3xg 0xffff92637d621e6d
0xffff92637d621e6d:     0xffffffffffffffff      0xffffffffffffffff
0xffff92637d621e7d:     0x00000000003fffff
```
所有的`ACPI xxx`都被滞为1.

## 查看一些reserved page
### be675000 ~ be6bdfff
```
crash> p 0xbe675-0xb8001
$40 = 26228
crash> p 26228/8
$41 = 3278
crash> p 26228%8
$42 = 4
crash> p 0x6be-0x675
$44 = 73
crash> p (char *)(0xffff92637d621000+3278)
$45 = 0xffff92637d621cce "\360\377\377\377\377\377\377\377\377\037"
crash> x/2xg 0xffff92637d621cce
0xffff92637d621cce:     0xfffffffffffffff0      0x0000000000001fff
crash> p (1<<13)-1
$50 = 8191
crash> eval 8191
hexadecimal: 1fff
    decimal: 8191
      octal: 17777
     binary: 0000000000000000000000000000000000000000000000000001111111111111
crash> x/20xg (0xffff92637d621cce-8*10)
0xffff92637d621c7e:     0x0000000000000000      0x0000000000000000
0xffff92637d621c8e:     0x0000000000000000      0x0000000000000000
0xffff92637d621c9e:     0x0000000000000000      0x0000000000000000
0xffff92637d621cae:     0x0000000000000000      0x0000000000000000
0xffff92637d621cbe:     0x0000000000000000      0x0000000000000000
0xffff92637d621cce:     0xfffffffffffffff0      0x0000000000001fff
0xffff92637d621cde:     0x0000000000000000      0x0000000000000000
0xffff92637d621cee:     0x0000000000000000      0x0000000000000000
0xffff92637d621cfe:     0x0000000000000000      0x0000000000000000
0xffff92637d621d0e:     0x0000000000000000      0x0000000000000000
```
可以发现，reserved 被精准定位了. 
