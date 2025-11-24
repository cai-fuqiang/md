## dmesg打印
```
[1610783.850515] device port-eiutx8oqst left promiscuous mode
[1610949.313615] INFO: task dockerd-current:5314 blocked for more than 122 seconds.
[1610949.314496]       Tainted: G S        IOE     5.10.0-60.70.0.94.jdcloud.1.x86_64 #1
[1610949.315211] "echo 0 > /proc/sys/kernel/hung_task_timeout_secs" disables this message.
[1610949.315882] task:dockerd-current state:D stack:    0 pid: 5314 ppid:     1 flags:0x00004080
[1610949.316553] Call Trace:
[1610949.317196]  __schedule+0x3f6/0x7d0
[1610949.317843]  ? xas_load+0x5/0x80
[1610949.318479]  schedule+0x46/0xb0
[1610949.319100]  schedule_timeout+0x240/0x2b0
[1610949.319721]  ? truncate_inode_pages_range+0x1b9/0x6f0
[1610949.320339]  wait_for_common+0x9f/0x150
[1610949.320955]  __flush_work.isra.0+0x60/0x80
[1610949.321564]  ? flush_workqueue_prep_pwqs+0x140/0x140
[1610949.322161]  destroy_inode+0x38/0x70
[1610949.322760]  do_unlinkat+0x1df/0x2f0
[1610949.323344]  do_syscall_64+0x3d/0x80
[1610949.323924]  entry_SYSCALL_64_after_hwframe+0x61/0xc6
[1610949.324502] RIP: 0033:0x4eab34
[1610949.325068] RSP: 002b:000000c424ffebe8 EFLAGS: 00000246 ORIG_RAX: 0000000000000107
[1610949.325648] RAX: ffffffffffffffda RBX: 0000000000000000 RCX: 00000000004eab34
[1610949.326220] RDX: 0000000000000000 RSI: 000000c4255ae750 RDI: ffffffffffffff9c
[1610949.326792] RBP: 000000c424ffec58 R08: 0000000000000000 R09: 0000000000000000
[1610949.327359] R10: 0000000000000000 R11: 0000000000000246 R12: 000000000000003a
[1610949.327923] R13: 00000000ffffffbb R14: 0000000000000750 R15: 0000000000000038
[1610949.328517] INFO: task jcs_eye:3341282 blocked for more than 122 seconds.
[1610949.329066]       Tainted: G S        IOE     5.10.0-60.70.0.94.jdcloud.1.x86_64 #1
[1610949.329628] "echo 0 > /proc/sys/kernel/hung_task_timeout_secs" disables this message.
[1610949.330183] task:jcs_eye         state:D stack:    0 pid:3341282 ppid:  2879 flags:0x00000080
[1610949.330750] Call Trace:
[1610949.331295]  __schedule+0x3f6/0x7d0
[1610949.331848]  schedule+0x46/0xb0
[1610949.332382]  schedule_timeout+0x240/0x2b0
[1610949.332919]  ? sched_clock+0x5/0x10
[1610949.333438]  ? sched_clock_cpu+0xc/0xb0
[1610949.333963]  ? __smp_call_single_queue+0x23/0x40
[1610949.334479]  ? ttwu_queue_wakelist.part.0+0xbd/0xe0
[1610949.334983]  wait_for_common+0x9f/0x150
[1610949.335487]  __flush_work.isra.0+0x60/0x80
[1610949.335984]  ? flush_workqueue_prep_pwqs+0x140/0x140
[1610949.336545]  xfs_inodegc_flush.part.0+0x3e/0x90 [xfs]
[1610949.337100]  xfs_fs_statfs+0x2d/0x1a0 [xfs]
[1610949.337585]  statfs_by_dentry+0x64/0x90
[1610949.338062]  user_statfs+0x57/0xc0
[1610949.338531]  __se_sys_statfs+0x25/0x60
[1610949.338990]  do_syscall_64+0x3d/0x80
[1610949.339438]  entry_SYSCALL_64_after_hwframe+0x61/0xc6
[1610949.339897] RIP: 0033:0x4ba370
[1610949.340338] RSP: 002b:000000c001bfb8e8 EFLAGS: 00000212 ORIG_RAX: 0000000000000089
[1610949.340793] RAX: ffffffffffffffda RBX: 000000c00002e000 RCX: 00000000004ba370
[1610949.341236] RDX: 0000000000000000 RSI: 000000c001bfb9b8 RDI: 000000c00221dd28
[1610949.341682] RBP: 000000c001bfb948 R08: 0000000000000000 R09: 0000000000000000
[1610949.342112] R10: 0000000000000000 R11: 0000000000000212 R12: 0000000000000012
[1610949.342539] R13: 0000000000000074 R14: 0000000000ce333a R15: 0000000000000000
```

