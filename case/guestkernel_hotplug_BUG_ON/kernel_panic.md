## Testing environment

**_software_**:

|software|version|base commit ID|
|---|---|---|
|qemu|10.0.50|6e1571533fd92bec67e5ab9b1dd1e15032925757|
|kernel|6.14.0-rc1|5c8c229261f14159b54b9a32f12e5fa89d88b905|


snippet of [qemu libvirt xml](./qemu.xml):

```xml
<domain>
  <memory unit='KiB'>16777216</memory>
  <vcpu placement='static' cpuset='0-31'>32</vcpu>
  <devices>
     <controller type='pci' index='1' model='pci-bridge'>
       <model name='pci-bridge'/>
       <target chassisNr='1'/>
       <alias name='pci.1'/>
       <address type='pci' domain='0x0000' bus='0x00' slot='0x1f' function='0x0'/>
     </controller>

     <!-- 
        total 8 pci bridge
     -->

     <interface type='vhostuser'>
       <mac address='fa:16:3e:1a:74:24'/>
       <source type='unix' path='/usr/local/var/run/openvswitch/vhost-user1' mode='client'/>
       <target dev='port-lmyfw7rm15'/>
       <model type='virtio'/>
       <driver queues='4' rx_queue_size='1024' tx_queue_size='1024'>
         <host mrg_rxbuf='on'/>
       </driver>
       <alias name='net0'/>
       <rom file=''/>
       <address type='pci' domain='0x0000' bus='0x00' slot='0x03' function='0x0'/>
     </interface>

     <!--
        total 100 vhostuser net
     -->
  </device>
</domain>
```

## Steps to Reproduce

execute below scripts:

```sh
sh test_all.sh
```

scripts body:

* test_all.sh
  ```sh
  ## FILE --- test_all.sha
  #!/bin/sh
  virsh start wang_disk_attach
  sh attach.sh
  ```
* attach.sh
  ```sh
  ## FILE --- 
  #!/bin/bash
  
  VM_NAME="wang_disk_attach"
  DISK_DIR="/disk"
  FORMAT="raw"

  # attach 100 disk
  for i in $(seq 1  100); do
      DISK_FILE="${DISK_DIR}/${i}.img"
      idx=$((i))
  
      # vdb ~ vdz, vdaa ~ vdaz, vdba ~ vdbz, ...
      n=$((idx - 1))
      dev_name=""
      while true; do
          c=$((n % 26))
          dev_name=$(printf "\x$(printf %x $((97 + c)) )")$dev_name
          n=$((n / 26 - 1))
          [ $n -ge 0 ] || break
      done
      dev_name="vd${dev_name}"

      echo "Attaching $DISK_FILE as $dev_name ..."

      virsh attach-disk "$VM_NAME" "$DISK_FILE" "$dev_name"  \
            --subdriver=$FORMAT --cache=none --persistent \
            --live --iothread $i

      if [ $? -eq 0 ];then
          sleep 10
      fi
  done
  ```

## kernel BUG ON stack

