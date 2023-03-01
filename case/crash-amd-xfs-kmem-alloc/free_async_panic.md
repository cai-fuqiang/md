# vmcore分析
## bt
```
 #6 [ffffb0ba31c77ce0] invalid_op at ffffffffbac00cc4
    [exception RIP: kfree+328]
    RIP: ffffffffba489ac8  RSP: ffffb0ba31c77d98  RFLAGS: 00010246
    RAX: fffffa9db3102408  RBX: ffff8a9104090304  RCX: ffff8a92627a6600
    RDX: 0000000000000000  RSI: ffff8a9104090308  RDI: 000075fb40000000
    RBP: ffffb0ba31c77e70   R8: ffff8a6437ceda00   R9: ffff8a1c5d1ad280
    R10: fffffa9db3102400  R11: 0000000000000000  R12: ffffffffba7d32bf
    R13: ffff8a92627a7380  R14: ffff8a1c5da61b20  R15: 0000000000000000
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
 #7 [ffffb0ba31c77db0] free_async at ffffffffba7d32bf
 #8 [ffffb0ba31c77dc8] usbdev_do_ioctl at ffffffffba7d4d1a
 #9 [ffffb0ba31c77e78] usbdev_ioctl at ffffffffba7d5d8a
#10 [ffffb0ba31c77e80] do_vfs_ioctl at ffffffffba4cdfe4
#11 [ffffb0ba31c77ef8] ksys_ioctl at ffffffffba4ce5d0
```