### 以一个进程`jcs-eye`分析
```
crash> ps |grep jcs_eye
   2885   2879  59  ffff941d43f8c180  IN   0.0 1256500  96268  jcs_eye
   3002   2879  65  ffff941d57ffc180  IN   0.0 1256500  96268  jcs_eye
   3003   2879   1  ffff941d57ff8000  IN   0.0 1256500  96268  jcs_eye
   3006   2879  38  ffff941d57ffa0c0  IN   0.0 1256500  96268  jcs_eye
   3007   2879  57  ffff941d610f4180  IN   0.0 1256500  96268  jcs_eye
   3023   2879  69  ffff941e72038000  IN   0.0 1256500  96268  jcs_eye
   3284   2879  22  ffff941d43364180  IN   0.0 1256500  96268  jcs_eye
   3292   2879  10  ffff93ee0e7e4180  IN   0.0 1256500  96268  jcs_eye
   3294   2879  58  ffff93ee40618000  IN   0.0 1256500  96268  jcs_eye
   3303   2879  61  ffff941d43f18000  IN   0.0 1256500  96268  jcs_eye
   4744   2879  14  ffff941da913a0c0  IN   0.0 1256500  96268  jcs_eye
  3338804   2879  59  ffff943403918000  IN   0.0 1256500  96268  jcs_eye
  3339703   2879  12  ffff93f021fcc180  IN   0.0 1256500  96268  jcs_eye
  3340657   2879  15  ffff941e70bca0c0  IN   0.0 1256500  96268  jcs_eye
  3341282   2879  64  ffff941d44524180  UN   0.0 1256500  96268  jcs_eye
  3341805   2879  28  ffff93fe75dda0c0  IN   0.0 1256500  96268  jcs_eye
```
有一个线程`D`了

查看线程堆栈:
```
crash> bt ffff941d44524180
PID: 3341282  TASK: ffff941d44524180  CPU: 64  COMMAND: "jcs_eye"
 #0 [ffffbbd86abdbbd0] __schedule at ffffffff9c46f786
 #1 [ffffbbd86abdbc60] schedule at ffffffff9c46fba6
 #2 [ffffbbd86abdbc78] schedule_timeout at ffffffff9c472d90
 #3 [ffffbbd86abdbd18] wait_for_common at ffffffff9c47054f
 #4 [ffffbbd86abdbd78] __flush_work at ffffffff9bb01700
 #5 [ffffbbd86abdbdf8] xfs_inodegc_flush at ffffffffc033b80e [xfs]
 #6 [ffffbbd86abdbe10] xfs_fs_statfs at ffffffffc03512fd [xfs]
 #7 [ffffbbd86abdbe50] statfs_by_dentry at ffffffff9bda5014
 #8 [ffffbbd86abdbe68] user_statfs at ffffffff9bda57a7
 #9 [ffffbbd86abdbea8] __se_sys_statfs at ffffffff9bda5835
#10 [ffffbbd86abdbf38] do_syscall_64 at ffffffff9c462d0d
#11 [ffffbbd86abdbf50] entry_SYSCALL_64_after_hwframe at ffffffff9c600099
    RIP: 00000000004ba370  RSP: 000000c001bfb8e8  RFLAGS: 00000212
    RAX: ffffffffffffffda  RBX: 000000c00002e000  RCX: 00000000004ba370
    RDX: 0000000000000000  RSI: 000000c001bfb9b8  RDI: 000000c00221dd28
    RBP: 000000c001bfb948   R8: 0000000000000000   R9: 0000000000000000
    R10: 0000000000000000  R11: 0000000000000212  R12: 0000000000000012
    R13: 0000000000000074  R14: 0000000000ce333a  R15: 0000000000000000
    ORIG_RAX: 0000000000000089  CS: 0033  SS: 002b
```
通过`xfs_fs_statfs` 堆栈，查找`dentry`
```sh
xfs_fs_statfs(
  struct dentry   *dentry,
  struct kstatfs   *statp)
```

堆栈分析:
```
0xffffffff9bda5000 <statfs_by_dentry+80>:       mov    0x68(%rbp),%rax
0xffffffff9bda5004 <statfs_by_dentry+84>:       mov    %rbx,%rsi --> 第二个参数
0xffffffff9bda5007 <statfs_by_dentry+87>:       mov    %rbp,%rdi --> 第一个参数
0xffffffff9bda500a <statfs_by_dentry+90>:       mov    0x30(%rax),%rax
0xffffffff9bda500e <statfs_by_dentry+94>:       mov    0x68(%rax),%rax
0xffffffff9bda5012 <statfs_by_dentry+98>:       callq  *%rax
0xffffffff9bda5014 <statfs_by_dentry+100>:      nopl   (%rax)rax),%rax

crash> dis -l xfs_fs_statfs
0xffffffffc03512d0 <xfs_fs_statfs>:     nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc03512d5 <xfs_fs_statfs+5>:   push   %r15
0xffffffffc03512d7 <xfs_fs_statfs+7>:   push   %r14
0xffffffffc03512d9 <xfs_fs_statfs+9>:   push   %r13
0xffffffffc03512db <xfs_fs_statfs+11>:  mov    %rsi,%r13
0xffffffffc03512de <xfs_fs_statfs+14>:  push   %r12
0xffffffffc03512e0 <xfs_fs_statfs+16>:  push   %rbp
0xffffffffc03512e1 <xfs_fs_statfs+17>:  push   %rbx
0xffffffffc03512e2 <xfs_fs_statfs+18>:  sub    $0x8,%rsp
0xffffffffc03512e6 <xfs_fs_statfs+22>:  mov    0x68(%rdi),%rax
0xffffffffc03512ea <xfs_fs_statfs+26>:  mov    0x30(%rdi),%r14
0xffffffffc03512ee <xfs_fs_statfs+30>:  mov    0x370(%rax),%r12
0xffffffffc03512f5 <xfs_fs_statfs+37>:  mov    %r12,%rdi
0xffffffffc03512f8 <xfs_fs_statfs+40>:  callq  0xffffffffc033d6d0 <xfs_inodegc_flush>
```

