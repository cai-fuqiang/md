# vmcore
##  bt 
```
 #7 [ffff800013b6f9a0] tcp_in_window at ffff80000a76b594 [nf_conntrack]
 #8 [ffff800013b6fa80] nf_conntrack_tcp_packet at ffff80000a76b9ac [nf_conntrack]
 #9 [ffff800013b6fb30] nf_conntrack_in at ffff80000a7641b8 [nf_conntrack]
#10 [ffff800013b6fbe0] ipv4_conntrack_in at ffff80000a7690e4 [nf_conntrack]
#11 [ffff800013b6fc00] nf_hook_slow at ffff800010963588
#12 [ffff800013b6fc40] ip_rcv at ffff800010970d0c
#13 [ffff800013b6fcb0] __netif_receive_skb_core at ffff8000108cb908
#14 [ffff800013b6fd60] __netif_receive_skb at ffff8000108cc144
#15 [ffff800013b6fd80] process_backlog at ffff8000108cde64
#16 [ffff800013b6fde0] __napi_poll at ffff8000108cd768
#17 [ffff800013b6fe20] net_rx_action at ffff8000108cdd40
#18 [ffff800013b6fed0] __softirqentry_text_start at ffff800010081aa4
#19 [ffff800013b6ff60] irq_exit_rcu at ffff8000100f9c30
#20 [ffff800013b6ff80] irq_exit at ffff8000100f9c58
#21 [ffff800013b6ff90] __handle_domain_irq at ffff8000101742a4
#22 [ffff800013b6ffd0] gic_handle_irq at ffff800010081770
```

## bt -f
```
    ffff800013b6f980: ffff800013b6f9a0 ffff80000a76b598
    ffff800013b6f990: ffff800013b6f9b0 ffff8000108c6238
 #7 [ffff800013b6f9a0] tcp_in_window at ffff80000a76b594 [nf_conntrack]
    ffff800013b6f9a0: ffff800013b6fa80 ffff80000a76b9b0
    ffff800013b6f9b0: ffff30005c0d6400[x19] ffff3000177b7864[x20]
    ffff800013b6f9c0: ffff800011b78788 ffff80000a7641bc
    ffff800013b6f9d0: 0000000000000003 0000000000000002
    ffff800013b6f9e0: ffff80001203e740[x25] ffff3000651a2e00[x26]
    ffff800013b6f9f0: 0000000000000014[x27] 0000000000000003
    ffff800013b6fa00: ffff800013b6fa40 005e055810965dac
    ffff800013b6fa10: 41b4ecfd005e054c ffff30005c0d6400
    ffff800013b6fa20: ffff800011b78788 ffff3000651a2e00
    ffff800013b6fa30: 000000140000015c ffff80001203e740
    ffff800013b6fa40: 0000015c00000000 0000000c00000003 
    ffff800013b6fa50: ffff30005c0d6400 ffff3000177b7864
    ffff800013b6fa60: ffff800011b78788 ffff80000a7641bc
    ffff800013b6fa70: ffff800013b6fc78 9fe757b2a7ebda00
 #8 [ffff800013b6fa80] nf_conntrack_tcp_packet at ffff80000a76b9ac [nf_conntrack]

```

