## 内核版本

```
kernel-6.6.0-117.0.0.122.oe2403sp2
```

## 问题堆栈

### kvm_ioeventfd问题

出现了 `list_add corruption` 的WARN:

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

查看`openeuler2403-sp2 122` KVM代码, 其操作 `kvm_shadow_list` 时，并未使用全局性的
lock保护，也未使用rcu保护，可能会出现free和read之间的冲突。

而该代码是openeuler KVM的一个新特性，用来加速多队列的申请ioeventfd的性能。会对热迁移
以及热插拔带来不小的性能提升, 具体来分析下代码.

我们先来看下，upstream(非openeuler) 目前的代码

## upstream ioeventfd

> [!NOTE]
>
> 本节只是简述eventfd的功能，以及和case相关的实现细节。

kernel 提供了一下的ioctl作为ioeventfd interface, 用来绑定address_space和
eventfd, 如果KVM 捕获到guest目前对该address space 做操作，则通过eventfd
通知到对端 (qemu, or other agent(ovs))

```
vm_ioctl: `KVM_IOEVENTFD`
```

并提供了一个参数, `kvm_ioeventfd` 用户将eventfd 和地址相关信息传递到内核态。

```cpp
struct kvm_ioeventfd {
 __u64 datamatch;
 __u64 addr;        /* legal pio/mmio address */
 __u32 len;         /* 1, 2, 4, or 8 bytes; or 0 to ignore length */
 __s32 fd;
 __u32 flags;
 __u8  pad[36];
};
```

其中flags 表示本次调用类型， 其中`KVM_IOEVENTFD_FLAG_DEASSIGN` 表示本次
为解绑 , 否则为绑定. 对应kvm两个接口调用:

* kvm_assign_ioeventfd
* kvm_deassign_ioeventfd

而KVM 对于模拟设备MMIO访问抽象成 `kvm_io_devices` 其包含三个回调:

* read
* write
* destructor

并且为这些device抽象了bus -- `kvm_io_bus`, 该bus 包含了一组
`kvm_io_range`, 而 `kvm_io_range` 除了包含`kvm_io_device` 外,还包含
`addr, len`等地址范围信息.

而bus 在系统中会有多个，由 `enum kvm_bus` 定义.

在触发 io emulate 的流程中, 遍历bus中的数组, 如果能匹配到则调用相关
回调。本次emulate 就执行完了:

```sh
handle_ept_misconfig
=> if (!is_guest_mode(vcpu) &&
     !kvm_io_bus_write(vcpu, KVM_FAST_MMIO_BUS, gpa, 0, NULL)) 
   => trace_kvm_fast_mmio(gpa);
   ## 完事了
   => return kvm_skip_emulated_instruction(vcpu);

## 如果返回0，则表示emulate 成功，可以success

kvm_io_bus_write
## get 对应bus
=> bus = kvm_get_bus_srcu(vcpu->kvm, bus_idx);
## 可能有多个range 重叠，一次通知每个range
=> r = __kvm_io_bus_write(vcpu, bus, &range, val);
   => idx = kvm_io_bus_get_first_dev(bus, range->addr, range->len);
   => while (idx < bus->dev_count &&
       kvm_io_bus_cmp(range, &bus->range[idx]) == 0)
      => if (!kvm_iodevice_write(vcpu, bus->range[idx].dev, range->addr,
        range->len, val))
              => dev->ops->write
         => return idx
      => idx++
=> return r < 0 ? r : 0;
```

一般像这种指针+数组的更新往往采用rcu的机制, 用来加速读者，
在`kvm_assign_ioeventfd` 会处理rcu 写者端逻辑:

```sh
kvm_assign_ioeventfd
=> kvm_assign_ioeventfd_idx
   => ret = kvm_io_bus_register_dev(kvm, bus_idx, p->addr, p->length,
       &p->dev);
      => bus = kvm_get_bus(kvm, bus_idx);
      => new_bus = kmalloc(,,bus->dev_count+1, )
      ## 先找到bus中的插入位置，在new_bus中数组中更新
      => for (i = 0; i < bus->dev_count; i++)
         => if (kvm_io_bus_cmp(&bus->range[i], &range) > 0)
            => break
      ## 更新前半部分
      => memcpy(new_bus, bus, sizeof(*bus) + i * sizeof(struct kvm_io_range));
      ## 处理新增加的元素
      => new_bus->dev_count++
      => new_bus->range[i] = range;
      ## 更新后半部分
     => memcpy(new_bus->range + i + 1, bus->range + i,
         (bus->dev_count - i) * sizeof(struct kvm_io_range));
     ## 发布
    => rcu_assign_pointer(kvm->buses[bus_idx], new_bus);
    => call_srcu(&kvm->srcu, &bus->rcu, __free_bus);
    ## 这需要注意，并未主动wait rcu callback complete
```