`bt -f` 堆栈打印:
```
 #6 [ffffbbd86abdbe10] xfs_fs_statfs at ffffffffc03512fd [xfs]
    ffffbbd86abdbe18: 0000000000000000 ffffbbd86abdbeb0[rbx]
    ffffbbd86abdbe28: ffff941d49109bd8[rbp] 0000000000000000[r12]
    ffffbbd86abdbe38: 0000000000000002[r13] 0000000000000000[r14]
    ffffbbd86abdbe48: 0000000000000000[r15] ffffffff9bda5014 [ip]
 #7 [ffffbbd86abdbe50] statfs_by_dentry at ffffffff9bda5014
    ffffbbd86abdbe58: 000000c00221dd28 ffffbbd86abdbeb0
    ffffbbd86abdbe68: ffffffff9bda57a7
```
查看`dentry`
```
crash> struct dentry.d_name,d_parent,d_sb ffff941d49109bd8
  d_name = {
    {
      {
        hash = 0,
        len = 1
      },
      hash_len = 4294967296
    },
    name = 0xffff941d49109c10 "/"
  }
  d_parent = 0xffff941d49109bd8
  d_sb = 0xffff941d45f98800
```
居然是根 !!

## 其他xfs-worker线程
```
root        1538  0.0  0.0      0     0 ?        I<   11月05   0:31 [kworker/33:1H-xfs-log/nvme0n1p1]
root        1642  0.0  0.0      0     0 ?        I<   11月05   0:30 [kworker/11:1H-xfs-log/nvme0n1p1]
root      101213  0.0  0.0      0     0 ?        D    12:02   0:00 [kworker/73:0+xfs-inodegc/nvme0n1p1]
root      155673  0.0  0.0      0     0 ?        D    12:03   0:00 [kworker/28:1+xfs-inodegc/nvme0n1p1]
root      165980  0.0  0.0      0     0 ?        D    12:04   0:00 [kworker/2:0+xfs-inodegc/nvme0n1p1]
root      189221  0.0  0.0      0     0 ?        D    12:05   0:00 [kworker/62:2+xfs-inodegc/nvme0n1p1]
root      215436  0.0  0.0      0     0 ?        D    12:05   0:00 [kworker/17:1+xfs-inodegc/nvme0n1p1]
root     2652854  0.0  0.0   6608  2276 pts/5    S+   15:43   0:00 grep --color=auto xfs
root     3461769  0.0  0.0      0     0 ?        D    11:35   0:00 [kworker/71:2+xfs-inodegc/nvme0n1p1]
root     3497751  0.0  0.0      0     0 ?        D    11:36   0:00 [kworker/u161:3+xfs-blockgc/nvme0n1p1]
root     3632854  0.0  0.0      0     0 ?        D    11:41   0:00 [kworker/79:1+xfs-inodegc/nvme0n1p1]
root     4089746  0.0  0.0      0     0 ?        D    11:55   0:00 [kworker/31:0+xfs-inodegc/nvme0n1p1]
root     4104022  0.0  0.0      0     0 ?        D    11:56   0:00 [kworker/19:1+xfs-inodegc/nvme0n1p1]
```

### 最后一个 4104022
```
crash> bt 4104022
PID: 4104022  TASK: ffff941d40384180  CPU: 19  COMMAND: "kworker/19:1"
 #0 [ffffbbd8737cb9d8] __schedule at ffffffff9c46f786
 #1 [ffffbbd8737cba68] schedule at ffffffff9c46fba6
 #2 [ffffbbd8737cba80] schedule_timeout at ffffffff9c472d90
 #3 [ffffbbd8737cbb20] __down at ffffffff9c4718b3
 #4 [ffffbbd8737cbb70] down at ffffffff9bb4bacb
 #5 [ffffbbd8737cbb88] xfs_buf_lock at ffffffffc0330c2c [xfs]
 #6 [ffffbbd8737cbb98] xfs_buf_find at ffffffffc0330fd5 [xfs]
 #7 [ffffbbd8737cbbf8] xfs_buf_get_map at ffffffffc0331414 [xfs]
 #8 [ffffbbd8737cbc48] xfs_buf_read_map at ffffffffc0332454 [xfs]
 #9 [ffffbbd8737cbca8] xfs_trans_read_buf_map at ffffffffc036a54c [xfs]
#10 [ffffbbd8737cbd08] xfs_read_agi at ffffffffc03169fe [xfs]
#11 [ffffbbd8737cbd58] xfs_iunlink_remove at ffffffffc0346ed1 [xfs]
#12 [ffffbbd8737cbdd8] xfs_ifree at ffffffffc0348b2a [xfs]
#13 [ffffbbd8737cbe28] xfs_inactive_ifree at ffffffffc0348cd2 [xfs]
#14 [ffffbbd8737cbe50] xfs_inactive at ffffffffc0348ed5 [xfs]
#15 [ffffbbd8737cbe68] xfs_inodegc_inactivate at ffffffffc033b446 [xfs]
#16 [ffffbbd8737cbe78] xfs_inodegc_worker at ffffffffc033d686 [xfs]
#17 [ffffbbd8737cbe98] process_one_work at ffffffff9bb01c7d
#18 [ffffbbd8737cbed8] worker_thread at ffffffff9bb021d9
#19 [ffffbbd8737cbf10] kthread at ffffffff9bb0741b
#20 [ffffbbd8737cbf50] ret_from_fork at ffffffff9ba0358f
```