## dis free_async层
```
0xffffffffba7d31e0 <free_async>:        nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffba7d31e5 <free_async+5>:      push   %rbp
0xffffffffba7d31e6 <free_async+6>:      push   %rbx
0xffffffffba7d31e7 <free_async+7>:      mov    %rdi,%rbx
...

0xffffffffba7d32ba <free_async+218>:    callq  0xffffffffba489980 <kfree> 		<----跳转
0xffffffffba7d32bf <free_async+223>:    jmp    0xffffffffba7d3284 <free_async+164>
/usr/src/debug/kernel-4.18.0-147.5.1.el8_1/linux-4.18.0-147.5.1.es8_24.x86_64/./include/linux/cred.h: 266
0xffffffffba7d32c1 <free_async+225>:    callq  0xffffffffba2d5b60 <__put_cred>
```
## dis kfree
```
0xffffffffba489980 <kfree>:     nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffba489985 <kfree+5>:   push   %r12
0xffffffffba489987 <kfree+7>:   push   %rbp
0xffffffffba489988 <kfree+8>:   push   %rbx
0xffffffffba489989 <kfree+9>:   mov    %rdi,%rbx
0xffffffffba48998c <kfree+12>:  mov    0x18(%rsp),%r12
```
## bt -f
```
 #6 [ffffb0ba31c77ce0] invalid_op at ffffffffbac00cc4
    [exception RIP: kfree+328]
    RIP: ffffffffba489ac8  RSP: ffffb0ba31c77d98  RFLAGS: 00010246
    RAX: fffffa9db3102408  RBX: ffff8a9104090304[address]  RCX: ffff8a92627a6600
    RDX: 0000000000000000  RSI: ffff8a9104090308  RDI: 000075fb40000000
    RBP: ffffb0ba31c77e70   R8: ffff8a6437ceda00   R9: ffff8a1c5d1ad280
    R10: fffffa9db3102400  R11: 0000000000000000  R12: ffffffffba7d32bf
    R13: ffff8a92627a7380  R14: ffff8a1c5da61b20  R15: 0000000000000000
    ORIG_RAX: ffffffffffffffff  CS: 0010  SS: 0018
    ffffb0ba31c77ce8: 0000000000000000 ffff8a1c5da61b20
    ffffb0ba31c77cf8: ffff8a92627a7380 ffffffffba7d32bf
    ffffb0ba31c77d08: ffffb0ba31c77e70 ffff8a9104090304
    ffffb0ba31c77d18: 0000000000000000 fffffa9db3102400
    ffffb0ba31c77d28: ffff8a1c5d1ad280 ffff8a6437ceda00
    ffffb0ba31c77d38: fffffa9db3102408 ffff8a92627a6600
    ffffb0ba31c77d48: 0000000000000000 ffff8a9104090308
    ffffb0ba31c77d58: 000075fb40000000 ffffffffffffffff
    ffffb0ba31c77d68: ffffffffba489ac8[EIP] 0000000000000010
    ffffb0ba31c77d78: 0000000000010246 ffffb0ba31c77d98[ESP]
    ffffb0ba31c77d88: 0000000000000018[SS] ffff8a1c5da61b20[对齐]
kfree stack
    ffffb0ba31c77d98: ffff8a927e1b9d80[rbx] ffffb0ba31c77e70
    ffffb0ba31c77da8: ffff8a7cc4e6e120 ffffffffba7d32bf
 #7 [ffffb0ba31c77db0] free_async at ffffffffba7d32bf
    ffffb0ba31c77db8: ffff8a927e1b9d80 ffffb0ba31c77e70
    ffffb0ba31c77dc8: ffffffffba7d4d1a
```
`ffffb0ba31c77d80`地址为发生异常是cpu进行对齐的地址，也可以从`ffffb0ba31c77d70(esp)`
里面的值看出来(`ffffb0ba31c77d98`)
查看`free_async`代码
```cpp
static void free_async(struct async *as)
{
        int i;

        put_pid(as->pid);
        if (as->cred)
                put_cred(as->cred);
        for (i = 0; i < as->urb->num_sgs; i++) {
                if (sg_page(&as->urb->sg[i]))
                        kfree(sg_virt(&as->urb->sg[i]));
        }

        kfree(as->urb->sg);
        if (as->usbm == NULL)
                kfree(as->urb->transfer_buffer);	//这里触发异常
        else
                dec_usb_memory_use_count(as->usbm, &as->usbm->urb_use_count);

        kfree(as->urb->setup_packet);
        usb_free_urb(as->urb);
        usbfs_decrease_memory_usage(as->mem_usage);
        kfree(as);
}
```
可以看出，`as->urb->transfer_buffer`有问题，查看反汇编, 
```
0xffffffffba7d32b2 <free_async+210>:    mov    0x40(%rbx),%rax
0xffffffffba7d32b6 <free_async+214>:    mov    0x68(%rax),%rdi
crash> x/1xg (0xffff8a927e1b9d80+0x40)
0xffff8a927e1b9dc0:     0xffff8a92627a6600
crash> x/1xg (0xffff8a92627a6600+0x68)
0xffff8a92627a6668:     0xffff8a9104090304
```
这里rbx是as, 可以通过堆栈得到，`transfer_buffer`也可以查看内存得出，
或者通过pt_regs得出来: `ffff8a9104090304`
```
crash> vtop 0xffff8a9104090304
VIRTUAL           PHYSICAL
ffff8a9104090304  8cc4090304

PGD DIRECTORY: ffffffffbb40a000
PAGE DIRECTORY: 55b7205067
   PUD: 55b7205220 => 8000008cc00000e3
  PAGE: 8cc0000000  (1GB)

      PTE          PHYSICAL   FLAGS
8000008cc00000e3  8cc0000000  (PRESENT|RW|ACCESSED|DIRTY|PSE|NX)

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa9db3102400 8cc4090000                0        0  0 157ffffc0000000
```
该地址, 是一个1GB的大页，但是并没有人使用(CNT=0)，所以可以简单判断为，该
地址是一个非法地址（软件层面）