`tcp_in_window` 反汇编
```
/usr/src/debug/kernel-4.18.0-372.19.1.el8_6/linux-4.18.0-372.19.1.es8_10.aarch64/net/netfilter/nf_conntrack_proto_tcp.c: 497
0xffff80000a76ae20 <tcp_in_window>:     sub     sp, sp, #0xf0
0xffff80000a76ae24 <tcp_in_window+4>:   stp     x29, x30, [sp,#16]
0xffff80000a76ae28 <tcp_in_window+8>:   add     x29, sp, #0x10
0xffff80000a76ae2c <tcp_in_window+12>:  stp     x19, x20, [sp,#32]
0xffff80000a76ae30 <tcp_in_window+16>:  stp     x21, x22, [sp,#48]
0xffff80000a76ae34 <tcp_in_window+20>:  stp     x23, x24, [sp,#64]
0xffff80000a76ae38 <tcp_in_window+24>:  stp     x25, x26, [sp,#80]
0xffff80000a76ae3c <tcp_in_window+28>:  stp     x27, x28, [sp,#96]

```
`nf_conntrack_tcp_packet` 反汇编
```
0xffff80000a76b984 <nf_conntrack_tcp_packet+404>:       mov     w23, w0
0xffff80000a76b988 <nf_conntrack_tcp_packet+408>:       mov     w2, w7
0xffff80000a76b98c <nf_conntrack_tcp_packet+412>:       mov     w5, w27
0xffff80000a76b990 <nf_conntrack_tcp_packet+416>:       mov     x6, x20
0xffff80000a76b994 <nf_conntrack_tcp_packet+420>:       mov     x4, x26  --> sk_buff
0xffff80000a76b998 <nf_conntrack_tcp_packet+424>:       mov     w3, w28
0xffff80000a76b99c <nf_conntrack_tcp_packet+428>:       add     x1, x19, #0xc0
0xffff80000a76b9a0 <nf_conntrack_tcp_packet+432>:       mov     x0, x19
0xffff80000a76b9a4 <nf_conntrack_tcp_packet+436>:       stp     w7, w9, [sp,#108]
0xffff80000a76b9a8 <nf_conntrack_tcp_packet+440>:       str     x10, [sp,#120]
0xffff80000a76b9ac <nf_conntrack_tcp_packet+444>:       bl      0xffff80000a76ae20 <tcp_in_window>
```

* **sk_buff**: x26 为 x4  --> `ffff3000651a2e00`
* **dataoff**: x27 为 x5 --> `0x14`
* **tcph**: x20 为 x6 --> `ffff3000177b7864`

```
tcp_sack {
  ...
  int length = (tcph->doff*4) - sizeof(struct tcphdr);
  ...
  ptr = skb_header_pointer(skb,
                           dataoff + sizeof(struct tcphdr),  --> 0x14 + 20 = 40
                           length,  8 * 4 - 20 = 12
                           buff);
  {
     skb_header_pointer
       __skb_header_pointer
       { 
          .offset = 40
          .hlen = skb->len(40) - skb->data_len(0) = 40
          .len = 12

          if (hlen - offset >= len) //40 - 40 = 0 < 12 不满足
                  return data + offset;                    
                                                           
          if (!skb ||
              skb_copy_bits(skb, offset, buffer, len) < 0) 
                  return NULL;                             
                                                           
          return buffer;
       }
  }
}

skb_copy_bits
{
  .offset = 40
  .len = 12

  if (offset > (int)skb->len - len)   // 40 - 12 不满足
        goto fault;               
}
```

dataoff 产生
```
unsigned int
nf_conntrack_in(struct sk_buff *skb, const struct nf_hook_state *state)
{
        ...
        dataoff = get_l4proto(skb, skb_network_offset(skb), state->pf, &protonum);

        ...

        ret = nf_conntrack_handle_packet(ct, skb, dataoff, ctinfo, state);

        ...
}
```

```
nf_conntrack_in
  get_l4proto
```

## iphdr
```
crash> struct iphdr 0xffff3000177b7850
struct iphdr {
  ihl = 5 '\005',   --> 规定了额外的长度, 5表示 5个4byte 5 << 2 = 20
  version = 4 '\004',
  tos = 0 '\000',
  tot_len = 47618,  --> 698
  id = 18334,
  frag_off = 64,
  ttl = 64 '@',
  protocol = 6 '\006',  --> tcp
  check = 41204,
  saddr = 35563572, ==> 
  daddr = 69118004
}
```

