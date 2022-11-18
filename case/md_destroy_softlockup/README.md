# 问题现象
在客户环境中遇到了softlockup的问题, 之后，便出现了ssh登录不通的情况

# 信息获取
通过message 文件可以得到softlockup 的堆栈:

## 第一次softlockup
第一次，打印如下:
```
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: NMI watchdog: BUG: soft lockup - CPU#12 stuck for 22s! [dockerd-current:39156]                                           
...
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OE  ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1     
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: Hardware name: Dell Inc. PowerEdge R740/01YM03, BIOS 2.2.11 06/13/2019                                                   
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: task: ffff881fd1603f80 ti: ffff881f5b314000 task.ti: ffff881f5b314000                                                    
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: RSP: 0018:ffff881f5b317c28  EFLAGS: 00000246                                                                             
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: RAX: ffff880f2d793cd0 RBX: ffff88103cd99cc0 RCX: ffff880f2d793ce8                                                        
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: RDX: ffff880f2d793ce8 RSI: 0000000000000246 RDI: 0000000000000246                                                        
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: RBP: ffff881f5b317c28 R08: 0000000000000000 R09: ffffffff8132618b
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: R10: ffff88103cd99cc0 R11: 0000000000000000 R12: ffff881f5b317bf0                                                        
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: R13: ffff881f5b317c28 R14: ffff881f5b317bf0 R15: 0000000000000000
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: FS:  00007f7c38f61700(0000) GS:ffff88103cd80000(0000) knlGS:0000000000000000                                             
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: CR2: 00007f7d1c00c000 CR3: 0000001017b58000 CR4: 00000000003407e0
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: DR0: 0000000000000000 DR1: 0000000000000000 DR2: 0000000000000000
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: DR3: 0000000000000000 DR6: 00000000fffe0ff0 DR7: 0000000000000400
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: Stack:
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: ffff881f5b317c60 ffffffff810bd514 ffff880f2d793cb8 ffff8820389a7290
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: ffff8820389a7290 0000000000000001 0000000000000131 ffff881f5b317c80
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: ffffffff812f7951 ffff881e1df21000 ffffffffc00168a0 ffff881f5b317cc0
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: Call Trace:
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffff810bd514>] __wake_up+0x44/0x50
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffff812f7951>] blk_set_queue_dying+0xa1/0xc0
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffffc00015eb>] __dm_destroy+0x7b/0x310 [dm_mod]
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffffc0003b43>] dm_destroy+0x13/0x20 [dm_mod]
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffffc00099be>] dev_remove+0x11e/0x180 [dm_mod]
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffffc00098a0>] ? dev_suspend+0x250/0x250 [dm_mod]
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffffc000a015>] ctl_ioctl+0x1e5/0x500 [dm_mod]
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffff810cc21b>] ? should_numa_migrate_memory+0x5b/0x150
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffffc000a343>] dm_ctl_ioctl+0x13/0x20 [dm_mod]
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffff81214fad>] do_vfs_ioctl+0x33d/0x540
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffff816b00d1>] ? __do_page_fault+0x171/0x450
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffff81215251>] SyS_ioctl+0xa1/0xc0
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: [<ffffffff816b50c9>] system_call_fastpath+0x16/0x1b
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: Code: 07 00 0f 1f 40 00 5d c3 0f 1f 40 00 66 2e 0f 1f 84 00 00 00 00 00 0f 1f 44 00 00 55 48 89 e5 c6 07 00 0f 1f 40 00 48 89 f7 57 9d <0f> 1f 44 00 00 5d c3 0f 1f 40 00 0f 1f 44 00 00 55 48 89 e5 48
2022-11-12 10:11:00 +0800 | messages | Nov 12 10:11:00 node-2 kernel: rbd: rbd1: encountered watch error: -107
2022-11-12 10:11:00 +0800 | messages | Nov 12 10:11:00 node-2 kernel: rbd: rbd4: encountered watch error: -107
2022-11-12 10:11:00 +0800 | messages | Nov 12 10:11:00 node-2 kernel: rbd: rbd3: encountered watch error: -107
2022-11-12 10:11:00 +0800 | messages | Nov 12 10:11:00 node-2 kernel: rbd: rbd0: encountered watch error: -107
```
可见, softlockup 是由时钟中断获取，获取到的上下文, 为发生时钟中断时的
`dockerd-current:39156`进程的内核栈的上下文, RIP 落在了 `_raw_spin_unlock_irqrestore+0x15`, 
而堆栈中，显示的最顶的堆栈`__wake_up`。我们结合反汇编看下:
```
0xffffffff810bd509 <__wake_up+57>:      mov    %r12,%rsi
0xffffffff810bd50c <__wake_up+60>:      mov    %rbx,%rdi
0xffffffff810bd50f <__wake_up+63>:      callq  0xffffffff816ab920 <_raw_spin_unlock_irqrestore>
0xffffffff810bd514 <__wake_up+68>:      pop    %rbx
0xffffffff810bd515 <__wake_up+69>:      pop    %r12
```
可以看到实际上， `__wake_up`是 call 了`_raw_spin_unlock_irqrestore`,
还未返回便触发了中断，再来看下`_raw_spin_unlock_irqrestore`
```
crash> dis _raw_spin_unlock_irqrestore
0xffffffff816ab920 <_raw_spin_unlock_irqrestore>:       nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff816ab925 <_raw_spin_unlock_irqrestore+5>:     push   %rbp
0xffffffff816ab926 <_raw_spin_unlock_irqrestore+6>:     mov    %rsp,%rbp
0xffffffff816ab929 <_raw_spin_unlock_irqrestore+9>:     callq  *0xffffffff81a13150
0xffffffff816ab930 <_raw_spin_unlock_irqrestore+16>:    mov    %rsi,%rdi
0xffffffff816ab933 <_raw_spin_unlock_irqrestore+19>:    push   %rdi
0xffffffff816ab934 <_raw_spin_unlock_irqrestore+20>:    popfq
0xffffffff816ab935 <_raw_spin_unlock_irqrestore+21>:    nopl   0x0(%rax,%rax,1)
0xffffffff816ab93a <_raw_spin_unlock_irqrestore+26>:    pop    %rbp
0xffffffff816ab93b <_raw_spin_unlock_irqrestore+27>:    retq
```
rip 时935， 说明执行完了 +20 指令，该段代码，没有循环，也没有阻塞的指令，
说明很可能卡在了上面的代码流程中(死循环)

