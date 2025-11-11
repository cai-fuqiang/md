## Problem Description

OS: redhat8.6

```
NMI watchdog: Watchdog detected hard LOCKUP on cpu 45
...
RIP: 0010:advance_periodic_target_expiration+0x4d/0x80 [kvm]
Code: 68 48 89 d6 48 8b bd a8 00 00 00 48 29 cb 48 09 c6 e8 37 69 fd ff 48 8b bd a8 00 00 00 48 89 c6 48 89 d8 0f be 97 22 0e 00 00 <89> d1 f7 d9 48 d3 e8 89 d1 48 d3 e3 85 d2 8b 97 24 0e 00 00 48 0f
RSP: 0018:ff4f88f5d98d8ef0 EFLAGS: 00000046
RAX: fff0103f91be678e RBX: fff0103f91be678e RCX: 00843a7d9e127bcc
RDX: 0000000000000002 RSI: 0052ca4003697505 RDI: ff440d5bfbdbd500
RBP: ff440d5956f99200 R08: ff2ff2a42deb6a84 R09: 000000000002a6c0
R10: 0122d794016332b3 R11: 0000000000000000 R12: ff440db1af39cfc0
R13: ff440db1af39cfc0 R14: ffffffffc0d4a560 R15: ff440db1af39d0f8
FS:  00007f04a6ffd700(0000) GS:ff440db1af380000(0000) knlGS:000000e38a3b8000
CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
CR2: 000000d5651feff8 CR3: 000000684e038002 CR4: 0000000000773ee0
PKRU: 55555554
Call Trace:
 <IRQ>
 apic_timer_fn+0x31/0x50 [kvm]
 __hrtimer_run_queues+0x100/0x280
 hrtimer_interrupt+0x100/0x210
 ? ttwu_do_wakeup+0x19/0x160
 smp_apic_timer_interrupt+0x6a/0x130
 apic_timer_interrupt+0xf/0x20
 </IRQ>
```

After crash analysis, it was found that a software periodic timer is currently
being used, and its target_expiration is significantly delta from the current
time (now).

> some case [1]

```
crash> struct kvm_lapic ff440d5956f99200 |grep -E 'period|expiration|tscdead'
    period = 1739904,
    target_expiration = 32733273709929306,
    tscdeadline = 10135384616420291996,
    expired_tscdeadline = 0,

rcx : now = 00843a7d9e127bcc = 37219008123141068
```
The expiration_target is delayed:

```
(37219008123141068 - 32733273709929306)/10^9/60/60/24
~= 51 days
```

and advance_expire_delta is very large. This indicates that a significant amount
of time has passed since the last time the hv timer ran.

```
crash> struct kvm_lapic ff440d5956f99200
    ...
    advance_expire_delta = 10582308149557745,
    ...
```

At a certain point in the past, hv period timer was stalled for an extended
period of time.

## steps to reproduce

1. use following BPF scripts to track sw timer(hrtimer callback):
   ```
   kprobe:advance_periodic_target_expiration
   {
       @all_count++;
   }
   
   
   kprobe:apic_timer_fn
   {
       @sw_timer_count++;
   }
   
   kprobe:kvm_lapic_expired_hv_timer
   {
       @hv_timer_count++;
   }
   
   interval:s:1
   {
       printf("all_count %d \t", @all_count);
       printf("hv_timer  %d \t", @hv_timer_count);
       printf("sw_timer  %d \n", @sw_timer_count);
       @all_count = 0;
       @sw_timer_count = 0;
       @hv_timer_count = 0;
   }
   ```
   and start
2. use link [2] kvm-units-test to start a vm use period timer and start.
3. start kvm-units-test apic test
4. use following kill command to start and stop qemu task.
   ```
   kill -19 `pidof qemu-system-x86_64` && sleep 10 && kill -18 `pidof qemu-system-x86_64`
   ```