通过该堆栈，尝试找到信号量数据结构:
```
0xffffffffc0330fbf <xfs_buf_find+783>:  mov    %rbx,%rdi
0xffffffffc0330fc2 <xfs_buf_find+786>:  testl  $0x40000000,0x14(%rsp)
0xffffffffc0330fca <xfs_buf_find+794>:  jne    0xffffffffc03311fd <xfs_buf_find+1357>
0xffffffffc0330fd0 <xfs_buf_find+800>:  callq  0xffffffffc0330c00 <xfs_buf_lock>

0xffffffff9c471830 <__down>:    nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9c471835 <__down+5>:  push   %r12
0xffffffff9c471837 <__down+7>:  mov    %rdi,%r12
0xffffffff9c47183a <__down+10>: push   %rbp
0xffffffff9c47183b <__down+11>: lea    0x8(%rdi),%rbp
0xffffffff9c47183f <__down+15>: push   %rbx [here]

 #3 [ffffbbd8737cbb20] __down at ffffffff9c4718b3
    ffffbbd8737cbb28: ffffbbd86c4d3b28 ffffbbd861ff7b28
    ffffbbd8737cbb38: ffff941d40384180 ffffffffc0355b00
    ffffbbd8737cbb48: ffff941d6146f800 bc5d15c0e87d9600
    ffffbbd8737cbb58: ffff941d6146df80[rbx] ffff941d6146dfa0
    ffffbbd8737cbb68: 0000000000000000 ffffffff9bb4bacb
```

找到其`xfs_buf_t` 数据结构:
```
crash> struct xfs_buf.b_sema ffff941d6146df80
  b_sema = {
    lock = {
      raw_lock = {
        {
          val = {
            counter = 0
          },
          {
            locked = 0 '\000',
            pending = 0 '\000'
          },
          {
            locked_pending = 0,
            tail = 0
          }
        }
      }
    },
    count = 0,
    wait_list = {
      next = 0xffffbbd861ff7b28,
      prev = 0xffffbbd84d3d37b8
    }
  }
```
查看其`list`:
```
crash> struct semaphore ffff941d6146dfa0 -o
struct semaphore {
  [ffff941d6146dfa0] raw_spinlock_t lock;
  [ffff941d6146dfa4] unsigned int count;
  [ffff941d6146dfa8] struct list_head wait_list;
}

crash> list -H ffff941d6146dfa8 -o semaphore_waiter.list -s semaphore_waiter.task,up
ffffbbd861ff7b28
  task = 0xffff942801b9a0c0
  up = false
ffffbbd8737cbb28
  task = 0xffff941d40384180
  up = false
ffffbbd86c4d3b28
  task = 0xffff93f36fe3c180
  up = false
ffffbbd8627679c0
  task = 0xffff93ee42ffc180
  up = false
ffffbbd86ffb77b8
  task = 0xffff941d46148000
  up = false
ffffbbd86fd97a58
  task = 0xffff93f51db920c0
  up = false
ffffbbd84d3d37b8
  task = 0xffff942964e94180
  up = false
```
查看这些waiter所代表的进程:
```
crash> task_struct.pid,comm 0xffff942801b9a0c0
  pid = 3632854
  comm = "kworker/79:1\000\000\000"
crash> task_struct.pid,comm 0xffff941d40384180
  pid = 4104022
  comm = "kworker/19:1\000\000\000"
crash> task_struct.pid,comm 0xffff93f36fe3c180
  pid = 155673
  comm = "kworker/28:1\000\000\000"
crash> task_struct.comm 0xffff93ee42ffc180
  comm = "jcs_agent\000\000\000\000\000\000"
crash> task_struct.comm 0xffff941d46148000
  comm = "jd-falcon-agent"
crash> task_struct.comm 0xffff93f51db920c0
  comm = "cell_vs_control"
crash> task_struct.comm 0xffff942964e94180
  comm = "xcgroup\000\060\000\000\000\000\000\000"
```
### 倒数第二个 4089746
```
 #3 [ffffbbd86c323870] __down at ffffffff9c4718b3
    ffffbbd86c323878: ffffbbd86fd6f878 ffffbbd865d4f878
    ffffbbd86c323888: ffff941d4081a0c0 0000000000000000
    ffffbbd86c323898: ffff941d4081a180 df9bf66c1ca66300
    ffffbbd86c3238a8: ffff941d6146ce00[rbx] ffff941d6146ce20
    ffffbbd86c3238b8: 0000000000000000 ffffffff9bb4bacb

crash> struct xfs_buf.b_sema  ffff941d6146ce00 -o
struct xfs_buf {
  [ffff941d6146ce20] struct semaphore b_sema;
}

crash> struct semaphore.wait_list ffff941d6146ce20 -o
struct semaphore {
  [ffff941d6146ce28] struct list_head wait_list;
}

crash> list -H ffff941d6146ce28  -o semaphore_waiter.list -s semaphore_waiter.task,up
ffffbbd865e9f578
  task = 0xffff942a30e8a0c0
  up = false
ffffbbd861f675f0
  task = 0xffff941d57efa0c0
  up = false
ffffbbd865d4f878
  task = 0xffff93f47f1720c0
  up = false
ffffbbd86c323878
  task = 0xffff941d4081a0c0
  up = false
ffffbbd86fd6f878
  task = 0xffff941d88fb0000
  up = false
ffffbbd866bcf878
  task = 0xffff940a13ef20c0
  up = false
ffffbbd862af7878
  task = 0xffff942ade4720c0
  up = false
ffffbbd87131f6f0
  task = 0xffff93fe9b124180
  up = false

crash> task_struct.pid,comm 0xffff942a30e8a0c0
  pid = 215436
  comm = "kworker/17:1\000\000\000"
crash> task_struct.pid,comm 0xffff941d57efa0c0
  pid = 883883
  comm = "cygnus_manager\000"
crash> task_struct.pid,comm 0xffff93f47f1720c0
  pid = 189221
  comm = "kworker/62:2\000\000\000"
crash> task_struct.pid,comm 0xffff941d4081a0c0
  pid = 4089746
  comm = "kworker/31:0\000\000\000"
crash> task_struct.pid,comm 0xffff941d88fb0000
  pid = 3461769
  comm = "kworker/71:2\000\000\000"
crash> task_struct.pid,comm 0xffff940a13ef20c0
  pid = 165980
  comm = "kworker/2:0\000\000\000\000"
crash> task_struct.pid,comm 0xffff942ade4720c0
  pid = 101213
  comm = "kworker/73:0\000\000\000"
crash> task_struct.pid,comm 0xffff93fe9b124180
  pid = 3497751
  comm = "kworker/u161:3\000"
```
自此，所有的worker均找到。都是waiter。那谁是owner ?