# NOTE
## nf_contrack_in -> nf_conntrack_proto_tcp 分析
```
0xffff80000a7641a4 <nf_conntrack_in+1324>:      mov     x4, x20
0xffff80000a7641a8 <nf_conntrack_in+1328>:      mov     w3, w26
0xffff80000a7641ac <nf_conntrack_in+1332>:      mov     w2, w22
0xffff80000a7641b0 <nf_conntrack_in+1336>:      mov     x1, x27
0xffff80000a7641b4 <nf_conntrack_in+1340>:      mov     x0, x19
0xffff80000a7641b8 <nf_conntrack_in+1344>:      bl      0xffff80000a76b7f0 <nf_conntrack_tcp_packet>


crash> dis nf_conntrack_tcp_packet
0xffff80000a76b7f0 <nf_conntrack_tcp_packet>:   stp     x29, x30, [sp,#-176]!
0xffff80000a76b7f4 <nf_conntrack_tcp_packet+4>: mov     x29, sp
0xffff80000a76b7f8 <nf_conntrack_tcp_packet+8>: stp     x19, x20, [sp,#16]
0xffff80000a76b7fc <nf_conntrack_tcp_packet+12>:        stp     x21, x22, [sp,#32]
0xffff80000a76b800 <nf_conntrack_tcp_packet+16>:        stp     x23, x24, [sp,#48]
0xffff80000a76b804 <nf_conntrack_tcp_packet+20>:        stp     x25, x26, [sp,#64]
0xffff80000a76b808 <nf_conntrack_tcp_packet+24>:        stp     x27, x28, [sp,#80]

 #8 [ffff800013b6fa80] nf_conntrack_tcp_packet at ffff80000a76b9ac [nf_conntrack]
    ffff800013b6fa80: ffff800013b6fb30 ffff80000a7641bc[29]
    ffff800013b6fa90: ffff30005c0d6400 ffff800013b6fc78[20]
    ffff800013b6faa0: 0000000000000000 0000000000000014[22] --> dataoff
    ffff800013b6fab0: ffff80000a7930f8 ffff800011b78788[24]
    ffff800013b6fac0: ffff80000a782e48 0000000000000002[26]
    ffff800013b6fad0: ffff3000651a2e00 00000000c2303ae1[28]
    ffff800013b6fae0: ffff80000a782e48 0000000000000000
    ffff800013b6faf0: ffff800000000000 0000000000000000
    ffff800013b6fb00: 00000000041ea834 0000000000000000
    ffff800013b6fb10: 031ea8340002e40c 0000000000000000
    ffff800013b6fb20: 0106ce8500000000 9fe757b2a7ebda00
```
# TCP option
```
crash> x/100xb 0xffff3000177b7878                                                          
0xffff3000177b7878:     0x01    0x01    0x08    0x0a    0x3b    0x8e    0xf3    0xb6
0xffff3000177b7880:     0xce    0xc0    0x88    0x55    0x00    0x00    0x00    0x00
0xffff3000177b7888:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b7890:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b7898:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b78a0:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b78a8:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b78b0:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b78b8:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b78c0:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b78c8:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b78d0:     0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
0xffff3000177b78d8:     0x00    0x00    0x00    0x00
```

* 0x01:
  + kind = 1: NOP
* 0x01:
  + kind = 1: NOP
* 0x8:
  + kind = 8: timestamps
  + len = 0xa


# tcp_in_window
param
```
0xffff80000a76b988 <nf_conntrack_tcp_packet+408>:       mov     w2, w7          dir    
0xffff80000a76b98c <nf_conntrack_tcp_packet+412>:       mov     w5, w27         dataoff
0xffff80000a76b990 <nf_conntrack_tcp_packet+416>:       mov     x6, x20         tcph
0xffff80000a76b994 <nf_conntrack_tcp_packet+420>:       mov     x4, x26         skb
0xffff80000a76b998 <nf_conntrack_tcp_packet+424>:       mov     w3, w28         index  
0xffff80000a76b99c <nf_conntrack_tcp_packet+428>:       add     x1, x19, #0xc0  state
0xffff80000a76b9a0 <nf_conntrack_tcp_packet+432>:       mov     x0, x19         ct
0xffff80000a76b9a4 <nf_conntrack_tcp_packet+436>:       stp     w7, w9, [sp,#108]
0xffff80000a76b9a8 <nf_conntrack_tcp_packet+440>:       str     x10, [sp,#120]
0xffff80000a76b9ac <nf_conntrack_tcp_packet+444>:       bl      0xffff80000a76ae20 <tcp_in_window>

0xffff80000a76ae20 <tcp_in_window>:     sub     sp, sp, #0xf0
0xffff80000a76ae24 <tcp_in_window+4>:   stp     x29, x30, [sp,#16]
0xffff80000a76ae28 <tcp_in_window+8>:   add     x29, sp, #0x10
0xffff80000a76ae2c <tcp_in_window+12>:  stp     x19, x20, [sp,#32]
0xffff80000a76ae30 <tcp_in_window+16>:  stp     x21, x22, [sp,#48]
0xffff80000a76ae34 <tcp_in_window+20>:  stp     x23, x24, [sp,#64]
0xffff80000a76ae38 <tcp_in_window+24>:  stp     x25, x26, [sp,#80]
0xffff80000a76ae3c <tcp_in_window+28>:  stp     x27, x28, [sp,#96]

 #7 [ffff800013b6f9a0] tcp_in_window at ffff80000a76b594 [nf_conntrack]
    ffff800013b6f9a0: ffff800013b6fa80          ffff80000a76b9b0 [29,30]
    ffff800013b6f9b0: ffff30005c0d6400[ct]      ffff3000177b7864[tcph]
    ffff800013b6f9c0: ffff800011b78788          ffff80000a7641bc [21,22]
    ffff800013b6f9d0: 0000000000000003          0000000000000002  [23,24]
    ffff800013b6f9e0: ffff80001203e740          ffff3000651a2e00[skb]
    ffff800013b6f9f0: 0000000000000014[dataoff] 0000000000000003[index]
    ffff800013b6fa00: ffff800013b6fa40 005e055810965dac
    ffff800013b6fa10: 41b4ecfd005e054c ffff30005c0d6400
    ffff800013b6fa20: ffff800011b78788 ffff3000651a2e00
    ffff800013b6fa30: 000000140000015c ffff80001203e740
    ffff800013b6fa40: 0000015c00000000 0000000c00000003
    ffff800013b6fa50: ffff30005c0d6400 ffff3000177b7864
    ffff800013b6fa60: ffff800011b78788 ffff80000a7641bc
    ffff800013b6fa70: ffff800013b6fc78 9fe757b2a7ebda00

```