看起来并没有什么问题. 但是`deassign`和`assign`不同，`assign` 是在原来的接口中
新增元素，其 old array 释放与否对后续流程无影响. 因为old array 是new array
的子集, 确保其他cpu可以看到新的映射关系, assign的语义就完成了。

但是`deassign`则完全不同，其 `new array` 是 `old array`子集，当前系统可能正在
使用`new array`中没有的映射关系，`deassign` 接口的结束应该确保映射关系解除完成。

所以，`deassign` 往往会等待rcu callback完成:

```sh
kvm_io_bus_unregister_dev
=> rcu_assign_pointer(kvm->buses[bus_idx], new_bus);
## wait...
=> synchronize_srcu_expedited(&kvm->srcu);
```

该wait在一般流程中不会影响性能，但是在热迁移过程中，最后一步是在位于downtime中停
io device, 而停io device, 恰好会调用上述流程。

并且为了提升虚拟机io性能, 现在的虚拟机配置往往采用多队列。而每个队列都会有一个
`host notify`, 用来driver(guest driver) 通知device(kvm,qemu emulate) 有请求到达。
而kvm目前只支持单个ioeventfd的 assign deassign, 所以在该场景下，性能代价更大一些。

## openeuler batch ioeventfd (de)assign

为此，openeuler搞了一个套新的ioeventfd的flags <sup>1</sup>, 用来实现batch下发,
从名字上看比较好理解, batch 下发无非是将eventfd批量下发到一个数组中，然后, 等
批量下发完成后，再调用一次 `rcu_assign_pointer()`.

而华为工程师并未改变太多rcu的接口或者数据结构，最大程度保证和之前的API兼容，使用
未更改的qemu也能正常工作。

怎么做的呢？增加两个flags bit:

* KVM_IOEVENTFD_FLAG_BATCH_BEGIN
* KVM_IOEVENTFD_FLAG_BATCH_END

在这两个flags处理的调用中做一些前置工作，或者准备工作。在`BATCH_BEGIN`中分配
相关数据结构，使后续的 assign/deassign ioeventfd 保留在该数据结构中，另外，在`BATCH_END`
中将这些eventfd 集中 commit.

如下代码:

```sh

## 分配一个 kvm_shadow数据结构, 并串联到 
## kvm_shadow_list结构中
kvm_ioeventfd_batch_begin
=> ks = kzalloc(sizeof(*ks), GFP_KERNEL_ACCOUNT);
=> INIT_LIST_HEAD(&ks->list);
=> INIT_LIST_HEAD(&ks->ioeventfds_shadow);
=> list_add_tail(&ks->list, &kvm_shadow_list);
=> ks->kvm = kvm;

kvm_assign_ioeventfd
=> kvm_assign_ioeventfd_idx
   ## struct _ioeventfd
   => p = kzalloc(sizeof(*p), GFP_KERNEL_ACCOUNT);
   => kvm_iodevice_init(&p->dev, &ioeventfd_ops);
   => ks = kvm_find_shadow(kvm);
   => if (ks)
      ## 分配一个eventfd_shadow数据结构, 然后串联到ks->ioeventfds_shadow链表中
      => es = kzalloc(sizeof(*es), GFP_KERNEL_ACCOUNT);
      => es->flag = IOEVENTFD_SHADOW_FLAG_ADD;
      => es->eventfd = p;
      => list_add_tail(&es->node, &ks->ioeventfds_shadow);

kvm_ioeventfd_batch_end
=> ks = kvm_find_shadow(kvm);
## copy old list to ks->buses_shadow, 作为base array
=> for (i = 0; i < KVM_NR_BUSES; i++)
   => bus = kvm_get_real_bus(kvm, i);
   => new_bus = kmalloc(struct_size(bus, range, bus->dev_count), GFP_KERNEL_ACCOUNT);
   => len = sizeof(*bus);
   => len += bus->dev_count * sizeof(struct kvm_io_range);
   => memcpy(new_bus, bus, len);
   => ks->buses_shadow[i] = new_bus
## 处理新添加的 eventfds shadow --  从ks->ioeventfds_shadow链表中依次提取
=> kvm_handle_ioeventfds_shadow(kvm);
   => list_for_each_entry_safe(es, tmp, &ks->ioeventfds_shadow, node)
      => if es->flag == IOEVENTFD_SHADOW_FLAG_ADD
         => kvm_io_bus_register_dev(kvm, p->bus_idx, p->addr,
      -- else
         => kvm_io_bus_unregister_dev(kvm, p->bus_idx, &p->dev);
## 替换数组
=> for (i = 0; i < KVM_NR_BUSES; i++)
   => old[i] = kvm_get_real_bus(kvm, i);
   => rcu_assign_pointer(kvm->buses[i], ks->buses_shadow[i]);
## 注意batch中可能有 assign 请求，也可能有deassign请求，也可能都有，所以这里
## 干脆等一次
=> synchronize_srcu_expedited(&kvm->srcu);
## 释放老的 bus
=> for (i = 0; i < KVM_NR_BUSES; i++)
   => kfree(old[i]);

kvm_io_bus_register_dev
## .. 省略相同流程
=> bus = kvm_get_bus(kvm, bus_idx);
   ## old bus 在 kvm->buses_shadow中找
   => return is_kvm_in_shadow(kvm) ?
         kvm_find_shadow(kvm)->buses_shadow[idx] : kvm_get_real_bus(kvm, idx);
=> ks = kvm_find_shadow(kvm);
=> if ks
   ## 将new bus 更新到 ks->buses_shadow中
   => ks->buses_shadow[bus_idx] = new_bus;
```

