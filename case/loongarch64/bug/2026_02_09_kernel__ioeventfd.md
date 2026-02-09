## 内核版本

```
kernel-6.6.0-117.0.0.122.oe2403sp2
```

## 问题堆栈

### kvm_ioeventfd问题

出现了 `list_add corruption` 的WARN:

<details>
<summary>具体堆栈展开</summary>

```
[26771.080369] ------------[ cut here ]------------
[26771.081331] list_add corruption. next->prev should be prev (ffff800003f89348), but was 9000001af36780c0. (next=ffff800003f89348).
[26771.083207] WARNING: CPU: 0 PID: 71855 at lib/list_debug.c:29 __list_add_valid_or_report+0xf0/0x128
[26771.084157] Modules linked in: joydev nft_limit xt_limit veth xt_CHECKSUM xt_MASQUERADE xt_conntrack ipt_REJECT nf_reject_ipv4 nft_compat nft_chain_nat nf_tables bridge stp llc openvswitch nf_conncount nf_nat nf_conntrack nf_defrag_ipv6 nf_defrag_ipv4 vfio_pci vfio_pci_core irqbypass vfio_iommu_type1 vfio nbd tun rfkill cbd(OE) ipmi_ssif rpcrdma(OE) rdma_ucm(OE) nls_cp936 vfat fat ib_srpt(OE) ib_isert(OE) iscsi_target_mod rdma_cm(OE) iw_cm(OE) target_core_mod snd_hda_intel ib_umad(OE) snd_intel_dspcfg snd_hda_codec ib_ipoib(OE) ib_cm(OE) ib_iser(OE) snd_hda_core kvm acpi_ipmi snd_hwdep snd_pcm ipmi_si cdc_ether efi_pstore pstore usbnet ipmi_devintf ses loongarch_iommu snd_timer enclosure ipmi_msghandler i2c_ls2x sg fuse nfnetlink mlx5_ib(OE) macsec ib_uverbs(OE) ib_core(OE) sd_mod t10_pi crc64_rocksoft_generic crc64_rocksoft crc64 mlx5_core(OE) mlxfw(OE) tls megaraid_sas r8169 psample mlxdevm(OE) realtek mlx_compat(OE) dwmac_loongson dm_mirror dm_region_hash dm_log dm_multipath dm_mod ipv6 crc_ccitt
[26771.092542] CPU: 0 PID: 71855 Comm: CPU 0/KVM Kdump: loaded Tainted: G          IOE       6.6.0-117.0.0.122.loongarch64 #1
[26771.093824] Hardware name: SuperCloud R3210 L12/Loongson-TD622E0, BIOS Loongson-FwSdk-V5.0.0030-beta 11/18/25 10:07:12
[26771.095005] pc 9000000000c29ab0 ra 9000000000c29ab0 tp 9000001b7e9c4000 sp 9000001b7e9c7d70
[26771.095605] a0 0000000000000075 a1 9000000008000708 a2 9000000008024e40 a3 90000000077d2990
[26771.096199] a4 90000000077d2988 a5 9000001b7e9c7b60 a6 0000000000000001 a7 0000000000000001
[26771.096794] t0 e55752379325ff98 t1 e55752379325ff98 t2 0000000000000000 t3 9000001b7e9c7a58
[26771.097387] t4 fffffffffffffffe t5 0000000000000000 t6 0000000000000000 t7 0000000000000000
[26771.098078] t8 0000000000000001 u0 90000000017227d0 s9 0000000000000000 s0 9000001a9c5c09c0
[26771.098665] s1 ffff800003f89348 s2 ffff800003f89348 s3 9000101a6abc4000 s4 9000101a6abc4008
[26771.099245] s5 00007fffdfc75a58 s6 000055558c6d0680 s7 00007ffff0837cd8 s8 00000000000000ff
[26771.099821]    ra: 9000000000c29ab0 __list_add_valid_or_report+0xf0/0x128
[26771.100399]   ERA: 9000000000c29ab0 __list_add_valid_or_report+0xf0/0x128
[26771.100964]  CRMD: 000000b0 (PLV0 -IE -DA +PG DACF=CC DACM=CC -WE)
[26771.101572]  PRMD: 00000004 (PPLV0 +PIE -PWE)
[26771.102187]  EUEN: 00000007 (+FPE +SXE +ASXE -BTE)
[26771.102745]  ECFG: 00071c7d (LIE=0,2-6,10-12 VS=7)
[26771.103289] ESTAT: 000c0000 [BRK] (IS= ECode=12 EsubCode=0)
[26771.103827]  PRID: 0014d011 (Loongson-64bit, Loongson-3C6000/D)
[26771.104360] CPU: 0 PID: 71855 Comm: CPU 0/KVM Kdump: loaded Tainted: G          IOE       6.6.0-117.0.0.122.loongarch64 #1
[26771.105417] Hardware name: SuperCloud R3210 L12/Loongson-TD622E0, BIOS Loongson-FwSdk-V5.0.0030-beta 11/18/25 10:07:12
[26771.106589] Stack : 9000000001ae37d0 0000000000000000 90000000002243f4 9000001b7e9c4000
[26771.107133]         9000001b7e9c79c0 9000001b7e9c79c8 0000000000000000 9000001b7e9c7b08
[26771.107671]         9000001b7e9c7b00 9000001b7e9c7b00 9000001b7e9c78e0 0000000000000001
[26771.108200]         0000000000000001 9000001b7e9c79c8 e55752379325ff98 900000010586b680
[26771.108726]         9000001b7e9c77d8 fffffffffffffffe 0000000000000000 0000000000000000
[26771.109253]         0000000000000000 0000000000000001 0000000006904000 0000000000000000
[26771.109869]         0000000000000000 0000000000000000 9000000001ae37d0 9000000002391000
[26771.110377]         9000000000c29ab0 000000000000001d 000055558c6d0680 00007ffff0837cd8
[26771.110882]         00000000000000ff 0000000000000000 900000000022440c 0000000040e00014
[26771.111384]         00000000000000b0 0000000000000004 0000000000000007 0000000000071c7d
[26771.111876]         ...
[26771.112350] Call Trace:
[26771.112353] [<900000000022440c>] show_stack+0x64/0x188
[26771.113276] [<90000000015cbd40>] dump_stack_lvl+0x5c/0x88
[26771.113805] [<900000000024a440>] __warn+0x88/0x148
[26771.114252] [<90000000015589e8>] report_bug+0x1f8/0x2b8
[26771.114690] [<90000000015cc690>] do_bp+0x258/0x3e0
[26771.115123] [<9000000000222980>] handle_bp+0x120/0x1c0
[26771.115538] [<9000000000c29ab0>] __list_add_valid_or_report+0xf0/0x128
[26771.115953] [<ffff800003f6fd18>] kvm_ioeventfd+0x220/0x2a8 [kvm]
[26771.116869] [<ffff800003f6c800>] kvm_vm_ioctl+0xfd8/0x1258 [kvm]
[26771.117387] [<90000000005f8458>] sys_ioctl+0xc8/0x150
[26771.117855] [<90000000015cd028>] do_syscall+0x88/0xd0
[26771.118253] [<9000000000221f18>] handle_syscall+0xb8/0x158
```