# 其他
```
mlx5e_complete_rx_cqe
```

# napi_poll
```
#15 [ffff800013b6fd80] process_backlog at ffff8000108cde64
#16 [ffff800013b6fde0] __napi_poll at ffff8000108cd768
#17 [ffff800013b6fe20] net_rx_action at ffff8000108cdd40

0xffff8000108cdd3c <net_rx_action+684>: mov     x0, x27
0xffff8000108cdd40 <net_rx_action+688>: bl      0xffff8000108cd728 <__napi_poll>

0xffff8000108cd760 <__napi_poll+56>:    mov     w1, w21
0xffff8000108cd764 <__napi_poll+60>:    mov     x0, x20  //x20
0xffff8000108cd768 <__napi_poll+64>:    blr     x2


0xffff8000108cddc0 <process_backlog>:   stp     x29, x30, [sp,#-96]!
0xffff8000108cddc4 <process_backlog+4>: mov     x29, sp
0xffff8000108cddc8 <process_backlog+8>: stp     x19, x20, [sp,#16]
0xffff8000108cddcc <process_backlog+12>:        stp     x21, x22, [sp,#32]
0xffff8000108cddd0 <process_backlog+16>:        stp     x23, x24, [sp,#48]
0xffff8000108cddd4 <process_backlog+20>:        stp     x25, x26, [sp,#64]
0xffff8000108cddd8 <process_backlog+24>:        stp     x27, x28, [sp,#80]
0xffff8000108cdddc <process_backlog+28>:        mov     x27, x0

#15 [ffff800013b6fd80] process_backlog at ffff8000108cde64
    ffff800013b6fd80: ffff800013b6fde0 ffff8000108cd76c [x29,x30]
    ffff800013b6fd90: 0000000000000001 ffff310effe5f810 [x19,x20 napi_poll]
    ffff800013b6fda0: 0000000000000040 ffff800013b6fea8 [x21,x22]
    ffff800013b6fdb0: ffff800013b6fea7 0000000101008571 [x23,x24]
    ffff800013b6fdc0: ffff800013b6feb8 ffff800011b71000 [x25,x26]
    ffff800013b6fdd0: ffff310effe5f810 00000000ffffffff [x27 napi_poll,x28]
```

