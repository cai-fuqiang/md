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

可以发现PTE缺少映射.看起来像是一个vmalloc的地址, 查看vmalloc:
```
crash> kmem -v |grep ffff9263f
ffff92637dab5a80  ffff9263fd693700  ffffb3f1c0962000 - ffffb3f1c0964000     8192
ffff92646e245780  ffff9263fd66f5c0  ffffb3f1c0968000 - ffffb3f1c096a000     8192
ffff92646e245f80  ffff9263fd66fd40  ffffb3f1c0972000 - ffffb3f1c0983000    69632
ffff92646e244380  ffff9263fd689600  ffffb3f1c0989000 - ffffb3f1c099a000    69632
ffff92646e245800  ffff9263fd66f600  ffffb3f1c099a000 - ffffb3f1c099c000     8192
...
crash> kmem -v |grep ffff9263fe
crash>
```
没有特别相近地址的`vmalloc`区间

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

## 其他
nr_copy_pages, nr_meta_pages:
```
crash> p nr_copy_pages
nr_copy_pages = $10 = 229023
crash> p nr_meta_pages
nr_meta_pages = $11 = 448
```
