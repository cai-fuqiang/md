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
可以看出，
search 该地址
```
15417dca70: ffff8a9104090304
15417dcba8: ffff8a9104090304
15417dcd10: ffff8a9104090304
55b6bd1f48: ffff8a9104090304
8e227a6668: ffff8a9104090304
```