# irqaction
```
crash> irq |grep mlx |grep mlx5_comp61   # 因为有6块网卡
 IRQ   IRQ_DESC/_DATA      IRQACTION      NAME
 611  ffff00ff8a17da00  ffff00ff90e09500  "mlx5_comp61@pci:0000:04:00.0"
 675  ffff00ff8d929e00  ffff00ff8e712200  "mlx5_comp61@pci:0000:04:00.1"
 739  ffff10000311b200  ffff10000d789900  "mlx5_comp61@pci:0001:01:00.0"
 804  ffff100002f58400  ffff10000ff6ab00  "mlx5_comp61@pci:0001:01:00.1"
 868  ffff500003448c00  ffff50000a18ee80  "mlx5_comp61@pci:0005:01:00.0"
 933  ffff500002048c00  ffff50000bd8c180  "mlx5_comp61@pci:0005:01:00.1"
crash> irqaction ffff00ff90e09500
struct irqaction {
  handler = 0xffff800009710578 <irq_int_handler>,
  dev_id = 0xffff00ff90e05a08,
  percpu_dev_id = 0x0,
  next = 0x0,
  thread_fn = 0x0,
  thread = 0x0,
  secondary = 0x0,
  irq = 611,
  flags = 0,
  thread_flags = 0,
  thread_mask = 0,
  name = 0xffff00ff90e05a20 "mlx5_comp61@pci:0000:04:00.0",
  dir = 0xffff00ff93aa3b00
}
```

* irq handler register
```
probe_one
mlx5_init_one
mlx5_load
mlx5_eq_table_create
create_comp_eqs
create_map_eq
mlx5_irq_request
irq_pool_request_affinity
irq_pool_create_irq
irq_request
err = request_irq(irq->irqn, irq_int_handler, 0, irq->name, 
                  &irq->nh);
```
* attach nb
```
mlx5_irq_attach_nb
```

# devlink
```
crash> list devlink.list -H devlink_list -s devlink.dev,ops
ffff00ff90600000
  dev = 0xffff3000035720b8
  ops = 0xffff8000097d4028 <mlx5_devlink_ops>
ffff00ff91780000
  dev = 0xffff3000035490b8
  ops = 0xffff8000097d4028 <mlx5_devlink_ops>
ffff10000c080000
  dev = 0xffff3000035620b8
  ops = 0xffff8000097d4028 <mlx5_devlink_ops>
ffff10000e600000
  dev = 0xffff30000355c0b8
  ops = 0xffff8000097d4028 <mlx5_devlink_ops>
ffff500007f00000
  dev = 0xffff100003c1b0b8
  ops = 0xffff8000097d4028 <mlx5_devlink_ops>
ffff50000a380000
  dev = 0xffff100003c080b8
  ops = 0xffff8000097d4028 <mlx5_devlink_ops>

crash> device.kobj.name 0xffff3000035720b8
  kobj.name = 0xffff100003b8f400 "0000:04:00.0",

```

# mlx5_core_dev.priv
```
crash> struct mlx5_core_dev 0xffff00ff90600180 -o |grep priv
  [ffff00ff90642780] struct mlx5_priv priv;
crash> mlx5_priv.eq_table ffff00ff90642780
  eq_table = 0xffff00ff80296000

crash> mlx5_eq_table  0xffff00ff80296000 -o |head -2
struct mlx5_eq_table {
  [ffff00ff80296000] struct list_head comp_eqs_list;

```
#
```
crash> list -H ffff00ff80296000 mlx5_eq_comp.list -s mlx5_eq_comp.irq_nb |head -10
ffff00ff84c3eb00
  irq_nb = {
    notifier_call = 0xffff8000096fb100 <mlx5_eq_comp_int>,
    next = 0x0,
    priority = 0
  }

crash> struct mlx5_eq_comp.core ffff00ff84c3eb00 -o
struct mlx5_eq_comp {
  [ffff00ff84c3eb00] struct mlx5_eq core;
}

struct mlx5_eq {
  [ffff00ff84c3eb38] struct mlx5_cq_table cq_table;
}

struct mlx5_cq_table {
  [ffff00ff84c3eb40] struct xarray tree;
}

crash> tree -t xarray ffff00ff84c3eb40
ffff00ff96c844f8
ffff00ff8a7504f8
ffff700001bbf4f8
ffff700001b8acf8
ffff100003770cf8
ffff00fff00d5978
ffff00fff00d3978
ffff00fff00d1a38
ffff00fff00d3db8
ffff00fff00d0138

crash> struct mlx5_core_cq.comp ffff00fff00d0138
  comp = 0xffff80000973ef38 <mlx5e_completion_event>
crash> struct mlx5e_cq -o |grep mcq
   [56] struct mlx5_core_cq mcq;


crash> eval ffff00fff00d0138-56
hexadecimal: ffff00fff00d0100
    decimal: 18446463697976885504  (-280375732666112)
      octal: 1777770017776003200400
     binary: 1111111111111111000000001111111111110000000011010000000100000000
crash> struct mlx5e_cq ffff00fff00d0100 |less
crash> struct mlx5e_cq.napi ffff00fff00d0100
  napi = 0xffff00fff00d3bd0
crash> struct napi_struct.poll 0xffff00fff00d3bd0
  poll = 0xffff80000973e7e8 <mlx5e_napi_poll>

crash> struct mlx5e_rq.cq -o
struct mlx5e_rq {
   [256] struct mlx5e_cq cq;
}

```