## 获取后面几次softlockup 的RIP
```
~/下载/ecs.20221112130918.latest/os : cat messages.node-2.20221112.log|grep  -E 'x86_64|kernel: RIP:'                        ✔
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OE  ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:10:59 +0800 | messages | Nov 12 10:10:59 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:11:32 +0800 | messages | Nov 12 10:11:32 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:11:32 +0800 | messages | Nov 12 10:11:32 node-2 kernel: RIP: 0010:[<ffffffff812f7951>]  [<ffffffff812f7951>] blk_set_queue_dying+0xa1/0xc0
2022-11-12 10:11:59 +0800 | messages | Nov 12 10:11:59 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:11:59 +0800 | messages | Nov 12 10:11:59 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:12:27 +0800 | messages | Nov 12 10:12:27 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:12:27 +0800 | messages | Nov 12 10:12:27 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:12:55 +0800 | messages | Nov 12 10:12:55 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:12:55 +0800 | messages | Nov 12 10:12:55 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:13:23 +0800 | messages | Nov 12 10:13:23 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:13:23 +0800 | messages | Nov 12 10:13:23 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:13:51 +0800 | messages | Nov 12 10:13:51 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:13:51 +0800 | messages | Nov 12 10:13:51 node-2 kernel: RIP: 0010:[<ffffffff810bd4e5>]  [<ffffffff810bd4e5>] __wake_up+0x15/0x50
2022-11-12 10:14:19 +0800 | messages | Nov 12 10:14:19 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:14:19 +0800 | messages | Nov 12 10:14:19 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:14:59 +0800 | messages | Nov 12 10:14:59 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:14:59 +0800 | messages | Nov 12 10:14:59 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:15:27 +0800 | messages | Nov 12 10:15:27 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:15:27 +0800 | messages | Nov 12 10:15:27 node-2 kernel: RIP: 0010:[<ffffffff816abc96>]  [<ffffffff816abc96>] _raw_spin_lock_irqsave+0x6/0x40
2022-11-12 10:15:55 +0800 | messages | Nov 12 10:15:55 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:15:55 +0800 | messages | Nov 12 10:15:55 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:16:03 +0800 | messages | Nov 12 10:16:03 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:16:03 +0800 | messages | Nov 12 10:16:03 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:16:23 +0800 | messages | Nov 12 10:16:23 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:16:23 +0800 | messages | Nov 12 10:16:23 node-2 kernel: RIP: 0010:[<ffffffff810bd4de>]  [<ffffffff810bd4de>] __wake_up+0xe/0x50
2022-11-12 10:16:31 +0800 | messages | Nov 12 10:16:31 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:16:31 +0800 | messages | Nov 12 10:16:31 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:16:51 +0800 | messages | Nov 12 10:16:51 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:16:51 +0800 | messages | Nov 12 10:16:51 node-2 kernel: RIP: 0010:[<ffffffff816ab93b>]  [<ffffffff816ab93b>] _raw_spin_unlock_irqrestore+0x1b/0x20
2022-11-12 10:16:59 +0800 | messages | Nov 12 10:16:59 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:16:59 +0800 | messages | Nov 12 10:16:59 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:17:19 +0800 | messages | Nov 12 10:17:19 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:17:19 +0800 | messages | Nov 12 10:17:19 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:17:27 +0800 | messages | Nov 12 10:17:27 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:17:27 +0800 | messages | Nov 12 10:17:27 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:17:55 +0800 | messages | Nov 12 10:17:55 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:17:55 +0800 | messages | Nov 12 10:17:55 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:17:59 +0800 | messages | Nov 12 10:17:59 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:17:59 +0800 | messages | Nov 12 10:17:59 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:18:23 +0800 | messages | Nov 12 10:18:23 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:18:23 +0800 | messages | Nov 12 10:18:23 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:18:27 +0800 | messages | Nov 12 10:18:27 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:18:27 +0800 | messages | Nov 12 10:18:27 node-2 kernel: RIP: 0010:[<ffffffff810bd4de>]  [<ffffffff810bd4de>] __wake_up+0xe/0x50
2022-11-12 10:18:47 +0800 | messages | Nov 12 10:18:47 node-2 kernel: CPU: 15 PID: 84 Comm: migration/15 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:18:47 +0800 | messages | Nov 12 10:18:47 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:18:51 +0800 | messages | Nov 12 10:18:51 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:18:51 +0800 | messages | Nov 12 10:18:51 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:18:55 +0800 | messages | Nov 12 10:18:55 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:18:55 +0800 | messages | Nov 12 10:18:55 node-2 kernel: RIP: 0010:[<ffffffff810bd4e5>]  [<ffffffff810bd4e5>] __wake_up+0x15/0x50
2022-11-12 10:19:15 +0800 | messages | Nov 12 10:19:15 node-2 kernel: CPU: 15 PID: 84 Comm: migration/15 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:19:15 +0800 | messages | Nov 12 10:19:15 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:19:19 +0800 | messages | Nov 12 10:19:19 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:19:19 +0800 | messages | Nov 12 10:19:19 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:19:23 +0800 | messages | Nov 12 10:19:23 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:19:23 +0800 | messages | Nov 12 10:19:23 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:19:43 +0800 | messages | Nov 12 10:19:43 node-2 kernel: CPU: 15 PID: 84 Comm: migration/15 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:19:43 +0800 | messages | Nov 12 10:19:43 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:19:47 +0800 | messages | Nov 12 10:19:47 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:19:47 +0800 | messages | Nov 12 10:19:47 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:19:51 +0800 | messages | Nov 12 10:19:51 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:19:51 +0800 | messages | Nov 12 10:19:51 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:20:11 +0800 | messages | Nov 12 10:20:11 node-2 kernel: CPU: 15 PID: 84 Comm: migration/15 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:20:11 +0800 | messages | Nov 12 10:20:11 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:20:15 +0800 | messages | Nov 12 10:20:15 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:20:15 +0800 | messages | Nov 12 10:20:15 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:20:19 +0800 | messages | Nov 12 10:20:19 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:20:19 +0800 | messages | Nov 12 10:20:19 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:20:39 +0800 | messages | Nov 12 10:20:39 node-2 kernel: CPU: 15 PID: 84 Comm: migration/15 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:20:39 +0800 | messages | Nov 12 10:20:39 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:20:43 +0800 | messages | Nov 12 10:20:43 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:20:43 +0800 | messages | Nov 12 10:20:43 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:20:59 +0800 | messages | Nov 12 10:20:59 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:20:59 +0800 | messages | Nov 12 10:20:59 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:21:07 +0800 | messages | Nov 12 10:21:07 node-2 kernel: CPU: 15 PID: 84 Comm: migration/15 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:21:07 +0800 | messages | Nov 12 10:21:07 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:21:11 +0800 | messages | Nov 12 10:21:11 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:21:11 +0800 | messages | Nov 12 10:21:11 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:21:27 +0800 | messages | Nov 12 10:21:27 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:21:27 +0800 | messages | Nov 12 10:21:27 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:21:35 +0800 | messages | Nov 12 10:21:35 node-2 kernel: CPU: 15 PID: 84 Comm: migration/15 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:21:35 +0800 | messages | Nov 12 10:21:35 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:21:39 +0800 | messages | Nov 12 10:21:39 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:21:39 +0800 | messages | Nov 12 10:21:39 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:21:55 +0800 | messages | Nov 12 10:21:55 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:21:55 +0800 | messages | Nov 12 10:21:55 node-2 kernel: RIP: 0010:[<ffffffff816ab935>]  [<ffffffff816ab935>] _raw_spin_unlock_irqrestore+0x15/0x20
2022-11-12 10:22:03 +0800 | messages | Nov 12 10:22:03 node-2 kernel: CPU: 15 PID: 84 Comm: migration/15 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:22:03 +0800 | messages | Nov 12 10:22:03 node-2 kernel: RIP: 0010:[<ffffffff81116818>]  [<ffffffff81116818>] multi_cpu_stop+0x48/0x100
2022-11-12 10:22:07 +0800 | messages | Nov 12 10:22:07 node-2 kernel: CPU: 13 PID: 74 Comm: migration/13 Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:22:07 +0800 | messages | Nov 12 10:22:07 node-2 kernel: RIP: 0010:[<ffffffff8111681a>]  [<ffffffff8111681a>] multi_cpu_stop+0x4a/0x100
2022-11-12 10:22:23 +0800 | messages | Nov 12 10:22:23 node-2 kernel: CPU: 12 PID: 39156 Comm: dockerd-current Tainted: G        W  OEL ------------   3.10.0-693.11.1.el7.es.10.x86_64 #1
2022-11-12 10:22:23 +0800 | messages | Nov 12 10:22:23 node-2 kernel: RIP: 0010:[<ffffffff816ab93b>]  [<ffffffff816ab93b>] _raw_spin_unlock_irqrestore+0x1b/0x20
```