### xfs_ilock堆栈
```
crash> bt ffff93fc992720c0
PID: 1760070  TASK: ffff93fc992720c0  CPU: 21  COMMAND: "node_monitor"
 #0 [ffffbbd84cdfbc60] __schedule at ffffffff9c46f786
 #1 [ffffbbd84cdfbcf0] schedule at ffffffff9c46fba6
 #2 [ffffbbd84cdfbd08] rwsem_down_write_slowpath at ffffffff9bb4c5d0
 #3 [ffffbbd84cdfbdb8] xfs_ilock at ffffffffc034753b [xfs]
 #4 [ffffbbd84cdfbdd0] xfs_file_buffered_aio_write at ffffffffc0336926 [xfs]
 #5 [ffffbbd84cdfbe40] new_sync_write at ffffffff9bd62580
 #6 [ffffbbd84cdfbec8] vfs_write at ffffffff9bd653b7
 #7 [ffffbbd84cdfbf00] ksys_write at ffffffff9bd6573f
 #8 [ffffbbd84cdfbf38] do_syscall_64 at ffffffff9c462d0d
 #9 [ffffbbd84cdfbf50] entry_SYSCALL_64_after_hwframe at ffffffff9c600099
    RIP: 00000000004c6f70  RSP: 000000c0000fa548  RFLAGS: 00000212
    RAX: ffffffffffffffda  RBX: 000000c000030500  RCX: 00000000004c6f70
    RDX: 0000000000003c4d  RSI: 000000c0003b4000  RDI: 0000000000000006
    RBP: 000000c0000fa598   R8: 0000000000000000   R9: 0000000000000000
    R10: 0000000000000000  R11: 0000000000000212  R12: 00000000000000f2
    R13: 0000000000000000  R14: 0000000000cf6740  R15: 0000000000000000
    ORIG_RAX: 0000000000000001  CS: 0033  SS: 002b
```

分析堆栈:
```
0xffffffffc03368f8 <xfs_file_buffered_aio_write+56>:    mov    %rdi,%rbx
0xffffffffc03368fb <xfs_file_buffered_aio_write+59>:    mov    %rsi,%r12
0xffffffffc03368fe <xfs_file_buffered_aio_write+62>:    lea    -0x138(%rbp),%r13
0xffffffffc0336905 <xfs_file_buffered_aio_write+69>:    xor    %r14d,%r14d
0xffffffffc0336908 <xfs_file_buffered_aio_write+72>:    mov    %gs:0x1f040,%r15
0xffffffffc0336911 <xfs_file_buffered_aio_write+81>:    mov    $0x1,%esi
0xffffffffc0336916 <xfs_file_buffered_aio_write+86>:    mov    %r13,%rdi [第一个]
0xffffffffc0336919 <xfs_file_buffered_aio_write+89>:    movl   $0x1,0xc(%rsp)
0xffffffffc0336921 <xfs_file_buffered_aio_write+97>:    callq  0xffffffffc0347470 <xfs_ilock>


## xfs_ilock没有保存
0xffffffffc0347470 <xfs_ilock>: nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc0347475 <xfs_ilock+5>:       push   %rbp
0xffffffffc0347476 <xfs_ilock+6>:       mov    %rdi,%rbp
0xffffffffc0347479 <xfs_ilock+9>:       push   %rbx
0xffffffffc034747a <xfs_ilock+10>:      mov    %esi,%ebx
0xffffffffc034747c <xfs_ilock+12>:      mov    0x10(%rsp),%rcx
0xffffffffc0347481 <xfs_ilock+17>:      nopl   0x0(%rax,%rax,1)
0xffffffffc0347486 <xfs_ilock+22>:      test   $0x1,%bl

## 
0xffffffff9bb4c2b0 <rwsem_down_write_slowpath>: nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9bb4c2b5 <rwsem_down_write_slowpath+5>:       push   %r15
0xffffffff9bb4c2b7 <rwsem_down_write_slowpath+7>:       push   %r14
0xffffffff9bb4c2b9 <rwsem_down_write_slowpath+9>:       push   %r13[第三个]
0xffffffff9bb4c2bb <rwsem_down_write_slowpath+11>:      push   %r12
0xffffffff9bb4c2bd <rwsem_down_write_slowpath+13>:      mov    %rdi,%r12
0xffffffff9bb4c2c0 <rwsem_down_write_slowpath+16>:      push   %rbp
0xffffffff9bb4c2c1 <rwsem_down_write_slowpath+17>:      push   %rbx
0xffffffff9bb4c2c2 <rwsem_down_write_slowpath+18>:      sub    $0x78,%rsp
```

