# b300 lspci 特别慢
执行下面脚本:
```
[root@10-75-4-155 11:23:10 1_gpu]# cat a.sh
GPU_ID_ARR=(1a:00.0
40:00.0
62:00.0
73:00.0
bd:00.0
df:00.0
f0:00.0)

for GPU_ID in ${GPU_ID_ARR[@]}
do
	echo ===$GPU_ID====
	time lspci -vvv -s $GPU_ID -nn > /dev/null
	time lspci -xxxx -s $GPU_ID -nn > /dev/null
done

echo ===9a:00.0====
time lspci -vvv -s 9a:00.0 -nn > /dev/null
```
执行结果
```
[root@10-75-4-155 11:22:48 1_gpu]# sh a.sh
===1a:00.0====

real	0m0.009s
user	0m0.004s
sys	0m0.005s

real	0m0.016s
user	0m0.005s
sys	0m0.011s
===40:00.0====

real	0m0.008s
user	0m0.007s
sys	0m0.001s

real	0m0.016s
user	0m0.005s
sys	0m0.011s
===62:00.0====

real	0m0.009s
user	0m0.007s
sys	0m0.002s

real	0m0.016s
user	0m0.004s
sys	0m0.012s
===73:00.0====

real	0m0.008s
user	0m0.006s
sys	0m0.002s

real	0m0.016s
user	0m0.004s
sys	0m0.012s
===bd:00.0====

real	0m0.009s
user	0m0.006s
sys	0m0.002s

real	0m0.016s
user	0m0.006s
sys	0m0.010s
===df:00.0====

real	0m0.008s
user	0m0.004s
sys	0m0.004s

real	0m0.016s
user	0m0.004s
sys	0m0.012s
===f0:00.0====

real	0m0.008s
user	0m0.005s
sys	0m0.003s

real	0m0.016s
user	0m0.003s
sys	0m0.013s
===9a:00.0====

real	0m20.244s
user	0m1.545s
sys	0m18.697s
```

可以看到只有 `9a:00.0` 机器特别慢

***

如果要对 `9a:00.0` 执行 
```
lspci -xxxx -s 9a:00.0
```
则时间特别长(2m 多) , 并且会造成 `Hard lockup`
```
real	2m25.783s
user	0m1.039s
sys	2m24.737s
```