可见，捕捉到的ip大概分配在下面几个函数(除去`multi_cpu_stop`):
``` 
_raw_spin_unlock_irqrestore+0x15/0x20
blk_set_queue_dying+0xa1/0xc0
__wake_up+0x15/0x50
_raw_spin_lock_irqsave+0x6/0x40
__wake_up+0xe/0x50
_raw_spin_unlock_irqrestore+0x1b/0x20
```
上面获取到的这些堆栈, 都是`CPU: 12 PID: 39156 Comm: dockerd-current`进程产生。
大致可以判断，像是阻塞在一个循环中了。查看kernel 代码。

## 结合代码分析

我们这里只分析`blk_set_queue_dying`, 因为他是最外层的，最能反映循环的范围
```cpp
void blk_set_queue_dying(struct request_queue *q)
{
	spin_lock_irq(q->queue_lock);
	queue_flag_set(QUEUE_FLAG_DYING, q);
	spin_unlock_irq(q->queue_lock);
	
	/*
	¦* When queue DYING flag is set, we need to block new req
	¦* entering queue, so we call blk_freeze_queue_start() to
	¦* prevent I/O from crossing blk_queue_enter().
	¦*/
	blk_freeze_queue_start(q);
	
	if (q->mq_ops)
	        blk_mq_wake_waiters(q);
	else {
	        struct request_list *rl;
	
	        blk_queue_for_each_rl(rl, q) {			 //line:494
	                if (rl->rq_pool) {			 //line:495
	                        wake_up(&rl->wait[BLK_RW_SYNC]); //line:496
	                        wake_up(&rl->wait[BLK_RW_ASYNC]);//line:497
	                }
	        }
	}
}
```
通过dis -l 获取`blk_set_queue_dying+0xa1/0xc0`
```
/usr/src/debug/kernel-3.10.0-693.11.1.el7/linux-3.10.0-693.11.1.el7.es.10.x86_64/block/blk-core.c: 495
0xffffffff812f7920 <blk_set_queue_dying+112>:   cmpq   $0x0,0x20(%rbx)
0xffffffff812f7925 <blk_set_queue_dying+117>:   je     0xffffffff812f7951 <blk_set_queue_dying+161>
/usr/src/debug/kernel-3.10.0-693.11.1.el7/linux-3.10.0-693.11.1.el7.es.10.x86_64/block/blk-core.c: 496
0xffffffff812f7927 <blk_set_queue_dying+119>:   lea    0x40(%rbx),%rdi
0xffffffff812f792b <blk_set_queue_dying+123>:   xor    %ecx,%ecx
0xffffffff812f792d <blk_set_queue_dying+125>:   mov    $0x1,%edx
0xffffffff812f7932 <blk_set_queue_dying+130>:   mov    $0x3,%esi
0xffffffff812f7937 <blk_set_queue_dying+135>:   callq  0xffffffff810bd4d0 <__wake_up>
/usr/src/debug/kernel-3.10.0-693.11.1.el7/linux-3.10.0-693.11.1.el7.es.10.x86_64/block/blk-core.c: 497
0xffffffff812f793c <blk_set_queue_dying+140>:   lea    0x28(%rbx),%rdi
0xffffffff812f7940 <blk_set_queue_dying+144>:   xor    %ecx,%ecx
0xffffffff812f7942 <blk_set_queue_dying+146>:   mov    $0x1,%edx
0xffffffff812f7947 <blk_set_queue_dying+151>:   mov    $0x3,%esi
0xffffffff812f794c <blk_set_queue_dying+156>:   callq  0xffffffff810bd4d0 <__wake_up>
/usr/src/debug/kernel-3.10.0-693.11.1.el7/linux-3.10.0-693.11.1.el7.es.10.x86_64/block/blk-core.c: 494
0xffffffff812f7951 <blk_set_queue_dying+161>:   mov    %rbx,%rdi
```
可以看到, 是在执行`callq __wake_up`指令, 对应的c代码，为494行, 
可以推测，代码应该是卡在了，`blk_queue_for_each_rl`循环之内，
一直循环，这里为什么会一直卡住，最好还是获取vmcore文件，
要不不好继续分析。