```
[   18.770688][  T215] ------------[ cut here ]------------
[   18.827848][  T215] kernel BUG at kernel/resource.c:792!
[   18.868395][  T215] Oops: invalid opcode: 0000 [#1] PREEMPT SMP PTI
[   18.869385][  T215] CPU: 1 UID: 0 PID: 215 Comm: kworker/u128:5 Not tainted 6.14.0-rc1+ #17
[   18.869385][  T215] Hardware name: JD JCloud Iaas Jvirt, BIOS unknown 2/2/2022
[   18.869385][  T215] Workqueue: kacpi_hotplug acpi_hotplug_work_fn
[   18.869385][  T215]
[   18.869385][  T215] RIP: 0010:reallocate_resource+0x197/0x1d0
[   18.869385][  T215] Code: 20 48 8b 44 24 28 48 89 43 28 48 8b 44 24 30 48 89 43 30 48 8b 44 24 38 48 89 43 38 e8 12 db ff ff 48 85 c0 0f 84 5d ff ff ff <0f> 0b 48 8b 74 24 08 48 3b 73 08 0f 82 1c ff ff ff 48 89 0b 48 89
[   18.869385][  T215] RSP: 0000:ffffc900008479b0 EFLAGS: 00010282
[   18.869385][  T215] RAX: ffff8881020c73b0 RBX: ffff8881021813b0 RCX: 000000000000343f
[   18.869385][  T215] RDX: 0000000000003400 RSI: ffff8881021813b0 RDI: ffff8881020c73b0
[   18.869385][  T215] RBP: 0000000000000000 R08: ffff8881021863e0 R09: 0000000000000040
[   18.869385][  T215] R10: 0000000000000000 R11: 000000000000343f R12: ffff88810020d6f0
[   18.869385][  T215] R13: ffffc90000847a20 R14: ffff88810020d6f0 R15: ffffffff82edb970
[   18.869385][  T215] FS:  0000000000000000(0000) GS:ffff88842ee80000(0000) knlGS:0000000000000000
[   18.869385][  T215] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[   18.869385][  T215] CR2: 0000000000000000 CR3: 0000000003036001 CR4: 0000000000170ef0
[   18.869385][  T215] Call Trace:
[   18.869385][  T215]  <TASK>
[   18.869385][  T215]  ? die+0x32/0x80
[   18.869385][  T215]  ? do_trap+0xd9/0x100
[   18.869385][  T215]  ? reallocate_resource+0x197/0x1d0
[   18.869385][  T215]  ? do_error_trap+0x65/0x80
[   18.869385][  T215]  ? reallocate_resource+0x197/0x1d0
[   18.869385][  T215]  ? exc_invalid_op+0x4c/0x60
[   18.869385][  T215]  ? reallocate_resource+0x197/0x1d0
[   18.869385][  T215]  ? asm_exc_invalid_op+0x16/0x20
[   18.869385][  T215]  ? reallocate_resource+0x197/0x1d0
[   18.869385][  T215]  allocate_resource+0x57/0xd0
[   18.869385][  T215]  ? __pfx_pcibios_align_resource+0x10/0x10
[   18.869385][  T215]  pci_bus_alloc_from_region+0x1df/0x240
[   18.869385][  T215]  ? __pfx_pcibios_align_resource+0x10/0x10
[   18.869385][  T215]  ? __pfx_pcibios_align_resource+0x10/0x10
[   18.869385][  T215]  ? __pfx_pcibios_align_resource+0x10/0x10
[   18.869385][  T215]  pci_bus_alloc_resource+0x86/0xb0
[   18.869385][  T215]  ? __pfx_pcibios_align_resource+0x10/0x10
[   18.869385][  T215]  _pci_assign_resource+0x9e/0x120
[   18.869385][  T215]  ? __pfx_pcibios_align_resource+0x10/0x10
[   18.869385][  T215]  pci_assign_resource+0xae/0x290
[   18.869385][  T215]  assign_requested_resources_sorted+0x4a/0xb0
[   18.869385][  T215]  __assign_resources_sorted+0x491/0x4d0
[   18.869385][  T215]  ? __dev_sort_resources+0x9b/0x2a0
[   18.869385][  T215]  __pci_bus_assign_resources+0x6f/0x1f0
[   18.869385][  T215]  enable_slot+0x25e/0x440
[   18.869385][  T215]  ? pci_device_is_present+0x49/0x70
[   18.869385][  T215]  acpiphp_check_bridge.part.0+0x117/0x150
[   18.869385][  T215]  hotplug_event+0x13d/0x220
[   18.869385][  T215]  ? __pfx_acpiphp_hotplug_notify+0x10/0x10
[   18.869385][  T215]  acpiphp_hotplug_notify+0x20/0x60
[   18.869385][  T215]  acpi_device_hotplug+0xae/0x240
[   18.869385][  T215]  acpi_hotplug_work_fn+0x1a/0x30
[   18.869385][  T215]  process_one_work+0x184/0x3a0
[   18.869385][  T215]  worker_thread+0x24d/0x360
[   18.869385][  T215]  ? __pfx_worker_thread+0x10/0x10
[   18.869385][  T215]  kthread+0xed/0x220
[   18.869385][  T215]  ? finish_task_switch.isra.0+0x88/0x2b0
[   18.869385][  T215]  ? __pfx_kthread+0x10/0x10
[   18.869385][  T215]  ret_from_fork+0x30/0x50
[   18.869385][  T215]  ? __pfx_kthread+0x10/0x10
[   18.869385][  T215]  ret_from_fork_asm+0x1a/0x30
[   18.869385][  T215]  </TASK>
[   18.869385][  T215] Modules linked in:
```