</details>

分析`kvm_ioeventfd`跳转位置:

```
/usr/src/debug/kernel-6.6.0-117.0.0.122.loongarch64/linux-6.6.0-117.0.0.122.loongarch64/arch/loongarch/kvm/../../../virt/kvm/eventfd.c: 1092
0xffff800003f6fd00 <kvm_ioeventfd+520>: addi.d          $t0, $a0, 24(0x18)
/usr/src/debug/kernel-6.6.0-117.0.0.122.loongarch64/linux-6.6.0-117.0.0.122.loongarch64/./include/linux/list.h: 38
0xffff800003f6fd04 <kvm_ioeventfd+524>: st.d            $a0, $a0, 8(0x8)
/usr/src/debug/kernel-6.6.0-117.0.0.122.loongarch64/linux-6.6.0-117.0.0.122.loongarch64/./include/linux/list.h: 37
0xffff800003f6fd08 <kvm_ioeventfd+528>: st.d            $t0, $a0, 24(0x18)
/usr/src/debug/kernel-6.6.0-117.0.0.122.loongarch64/linux-6.6.0-117.0.0.122.loongarch64/./include/linux/list.h: 38
0xffff800003f6fd0c <kvm_ioeventfd+532>: move            $a2, $s1
0xffff800003f6fd10 <kvm_ioeventfd+536>: st.d            $t0, $a0, 32(0x20)
/usr/src/debug/kernel-6.6.0-117.0.0.122.loongarch64/linux-6.6.0-117.0.0.122.loongarch64/./include/linux/list.h: 88
0xffff800003f6fd14 <kvm_ioeventfd+540>: move            $a1, $s2
0xffff800003f6fd18 <kvm_ioeventfd+544>: bl              72600(0x11b98)  # 0xffff800003f818b0 <kvm_loongarch_ipi_create+1944>
```

堆栈回溯中显示的bl的地址时错误的. 跳转地址怎么算的呢?