另外，在softlockup发生后，出现了rbd的异常。
```
2022-11-12 10:11:00 +0800 | messages | Nov 12 10:11:00 node-2 kernel: rbd: rbd1: encountered watch error: -107
2022-11-12 10:11:00 +0800 | messages | Nov 12 10:11:00 node-2 kernel: rbd: rbd4: encountered watch error: -107
2022-11-12 10:11:00 +0800 | messages | Nov 12 10:11:00 node-2 kernel: rbd: rbd3: encountered watch error: -107
...
2022-11-12 10:15:00 +0800 | messages | Nov 12 10:15:00 node-2 kernel: rbd: rbd0: encountered watch error: -107
2022-11-12 10:15:30 +0800 | messages | Nov 12 10:15:30 node-2 kernel: rbd: rbd0: encountered watch error: -107
2022-11-12 10:16:00 +0800 | messages | Nov 12 10:16:00 node-2 kernel: rbd: rbd0: encountered watch error: -107
```

持续到了 10:16:00, 比softlockup持续时间短一些，需要存储同时帮忙看下。

# 代码详细分析
## blk_set_queue_dying()
```cpp
void blk_set_queue_dying(struct request_queue *q)
{
        spin_lock_irq(q->queue_lock);
        queue_flag_set(QUEUE_FLAG_DYING, q);
        spin_unlock_irq(q->queue_lock);

        /*
        ¦* When queue DYING flag is set, we need to block new req
        ¦* entering queue, so we call blk_freeze_queue_start() to
        ¦* prevent I/O from crossing blk_queue_enter().
        ¦*/
        blk_freeze_queue_start(q);

        if (q->mq_ops)
                blk_mq_wake_waiters(q);
        else {
                struct request_list *rl;

                blk_queue_for_each_rl(rl, q) {
                        if (rl->rq_pool) {
                                wake_up(&rl->wait[BLK_RW_SYNC]);
                                wake_up(&rl->wait[BLK_RW_ASYNC]);
                        }
                }
        }
}
```