# sk_buff
```
crash> struct sk_buff ffff3000651a2e00 |grep -E 'tail|head|data|truesize'
  data_len = 0,
  head_frag = 1 '\001',
  headers_start = 0xffff3000651a2e90,
    reserved_tailroom = 0
  inner_transport_header = 0,
  inner_network_header = 0,
  inner_mac_header = 0,
  transport_header = 100,
  network_header = 80,
  mac_header = 66,
  headers_end = 0xffff3000651a2ec8,
  tail = 120,
  head = 0xffff3000177b7800 "\001",
  data = 0xffff3000177b7850 "E",
  truesize = 1408,
```

headroom = 0x50
cqe_bcnt = 40

# chksum
```
[40886.775572] skb linear:   00000000: 45 00 02 ba a3 2c 40 00 40 06 74 63 0a 98 05 bf
[40886.783853] skb linear:   00000010: 0a 98 05 c0 1a 9c 94 54 38 d3 10 d8 18 ca 3a 29
[40886.792107] skb linear:   00000020: 80 18 01 5c 47 19 00 00


crash> x/40xb 0xffff3000177b7850
0xffff3000177b7850:     0x45    0x00    0x02    0xba    0x9e    0x47    0x40    0x00
0xffff3000177b7858:     0x40    0x06    0xf4    0xa0    0x34    0xa8    0x1e    0x02
0xffff3000177b7860:     0x34    0xa8    0x1e    0x04    0x1a    0x92    0x9e    0xe4
0xffff3000177b7868:     0x00    0x5e    0x05    0x58    0x41    0xb4    0xec    0xfd
0xffff3000177b7870:     0x80    0x18    0x01    0x5c    0x5e    0x99    0x00    0x00
crash> tcphdr -o 0xffff3000177b7864
struct tcphdr {
  [ffff3000177b7864] __be16 source;
  [ffff3000177b7866] __be16 dest;
  [ffff3000177b7868] __be32 seq;
  [ffff3000177b786c] __be32 ack_seq;
  [ffff3000177b7870] __u16 res1 : 4;
  [ffff3000177b7870] __u16 doff : 4;
  [ffff3000177b7871] __u16 fin : 1;
  [ffff3000177b7871] __u16 syn : 1;
  [ffff3000177b7871] __u16 rst : 1;
  [ffff3000177b7871] __u16 psh : 1;
  [ffff3000177b7871] __u16 ack : 1;
  [ffff3000177b7871] __u16 urg : 1;
  [ffff3000177b7871] __u16 ece : 1;
  [ffff3000177b7871] __u16 cwr : 1;
  [ffff3000177b7872] __be16 window;
  [ffff3000177b7874] __sum16 check;
  [ffff3000177b7876] __be16 urg_ptr;

crash> tcphdr 0xffff3000177b7864
struct tcphdr {
  source = 37402,
  dest = 58526,
  seq = 1476746752,
  ack_seq = 4260148289,
  res1 = 0,
  doff = 8,
  fin = 0,
  syn = 0,
  rst = 0,
  psh = 1,
  ack = 1,
  urg = 0,
  ece = 0,
  cwr = 0,
  window = 23553,
  check = 39262,
  urg_ptr = 0
}
```