<!--另外，还有 [T11] 线程打印的同时，[T1] 线程也在打印, 并且还有`can't clain;address conflict`报错:-->

In addition, while the [T11] thread is printing, the [T1] thread is also
printing, and there is also some `can’t claim; address conflict` errors.

```
[   17.894008][  T215] pci 0000:06:0f.0: BAR 4 [mem 0x382800038000-0x38280003bfff 64bit pref]: assigned
[   17.973007][  T215] pci 0000:06:10.0: BAR 4 [mem 0x38280003c000-0x38280003ffff 64bit pref]: assigned
[   17.976285][    T1] pci 0000:06:11.0: BAR 4 [mem 0x382800004000-0x382800007fff 64bit pref]: can't claim; address conflict with 0000:06:02.0 [mem 0x382800004000-0x382800007fff 64bit pref]
[   18.050007][  T215] pci 0000:06:11.0: BAR 4 [mem 0x382800040000-0x382800043fff 64bit pref]: assigned
[   18.057028][    T1] pci 0000:06:12.0: BAR 4 [mem 0x382800000000-0x382800003fff 64bit pref]: can't claim; address conflict with 0000:06:01.0 [mem 0x382800000000-0x382800003fff 64bit pref]
[   18.060002][  T215] pci 0000:06:12.0: BAR 4 [mem 0x382800040000-0x382800043fff 64bit pref]: assigned
[   18.070002][  T215] pci 0000:06:13.0: BAR 4 [mem 0x382800040000-0x382800043fff 64bit pref]: assigned
```

## add debug code

<!--我们增加一些调试信息, 具体代码见 [patch](), 打印信息拆解如下:-->