`blk_set_queue_dying`, 会在`request_queue`被销毁时调用，
同时设置 `queue_flag  QUEUE_FLAG_DYING`, 使其对该设备产生IO立即
返回错误。
```cpp
#define blk_queue_dying(q)      test_bit(QUEUE_FLAG_DYING, &(q)->queue_flags)

get_request
	__get_request
static struct request *__get_request(struct request_list *rl, int rw_flags,
                                ¦    struct bio *bio, gfp_t gfp_mask)
{
	if (unlikely(blk_queue_dying(q)))
        	return ERR_PTR(-ENODEV);
}
```

### blk_queue_for_each_rl
```
#define blk_queue_for_each_rl(rl, q)    \
        for ((rl) = &(q)->root_rl; (rl); (rl) = __blk_queue_next_rl((rl), (q)))
```

#### __blk_queue_next_rl
```cpp
struct request_list *__blk_queue_next_rl(struct request_list *rl,
					 struct request_queue *q)
{
	struct list_head *ent;
	struct blkcg_gq *blkg;

	/*
	 * Determine the current blkg list_head.  The first entry is
	 * root_rl which is off @q->blkg_list and mapped to the head.
	 */
	if (rl == &q->root_rl) {
		ent = &q->blkg_list;
		/* There are no more block groups, hence no request lists */
		if (list_empty(ent))
			return NULL;
	} else {
		blkg = container_of(rl, struct blkcg_gq, rl);
		ent = &blkg->q_node;
	}

	/* walk to the next list_head, skip root blkcg */
	ent = ent->next;
	if (ent == &q->root_blkg->q_node)
		ent = ent->next;
	if (ent == &q->blkg_list)
		return NULL;

	blkg = container_of(ent, struct blkcg_gq, q_node);
	return &blkg->rl;
}
```