再查看相关数据结构:
```
struct async {
  asynclist = {
    next = 0xffff8a927e1b9d80, 
    prev = 0xffff8a927e1b9d80
  }, 
  ps = 0xffff8a92627a7380, 
  pid = 0xffff8a9284d15580, 
  cred = 0xffff8a4b8eda7080, 
  signr = 0, 
  ifnum = 4294967295, 
  userbuffer = 0x55fb0c5fa008, 
  userurb = 0x55fb0c624110, 
  urb = 0xffff8a92627a6600, 
  usbm = 0x0, 
  mem_usage = 550, 
  status = 0, 
  bulk_addr = 0 '\000', 
  bulk_status = 0 '\000'
}
crash> struct urb 0xffff8a92627a6600
struct urb {
  kref = {
    refcount = {
      refs = {
        counter = 1
      }
    }
  },
  hcpriv = 0x0,
  use_count = {
    counter = 0
  },
  reject = {
    counter = 0
  },
  unlinked = 0,
  urb_list = {
    next = 0xffff8a92627a6620,
    prev = 0xffff8a92627a6620
  },
  anchor_list = {
    next = 0xffff8a92627a6630,
    prev = 0xffff8a92627a6630
  },
  anchor = 0x0,
  dev = 0xffff8a7cc4e6e000,
  ep = 0xffff8a7cc4e6e050,
  pipe = 2147484032,
  stream_id = 0,
  status = 0,
  transfer_flags = 512,
  transfer_buffer = 0xffff8a9104090304,
  transfer_dma = 0,
  sg = 0x0,
  num_mapped_sgs = 0,
  num_sgs = 0,
  transfer_buffer_length = 254,
  actual_length = 4,
  setup_packet = 0xffff8a9416fab1a0 "\200\006",
  setup_dma = 0,
  start_frame = 0,
  number_of_packets = 0,
  interval = 0,
  error_count = 0,
  context = 0xffff8a927e1b9d80,
  complete = 0xffffffffba7d25c0,
  iso_frame_desc = 0xffff8a92627a66c0
}
```
search 该地址
```
15417dca70: ffff8a9104090304
15417dcba8: ffff8a9104090304
15417dcd10: ffff8a9104090304
55b6bd1f48: ffff8a9104090304
8e227a6668: ffff8a9104090304
```
查看相关page
```
crash> kmem 15417dca70
   VMAP_AREA         VM_STRUCT                 ADDRESS RANGE                SIZE
ffff8a198dd6f6c0  ffff8aac56c65a80  ffffb0ba31c74000 - ffffb0ba31c79000    20480

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa9bd505f700 15417dc000                0        0  1 17ffffc0000000

crash> kmem 15417dcba8
   VMAP_AREA         VM_STRUCT                 ADDRESS RANGE                SIZE
ffff8a198dd6f6c0  ffff8aac56c65a80  ffffb0ba31c74000 - ffffb0ba31c79000    20480

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa9bd505f700 15417dc000                0        0  1 17ffffc0000000

crash> kmem  15417dcd10
   VMAP_AREA         VM_STRUCT                 ADDRESS RANGE                SIZE
ffff8a198dd6f6c0  ffff8aac56c65a80  ffffb0ba31c74000 - ffffb0ba31c79000    20480

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa9bd505f700 15417dc000                0        0  1 17ffffc0000000

crash> kmem 55b6bd1f48
      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa9cd6daf440 55b6bd1000                0        0  1 d7ffffc0000800 reserved

crash> kmem 8e227a6668
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff8a0547c0ec00      192      20133     50694   1207     8k  kmalloc-192
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  fffffa9db889e980  ffff8a92627a6000     5     42         34     8
  FREE / [ALLOCATED]
  [ffff8a92627a6600]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa9db889e980 8e227a6000 ffff8a0547c0ec00 ffff8a92627a63c0  1 157ffffc0008100 slab,head
```