```
PC + offset = 0xffff800003f6fd18 + 0x11b98 = 0xffff800003f818b0

crash> p (char *)(0xffff800003f6fd18+0x11b98)
$3 = 0xffff800003f818b0 <kvm_loongarch_ipi_create+1944> "-\205\001\024\r"
```

不知道哪个地方除了问题, 如果单从`list.h:88`报告的行数来说:

```cpp
    23   static __always_inline bool __list_add_valid(struct list_head *new,
    22   │ > > > >      struct list_head *prev,
    21   │ > > > >      struct list_head *next)
    20   {
    19   │ bool ret = true;
    18   │
    17   │ if (!IS_ENABLED(CONFIG_DEBUG_LIST)) {
    16   │ │ /*
    15   │ │  * With the hardening version, elide checking if next and prev
    14   │ │  * are NULL, since the immediate dereference of them below would
    13   │ │  * result in a fault if NULL.
    12   │ │  *
    11   │ │  * With the reduced set of checks, we can afford to inline the
    10   │ │  * checks, which also gives the compiler a chance to elide some
     9   │ │  * of them completely if they can be proven at compile-time. If
     8   │ │  * one of the pre-conditions does not hold, the slow-path will
     7   │ │  * show a report which pre-condition failed.
     6   │ │  */
     5   │ │ if (likely(next->prev == prev && prev->next == next && new != prev && new != next))
     4   │ │ │ return true;
     3   │ │ ret = false;
     2   │ }
     1   │
    88   │ ret &= __list_add_valid_or_report(new, prev, next);
     1   │ return ret;
     2   }
     3
```

看起来倒是符合预期。

再次查看 `kvm_ioeventfd_batch_begin()`代码:

```cpp
    18   static int kvm_ioeventfd_batch_begin(struct kvm *kvm)
    17   {
    16   │ struct kvm_shadow *ks;
    15   │ int ret = 0;
    14   │
    13   │ mutex_lock(&kvm->slots_lock);
    12   │
    11   │ if (is_kvm_in_shadow(kvm)) {
    10   │ │ ret = -EBUSY;
     9   │ │ goto out;
     8   │ }
     7   │
     6   │ ks = kzalloc(sizeof(*ks), GFP_KERNEL_ACCOUNT);
     5   │ if (!ks) {
     4   │ │ ret = -ENOMEM;
     3   │ │ goto out;
     2   │ }
     1   │ INIT_LIST_HEAD(&ks->list);
      //这个地方不太对
  1092   │ INIT_LIST_HEAD(&ks->ioeventfds_shadow);
     1   │ ks->kvm = kvm;
      //很可能是这个地方
     2   │ list_add_tail(&ks->list, &kvm_shadow_list);
     3   │
     4   out:
     5   │ mutex_unlock(&kvm->slots_lock);
     6   │ return ret;
     7   }
```

那也就是说将 `ks->list`向 `kvm_shadow_list`加的时候出问题。
此时使用的是`list_add_tail()`

`list_add_tail()`

* prev: `kvm_shadow_list->prev`
* next: `&kvm_shadow_list`

而报错的代码为:

```cpp
     CHECK_DATA_CORRUPTION(next->prev != prev,
   "list_add corruption. next->prev should be prev (%px), but was %px. (next=%px).\n",
    prev, next->prev, next)
//[26771.081331] list_add corruption. next->prev should be prev (ffff800003f89348), but was 9000001af36780c0. (next=ffff800003f89348).
```

`kvm_shadow_list` 目前的值:

```
crash> p kvm_shadow_list
kvm_shadow_list = $4 = {
  next = 0xffff800003f89348 <kvm_shadow_list>,
  prev = 0xffff800003f89348 <kvm_shadow_list>
}
```

这就非常奇怪了, prev get的时候就是通过`kvm_shadow_list->prev`  获取的。但是在
`__list_add_valid_or_report` 中却检测出`next->prev`和`prev` 不相等的情况。
当再次获取next->prev时，发现已经为 `ffff800003f89348(kvm_shadow_list)`,
疑似发生了 `list_del`

经定位疑似 KVM BUG<sup>1</sup>, 似乎可以通过<sup>2</sup>, 具体细节待补充

> TODO
>
> 补充细节

## 参考链接

1. [kvm:Optimize the processing performance of multiple eventfd and irqfd](https://gitee.com/openeuler/kernel/pulls/16100/commits)
2. [kvm: Only enable shadow mode for ioeventfd and Align the lifecycle of KVM shadow with KVM](https://gitee.com/openeuler/kernel/pulls/17661)