### request list 

而 这里的`struct request_list`的作用，是将 `request_queue`和`blkcg_gq`联系到一起。
```cpp
struct request_list {
        struct request_queue    *q;     /* the queue this rl belongs to */
#ifdef CONFIG_BLK_CGROUP
        struct blkcg_gq         *blkg;  /* blkg this request pool belongs to */
#endif
        /*
        ¦* count[], starved[], and wait[] are indexed by
        ¦* BLK_RW_SYNC/BLK_RW_ASYNC
        ¦*/
        int                     count[2];
        int                     starved[2];
        mempool_t               *rq_pool;
        wait_queue_head_t       wait[2];
        unsigned int            flags;
};
```
而`blkcg_gq` 是将 `blkcg` 和`request_queue`联系到一起
```cpp
struct blkcg_gq {
        /* Pointer to the associated request_queue */
        struct request_queue            *q;
        struct list_head                q_node;
        struct hlist_node               blkcg_node;
        struct blkcg                    *blkcg;

        /* all non-root blkcg_gq's are guaranteed to have access to parent */
        struct blkcg_gq                 *parent;

        /* request allocation list for this blkcg-q pair */
        struct request_list             rl;
...
};
```
这里blkcg_gq 中有 request_list的实例，所以实际上表示的还是
`blkcg`和`request_queue`之间的关系。