bt -f 
```
 #2 [ffffbbd84cdfbd08] rwsem_down_write_slowpath at ffffffff9bb4c5d0
    ffffbbd84cdfbd10: ffff93fa683d9fac 0000000200000000
    ffffbbd84cdfbd20: 0000000000000000 0000000000000000
    ffffbbd84cdfbd30: ffffffff9c46f78e 0000000000000001
    ffffbbd84cdfbd40: ffffbbd84cdfbd38 ffff93fa683d9fb0
    ffffbbd84cdfbd50: ffff93fa683d9fb0 ffff93fc992720c0
    ffffbbd84cdfbd60: ffff93fc00000000 0000000160038ccc
    ffffbbd84cdfbd70: ffff93fc992720c0 0000000000000246
    ffffbbd84cdfbd80: 1a667e5c998d6c00 0000000000000001
    ffffbbd84cdfbd90: ffff93fa683d9dc0 ffffbbd84cdfbe58
    ffffbbd84cdfbda0: ffff93fa683d9dc0[this] 0000000000000000
    ffffbbd84cdfbdb0: ffff93fc992720c0 ffffffffc034753b
```

查看owner:
```
crash> xfs_inode_t.i_lock ffff93fa683d9dc0 -x
  i_lock = {
    mr_lock = {
      count = {
        counter = 0x1
      },
      owner = {
        counter = 0xffff93fe9b124180
      },
      osq = {
        tail = {
          counter = 0x0
        }
      },
      wait_lock = {
        raw_lock = {
          {
            val = {
              counter = 0x0
            },
            {
              locked = 0x0,
              pending = 0x0
            },
            {
              locked_pending = 0x0,
              tail = 0x0
            }
          }
        }
      },
      wait_list = {
        next = 0xffff93fa683d9e50,
        prev = 0xffff93fa683d9e50
      }
    }
  }
```
查看堆栈:

```
crash> bt 0xffff93fe9b124180
PID: 3497751  TASK: ffff93fe9b124180  CPU: 76  COMMAND: "kworker/u161:3"
 #0 [ffffbbd87131f5a0] __schedule at ffffffff9c46f786
 #1 [ffffbbd87131f630] schedule at ffffffff9c46fba6
 #2 [ffffbbd87131f648] schedule_timeout at ffffffff9c472d90
 #3 [ffffbbd87131f6e8] __down at ffffffff9c4718b3
 #4 [ffffbbd87131f738] down at ffffffff9bb4bacb
 #5 [ffffbbd87131f750] xfs_buf_lock at ffffffffc0330c2c [xfs]
 #6 [ffffbbd87131f760] xfs_buf_find at ffffffffc0330fd5 [xfs]
 #7 [ffffbbd87131f7c0] xfs_buf_get_map at ffffffffc0331414 [xfs]
 #8 [ffffbbd87131f810] xfs_buf_read_map at ffffffffc0332454 [xfs]
 #9 [ffffbbd87131f870] xfs_trans_read_buf_map at ffffffffc036a54c [xfs]
#10 [ffffbbd87131f8d0] xfs_read_agf at ffffffffc02e5b47 [xfs]
#11 [ffffbbd87131f928] xfs_alloc_read_agf at ffffffffc02e5c03 [xfs]
#12 [ffffbbd87131f950] xfs_alloc_fix_freelist at ffffffffc02e6086 [xfs]
#13 [ffffbbd87131fa40] xfs_free_extent_fix_freelist at ffffffffc02e6625 [xfs]
#14 [ffffbbd87131faf0] __xfs_free_extent at ffffffffc02e66c8 [xfs]
#15 [ffffbbd87131fb50] xfs_trans_free_extent at ffffffffc036026e [xfs]
#16 [ffffbbd87131fb88] xfs_extent_free_finish_item at ffffffffc0360324 [xfs]
#17 [ffffbbd87131fba0] xfs_defer_finish_one at ffffffffc0306592 [xfs]
#18 [ffffbbd87131fbf0] xfs_defer_finish_noroll at ffffffffc0306ac5 [xfs]
#19 [ffffbbd87131fc28] xfs_defer_finish at ffffffffc0306c31 [xfs]
#20 [ffffbbd87131fc48] xfs_itruncate_extents_flags at ffffffffc03485f1 [xfs]
#21 [ffffbbd87131fca0] xfs_free_eofblocks at ffffffffc032e308 [xfs]
#22 [ffffbbd87131fd00] xfs_icwalk_ag at ffffffffc033cc31 [xfs]
#23 [ffffbbd87131fe78] xfs_blockgc_worker at ffffffffc033d1c4 [xfs]
#24 [ffffbbd87131fe98] process_one_work at ffffffff9bb01c7d
#25 [ffffbbd87131fed8] worker_thread at ffffffff9bb021d9
#26 [ffffbbd87131ff10] kthread at ffffffff9bb0741b
#27 [ffffbbd87131ff50] ret_from_fork at ffffffff9ba0358f
```