We have added some debugging information. Please refer to the 
[patch](https://github.com/cai-fuqiang/linux/commit/ea16405a19e2eb412d3c4f9c503abf7ccca2a2d0)
for the specific code. The breakdown of the log messages is as follows:

**dmesg analysis during reproduction**:

```
[    8.229341][    T1] pci 0000:06:12.0: [1af4:1001] type 00 class 0x010000 conventional PCI endpoint
[    8.349623][    T1] pci 0000:06:12.0: BAR 0 [io  0x3000-0x307f]
[    8.353737][    T1] pci 0000:06:12.0: BAR 1 [mem 0xc0400000-0xc0400fff]
[    8.355766][    T1] pci 0000:06:12.0: BAR 4 [mem 0x382800000000-0x382800003fff 64bit pref]
[    8.360348][    T1] pci 0000:06:13.0: [1af4:1001] type 00 class 0x010000 conventional PCI endpoint
[    8.364154][    T1] pci 0000:06:13.0: BAR 0 [io  0x0000-0x007f]
[    8.365736][    T1] pci 0000:06:13.0: BAR 1 [mem 0x00000000-0x00000fff]
[    8.368765][    T1] pci 0000:06:13.0: BAR 4 [mem 0x00000000-0x00003fff 64bit pref]
[    8.372229][    T1] pci 0000:06:14.0: [1af4:1001] type 00 class 0x010000 conventional PCI endpoint
[    8.376137][    T1] pci 0000:06:14.0: BAR 0 [io  0x0000-0x007f]
[    8.377736][    T1] pci 0000:06:14.0: BAR 1 [mem 0x00000000-0x00000fff]
[    8.380765][    T1] pci 0000:06:14.0: BAR 4 [mem 0x00000000-0x00003fff 64bit pref]
[    8.384226][    T1] pci 0000:06:15.0: [1af4:1001] type 00 class 0x010000 conventional PCI endpoint
[    8.388136][    T1] pci 0000:06:15.0: BAR 0 [io  0x0000-0x007f]
[    8.389736][    T1] pci 0000:06:15.0: BAR 1 [mem 0x00000000-0x00000fff]
[    8.392765][    T1] pci 0000:06:15.0: BAR 4 [mem 0x00000000-0x00003fff 64bit pref]
[    8.396212][    T1] pci 0000:06:16.0: [1af4:1001] type 00 class 0x010000 conventional PCI endpoint
[    8.400134][    T1] pci 0000:06:16.0: BAR 0 [io  0x0000-0x007f]
[    8.401736][    T1] pci 0000:06:16.0: BAR 1 [mem 0x00000000-0x00000fff]
[    8.403765][    T1] pci 0000:06:16.0: BAR 4 [mem 0x00000000-0x00003fff 64bit pref]
[    8.408174][    T1] pci 0000:00:1d.0: PCI bridge to [bus 06]
```

<!--上面信息可知, `06:13.0, 06:14.0, 06:15.0, 06:16.0`是已经hotplug的devices-->

From the above information, we can see that `06:13.0, 06:14.0, 06:15.0, and 06:16.0`
are devices that have already been hot-plugged.

***

```sh
## ----
## T212
## ----

[   17.274731][  T212] acpiphp_glue: hotplug_event BEG

## hotplug_event, DEVICE_CHECK 06:17.0

[   17.278951][  T212] pci 0000:06:17.0: [1af4:1001] type 00 class 0x010000 conventional PCI endpoint
[   17.285861][  T212] pci 0000:06:17.0: BAR 0 [io  0x0000-0x007f]
[   17.285876][  T212] pci 0000:06:17.0: BAR 1 [mem 0x00000000-0x00000fff]
[   17.291901][  T212] pci 0000:06:17.0: BAR 4 [mem 0x00000000-0x00003fff 64bit pref]

## __dev_sort_resources, get all resources that parent IS NULL under the bus 6

[   17.301488][  T212] __dev_sort_resources end, print BEG
[   17.303722][  T212]  name(0000:06:01.0) start(382800044000) end(382800047fff) parent(0)

                               ## ---
                               ## T1
                               ## ---
                               ## pcibios_init start work, BUT it begin with bus 0, BUT BOTH do NOT intersect.

                               [   17.311723][    T1] pcibios_init BEG
                               [   17.320822][    T1] pci_claim_resource name(0000:00:01.1) start(1f0) end(1f7) parent(0)
                               [   17.326795][    T1] pci_claim_resource name(0000:00:01.4) start(4000) end(407f) parent(0)

                               ## SO T212's __dev_sort_resources function, LINK ALL RESOURCE to the head list

                               # pbus_assign_resources_sorted(...)
                               # |=> LIST_HEAD(head);
                               # 
                               # |=> list_for_each_entry(dev, &bus->devices, bus_list)
                               #        __dev_sort_resources(dev, &head);
                               # 
                               #  // HERE, pcibios_init HAVE NOT work, so ALL report 
                               #  // device's res will link into head
                               #
                               # |=> pr_info("__dev_sort_resources end, print BEG\n");

## But T1 is so fast, some resource in head list will print parent ISN'T NULL
[   17.752721][  T212]  name(0000:06:01.0) start(3480) end(34bf) parent(ffff888101b9e6f0)

                               ## before end of traversaling __dev_sort_resources,
                               ## T1 is claiming bus 7, so most devices's resource of bus 6 
                               ## (expect hotplug device) will HAVE PARENT, BUT ALSO link 
                               ## into head in T212
                               [   17.847735][    T1] pci_claim_resource name(0000:07:0d.0) start(2480) end(24bf) parent(0)
[   17.850720][  T212] __dev_sort_resources end, print END END
```

Due to the above process, we are fortunate to obtain a head list node whose
resource’s parent may be non-NULL.

But `06:13.0, 06:14.0, 06:15.0, 06:16.0, 06:17.0` is hotplug devices, so the
parent of resources is also NULL.

***

When allocating space for `06:12.0`(not hotplug devices) , since the space assigned 
by the BIOS `[0x3000-0x307f]` meets the alignment requirements, there is no need to 
reallocate.

```
[   19.458722][  T212] print_asigned device BEG
[   19.463721][  T212]  name(0000:06:12.0) start(3000) end(307f) parent(ffff888101b9e6f0)
...
[   19.541810][  T212] pci 0000:06:12.0: BAR 0 [io  0x3000-0x307f]: assigned
```

***

<!--`06:13.0, 06:14.0` 虽然是hotplug devices， 但是其也找到了合适的空间.-->

Although `06:13.0` and `06:14.0` are hotplug devices, they have also found suitable
space.

```sh
## 06:13.0
[   19.551157][  T212]  name(0000:06:13.0) start(0) end(7f) parent(0)
...
[   19.640425][  T212] pci 0000:06:13.0: BAR 0 [io  0x3800-0x387f]: assigned

## 06:14.0
[   19.708890][  T212]  name(0000:06:14.0) start(0) end(7f) parent(0)
...
[   20.309517][  T212] pci 0000:06:14.0: BAR 0 [io  0x3880-0x38ff]: assigned

## 06:15.0
[   20.437256][  T212]  name(0000:06:15.0) start(0) end(7f) parent(0)
...
[   20.653594][  T212] pci 0000:06:15.0: BAR 0 [io  0x3c00-0x3c7f]: assigned

## 06:16.0
[   20.669212][  T212]  name(0000:06:16.0) start(0) end(7f) parent(0)
...
[   20.772897][  T212] pci 0000:06:16.0: BAR 0 [io  0x3c80-0x3cff]: assigned

```

***

<!--但是 `06:17.0` 就没有这么好运气, [0x3000, 0x3fff] 已经没有合适的空间分配给它了。-->

However, `06:17.0` was not so lucky; there was no suitable space left for it in
the range `[0x3000, 0x3fff]`.

```sh
[   20.784227][  T212] print_asigned device BEG
[   20.789872][  T212]  name(0000:06:17.0) start(0) end(7f) parent(0)
[   20.795517][  T212] print_asigned device END
[   20.801132][  T212]  name(0000:06:12.0) start(3000) end(307f) parent(ffff888101b9e6f0)
[   20.806787][  T212]  name(0000:06:11.0) start(3080) end(30bf) parent(ffff888101b9e6f0)
[   20.812429][  T212]  name(0000:06:10.0) start(30c0) end(30ff) parent(ffff888101b9e6f0)
[   20.818046][  T212]  name(0000:06:0f.0) start(3100) end(313f) parent(ffff888101b9e6f0)
[   20.823712][  T212]  name(0000:06:0e.0) start(3140) end(317f) parent(ffff888101b9e6f0)
[   20.829346][  T212]  name(0000:06:0d.0) start(3180) end(31bf) parent(ffff888101b9e6f0)
[   20.834195][  T212]  name(0000:06:0c.0) start(31c0) end(31ff) parent(ffff888101b9e6f0)
[   20.837458][  T212]  name(0000:06:0b.0) start(3200) end(323f) parent(ffff888101b9e6f0)
[   20.840532][  T212]  name(0000:06:0a.0) start(3240) end(327f) parent(ffff888101b9e6f0)
[   20.845530][  T212]  name(0000:06:09.0) start(3280) end(32bf) parent(ffff888101b9e6f0)
[   20.848601][  T212]  name(0000:06:08.0) start(32c0) end(32ff) parent(ffff888101b9e6f0)
[   20.854249][  T212]  name(0000:06:07.0) start(3300) end(333f) parent(ffff888101b9e6f0)
[   20.859881][  T212]  name(0000:06:06.0) start(3340) end(337f) parent(ffff888101b9e6f0)
[   20.865514][  T212]  name(0000:06:05.0) start(3380) end(33bf) parent(ffff888101b9e6f0)
[   20.871132][  T212]  name(0000:06:04.0) start(33c0) end(33ff) parent(ffff888101b9e6f0)
[   20.876765][  T212]  name(0000:06:03.0) start(3400) end(343f) parent(ffff888101b9e6f0)
[   20.882400][  T212]  name(0000:06:02.0) start(3440) end(347f) parent(ffff888101b9e6f0)
[   20.888027][  T212]  name(0000:06:01.0) start(3480) end(34bf) parent(ffff888101b9e6f0)
[   20.893649][  T212]  name(0000:06:13.0) start(3800) end(387f) parent(ffff888101b9e6f0)
[   20.899277][  T212]  name(0000:06:14.0) start(3880) end(38ff) parent(ffff888101b9e6f0)
[   20.904903][  T212]  name(0000:06:15.0) start(3c00) end(3c7f) parent(ffff888101b9e6f0)
[   20.910531][  T212]  name(0000:06:16.0) start(3c80) end(3cff) parent(ffff888101b9e6f0)
[   20.916185][  T212] pci 0000:06:17.0: BAR 0 [io  size 0x0080]: can't assign; no space
[   20.921840][  T212] pci 0000:06:17.0: BAR 0 [io  size 0x0080]: failed to assign
[   20.927473][  T212] _pci_assign_resource before
```

> NOTE
>
> <!--明明`(0x3cff, 0x3fff]` 仍然有大量空间容纳`0x80` size, 为什么分配不出来呢? 主要原因是因为`align`需求-->
>
> Clearly, the range `(0x3cff, 0x3fff]` still has plenty of space to 
> accommodate a size of 0x80, so why couldn’t it be allocated?
>
> The main reason is due to x86's alignment requirements.
>
> x86 align function
> ```cpp
> //======arch/x86/pci/i386.c========
> resource_size_t
> pcibios_align_resource(void *data, const struct resource *res,
>                         resource_size_t size, resource_size_t align)
> {
>         struct pci_dev *dev = data;
>         resource_size_t start = res->start;
> 
>         if (res->flags & IORESOURCE_IO) {
>                 if (skip_isa_ioresource_align(dev))
>                         return start;
>                 if (start & 0x300)
>                         start = (start + 0x3ff) & ~0x3ff;
>         } else if (res->flags & IORESOURCE_MEM) {
>                 /* The low 1MB range is reserved for ISA cards */
>                 if (start < BIOS_END)
>                         start = BIOS_END;
>         }
>         return start;
> }
> ```
> The rules for the allocated range are:
> ```
> 0x000-0x0ff region modulo 0x400.
> ```
> eg:
> ```
> [0x0, 0xff],    [0x400, 0x4ff],   [0x800, 0x8ff], [0xc00, 0xcff] 
> [0x1000, 0x10ff]
> ...
> ```

***

<!--后续, `06:01.0`, `06:02.0`, `06:03.0`, `06:04.0` 都因bios为其分配的空间满足对齐
要求。所以不需要重新分配, 这里略过这些设备.-->

Subsequently, `06:01.0`, `06:02.0`, `06:03.0`, and `06:04.0` did not require
reallocation because the spaces assigned to them by the BIOS already met the
alignment requirements. Therefore, these devices are omitted here.

***

<!--而`06:05.0` 被bios分配为`[0x3380, 0x33bf]` 不满足对齐要求, 所以需要重新分配
-->

However, `06:05.0` was assigned `[0x3380, 0x33bf]` by the BIOS, which does not meet
the alignment requirements, so it needs to be reallocated.


```
[   22.852006][  T212] print_asigned device BEG
[   22.856719][  T212]  name(0000:06:05.0) start(3380) end(33bf) parent(ffff888101b9e6f0)
[   22.861405][  T212] print_asigned device END

...
[   22.964195][  T212]  name(0000:06:07.0) start(3300) end(333f) parent(ffff888101b9e6f0)
[   23.005904][  T212]  name(0000:06:06.0) start(3340) end(337f) parent(ffff888101b9e6f0)
[   23.046446][  T212]  name(0000:06:05.0) start(3380) end(33bf) parent(ffff888101b9e6f0)
[   23.211924][  T212]  name(0000:06:04.0) start(34c0) end(34ff) parent(ffff888101b9e6f0)

...

[   23.335800][  T212] pci 0000:06:05.0: BAR 0 [io  size 0x0040]: can't assign; no space
[   23.377220][  T212] pci 0000:06:05.0: BAR 0 [io  size 0x0040]: failed to assign
```
<!--
但是重新分配资源时, 因为bios 没有按照对齐分配，所以其空间是紧密排布的。所以，即
使将`06:05.0`的空间释放，在进行分配，也满足不了内核按照其align的规则分配的需求。

而随后，由于其分配失败, -->

However, when reallocating resources, since the BIOS did not allocate space with
proper alignment, the available space is tightly packed. Therefore, even if the
space of `06:05.0` is released and reallocated, it still cannot meet the kernel’s
alignment requirements.

Due to the allocation failure, the resource will subsequently be reset, setting
[start, end] to zero.

```sh
assign_requested_resources_sorted
|=> if resource_size(res) && pci_assign_resource(dev_res->dev, idx)
    ## reset
    |=> reset_resource()
        |=> res->start, end, flags = 0
```

However, it still has a parent and can be found through the parent’s sibling
linked list, which may affect subsequent resource allocations.

***

<!--当`06:06.0`被bios分配为`[0x3340,0x337f]`, 不满足对齐要求，需要重新分配-->

When 06:06.0 is assigned [0x3340, 0x337f] by the BIOS, it does not meet the
alignment requirements and needs to be reallocated.

```sh
[   23.418676][  T212] print_asigned device BEG
[   23.418677][  T212]  name(0000:06:06.0) start(3340) end(337f) parent(ffff888101b9e6f0)
[   23.418679][  T212] print_asigned device END
...
[   23.585331][  T212]  name(0000:06:07.0) start(3300) end(333f) parent(ffff888101b9e6f0)
[   23.626534][  T212]  name(0000:06:06.0) start(3340) end(337f) parent(ffff888101b9e6f0)
[   23.626536][  T212]  name(0000:06:05.0) start(0) end(0) parent(ffff888101b9e6f0)
[   23.626538][  T212]  name(0000:06:03.0) start(3400) end(343f) parent(ffff888101b9e6f0)
...
[   23.713872][  T212] resource: new res
[   23.755665][  T212] resource: the old conflict with sibling
[   23.833258][  T212] resource: new res name(0000:06:06.0) start(3400) end(343f) parent(ffff888101b9e6f0)
[   23.833261][  T212] resource: conflict res name(0000:06:03.0) start(3400) end(343f) parent(ffff888101b9e6f0)
[   23.838380][  T212] resource: SKIP BUG_ON: reallocate_resource:803
```

<!--
上面的流程造成了一个比较奇怪的resource: `06:05.0` resource为`[0, 0]`, 在执行
`__find_resource_space`时, 会尝试在[06:06.0.start, 06:05.0.start - 1] 寻找合适的
空间，但是此时`06:05.0.start - 1 = 0 - 1 = 0xffffffffffffffff`, 所以, 可以分配给
`06:06.0`的范围为`[0x3340, ∞]`, 于是, `06:06.0`选择了一个合适的空间:
`[0x3340, 0x3400]`作为其realloc的范围.

但是，这个空间和`06:05.0` 冲突，于是报BUG_ON.
-->

There is a rather strange resource here: `06:05.0` has a resource range of `[0, 0]`.
When executing `__find_resource_space`, it will attempt to find a suitable space
in the range `[06:07.0.end + 1, 06:05.0.start - 1]`. However, at this point, 
`06:05.0.start - 1 = 0 - 1 = 0xffffffffffffffff`, so the allocatable range for 
`06:06.0` becomes `[0x3340, ∞]`. As a result, `06:06.0` selects a suitable space, 
`[0x3400,0x343f]`, as its reallocation range.

![pci_assign_resource_BUG_ON](./pic/pci_assign_resource_BUG_ON.svg)

However, this space conflicts with 06:05.0, which triggers a `BUG_ON`.
```
pci_assign_resource
 _pci_assign_resource
  __pci_assign_resource
   pci_bus_alloc_resource
    pci_bus_alloc_from_region
     allocate_resource
      reallocate_resource {
        if (!resource_contains(old, &new)) {
          conflict = __request_resource(root, old);
          BUG_ON(conflict);
        }
      }
```

## others
1. [the entire dmesg](./2025-0627-1439-dmesg.md)
2. [gdb attach information when guest kernel panic](./2025-0627-1439-dmesg.md)