我们知道，进程是和cgroup css 联系到一起的，每个进程
拥有一个blkcg(可能会和其他进程共享), 而request_queue 是和具体
的blk 设备联系到一起的，每个设备有一个request_queue, 而他们之
间也是多对多的关系，一个blkcg 可能对应多个block 设备，每个block
设备可能对应多个 request_queue

所以可以看到, `__blk_queue_next_rl`, 实际上就是根据`blkcg_gq`
找到该`request_queue` 对应的所有的 `request_list`

#### 创建request_list流程
代码路径
```
blk_init_allocated_queue  :  init root rl  root rl 用于 root 层级 blkcg
  blk_queue_make_request(q, blk_queue_bio);  : q->make_request_fn -->  blk_queue_bio
	该函数是将 bio 转换为request
    get_request : 获取request
      blk_get_rl
        bio_blkcg : 通过bio->bi_css 获取/ task_blkcg获取 blkcg
	if (blkcg == &blkcg_root) goto root_rl;
	blkcg_lookup_create()
          blkg = __blkg_lookup()   if (blkg) return blkg
	  blkg_create()
	    blkg_alloc()
	      blk_init_rl()
	    radix_tree_insert()
	    hlist_add_head_rcu(&blkg->blkcg_node, &blkcg->blkg_list);
	    list_add(&blkg->q_node, &q->blkg_list);
      retry:
      __get_request(rl, rw_flags, bio, gfp_mask);
      if (!IS_ERR(rq))
        return rq;
      if (!(gfp_mask & __GFP_WAIT) || unlikely(blk_queue_dying(q))) {
        blk_put_rl(rl);
        return rq;
      }
      prepare_to_wait_exclusive(&rl->wait[is_sync], &wait,
                        ¦ TASK_UNINTERRUPTIBLE);
      io_schedule();
      finish_wait(&rl->wait[is_sync], &wait);
      goto retry;
```
这里就可以大概了解request_list->wait 等待队列的用法了, 如果`__guest_guest`
获取request 失败（很可能由于blkcg的关系, 这块代码没有去分析), 则会加入到
等待队列中，等待唤醒，如果是 queue dying 的场景，这里唤醒后，会执行goto retry
在上面 unlikely(blk_queue_dying()) 判断成立，返回错误。
