## 复现步骤

1. 编译main.c, 执行./main
   ```sh
   [root@localhost ~]# ./main
   Parent PID: 1357
   ```
   该程序会持续创建子进程，子进程在执行一定负载后退出. 共创建
   `MAX_CHILDREN(10)` 个

2. 执行 `move_cgroup.sh $PID`, 该脚本会创建多个cgroup, 并循环移动
   `$PID`所代表的进程 的 task cgroup

3. 执行`rm_cgroup.sh`, 循环删除多个cgroup


## 复现现象
发生panic, 并且有多种堆栈, 现截取和`vmcore`中类似的堆栈:
```
[  118.683654] ---[ end trace 9936d0da6b90f91b ]---
[  118.685250] BUG: unable to handle kernel paging request at 00000000fffd39ec
[  118.687544] IP: [<ffffffff810ce6c7>] update_blocked_averages+0x87/0x700
[  118.689470] PGD 80000000bb4e0067 PUD 0
[  118.690721] Oops: 0000 [#1] SMP
[  118.691814] Modules linked in: sg pcspkr joydev i2c_piix4 i2c_core ip_tables xfs libcrc32c sd_mod crc_t10dif crct10dif_generic crct10dif_common ata_generc
[  118.699956] CPU: 1 PID: 2466 Comm: main Tainted: G        W      ------------   3.10.0-693.21.6.el7.x86_64 #1
[  118.702687] Hardware name: QEMU Standard PC (i440FX + PIIX, 1996), BIOS rel-1.17.0-0-gb52ca86e094d-prebuilt.qemu.org 04/01/2014
[  118.705826] task: ffff8800b2ce0fd0 ti: ffff8800b2d60000 task.ti: ffff8800b2d60000
[  118.707922] RIP: 0010:[<ffffffff810ce6c7>]  [<ffffffff810ce6c7>] update_blocked_averages+0x87/0x700
[  118.710508] RSP: 0000:ffff88013b243da0  EFLAGS: 00010087
[  118.711982] RAX: 0000000000000001 RBX: ffff88013683a840 RCX: 0000000000000000
[  118.713964] RDX: 00000000fffd39ac RSI: 00000000000004cb RDI: ffff8800ba2ef400
[  118.715953] RBP: ffff88013b243e08 R08: ffff880134a11800 R09: 0000000000000000
[  118.717943] R10: ffffffffffffffff R11: 0000000000008da0 R12: ffff88013b5d9230
[  118.719924] R13: ffff8800ba2ef400 R14: ffff88013b258a80 R15: ffff88013b2592e0
[  118.721911] FS:  00007f6e52b2d740(0000) GS:ffff88013b240000(0000) knlGS:0000000000000000
[  118.724142] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[  118.725774] CR2: 00000000fffd39ec CR3: 00000000b2db8000 CR4: 00000000000006e0
[  118.727768] DR0: 0000000000000000 DR1: 0000000000000000 DR2: 0000000000000000
[  118.729747] DR3: 0000000000000000 DR6: 00000000ffff0ff0 DR7: 0000000000000400
[  118.731729] Call Trace:
[  118.732473]  <IRQ>
[  118.733036]  [<ffffffff810d5e8d>] rebalance_domains+0x4d/0x2b0
[  118.734880]  [<ffffffff810d6212>] run_rebalance_domains+0x122/0x1e0
[  118.736645]  [<ffffffff81094035>] __do_softirq+0xf5/0x280
[  118.738143]  [<ffffffff816c4afc>] call_softirq+0x1c/0x30
[  118.739664]  [<ffffffff8102d435>] do_softirq+0x65/0xa0
[  118.741099]  [<ffffffff810943b5>] irq_exit+0x105/0x110
[  118.742567]  [<ffffffff816c5e78>] smp_apic_timer_interrupt+0x48/0x60
[  118.744322]  [<ffffffff816c2732>] apic_timer_interrupt+0x162/0x170
[  118.746052]  <EOI>
[  118.746662] Code: 48 89 45 c8 48 8b 45 c8 48 39 c7 4c 8d a0 50 ff ff ff 0f 84 ab 01 00 00 0f 1f 40 00 49 8b 94 24 c0 00 00 00 49 63 86 20 09 00 00 <48> 8
[  118.756863] RIP  [<ffffffff810ce6c7>] update_blocked_averages+0x87/0x700
[  118.758812]  RSP <ffff88013b243da0>
[  118.759821] CR2: 00000000fffd39ec
```

