# 第一个WARN分析
堆栈
```
[498349.159738] WARNING: CPU: 110 PID: 28170 at net/ipv4/af_inet.c:153 inet_sock_destruct+0x158/0x188
...
[498349.259092] CPU: 110 PID: 28170 Comm: etcd Kdump: loaded Tainted: G           OE    --------- -  - 4.18.0-372.19.1.es8_10.aarch64 #1
[498349.272729] Hardware name: Inspur CS5260-F/CS5260-F, BIOS 2.12.01 03/23/2024
[498349.280680] pstate: 60400009 (nZCv daif +PAN -UAO)
[498349.286372] pc : inet_sock_destruct+0x158/0x188
[498349.291840] lr : inet_sock_destruct+0x4c/0x188
[498349.297173] sp : ffff80001418fc10
[498349.301361] x29: ffff80001418fc10 x28: ffff60000f6d7000
[498349.307547] x27: ffff600012c61c90 x26: ffff800011b71000
[498349.313741] x25: ffff31011e57c768 x24: ffff800011b787b8
[498349.319925] x23: 0000000000000001 x22: 0000000000000042
[498349.326122] x21: ffff31000302d568 x20: ffff31000302d730
[498349.332307] x19: ffff31000302d680 x18: 0000000000000000
[498349.338494] x17: 0000004007311ec0 x16: 00000040073103a0
[498349.344671] x15: 0000000000000001 x14: 000000401171a000
[498349.350838] x13: 000000400d180000 x12: 000000007fffffff
[498349.357010] x11: 0001c55320fd5be3 x10: 0000000000000040
[498349.363165] x9 : ffff800011f30ae0 x8 : ffff800011f30ad8
[498349.369316] x7 : 0000000000000001 x6 : ffff21008f5a0000
[498349.375476] x5 : 0000000000210d00 x4 : ffffffe84003d6a0
[498349.381625] x3 : 00000000810000ec x2 : ffff21008f5a0000
[498349.387773] x1 : 0000000000000300 x0 : 00000000fffffd00
[498349.393929] Call trace:
[498349.397215]  inet_sock_destruct+0x158/0x188
[498349.402239]  __sk_destruct+0x34/0x1a8
[498349.406734]  sk_destruct+0x54/0x70
[498349.410955]  __sk_free+0x40/0xd8
[498349.415001]  __sock_wfree+0x44/0x50
[498349.419305]  skb_release_head_state+0x64/0x128
[498349.424559]  skb_release_all+0x20/0x40
[498349.429133]  napi_consume_skb+0x5c/0x158
[498349.433871]  i40e_napi_poll+0x49c/0x830 [i40e]
[498349.439158]  __napi_poll+0x44/0x1b8
[498349.443461]  net_rx_action+0x2b4/0x330
[498349.448018]  __do_softirq+0x118/0x320
[498349.452482]  irq_exit_rcu+0x10c/0x120
[498349.456943]  irq_exit+0x14/0x20
[498349.460873]  __handle_domain_irq+0x70/0xc0
[498349.465753]  gic_handle_irq+0xd4/0x178
[498349.470266]  el0_irq_naked+0x50/0x58
[498349.474594] ---[ end trace 46956112552af2a6 ]---
```
反汇编
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_10.aarch64/./include/linux/compiler.h: 276
0xffff8000109b90c4 <inet_sock_destruct+212>:    ldr     w0, [x19,#224]
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_10.aarch64/net/ipv4/af_inet.c: 153
0xffff8000109b90c8 <inet_sock_destruct+216>:    cbnz    w0, 0xffff8000109b9148 <inet_sock_destruct+344>

crash> struct sock -o
struct sock {
  [200] struct sk_buff_head sk_receive_queue;
        struct {
            atomic_t rmem_alloc;
            int len;
            struct sk_buff *head;
            struct sk_buff *tail;
  [224] } sk_backlog;
  [248] int sk_forward_alloc;
crash> eval 0xfffffffffffffd00
hexadecimal: fffffffffffffd00
    decimal: 18446744073709550848  (-768)
      octal: 1777777777777777776400
     binary: 1111111111111111111111111111111111111111111111111111110100000000

```
可以得出 x19为sock, w0 为 rmem_alloc
而在这个代码之前有一段代码获取了 `sk_forward_alloc`
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_10.aarch64/./include/net/sock.h: 1522
0xffff8000109b90a4 <inet_sock_destruct+180>:    ldr     w1, [x19,#248]
0xffff8000109b90a8 <inet_sock_destruct+184>:    cmp     w1, #0xfff
```
`w1` 为 `sk_forward_alloc`

所以可以得出
* sk_forward_alloc: 768 (0x300)
* rmem_alloc: -768 (0xfffffd00)

<details markdown=1>
<summary>另一种方式查看</summary>

第二个WARN
```
[498349.480024] WARNING: CPU: 110 PID: 28170 at net/ipv4/af_inet.c:156 inet_sock_destruct+0x150/0x188
[498349.578679] CPU: 110 PID: 28170 Comm: etcd Kdump: loaded Tainted: G        W  OE    --------- -  - 4.18.0-372.19.1.es8_10.aarch64 #1
[498349.592020] Hardware name: Inspur CS5260-F/CS5260-F, BIOS 2.12.01 03/23/2024
[498349.599844] pstate: 60400009 (nZCv daif +PAN -UAO)
[498349.605425] pc : inet_sock_destruct+0x150/0x188
[498349.610749] lr : inet_sock_destruct+0x4c/0x188
[498349.615974] sp : ffff80001418fc10
[498349.620069] x29: ffff80001418fc10 x28: ffff60000f6d7000
[498349.626186] x27: ffff600012c61c90 x26: ffff800011b71000
[498349.632282] x25: ffff31011e57c768 x24: ffff800011b787b8
[498349.638370] x23: 0000000000000001 x22: 0000000000000042
[498349.644470] x21: ffff31000302d568 x20: ffff31000302d730
[498349.650560] x19: ffff31000302d680 x18: 0000000000000000
[498349.656665] x17: 0000004007311ec0 x16: 00000040073103a0
[498349.662756] x15: 0000000000000001 x14: 000000401171a000
[498349.668855] x13: 000000400d180000 x12: 000000007fffffff
[498349.674946] x11: 0001c55320fd5be3 x10: 0000000000000040
[498349.681059] x9 : ffff800011f30ae0 x8 : ffff800011f30ad8
[498349.687157] x7 : 0000000000000001 x6 : ffff21008f5a0000
[498349.693279] x5 : 0000000000210d00 x4 : ffffffe84003d6a0
[498349.699373] x3 : 00000000810000ec x2 : ffff21008f5a0000
[498349.705471] x1 : 0000000000000300 x0 : 0000000000000300
[498349.711562] Call trace:
[498349.714786]  inet_sock_destruct+0x150/0x188
```

反汇编
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_10.aarch64/net/ipv4/af_inet.c: 156
0xffff8000109b90dc <inet_sock_destruct+236>:    ldr     w0, [x19,#248]
0xffff8000109b90e0 <inet_sock_destruct+240>:    cbnz    w0, 0xffff8000109b9140 <inet_sock_destruct+336>
```
此时w0为 sk_forward_alloc, 为 768


</details>


# 第四个WARN
dmesg:
```
[498380.462245] TCP recvmsg seq # bug: copied C6006B71, seq 42, rcvnxt C6006CA3, fl 0
[498380.472680] WARNING: CPU: 38 PID: 26557 at net/ipv4/tcp.c:2035 tcp_recvmsg+0x4cc/0x9c8
...
[498380.806628] CPU: 38 PID: 26557 Comm: msgr-worker-0 Kdump: loaded Tainted: G        W  OE    --------- -  - 4.18.0-372.19.1.es8_10.aarch64 #1
[498380.820566] Hardware name: Inspur CS5260-F/CS5260-F, BIOS 2.12.01 03/23/2024
[498380.828331] pstate: 40400009 (nZcv daif +PAN -UAO)
[498380.833836] pc : tcp_recvmsg+0x4cc/0x9c8
[498380.838483] lr : tcp_recvmsg+0x4cc/0x9c8
[498380.843117] sp : ffff80003a16fb00
[498380.847143] x29: ffff80003a16fb00 x28: ffff21008f5a0000
[498380.853169] x27: 0000000000000000 x26: 0000000000000000
[498380.859185] x25: ffff00ff862fc148 x24: 0000000000001000
[498380.865189] x23: 0000000000000000 x22: ffff800011b78788
[498380.871191] x21: 0000000000000000 x20: ffff00ff862fc080
[498380.877192] x19: ffff21008f5a0000 x18: 0000000000000010
[498380.883194] x17: 0000ffff9032f380 x16: 0000000000000000
[498380.889194] x15: 0000000000aaaaaa x14: 0e200e200e200e20
[498380.895196] x13: 0000000000000001 x12: 00000000ffffffff
[498380.901196] x11: ffff800009830090 x10: 0000000000000001
[498380.907195] x9 : 0000000000000001 x8 : 0000000000000020
[498380.913196] x7 : 0000000000000000 x6 : ffff210eff972358
[498380.919196] x5 : ffff210eff972358 x4 : 0000000000000000
[498380.925200] x3 : ffff210eff9fa908 x2 : ffff210eff972358
[498380.931205] x1 : 6bc181616e34f700 x0 : 0000000000000000
[498380.937211] Call trace:
[498380.940349]  tcp_recvmsg+0x4cc/0x9c8
[498380.944625]  inet_recvmsg+0x70/0x110
[498380.948899]  sock_recvmsg+0x58/0x68
[498380.953086]  sock_read_iter+0x94/0xe8
[498380.957451]  new_sync_read+0x100/0x148
[498380.961899]  __vfs_read+0x74/0x90
[498380.965910]  vfs_read+0x9c/0x158
[498380.969834]  ksys_read+0x6c/0xd0
[498380.973755]  __arm64_sys_read+0x24/0x30
[498380.978292]  el0_svc_handler+0xb0/0x180
[498380.982836]  el0_svc+0x8/0xc
[498380.986422] ---[ end trace 46956112552af2a9 ]---
[498380.995870] Starting crashdump kernel...
[498381.000489] Bye!
```

反汇编
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_10.aarch64/net/ipv4/tcp.c: 2032
0xffff800010983e1c <tcp_recvmsg+332>:   ldr     w1, [x6]                //*seq
0xffff800010983e20 <tcp_recvmsg+336>:   add     x0, x19, #0x28
0xffff800010983e24 <tcp_recvmsg+340>:   ldr     w2, [x19,#40]           //TCP_SKB_CB(skb)  == sk_buff->cb[0]->seq
crash> struct -o sk_buff
struct sk_buff {
   [40] char cb[48];

crash> struct tcp_skb_cb -o
struct tcp_skb_cb {
   [0] __u32 seq;

0xffff800010983e28 <tcp_recvmsg+344>:   subs    w3, w1, w2
0xffff800010983e2c <tcp_recvmsg+348>:   b.mi    0xffff800010984180 <tcp_recvmsg+1200>
...
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_10.aarch64/net/ipv4/tcp.c: 2032
0xffff800010984180 <tcp_recvmsg+1200>:  ldr     w3, [x20,#1624]         //tp->rcv_nxt
0xffff800010984184 <tcp_recvmsg+1204>:  mov     w4, w21                 //flag
0xffff800010984188 <tcp_recvmsg+1208>:  adrp    x0, 0xffff800010e79000  //"TCP recvmsg seq # bug: "
0xffff80001098418c <tcp_recvmsg+1212>:  add     x0, x0, #0x820
0xffff800010984190 <tcp_recvmsg+1216>:  str     w9, [sp,#112]
0xffff800010984194 <tcp_recvmsg+1220>:  str     x6, [sp,#120]           //flags
0xffff800010984198 <tcp_recvmsg+1224>:  bl      0xffff8000100f27e8 <__warn_printk>
0xffff80001098419c <tcp_recvmsg+1228>:  brk     #0x800
0xffff8000109841a0 <tcp_recvmsg+1232>:  ldr     w9, [sp,#112]
```

* sk_buff:x19: 0xffff21008f5a0000
* sock: x20: ffff00ff862fc080
  ```
  int tcp_recvmsg(struct sock *sk, struct msghdr *msg, size_t len, int nonblock,
                int flags, int *addr_len)
  0xffff800010983cec <tcp_recvmsg+28>:    mov     x20, x0
  ```

# 第三个WARN

dmesg:
```
[498379.652268] TCP recvmsg seq # bug: copied C6006B71, seq 3F31B80, rcvnxt C6006CA3, fl 0
[498379.669603] WARNING: CPU: 42 PID: 26557 at net/ipv4/tcp.c:2035 tcp_recvmsg+0x4cc/0x9c8
```

* x19 为`sk_buff`: 0xffff21008f5a0000
* sock: x20: 0xffff00ff862fc080

# skb_set_owner_r

```
 skb_set_owner_r
 0xffffffff8248caf3 : tcp_queue_rcv+0xd3/0x150 [kernel]
 0xffffffff82493035 : tcp_data_queue+0x215/0xb80 [kernel]
 0xffffffff82493b62 : tcp_rcv_established+0x1c2/0x5c0 [kernel]
 0xffffffff824a056a : tcp_v4_do_rcv+0x12a/0x1e0 [kernel]
 0xffffffff824a28d3 : tcp_v4_rcv+0xb43/0xc50 [kernel]

 0xffffffff82475d4c : ip_protocol_deliver_rcu+0x2c/0x1d0 [kernel]
 0xffffffff82475f3d : ip_local_deliver_finish+0x4d/0x60 [kernel]
 0xffffffff82475fbb : ip_local_deliver+0x6b/0xf0 [kernel]
                                skb_dst(skb)->input(skb);
                                dst_input
                                ip_rcv_finish
 0xffffffff824762bb : ip_rcv+0x27b/0x36f [kernel]
 0xffffffff823df650 : __netif_receive_skb_core+0xba0/0xcb0 [kernel]
 0xffffffff823df7fd : netif_receive_skb_internal+0x3d/0xb0 [kernel]
 0xffffffff823e02a8 : napi_gro_receive+0x108/0x150 [kernel]
 0xffffffffc0427b40
 0xffffffffc0427b40 (inexact)
 0xffffffffc04286c4 (inexact)
 0xffffffff823e0b9d : __napi_poll+0x2d/0x130 [kernel] (inexact)
 0xffffffff823e1073 : net_rx_action+0x253/0x320 [kernel] (inexact)
 0xffffffff828000d7 : __do_softirq+0xd7/0x2c4 [kernel] (inexact)
 0xffffffff81cf445b : irq_exit_rcu+0xcb/0xd0 [kernel] (inexact)
 0xffffffff81cf446a : irq_exit+0xa/0x10 [kernel] (inexact)
 0xffffffff82601e7f : do_IRQ+0x7f/0xd0 [kernel] (inexact)
 0xffffffff82600a8f : ret_from_intr+0x0/0x1d [kernel] (inexact)
 0xffffffff825a4fee : native_safe_halt+0xe/0x10 [kernel] (inexact)
 0xffffffff825a4eaa : __cpuidle_text_start+0xa/0x10 [kernel] (inexact)
 0xffffffff825a5140 : default_idle_call+0x40/0xf0 [kernel] (inexact)
 0xffffffff81d25d5f : do_idle+0x20f/0x2c0 [kernel] (inexact)
 0xffffffff81d25fdf : cpu_startup_entry+0x6f/0x80 [kernel] (inexact)
 0xffffffff81c59ed6 : start_secondary+0x1a6/0x1e0 [kernel] (inexact)
 0xffffffff81c00107 : secondary_startup_64_no_verify+0xc2/0xcb [kernel] (inexact)
```


```
function is sock_rfree
 0xffffffff823bc630 : sock_rfree+0x0/0x60 [kernel]
 0xffffffff823c36a8 : skb_release_head_state+0x68/0x100 [kernel]
 0xffffffff823c374e : skb_release_all+0xe/0x30 [kernel]
 0xffffffff823c377e : __kfree_skb+0xe/0x20 [kernel]
                                                        //sk_eat_skb
 0xffffffff82488735 : tcp_recvmsg+0x7a5/0xa90 [kernel]
 0xffffffff824bc78b : inet_recvmsg+0x5b/0xd0 [kernel]
 0xffffffff823b69b2 : sock_read_iter+0x92/0xf0 [kernel]
 0xffffffff81f3e09f : new_sync_read+0x10f/0x150 [kernel]
 0xffffffff81f40ad1 : vfs_read+0x91/0x140 [kernel]
 0xffffffff81f40f0f : ksys_read+0x4f/0xb0 [kernel]
 0xffffffff81c0430b : do_syscall_64+0x5b/0x1a0 [kernel]
 0xffffffff826000ad : entry_SYSCALL_64_after_hwframe+0x65/0xca [kernel]
 0xffffffff826000ad : entry_SYSCALL_64_after_hwframe+0x65/0xca [kernel] (inexact)

```

# search -p ffff21008f5a0000
```
********************************************************************************
**************************CPU 14 中断栈 OVS 转发 感觉没啥用 ********************
********************************************************************************

crash> search -p ffff21008f5a0000
100003fe8d0: ffff21008f5a0000                    ffff80001358e8d0: ffff21008f5a0000
100003fe920: ffff21008f5a0000                    ffff80001358e920: ffff21008f5a0000
100003fe968: ffff21008f5a0000                    ffff80001358e968: ffff21008f5a0000
100003fe990: ffff21008f5a0000                    ffff80001358e990: ffff21008f5a0000
100003fea90: ffff21008f5a0000                    ffff80001358ea90: ffff21008f5a0000
100003feac0: ffff21008f5a0000                    ffff80001358eac0: ffff21008f5a0000
100003feae0: ffff21008f5a0000                    ffff80001358eae0: ffff21008f5a0000
100003feb20: ffff21008f5a0000                    ffff80001358eb20: ffff21008f5a0000
100003feb70: ffff21008f5a0000                    ffff80001358eb70: ffff21008f5a0000
100003feb90: ffff21008f5a0000                    ffff80001358eb90: ffff21008f5a0000
100003fec20: ffff21008f5a0000                    ffff80001358ec20: ffff21008f5a0000
100003fec40: ffff21008f5a0000                    ffff80001358ec40: ffff21008f5a0000
                                                 ffff80001388ff20: ffff21008f5a0000

crash> kmem 100003feb90
   VMAP_AREA         VM_STRUCT                 ADDRESS RANGE                SIZE
ffff00ff800cae40  ffff00ff80261400  ffff800013580000 - ffff8000135a0000   131072

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffffe03fc00fc0 100003f0000                0        0  1 fffff8000000000

从这个大小看, 像一个堆栈

********************************************************************************
*******************************CPU 38 中断栈************************************
********************************************************************************

1000057ff20: ffff21008f5a0000 ---- ffff80001388ff20
crash> kmem 1000057ff20
   VMAP_AREA         VM_STRUCT                 ADDRESS RANGE                SIZE
ffff00ff800c3180  ffff00ff80263900  ffff800013880000 - ffff8000138a0000   131072

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffffe03fc015c0 10000570000                0        0  1 fffff8000000000

********************************************************************************
******************************cpu 38 sock **************************************
********************************************************************************

100062fc148: ffff21008f5a0000  CPU 38 sock
100062fc150: ffff21008f5a0000

crash> kmem 100062fc148
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff500003c86600     2456       2824     10452    201   128k  TCP
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffffe03fc18b80  ffff00ff862e0000     0     52         42    10
  FREE / [ALLOCATED]
  [ffff00ff862fc080]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffffe03fc18bc0 100062f0000 dead000000000400        0  0 fffff8000000000

********************************************************************************
********************************************************************************
********************************************************************************

210007edca48: ffff21008f5a0000
crash> kmem 210007edca48
      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffffe83fc1fb40 210007ed0000                0        0 65407 4fffff8000000000

********************************************************************************
***********************出问题的slab object**************************************
********************************************************************************

21010f5a0008: ffff21008f5a0000      //slab object: ffff21008f5a000 + 8



40009934f9a0: ffff21008f5a0000      //堆栈
40009934fa58: ffff21008f5a0000
40009934faa0: ffff21008f5a0000

crash> kmem 40009934f9a0
   VMAP_AREA         VM_STRUCT                 ADDRESS RANGE                SIZE
ffff400003c94100  ffff400017ed1100  ffff80003a160000 - ffff80003a180000   131072

 #4 [ffff80003a16fb00] tcp_recvmsg at ffff80001098419c

```

# CPU 14 中断栈

```
0xffff80001358e730:     0xffff80001358e814      0xffff00ff9ed2cce8
0xffff80001358e740:     0xffff20000a1b1880      0xffff20000a1b1880
0xffff80001358e750:     0xffff00ff9ed2cce8      0x0000000000000000
0xffff80001358e760:     0xffff00ff9ed2ccac      0x0000000000000010
0xffff80001358e770:     0xffff80001358e7d0      0xffff800010931cf8
0xffff80001358e780:     0xffff00ff9ed2cc00      0x0000000000000040
0xffff80001358e790:     0x00000000000001c0      0xffff00ff9ed2ccac
0xffff80001358e7a0:     0xffff800011b78788      0xffff20000a1b1880
0xffff80001358e7b0:     0x0000000000000000      0x0000000000000000
0xffff80001358e7c0:     0xffff80001358e820      0xffff8000108ca5ac
0xffff80001358e7d0:     0xffff80001358e820      0xffff8000108ca5c0
0xffff80001358e7e0:     0xffff21008f5af600      0xffff00ff9ed2cc00
0xffff80001358e7f0:     0x0000000000000000      0xffff200005213000
0xffff80001358e800:     0xffff80001358e820      0xffff8000108caa50
0xffff80001358e810:     0xffff21008f5af600      0x6bc181616e34f700
0xffff80001358e820:     0xffff80001358e8b0      0xffff8000108cadec
0xffff80001358e830:     0xffff21008f5af600      0xffff7000117f0680
0xffff80001358e840:     0x0000000000000001      0x00000000000005dc
0xffff80001358e850:     0x0000000000000000      0x0000000000000000
0xffff80001358e860:     0x0000000000000008      0xffff210077d5c2a4
0xffff80001358e870:     0xffff80001358e8d0      0xffff80000907113c
0xffff80001358e880:     0x0000000000280012      0xffff800011b78788
0xffff80001358e890:     0xffff21008f5a3a00      0xffff60000f6d7000
0xffff80001358e8a0:     0xffff80001358e8d0      0xffff800009071268
0xffff80001358e8b0:     0xffff80001358e910      0xffff80000907113c
0xffff80001358e8c0:     0x0000000000280012      0xffff800011b78788
0xffff80001358e8d0:     0xffff21008f5a0000      0xffff60000f6d7000
0xffff80001358e8e0:     0xffff80001358e910      0xffff800009071268<
0xffff80001358e8f0:     0x0000000000000050      0xffff800011b78788
0xffff80001358e900:     0x0000010124b6b48e      0x6bc181616e34f700
0xffff80001358e910:     0xffff80001358e9c0      0xffff80000907208c<i40e_lan_xmit_frame+68>
0xffff80001358e920:     0xffff21008f5a0000      0xffff60000f6d7000
0xffff80001358e930:     0x0000000000000042      0xffff60000f751880
0xffff80001358e940:     0xffff800011b86918      0x0000000000000000
0xffff80001358e950:     0xffff800011b71980      0xffff80001178f000
0xffff80001358e960:     0xffff80001178c5c0      0xffff21008f5a0000
0xffff80001358e970:     0xffff600012c604f0      0xffff210000000000
0xffff80001358e980:     0x000000281358e9f0      0xffff8000108c9b08
0xffff80001358e990:     0xffff21008f5a0000      0x000190021fd548b3
0xffff80001358e9a0:     0xffff00ff9ed23cac      0xffff600005188000
0xffff80001358e9b0:     0xffff80000658e9f0      0x6bc181616e34f700




0xffff80001358e9c0:     0xffff80001358ea20      0xffff800010930e38<dequeue_skb+120>
0xffff80001358e9d0:     0xffff00ff9ed23c00      0xffff00ff9ed23c80
0xffff80001358e9e0:     0xffff80001358eac4      0xffff00ff9ed23ce8
0xffff80001358e9f0:     0xffff60000f751880      0xffff60000f751880
0xffff80001358ea00:     0xffff00ff9ed23ce8      0x0000000000000000
0xffff80001358ea10:     0xffff00ff9ed23cac      0x0000000000000007

[dequeue_skb]: 96

0xffff80001358ea20:     0xffff80001358ea80      0xffff800010931cf8<__qdisc_run+160>
0xffff80001358ea30:     0xffff00ff9ed23c00      0x0000000000000040
0xffff80001358ea40:     0x00000000000001c0      0xffff00ff9ed23cac
0xffff80001358ea50:     0xffff800011b78788      0xffff60000f751880
0xffff80001358ea60:     0x0000000000000000      0x0000000000000000
0xffff80001358ea70:     0xffff80001358ead0      0xffff8000108ca5ac<

[__qdisc_run]: 80

0xffff80001358ea80:     0xffff80001358ead0      0xffff8000108ca5c0<__dev_queue_xmit+720>
0xffff80001358ea90:     0xffff21008f5a0000      0xffff00ff9ed23c00
0xffff80001358eaa0:     0x0000000000000000      0xffff600005188000
0xffff80001358eab0:     0xffff80001358ead0      0xffff8000108caa50
0xffff80001358eac0:     0xffff21008f5a0000      0x6bc181616e34f700

[__dev_queue_xmit]: 144

0xffff80001358ead0:     0xffff80001358eb60      0xffff8000108cadec<dev_queue_xmit+36>
0xffff80001358eae0:     0xffff21008f5a0000      0xffff7000091ea280
0xffff80001358eaf0:     0x0000000000000001      0x00000000000005dc
0xffff80001358eb00:     0x0000000000000000      0x0000000000000000
0xffff80001358eb10:     0x0000000000000008      0xffff10001acb3c1c
0xffff80001358eb20:     0xffff21008f5a0000      0xffff80000a553150
0xffff80001358eb30:     0xffff7000162e0e00      0x00000000003e3c80
0xffff80001358eb40:     0x0000000000000000      0xffff80000a55396c
0xffff80001358eb50:     0x0000000000000002      0x6bc181616e34f700

[dev_queue_xmit]: 32
x19 为sk_buff

0xffff80001358eb60:     0xffff80001358eb80      0xffff80000a555674<ovs_vport_send+164>
0xffff80001358eb70:     0xffff21008f5a0000[x19] 0xffff80000a542348

[ovs_vport_send] : 48 
  x19 为 sk_buff

0xffff80001358eb80:     0xffff80001358ebb0      0xffff80000a5407f4<do_output+100> [x0->x19]:sk_buff
0xffff80001358eb90:     0xffff21008f5a0000[x19] 0xffff7000091ea280
0xffff80001358eba0:     0xffff30000dba6b80      0xffff8be00402d6d8

[do_output]: 64
0xffff80001358ebb0:     0xffff80001358ebf0      0xffff80000a542348<do_execute_actions+3144> -- [x0->x20] [x1->x19]:sk_buff
0xffff80001358ebc0:     0xffff8be00402d6d8      0xffff80000a5601d0
0xffff80001358ebd0:     0xffff800011b78788      0xffff30000dba6b80
0xffff80001358ebe0:     0x0000000000000008      0xffff8be00402d6d8

[do_execute_actions] : 432

[x23] 为sk_buff

0xffff80001358ebf0:     0xffff80001358eda0      0xffff80000a542658<ovs_execute_actions+112>
0xffff80001358ec00:     0xffff80001178fb90      0x0000000000000002
0xffff80001358ec10:     0xffff30000dba6b80      0xffff10001acb3c00
0xffff80001358ec20:     0xffff21008f5a0000[x23] 0xffff8be00402d6d8
0xffff80001358ec30:     0xffff700003f31b80      0x0000000000000002
0xffff80001358ec40:     0xffff21008f5a0000      0x0000000000000008
0xffff80001358ec50:     0xffff21008f5a9b00      0xffff10001acb3c1c
0xffff80001358ec60:     0xffff800011700018      0x00000000000005dc
0xffff80001358ec70:     0x0000000000000000      0x0000000000000000
0xffff80001358ec80:     0x0000000000000000      0x000000e50001000a
0xffff80001358ec90:     0x0000000000000002      0x0000000000000000
0xffff80001358eca0:     0x0000000028100081      0x0000000000000008
0xffff80001358ecb0:     0x0000000000000000      0x0000000000000000
0xffff80001358ecc0:     0x0000000000000000      0x000000240001000a
0xffff80001358ecd0:     0x0000000000000002      0x0000000000000000
0xffff80001358ece0:     0x0000000028100081      0x0000000000000008
0xffff80001358ecf0:     0x0000000000000000      0x0000000000000000
0xffff80001358ed00:     0xffff80001358ed10      0xffff80001034aa04
0xffff80001358ed10:     0xffff80001358ed60      0x6bc181616e34f700
0xffff80001358ed20:     0xffff80001358ed60      0xffff80000a5540fc
0xffff80001358ed30:     0x000000001f454fb7      0xffff8be00402dd78
0xffff80001358ed40:     0xffff80001358ed50      0xffff80001034aa04
0xffff80001358ed50:     0xffff80001358eda0      0xffff80000a54ad10<!!!!ovs_flow_stats_update+328> --> <kmem_cache_alloc_node> [x1->x23]
0xffff80001358ed60:     0xffff0100e273aaa0      0x0000000000000046
0xffff80001358ed70:     0xffff80001358edc0      0xffff80000b6da574
0xffff80001358ed80:     0xffff00ff9e437000      0x00000000000000c0
0xffff80001358ed90:     0x00000000000002e0      0x0000000000000080

[ovs_execute_actions]: 80
0xffff80001358eda0:     0x0000000000000058      0xffff80000a549834<ovs_dp_process_packet+156> --> <ovs_execute_actions>
0xffff80001358edb0:     0xffff80001358ee00      0x6bc181616e34f700
0xffff80001358edc0:     0xffff80001358ee00      0xffff80000b6da740
0xffff80001358edd0:     0xffff80001358ef18      0xffff70001420d000
0xffff80001358ede0:     0x0000000000000010      0x000000000000000b

0xffff80001358edf0:     0xffff00ff80051c00      0xffff00ff8146af46
0xffff80001358ee00:     0xffff80001358ee20      0xffff80000b6da968
0xffff80001358ee10:     0xffff70001420d000      0xffff80001358ef18

...
HOLE
...

--- cpu 14 异常栈顶
 #0 [ffff80001358f930] crash_kexec at ffff8000101c4f9c
    ffff80001358f930: ffff80001358f960 ffff80001008ccf0
    ffff80001358f940: ffff8000121000f8 0000000000000000
    ffff80001358f950: ffff80001358fa60 6bc181616e34f700
```

# command
```
mod -s nf_conntrack 
mod -s nf_nat 
mod -s udp_tunnel 
mod -s libcrc32c 
mod -s nf_defrag_ipv6
```