大概的流程是, 在`begin_batch`中新增一个数据结构。并做初始化，在之后`assign`,`deassign`
过程中，如果发现有新增的数据结构(`kvm_find_shadow()`)， 则认为本次是要做batch，
在本次`a/de ssign` 中不去`kvm_io_bus_(un)register_dev()`, 而是更新到一个链表中。
在`end batch`接口中，遍历该链表，并依次做`kvm_io_bus_(un)register_dev()`, 并将
new array先更新到`buses_shadow`中，等所有请求处理完成后，将带有所有新请求的array
更新到`kvm->buses`上，并调用 **一次** `synchronize_srcu_expedited()`.

对应qemu的逻辑:

```diff
diff --git a/system/memory.c b/system/memory.c
index fd76eb7048..08d34262c3 100644
--- a/system/memory.c
+++ b/system/memory.c
@@ -1134,10 +1134,12 @@ void memory_region_commit(void)
         ioeventfd_update_pending = false;
         MEMORY_LISTENER_CALL_GLOBAL(commit, Forward);
     } else if (ioeventfd_update_pending) {
+        MEMORY_LISTENER_CALL_GLOBAL(eventfd_begin, Forward);
         QTAILQ_FOREACH(as, &address_spaces, address_spaces_link) {
             address_space_update_ioeventfds(as);
         }
         ioeventfd_update_pending = false;
+        MEMORY_LISTENER_CALL_GLOBAL(eventfd_end, Forward);
     }
 }
```

在`update ioeventfds`前后，用`BATCH BEGIN`, `BATCH END` 包裹。

## 最初batch patch问题

在`batch_begin` 将ks加入`kvm_shadow_list`, 和在`batch end`将`ks` 移除 `kvm_shadow_list`
时, 未加全局性的锁(虽然加了`kvm->slots_lock`但是这时per kvm 粒度的), 导致多个虚拟机
同时搞会有问题。

但是, 如果每个虚拟机都在ioeventfd中申请全局性的锁，有很影响性能。所以这个方式得改。
patch <sup>2</sup> 将`ks`的生命周期，搞成了和kvm的生命周期一致，在`kvm_create_vm()`
中创建`ks`, 并在 `kvm_destroy_vm()`中释放`ks`, 并在操作这些时，通过`kvm_lock`大锁保护.

以`kvm_destroy_vm()`为例:

```sh
kvm_destroy_vm
=> mutex_lock(&kvm->slots_lock);
=> ks = kvm_find_shadow(kvm);
=> if (ks && ks->in_shadow) {
   => kvm_release_ioeventfds_shadow(ks);
   => ks->in_shadow = false;
## 加大锁
=> mutex_lock(&kvm_lock);
   => if (ks)
      ## RCU?
      => list_del_rcu(&ks->list);
=> mutex_unlock(&kvm_lock);
=> synchronize_rcu();
=> kfree(ks)
```

需要注意的时，现在的释放逻辑，完全脱离了`assign/deassign`流程，这意味着读取和释放
可能并行，所以需要rcu保护。

另外还有一个更重要的是, 由于ks的生命周期变了，需要一个其他的方式标记，本次请求是否是batch
的, 而新增的方式是`kvm_shadow`中新增`in_shadow`变量。

* begin_batch: ==> true
* end_batch: ==> false

当`in_shadow`为true时，表示在使用`batch ioeventfd`

## 参考链接

1. [kvm:Optimize the processing performance of multiple eventfd and irqfd](https://gitee.com/openeuler/kernel/pulls/16100/commits)
2. [kvm: Only enable shadow mode for ioeventfd and Align the lifecycle of KVM shadow with KVM](https://gitee.com/openeuler/kernel/pulls/17661)