仍然指向`xfs_buf_lock`. 

### 其他 2 down_read(&sb->s_mount)

例如`4129433`堆栈，显示锁`sb`
```
[root@11-211-65-37 18:59:39 ~]# cat /proc/4129433/stack
 #0 [ffffbbd86271fd58] __schedule at ffffffff9c46f786
 #1 [ffffbbd86271fde8] schedule at ffffffff9c46fba6
 #2 [ffffbbd86271fe00] rwsem_down_write_slowpath at ffffffff9bb4c5d0
 #3 [ffffbbd86271feb0] do_reconfigure_mnt at ffffffff9bd8ba7d
 #4 [ffffbbd86271fee8] __se_sys_mount at ffffffff9bd90005
 #5 [ffffbbd86271ff38] do_syscall_64 at ffffffff9c462d0d
 #6 [ffffbbd86271ff50] entry_SYSCALL_64_after_hwframe at ffffffff9c600099
```

查看堆栈:
```
0xffffffff9bd8fff1 <__se_sys_mount+337>:        mov    %r12,%rdi
0xffffffff9bd8fff4 <__se_sys_mount+340>:        mov    %r15,%r8
0xffffffff9bd8fff7 <__se_sys_mount+343>:        mov    %r14,%rcx
0xffffffff9bd8fffa <__se_sys_mount+346>:        mov    %rbp,%rdx
0xffffffff9bd8fffd <__se_sys_mount+349>:        mov    %rsp,%rsi
0xffffffff9bd90000 <__se_sys_mount+352>:        callq  0xffffffff9bd8fb70 <path_mount>

0xffffffff9bd8fe39 <path_mount+713>:    add    $0x20,%rsp
0xffffffff9bd8fe3d <path_mount+717>:    mov    %r13,%rdi
0xffffffff9bd8fe40 <path_mount+720>:    mov    %r10d,%esi
0xffffffff9bd8fe43 <path_mount+723>:    pop    %rbx
0xffffffff9bd8fe44 <path_mount+724>:    pop    %rbp
0xffffffff9bd8fe45 <path_mount+725>:    pop    %r12
0xffffffff9bd8fe47 <path_mount+727>:    pop    %r13
0xffffffff9bd8fe49 <path_mount+729>:    pop    %r14
0xffffffff9bd8fe4b <path_mount+731>:    jmpq   0xffffffff9bd8ba10 <do_reconfigure_mnt>

0xffffffff9bd8ba10 <do_reconfigure_mnt>:        nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9bd8ba15 <do_reconfigure_mnt+5>:      push   %r15
0xffffffff9bd8ba17 <do_reconfigure_mnt+7>:      mov    %gs:0x1f040,%rax
0xffffffff9bd8ba20 <do_reconfigure_mnt+16>:     push   %r14
0xffffffff9bd8ba22 <do_reconfigure_mnt+18>:     push   %r13
0xffffffff9bd8ba24 <do_reconfigure_mnt+20>:     push   %r12 [this]
0xffffffff9bd8ba26 <do_reconfigure_mnt+22>:     push   %rbp
0xffffffff9bd8ba27 <do_reconfigure_mnt+23>:     push   %rbx

...

0xffffffff9bd8ba6d <do_reconfigure_mnt+93>:     add    $0x70,%r14
0xffffffff9bd8ba71 <do_reconfigure_mnt+97>:     lea    -0x20(%rbp),%r15
0xffffffff9bd8ba75 <do_reconfigure_mnt+101>:    mov    %r14,%rdi[看这个把]
0xffffffff9bd8ba78 <do_reconfigure_mnt+104>:    callq  0xffffffff9c4722d0 <down_write>

0xffffffff9bb4c2b0 <rwsem_down_write_slowpath>: nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffff9bb4c2b5 <rwsem_down_write_slowpath+5>:       push   %r15
0xffffffff9bb4c2b7 <rwsem_down_write_slowpath+7>:       push   %r14
0xffffffff9bb4c2b9 <rwsem_down_write_slowpath+9>:       push   %r13


 #2 [ffffbbd86271fe00] rwsem_down_write_slowpath at ffffffff9bb4c5d0
    ffffbbd86271fe08: ffff941d45f98884 0000000200000000
    ffffbbd86271fe18: 0000000000000006 0000000000000000
    ffffbbd86271fe28: 0000000000000000 0000000000000001
    ffffbbd86271fe38: ffffbbd86271fe30 ffffbbd86408be40
    ffffbbd86271fe48: ffffbbd86f067e40 ffff943295670000
    ffffbbd86271fe58: ffffffff00000000 0000000160755a88
    ffffbbd86271fe68: 0000000000000000 0000000000000000
    ffffbbd86271fe78: ec533ef1b9148600 0000000000000001
    ffffbbd86271fe88: ffff93eee4ca9560 0000000000000060
    ffffbbd86271fe98: ffffbbd86271fef0 ffff941d45f98870[r14]
    ffffbbd86271fea8: ffff93eee4ca9540 ffffffff9bd8ba7d
 #3 [ffffbbd86271feb0] do_reconfigure_mnt at ffffffff9bd8ba7d
    ffffbbd86271feb8: 0000000000000000 0000000000000000
    ffffbbd86271fec8: 0000000000000000 0000000000000000
    ffffbbd86271fed8: 0000000000001021 0000000000000000
    ffffbbd86271fee8: ffffffff9bd90005
 #4 [ffffbbd86271fee8] __se_sys_mount at ffffffff9bd90005
    ffffbbd86271fef0: ffff93eee4ca9560 ffff941d49109bd8
    ffffbbd86271ff00: ec533ef1b9148600 0000000000000000
    ffffbbd86271ff10: ffffbbd86271ff48 ffffbbd86271ff58
    ffffbbd86271ff20: 0000000000000000 0000000000000000
    ffffbbd86271ff30: 0000000000000000 ffffffff9c462d0d
```
查看owner:
```
crash> struct rw_semaphore ffff941d45f98870 -x
struct rw_semaphore {
  count = {
    counter = 0x102
  },
  owner = {
    counter = 0xffff9431354120c7
  },

crash> struct task_struct.pid,comm 0xffff9431354120c7
  pid = 1859518464
  comm = "/u162:2\000\000\000\000\000\000\000\000"

crash> kmem 0xffff9431354120c7
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff93ee00047700     8328       3335      3726   1242    32k  task_struct
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  fffffb63d0d50400  ffff943135410000     1      3          2     1
  FREE / [ALLOCATED]
  [ffff9431354120c0]

    PID: 2640498
COMMAND: "kworker/u162:2"
   TASK: ffff9431354120c0  [THREAD_INFO: ffff9431354120c0]
    CPU: 65
  STATE: TASK_UNINTERRUPTIBLE

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
fffffb63d0d50480 4435412000 dead000000000400        0  0 57ffffc0000000
```