bpftrace scripts print:
```
all_count 4768  hv_timer  4768  sw_timer  0
all_count 4768  hv_timer  4768  sw_timer  0
//==suspend
all_count 910   hv_timer  910   sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
//==resume ==(1)==
all_count 51543         hv_timer  1     sw_timer  51541
all_count 4768  hv_timer  0     sw_timer  4768
all_count 4768  hv_timer  0     sw_timer  4768
all_count 4769  hv_timer  0     sw_timer  4769
all_count 4768  hv_timer  0     sw_timer  4768
```

At time point 1, the system processed the number of period timer expirations
that had accumulated over approximately 10 seconds, indicating that the hv timer
had stopped during that period.

If I continuously monitor the progress of the hv timer using crash after
executing the command `kill -18 pidof qemu-system-x86_64`:
```
## haven't stop qemu process
crash> struct kvm_lapic.lapic_timer.target_expiration,lapic_timer.tscdeadline,lapic_timer.hv_timer_in_use 0xffffa07a44efab00
  lapic_timer.target_expiration = 351293972517480,
  lapic_timer.tscdeadline = 395328387101,
  lapic_timer.hv_timer_in_use = true
crash> struct kvm_lapic.lapic_timer.target_expiration,lapic_timer.tscdeadline,lapic_timer.hv_timer_in_use 0xffffa07a44efab00
  lapic_timer.target_expiration = 351294486319230,
  lapic_timer.tscdeadline = 396665904396,
  lapic_timer.hv_timer_in_use = true
crash> struct kvm_lapic.lapic_timer.target_expiration,lapic_timer.tscdeadline,lapic_timer.hv_timer_in_use 0xffffa07a44efab00
  lapic_timer.target_expiration = 351295013333025,
  lapic_timer.tscdeadline = 398033956132,
  lapic_timer.hv_timer_in_use = true

## stopped qemu process
crash> struct kvm_lapic.lapic_timer.target_expiration,lapic_timer.tscdeadline,lapic_timer.hv_timer_in_use 0xffffa07a44efab00
  lapic_timer.target_expiration = 351346796160825,
  lapic_timer.tscdeadline = 532499428298,
  lapic_timer.hv_timer_in_use = true
crash> struct kvm_lapic.lapic_timer.target_expiration,lapic_timer.tscdeadline,lapic_timer.hv_timer_in_use 0xffffa07a44efab00
  lapic_timer.target_expiration = 351346796160825,
  lapic_timer.tscdeadline = 532499428298,
  lapic_timer.hv_timer_in_use = true
crash> struct kvm_lapic.lapic_timer.target_expiration,lapic_timer.tscdeadline,lapic_timer.hv_timer_in_use 0xffffa07a44efab00
  lapic_timer.target_expiration = 351346796160825,
  lapic_timer.tscdeadline = 532499428298,
  lapic_timer.hv_timer_in_use = true
```
When the qemu task is stopped, the hv timer is also stopped at the same time.

> NOTE
>
> use
> ```
> virsh suspend $vm_name && sleep 10 && virsh resume $vm_name
> ```
> can get same test results.

## merge patch set

When we merge patch [3], we can get the following test results.

```
all_count 4768  hv_timer  4768  sw_timer  0
all_count 4248  hv_timer  4248  sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 0     hv_timer  0     sw_timer  0
all_count 344   hv_timer  344   sw_timer  0
all_count 4768  hv_timer  4768  sw_timer  0
all_count 4768  hv_timer  4768  sw_timer  0
```

As can be seen, after sleep and wake-up, the accumulation of period timer
callback processing did not occur.

## Reference Link

1. [\[PATCH\] KVM: x86: fix hardlockup due to LAPIC hrtimer period drift](https://lore.kernel.org/kvm/YgahsSubOgFtyorl@fuller.cnet/)
2. [kvm-unit-test keep period timers on](https://github.com/cai-fuqiang/kvm-unit-tests/tree/loop_period_timer)
3. [Patch]()