## dmesg
```
[168403.862085] br-storagepub: hw csum failure
[168403.868069] skb len=40 headroom=80 headlen=40 tailroom=712
                mac=(66,14) net=(80,20) trans=100
                shinfo(txflags=0 nr_frags=0 gso(size=0 type=0 segs=0))
                csum(0x73d6a68 ip_summed=2 complete_sw=0 valid=0 level=0)
                hash(0x46d8223c sw=0 l4=1) proto=0x0800 pkttype=0 iif=21
[168403.906316] dev name=br-storagepub feat=0x0x002000064fdd58e9
[168403.913887] skb linear:   00000000: 45 00 02 ba 9e 47 40 00 40 06 f4 a0 34 a8 1e 02
[168403.923706] skb linear:   00000010: 34 a8 1e 04 1a 92 9e e4 00 5e 05 58 41 b4 ec fd
[168403.933296] skb linear:   00000020: 80 18 01 5c 5e 99 00 00
[168403.940778] CPU: 61 PID: 16605 Comm: kubelet Kdump: loaded Tainted: G           OE  X --------- -  - 4.18.0-372.19.1.es8_10.aarch64 #1
[168403.956688] Hardware name: KaiTian KaiTian KR722f G2   /TAIFENG , BIOS KL4.2A.KR722f_G2.R.027.240430 04/30/2024 16:26:26
[168403.969630] Call trace:
[168403.974120]  dump_backtrace+0x0/0x158
[168403.979835]  show_stack+0x24/0x30
[168403.985175]  dump_stack+0x5c/0x74
[168403.990503]  netdev_rx_csum_fault.part.30+0x50/0x5c
[168403.997393]  netdev_rx_csum_fault+0x30/0x40
[168404.003568]  __skb_checksum_complete+0xd4/0xd8
[168404.010000]  nf_ip_checksum+0x84/0x148
[168404.015722]  nf_checksum+0x84/0xa0
[168404.021082]  nf_conntrack_tcp_packet+0x328/0xba8 [nf_conntrack]
[168404.029012]  nf_conntrack_in+0x544/0x6f8 [nf_conntrack]
[168404.036166]  ipv4_conntrack_in+0x28/0x38 [nf_conntrack]
[168404.043263]  nf_hook_slow+0x4c/0xe8
[168404.048548]  ip_rcv+0x338/0x3c0
[168404.053420]  __netif_receive_skb_core+0x3a4/0xb88
[168404.059870]  __netif_receive_skb+0x28/0x78
[168404.065696]  process_backlog+0xa8/0x1a0
[168404.071276]  __napi_poll+0x44/0x1b8
[168404.076498]  net_rx_action+0x2b4/0x330
[168404.081975]  __do_softirq+0x118/0x320
[168404.087521]  irq_exit_rcu+0x10c/0x120
[168404.092907]  irq_exit+0x14/0x20
[168404.097756]  __handle_domain_irq+0x70/0xc0
[168404.103572]  gic_handle_irq+0xd4/0x178
[168404.109023]  el0_irq_naked+0x50/0x58
```