堆栈:
```
crash> bt ffff9431354120c0
PID: 2640498  TASK: ffff9431354120c0  CPU: 65  COMMAND: "kworker/u162:2"
 #0 [ffffbbd867db7290] __schedule at ffffffff9c46f786
 #1 [ffffbbd867db7320] schedule at ffffffff9c46fba6
 #2 [ffffbbd867db7338] schedule_timeout at ffffffff9c472d90
 #3 [ffffbbd867db73d8] __down at ffffffff9c4718b3
 #4 [ffffbbd867db7428] down at ffffffff9bb4bacb
 #5 [ffffbbd867db7440] xfs_buf_lock at ffffffffc0330c2c [xfs]
 #6 [ffffbbd867db7450] xfs_buf_find at ffffffffc0330fd5 [xfs]
 #7 [ffffbbd867db74b0] xfs_buf_get_map at ffffffffc0331414 [xfs]
 #8 [ffffbbd867db7500] xfs_buf_read_map at ffffffffc0332454 [xfs]
 #9 [ffffbbd867db7560] xfs_trans_read_buf_map at ffffffffc036a54c [xfs]
#10 [ffffbbd867db75c0] xfs_imap_to_bp at ffffffffc031b841 [xfs]
#11 [ffffbbd867db7618] xfs_trans_log_inode at ffffffffc03291b7 [xfs]
#12 [ffffbbd867db7678] xfs_bmap_btalloc at ffffffffc02f4fc3 [xfs]
#13 [ffffbbd867db7760] xfs_bmapi_allocate at ffffffffc02f5160 [xfs]
#14 [ffffbbd867db77b8] xfs_bmapi_convert_delalloc at ffffffffc02f61a5 [xfs]
#15 [ffffbbd867db78d8] xfs_map_blocks at ffffffffc032b020 [xfs]
#16 [ffffbbd867db7980] iomap_writepage_map at ffffffff9bdf6afb
#17 [ffffbbd867db79e8] write_cache_pages at ffffffff9bca153f
#18 [ffffbbd867db7ad8] iomap_writepages at ffffffff9bdf641c
#19 [ffffbbd867db7ae8] xfs_vm_writepages at ffffffffc032aa44 [xfs]
#20 [ffffbbd867db7b78] do_writepages at ffffffff9bca3a91
#21 [ffffbbd867db7bc8] __writeback_single_inode at ffffffff9bd9c6f9
#22 [ffffbbd867db7c00] writeback_sb_inodes at ffffffff9bd9d37a
#23 [ffffbbd867db7ce8] __writeback_inodes_wb at ffffffff9bd9d6ac
#24 [ffffbbd867db7d28] wb_writeback at ffffffff9bd9d9d8
#25 [ffffbbd867db7da0] wb_check_old_data_flush at ffffffff9bd9dc36
#26 [ffffbbd867db7df8] wb_do_writeback at ffffffff9bd9dd01
#27 [ffffbbd867db7e70] wb_workfn at ffffffff9bd9ed7a
#28 [ffffbbd867db7e98] process_one_work at ffffffff9bb01c7d
#29 [ffffbbd867db7ed8] worker_thread at ffffffff9bb021d9
#30 [ffffbbd867db7f10] kthread at ffffffff9bb0741b
#31 [ffffbbd867db7f50] ret_from_fork at ffffffff9ba0358f
```