查看第一个地址
```
0xffff8a19817dca50:     0xffff8a1c5da61b20(shmem_inode_cache)(mm/shmem.c:3586)      0xffff8a92627a7380(kmalloc-192)
0xffff8a19817dca60:     0xffffffffba7d32bf(free_async+223 devio.c: 388)      0xffffb0ba31c77e70
0xffff8a19817dca70:     0xffff8a9104090304(1)      0x0000000000000000
0xffff8a19817dca80:     0xfffffa9db3102400      0xffff8a1c5d1ad280(files_cache)
0xffff8a19817dca90:     0xffff8a6437ceda00(filp)(fs/file_table.c:361)      0xfffffa9db3102408
0xffff8a19817dcaa0:     0xffff8a92627a6600      0x0000000000000000
0xffff8a19817dcab0:     0xffff8a9104090308(1.1)      0x000075fb40000000
0xffff8a19817dcac0:     0xffffffffffffffff      0xffffffffba489ac8(ffffffffba489ac8 (T) kfree+328 slub.c:3900)
0xffff8a19817dcad0:     0x0000000000000010      0x0000000000010246
0xffff8a19817dcae0:     0x0000000000000000      0x0000000000000046
0xffff8a19817dcaf0:     0xffffb0ba31c77ce8      0x0000000000000002
0xffff8a19817dcb00:     0x0000000000000004      0x0000000000000001
0xffff8a19817dcb10:     0xffff8a1c5dea6400(kmalloc-1k)      0x0000000000000000
0xffff8a19817dcb20:     0xffffffffba2587ce(ffffffffba2587ce (T) machine_kexec+446 machine_kexec_64.c: 436)      0x0000aee05a332800
0xffff8a19817dcb30:     0xffff8a0440000000      0x000000005f003000
0xffff8a19817dcb40:     0xffff8a049f003000      0x000000005f002000
0xffff8a19817dcb50:     0x7ed8320b58300800      0x56d7aee05a332800
0xffff8a19817dcb60:     0xffffb0ba31c77ce8      0xffffb0ba31c77b80
0xffff8a19817dcb70:     0xffffb0ba31c77ce8      0xffffffffba355ffd(ffffffffba355ffd (T) __crash_kexec+109 kexec_core.c: 956)
0xffff8a19817dcb80:     0x0000000000000000	

0xffff8a19817dcb88:	0xffff8a1c5da61b20(shmem_inode_cache)
0xffff8a19817dcb90:     0xffff8a92627a7380(kmalloc-192)      0xffffffffba7d32bf (ffffffffba7d32bf (t) free_async+223 devio.c: 388)
0xffff8a19817dcba0:     0xffffb0ba31c77e70      0xffff8a9104090304(2)
0xffff8a19817dcbd0:     0xfffffa9db3102408      0xffff8a92627a6600
0xffff8a19817dcbe0:     0x0000000000000000      0xffff8a9104090308(2.1)
0xffff8a19817dcbf0:     0x000075fb40000000      0xffffffffffffffff
0xffff8a19817dcc00:     0xffffffffba489ac8      0x0000000000000010
0xffff8a19817dcc10:     0x0000000000010246      0xffffb0ba31c77d98
0xffff8a19817dcc20:     0x0000000000000018      0x56d7aee05a332800
0xffff8a19817dcc30:     0xffffb0ba31c77ce8      0x000000000000000b
0xffff8a19817dcc40:     0xffffffffba356edd      0xffffffffbb26e787
0xffff8a19817dcc50:     0x0000000000000246      0xffffffffba220e7d
0xffff8a19817dcc60:     0x0000000000000006      0xffff8a1c528a17c0
0xffff8a19817dcc70:     0x0000000000000000      0xffffffffba21d6dc
0xffff8a19817dcc80:     0xffffffffbb26e787      0xffffffffba489ac8
0xffff8a19817dcc90:     0xffffb0ba31c77ce8      0x0000000000000000
0xffff8a19817dcca0:     0x0000000000000000      0x0000000000000000
0xffff8a19817dccb0:     0x0000000000000000      0x0000000000000000
0xffff8a19817dccc0:     0xffffffffba21dfa6      0xffffffffba489ac8
0xffff8a19817dccd0:     0x0000000000000000      0x0000000000000000
0xffff8a19817dcce0:     0xffffffffbac00cc4      0x0000000000000000

0xffff8a19817dccf0:     0xffff8a1c5da61b20(shmem_inode_cache)      0xffff8a92627a7380(kmalloc-192)
0xffff8a19817dcd00:     0xffffffffba7d32bf(free_async+233)      0xffffb0ba31c77e70
0xffff8a19817dcd10:     0xffff8a9104090304(3)      0x0000000000000000

0xffff8a19817dcd20:     0xfffffa9db3102400      0xffff8a1c5d1ad280
0xffff8a19817dcd30:     0xffff8a6437ceda00      0xfffffa9db3102408
```

```
////////////1/////////////
crash> kmem 0xffff8a1c5d1ad280
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff8a1cc7c16780      704        645      5336    116    32k  files_cache
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  fffffa9be0746a00  ffff8a1c5d1a8000     0     46         43     3
  FREE / [ALLOCATED]
  [ffff8a1c5d1ad280]

////////////2/////////////
      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa9be0746b40 181d1ad000 dead000000000400        0  0 17ffffc0000000

crash> kmem 0xffff8a6437ceda00
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff8a1cc7c14c80      256      16553     36480   1140     8k  filp
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  fffffa9cffdf3b00  ffff8a6437cec000     3     32         32     0
  FREE / [ALLOCATED]
  [ffff8a6437ceda00]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffa9cffdf3b40 5ff7ced000 dead000000000400        0  0 d7ffffc0000000

////////////3/////////////
```