```cpp
__sum16 __skb_checksum_complete(struct sk_buff *skb)
{
        __wsum csum;
        __sum16 sum;

        csum = skb_checksum(skb, 0, skb->len, 0);

        /* skb->csum holds pseudo checksum */
        sum = csum_fold(csum_add(skb->csum, csum));
        if (likely(!sum)) {
                if (unlikely(skb->ip_summed == CHECKSUM_COMPLETE) &&
                    !skb->csum_complete_sw)
                        netdev_rx_csum_fault(skb->dev, skb);        //---> here
        }

        if (!skb_shared(skb)) {
                /* Save full packet checksum */
                skb->csum = csum;
                skb->ip_summed = CHECKSUM_COMPLETE;
                skb->csum_complete_sw = 1;
                skb->csum_valid = !sum;
        }

        return sum;
}
```
# ipchecksum
```
[124129.288404] br-storagepub: hw csum failure
[124129.294238] skb len=52 headroom=80 headlen=52 tailroom=60
                mac=(66,14) net=(80,20) trans=100
                shinfo(txflags=0 nr_frags=0 gso(size=0 type=0 segs=0))
                csum(0x63d7668 ip_summed=2 complete_sw=0 valid=0 level=0)
                hash(0xc76a4177 sw=0 l4=1) proto=0x0800 pkttype=0 iif=21
[124129.331487] dev name=br-storagepub feat=0x0x002000064fdd58e9
[124129.338846] skb linear:   00000000: 45 00 00 34 5a d0 40 00 40 06 3a 9e 34 a8 1e 02
[124129.348238] skb linear:   00000010: 34 a8 1e 04 c3 02 1a aa a5 05 8c 7a 84 d5 5b 8b
[124129.357623] skb linear:   00000020: 80 10 01 5a 9a f8 00 00 01 01 08 0a 38 eb 55 f0
[124129.367008] skb linear:   00000030: cc 1c ea 8e

45 00 00 34 [len = 52]
5a d0 40 00 
40 06[TCP] 3a 9e 
34 a8 1e 02 [Src addr]
34 a8 1e 04 [Dst addr]

c3 02 1a aa a5 05 
8c 7a 84 d5 5b 8b
80 10 01 5a 9a f8 
00 00 01 01 08 0a 
38 eb 55 f0 cc 1c 
ea 8e



crash> struct skb_shared_info ffff3000177b7e40
struct skb_shared_info {
  frags = {{
      page = {
        p = 0xffffffebffe5dec0
      },
      page_offset = 32900,
      size = 646
    }, {
crash> kmem 0xffff3000177b7800
      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffffebffe5dec0 3000977b0000                0        0  3 6fffff8000000000
crash> ptov 3000977b0000
VIRTUAL           PHYSICAL
ffff3000177b0000  3000977b0000
crash> eval ffff3000177b0000+32900
hexadecimal: ffff3000177b8084    //head 0xffff3000177b7800  data 0xffff3000177b7850  tcp_data : ffff3000177b7884
```
# state
```
0xffff80000a7641a4 <nf_conntrack_in+1324>:      mov     x4, x20 [state]
0xffff80000a7641a8 <nf_conntrack_in+1328>:      mov     w3, w26
0xffff80000a7641ac <nf_conntrack_in+1332>:      mov     w2, w22 [dataoff]
0xffff80000a7641b0 <nf_conntrack_in+1336>:      mov     x1, x27 [skb]
0xffff80000a7641b4 <nf_conntrack_in+1340>:      mov     x0, x19
0xffff80000a7641b8 <nf_conntrack_in+1344>:      bl      0xffff80000a76b7f0 <nf_conntrack_tcp_packet>

0xffff80000a76b7f0 <nf_conntrack_tcp_packet>:   stp     x29, x30, [sp,#-176]!
0xffff80000a76b7f4 <nf_conntrack_tcp_packet+4>: mov     x29, sp
0xffff80000a76b7f8 <nf_conntrack_tcp_packet+8>: stp     x19, x20, [sp,#16]
0xffff80000a76b7fc <nf_conntrack_tcp_packet+12>:        stp     x21, x22, [sp,#32]
0xffff80000a76b800 <nf_conntrack_tcp_packet+16>:        stp     x23, x24, [sp,#48]
0xffff80000a76b804 <nf_conntrack_tcp_packet+20>:        stp     x25, x26, [sp,#64]
0xffff80000a76b808 <nf_conntrack_tcp_packet+24>:        stp     x27, x28, [sp,#80]

 #8 [ffff800013b6fa80] nf_conntrack_tcp_packet at ffff80000a76b9ac [nf_conntrack]
    ffff800013b6fa80: ffff800013b6fb30 ffff80000a7641bc
    ffff800013b6fa90: ffff30005c0d6400 ffff800013b6fc78[x20] --> state
    ffff800013b6faa0: 0000000000000000 0000000000000014[x22] --> dataoff  20
    ffff800013b6fab0: ffff80000a7930f8 ffff800011b78788
    ffff800013b6fac0: ffff80000a782e48 0000000000000002
    ffff800013b6fad0: ffff3000651a2e00[x27] --> skb 00000000c2303ae1
    ffff800013b6fae0: ffff80000a782e48 0000000000000000
    ffff800013b6faf0: ffff800000000000 0000000000000000
    ffff800013b6fb00: 00000000041ea834 0000000000000000
    ffff800013b6fb10: 031ea8340002e40c 0000000000000000
    ffff800013b6fb20: 0106ce8500000000 9fe757b2a7ebda00
```


# 参考链接

[sk_buff](http://vger.kernel.org/~davem/skb_data.html)

[tcp option](https://www.geeksforgeeks.org/options-field-in-tcp-header/)