hardlockup 堆栈:
```
[ 1369.025799] CPU: 64 PID: 3187 Comm: kworker/64:3 Kdump: loaded Tainted: G S         OE      6.6.0-jdcloud7.sp1.131d3847b.x86_64 #1
[ 1369.025803] Hardware name: Supermicro SYS-822GS-NB3RT/X14DBG-LC, BIOS 1.4 08/19/2025
[ 1369.025804] Workqueue: events smp_call_on_cpu_callback
[ 1369.025825] RIP: 0010:bitfill_aligned+0xa1/0xd0
[ 1369.025840] Code: 08 48 31 d0 48 21 d8 48 31 d0 49 89 42 f8 44 89 c8 29 e8 41 29 c0 44 89 c0 31 d2 4c 89 d7 41 f7 f1 49 89 c0 48 89 c1 48 89 f0 <f3> 48 ab 4d 85 e4 74 b8 4b 8d 04 c2 48 8b 10 48 31 d6 4c 21 e6 48
[ 1369.025842] RSP: 0018:ffa000001a5e48d0 EFLAGS: 00010046
[ 1369.025843] RAX: 0000000000000000 RBX: ffffffffffffffff RCX: 00000000000000b0
[ 1369.025845] RDX: 0000000000000000 RSI: 0000000000000000 RDI: ffa000003b7a0080
[ 1369.081926] RBP: 0000000000000000 R08: 00000000000003c0 R09: 0000000000000040
[ 1369.081927] R10: ffa000003b79e800 R11: 000000000000f000 R12: 0000000000000000
[ 1369.081928] R13: ffffffff94bb42f0 R14: 000000000000f000 R15: ffa000003b79e800
[ 1369.081929] FS:  0000000000000000(0000) GS:ff1100fe7de00000(0000) knlGS:0000000000000000
[ 1369.081929] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[ 1369.081930] CR2: 0000562ddad590c8 CR3: 000000ed63b18006 CR4: 0000000000773ee0
[ 1369.081931] DR0: 0000000000000000 DR1: 0000000000000000 DR2: 0000000000000000
[ 1369.081931] DR3: 0000000000000000 DR6: 00000000fffe07f0 DR7: 0000000000000400
[ 1369.081932] PKRU: 55555554
[ 1369.494176] Call Trace:
[ 1369.494181]  <NMI>
[ 1369.494183]  ? watchdog_hardlockup_check+0xaa/0x140
[ 1369.494191]  ? __perf_event_overflow+0xe5/0x2a0
[ 1369.748292]  ? handle_pmi_common+0x159/0x3a0
[ 1369.904570]  ? set_pte_vaddr_p4d+0x3f/0x50
[ 1369.904575]  ? flush_tlb_one_kernel+0xa/0x20
[ 1370.060663]  ? native_set_fixmap+0x42/0x80
[ 1370.060666]  ? ghes_copy_tofrom_phys+0x78/0x110
[ 1370.220824]  ? __ghes_peek_estatus.isra.0+0x49/0xb0
[ 1370.220827]  ? intel_pmu_handle_irq+0x101/0x2a0
[ 1370.220828]  ? perf_event_nmi_handler+0x26/0x50
[ 1370.326979]  ? nmi_handle+0x53/0x120
[ 1370.481086]  ? default_do_nmi+0x43/0x110
[ 1370.587179]  ? exc_nmi+0x100/0x190
[ 1370.587180]  ? end_repeat_nmi+0x16/0x67
[ 1370.587190]  ? __pfx_bitfill_aligned+0x10/0x10
[ 1370.587192]  ? bitfill_aligned+0xa1/0xd0
[ 1370.587194]  ? bitfill_aligned+0xa1/0xd0
[ 1370.587196]  ? bitfill_aligned+0xa1/0xd0
[ 1370.587198]  </NMI>
[ 1370.587198]  <IRQ>
[ 1370.587199]  sys_fillrect+0x149/0x370
[ 1370.587203]  drm_fbdev_generic_defio_fillrect+0x12/0x30 [drm_kms_helper]
[ 1370.795325]  bit_clear+0xac/0x120
[ 1370.795329]  fbcon_scroll+0x15f/0x190
[ 1370.795332]  con_scroll+0xf7/0x200
[ 1370.795339]  lf+0xa9/0xb0
[ 1370.795341]  vt_console_print+0x2d4/0x4b0
[ 1370.795343]  console_flush_all+0x17e/0x3a0
[ 1370.795351]  console_unlock+0x78/0xe0
[ 1370.795353]  vprintk_emit+0x18c/0x1e0
[ 1370.795356]  ? __pfx_watchdog_timer_fn+0x10/0x10
[ 1370.795357]  _printk+0x58/0x80
[ 1370.795360]  ? smp_call_function_single_async+0x17/0x30
[ 1370.795362]  watchdog_timer_fn+0x1a2/0x220
[ 1370.795364]  __run_hrtimer+0x59/0x190
[ 1370.795369]  __hrtimer_run_queues+0x7c/0xe0
[ 1370.795370]  hrtimer_interrupt+0xf4/0x230
[ 1370.795372]  __sysvec_apic_timer_interrupt+0x46/0xe0
[ 1370.795378]  sysvec_apic_timer_interrupt+0x35/0x90
[ 1370.795383]  asm_sysvec_apic_timer_interrupt+0x16/0x20
[ 1370.795385] RIP: 0010:crng_make_state+0x5e/0x120
[ 1370.795390] Code: 65 48 03 2d 0c 30 36 6b 48 39 45 20 75 55 48 89 d9 4c 89 e2 4c 89 ee 48 89 ef e8 6d f8 ff ff 41 f7 c6 00 02 00 00 74 01 fb 5b <5d> 41 5c 41 5d 41 5e c3 cc cc cc cc 83 3d 5b 07 8c 01 01 77 af 48
[ 1370.795391] RSP: 0018:ffa000001a5e4dc0 EFLAGS: 00000206
[ 1370.795392] RAX: 0000000000000000 RBX: 0000000000000008 RCX: 0000000000000000
[ 1370.795392] RDX: 0000000000000008 RSI: ffa000001a5e4d70 RDI: ffa000001a5e4d88
[ 1370.795393] RBP: ff1100fe7de32180 R08: 00000000944df6b4 R09: 00000000a09c5bf1
[ 1370.795393] R10: 00000000484ec753 R11: 00000000579a7d23 R12: ffa000001a5e4e98
[ 1370.795394] R13: ffa000001a5e4de8 R14: 0000000000000287 R15: 0000000000000001
[ 1370.795396]  _get_random_bytes+0x7a/0x190
[ 1370.795397]  ? __pfx_poll_health+0x10/0x10 [mlx5_core]
[ 1371.631955]  poll_health+0x97/0x230 [mlx5_core]
[ 1372.396402]  ? __pfx_poll_health+0x10/0x10 [mlx5_core]
[ 1373.380911]  call_timer_fn+0x24/0xf0
[ 1373.786964]  expire_timers+0xa7/0x110
[ 1373.786966]  run_timer_softirq+0xc0/0x1e0
[ 1373.786968]  handle_softirqs+0xe0/0x2a0
[ 1373.793032]  irq_exit_rcu+0x9e/0xc0
[ 1373.793033]  sysvec_apic_timer_interrupt+0x6e/0x90
[ 1374.001133]  </IRQ>
[ 1374.001134]  <TASK>
[ 1374.001135]  asm_sysvec_apic_timer_interrupt+0x16/0x20
[ 1374.001136] RIP: 0010:__rmid_read+0x1e/0x80
[ 1374.001218] Code: 90 90 90 90 90 90 90 90 90 90 90 90 0f 1f 44 00 00 55 89 f0 48 89 d5 b9 8d 0c 00 00 53 89 fa 0f 30 66 90 b9 8e 0c 00 00 0f 32 <48> c1 e2 20 48 89 d3 48 09 c3 66 90 48 85 db 78 44 48 0f ba e3 3e
[ 1374.001219] RSP: 0018:ffa0000030967db0 EFLAGS: 00000247
[ 1374.001220] RAX: 00000000000010c3 RBX: 0000000000000003 RCX: 0000000000000c8e
[ 1374.001224] RDX: 0000000000000000 RSI: 0000000000000003 RDI: 0000000000000001
[ 1374.001224] RBP: ffa0000030967dc8 R08: 0000000000000003 R09: ffa0000030967e10
[ 1374.001225] R10: 0000000000000007 R11: 0000000002df59c6 R12: ffa0000030967e10
[ 1374.001225] R13: 0000000000000001 R14: ff1100eb68dd7000 R15: ffa0000034323cb0
[ 1374.001228]  resctrl_arch_rmid_read+0x53/0x120
[ 1374.003239]  __mon_event_count+0xb4/0xe0
[ 1374.273438]  mon_event_count+0x26/0x90
[ 1374.273441]  smp_mon_event_count+0xa/0x20
[ 1374.429497]  smp_call_on_cpu_callback+0x26/0x50
[ 1374.429500]  process_one_work+0x17c/0x310
[ 1374.533537]  worker_thread+0x26d/0x2f0
[ 1374.533539]  ? __pfx_worker_thread+0x10/0x10
[ 1374.533540]  kthread+0xc9/0x100
[ 1374.847757]  ? __pfx_kthread+0x10/0x10
[ 1374.847759]  ret_from_fork+0x2d/0x50
[ 1374.847765]  ? __pfx_kthread+0x10/0x10
[ 1374.847766]  ret_from_fork_asm+0x1b/0x30
[ 1374.847770]  </TASK>
[ 1374.847773] INFO: NMI handler (perf_event_nmi_handler) took too long to run: 7781.310 msecs
[ 1386.839029] rcu: INFO: rcu_sched detected stalls on CPUs/tasks:
[ 1390.883857] Modules linked in:
[ 1390.959756] rcu: 	64-...0: (115 ticks this GP) idle=7bbc/1/0x4000000000000000 softirq=12297/12297 fqs=20993
[ 1390.960753] rcu: 	(detected by 194, t=64123 jiffies, g=192181, q=1018686 ncpus=256)
[ 1390.961256] Sending NMI from CPU 194 to CPUs 64:
[ 1390.993937] NMI backtrace for cpu 64
[ 1391.262097] CPU: 64 PID: 3187 Comm: kworker/64:3 Kdump: loaded Tainted: G S         OE      6.6.0-jdcloud7.sp1.131d3847b.x86_64 #1
[ 1391.262100] Hardware name: Supermicro SYS-822GS-NB3RT/X14DBG-LC, BIOS 1.4 08/19/2025
[ 1391.262101] Workqueue: events smp_call_on_cpu_callback
[ 1391.262116] RIP: 0010:_raw_spin_lock+0x0/0x30
[ 1391.262129] Code: cc cc 89 c6 e8 81 04 00 00 90 c3 cc cc cc cc 66 66 2e 0f 1f 84 00 00 00 00 00 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 <0f> 1f 44 00 00 31 c0 ba 01 00 00 00 f0 0f b1 17 75 05 c3 cc cc cc
[ 1391.262130] RSP: 0018:ffa000001a5e4ae8 EFLAGS: 00000002
[ 1391.262131] RAX: 00000000ffffffff RBX: 0000000000000000 RCX: ff1102007fa050e8
[ 1391.262132] RDX: 0000000000000001 RSI: 0000000000000002 RDI: ffffffff96e4f920
[ 1391.262133] RBP: 0000000000000046 R08: 000000000000461d R09: ffa000001a5e4b50
[ 1391.262134] R10: 0000000000000001 R11: 00000000ffff175e R12: ffffffff958bc7e9
[ 1391.262134] R13: ffa000001a5e4b50 R14: 0000000000000012 R15: 0000000000000000
[ 1391.262135] FS:  0000000000000000(0000) GS:ff1100fe7de00000(0000) knlGS:0000000000000000
[ 1391.262136] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[ 1391.262137] CR2: 0000562ddad590c8 CR3: 000000ed63b18006 CR4: 0000000000773ee0
[ 1391.262138] DR0: 0000000000000000 DR1: 0000000000000000 DR2: 0000000000000000
[ 1391.262139] DR3: 0000000000000000 DR6: 00000000fffe07f0 DR7: 0000000000000400
[ 1391.262139] PKRU: 55555554
[ 1391.262141] Call Trace:
[ 1391.262144]  <NMI>
[ 1391.262145]  ? nmi_cpu_backtrace+0x84/0xf0
[ 1391.262152]  ? nmi_cpu_backtrace_handler+0xd/0x20
[ 1391.314145]  ? nmi_handle+0x53/0x120
[ 1391.516217]  ? default_do_nmi+0x43/0x110
[ 1391.516225]  ? exc_nmi+0x100/0x190
[ 1391.572267]  ? end_repeat_nmi+0x16/0x67
[ 1391.684320]  ? __pfx__raw_spin_lock+0x10/0x10
[ 1391.684322]  ? __pfx__raw_spin_lock+0x10/0x10
[ 1391.684323]  ? __pfx__raw_spin_lock+0x10/0x10
[ 1391.684325]  </NMI>
[ 1391.684325]  <IRQ>
[ 1391.684325]  console_trylock_spinning+0x35/0xd0
[ 1391.742517]  vprintk_emit+0x135/0x1e0
[ 1391.746629]  ? __pfx_watchdog_timer_fn+0x10/0x10
[ 1392.070712]  _printk+0x58/0x80
[ 1392.070719]  print_modules+0x27/0xd0
[ 1392.070726]  watchdog_timer_fn+0x1a7/0x220
[ 1392.070728]  __run_hrtimer+0x59/0x190
[ 1392.070732]  __hrtimer_run_queues+0x7c/0xe0
[ 1392.070735]  hrtimer_interrupt+0xf4/0x230
[ 1392.120732]  __sysvec_apic_timer_interrupt+0x46/0xe0
[ 1392.120738]  sysvec_apic_timer_interrupt+0x35/0x90
[ 1392.170745]  asm_sysvec_apic_timer_interrupt+0x16/0x20
[ 1392.170747] RIP: 0010:crng_make_state+0x5e/0x120
```
