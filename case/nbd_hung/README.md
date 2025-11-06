## 执行blkid 卡住
查看D住的blkid的进程:
```
crash> ps |grep blkid
  2104525  2104522  41  ffff9cf91d000000  UN   0.0   22732   3792  blkid
```

查看其堆栈:
```
crash> bt = 2104525
bt: invalid task or pid value:

PID: 2104525  TASK: ffff9cf91d000000  CPU: 41  COMMAND: "blkid"
 #0 [ffffb6c3a497fae0] __schedule at ffffffffa126f786
 #1 [ffffb6c3a497fb70] schedule at ffffffffa126fba6
 #2 [ffffb6c3a497fb88] io_schedule at ffffffffa126fe12
 #3 [ffffb6c3a497fb98] wait_on_page_bit_killable at ffffffffa0a96cc3
 #4 [ffffb6c3a497fc28] generic_file_buffered_read_pagenotuptodate at ffffffffa0a98ef7
 #5 [ffffb6c3a497fc70] generic_file_buffered_read_get_pages at ffffffffa0a9a0e8
 #6 [ffffb6c3a497fd28] generic_file_buffered_read at ffffffffa0a9a4e7
 #7 [ffffb6c3a497fe28] blkdev_read_iter at ffffffffa0bb0364
 #8 [ffffb6c3a497fe40] new_sync_read at ffffffffa0b623cd
 #9 [ffffb6c3a497fec8] vfs_read at ffffffffa0b6502e
#10 [ffffb6c3a497ff00] ksys_read at ffffffffa0b6561f
#11 [ffffb6c3a497ff38] do_syscall_64 at ffffffffa1262d0d
#12 [ffffb6c3a497ff50] entry_SYSCALL_64_after_hwframe at ffffffffa1400099
    RIP: 00007fda8ff00062  RSP: 00007fff1af52248  RFLAGS: 00000246
    RAX: ffffffffffffffda  RBX: 0000559c20656970  RCX: 00007fda8ff00062
    RDX: 0000000000000040  RSI: 0000559c2066d988  RDI: 0000000000000006
    RBP: 00000018ffff0000   R8: 0000000000000001   R9: 0000000000000000
    R10: 00007fda8fff5190  R11: 0000000000000246  R12: 0000000000000040
    R13: 0000559c206569c0  R14: 0000559c2066d978  R15: 0000559c2066d960
    ORIG_RAX: 0000000000000000  CS: 0033  SS: 002b
```

找到其阻塞的 块设备:

查看`new_sync_read`, `blkdev_read_iter`, 汇编:
```
0xffffffffa0b623b4 <new_sync_read+244>: callq  0xffffffffa0d1e040 <iov_iter_init>
0xffffffffa0b623b9 <new_sync_read+249>: mov    0x28(%rbx),%rax
0xffffffffa0b623bd <new_sync_read+253>: lea    0x10(%rsp),%rsi
0xffffffffa0b623c2 <new_sync_read+258>: lea    0x38(%rsp),%rdi (第一个参数)
0xffffffffa0b623c7 <new_sync_read+263>: mov    0x20(%rax),%rax
0xffffffffa0b623cb <new_sync_read+267>: callq  *%rax -->调用 blkdev_read_iter
0xffffffffa0b623cd <new_sync_read+269>: nopl   (%rax)

crash> dis blkdev_read_iter |head -100
0xffffffffa0bb0320 <blkdev_read_iter>:  nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffa0bb0325 <blkdev_read_iter+5>:        mov    (%rdi),%rax
0xffffffffa0bb0328 <blkdev_read_iter+8>:        mov    0x8(%rdi),%rdx
0xffffffffa0bb032c <blkdev_read_iter+12>:       mov    0xf0(%rax),%rax
0xffffffffa0bb0333 <blkdev_read_iter+19>:       mov    (%rax),%rax
0xffffffffa0bb0336 <blkdev_read_iter+22>:       mov    0x50(%rax),%rax
0xffffffffa0bb033a <blkdev_read_iter+26>:       cmp    %rax,%rdx
0xffffffffa0bb033d <blkdev_read_iter+29>:       jge    0xffffffffa0bb036f <blkdev_read_iter+79>
0xffffffffa0bb033f <blkdev_read_iter+31>:       push   %rbp
0xffffffffa0bb0340 <blkdev_read_iter+32>:       sub    %rdx,%rax
0xffffffffa0bb0343 <blkdev_read_iter+35>:       xor    %ebp,%ebp
0xffffffffa0bb0345 <blkdev_read_iter+37>:       push   %rbx
0xffffffffa0bb0346 <blkdev_read_iter+38>:       mov    0x10(%rsi),%rcx
```

一共压了 
```
 #7 [ffffb6c3a497fe28] blkdev_read_iter at ffffffffa0bb0364
    ffffb6c3a497fe30: ffff9cf8eccd57c0 ffffb6c3a497ff08
    ffffb6c3a497fe40: ffffffffa0b623cd
 #8 [ffffb6c3a497fe40] new_sync_read at ffffffffa0b623cd
    ffffb6c3a497fe48: 0000559c2066d988 0000000000000040
    ffffb6c3a497fe58: ffffb6c300000004 0000000000000000
    ffffb6c3a497fe68: 0000000000000040 ffffb6c3a497fe48
    ffffb6c3a497fe78: 0000000000000001 ffff9cf8eccd57c0
    ffffb6c3a497fe88: 00000018ffff0000 0000000000000000
    ffffb6c3a497fe98: 0000000000000000 0000000000000000
    ffffb6c3a497fea8: 0000000000000000 c77a84e51ae85900
    ffffb6c3a497feb8: ffff9cf8eccd57c0 0000000000000001
    ffffb6c3a497fec8: ffffffffa0b6502e
```

在`new_sync_read`执行过程中，rsp为`ffffb6c3a497fe48`, rsp+0x38为
`ffffb6c3a497fe80`， 所以 kiocb为`ffffb6c3a497fe80`

```
crash> kiocb.ki_filp ffffb6c3a497fe80
  ki_filp = 0xffff9cf8eccd57c0
```

确认:
```
crash> kmem 0xffff9cf8eccd57c0
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff9cfd80036700      320       7254     31722    622    16k  filp
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffe4ad6eb33500  ffff9cf8eccd4000     0     51         22    29
  FREE / [ALLOCATED]
  [ffff9cf8eccd57c0]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffe4ad6eb33540 3baccd5000 dead000000000004        0  0 17ffffc0000000
```
获取文件名:
```
crash> struct file.f_path ffff9cf8eccd57c0
  f_path = {
    mnt = 0xffff9cfb46ed91a0,
    dentry = 0xffff9d39276a51b8
  }
crash> struct dentry.d_name.name 0xffff9d39276a51b8
  d_name.name = 0xffff9d39276a51f0 "cbd6"
```

可以看到blkid是因为`cbd6`卡了，执行命令试试:
```
[root@11-211-65-69 11:34:58 ~]# nohup xxd -l 100 /dev/cbd6   &
[1] 2985799
[root@11-211-65-69 11:35:05 ~]# nohup: 忽略输入并把输出追加到'nohup.out'

[root@11-211-65-69 11:35:06 ~]#
[root@11-211-65-69 11:35:06 ~]# ps aux |grep xxd
root     2985799  0.0  0.0   2580   924 pts/10   D    11:35   0:00 xxd -l 100 /dev/cbd6
root     2986278  0.0  0.0  21832  1000 pts/10   S+   11:35   0:00 grep --color=auto xxd
[root@11-211-65-69 11:35:10 ~]# cat /proc/2985799/stack
[<0>] wait_on_page_bit_killable+0x113/0x280
[<0>] generic_file_buffered_read_pagenotuptodate+0x67/0x2c0
[<0>] generic_file_buffered_read_get_pages+0x2c8/0x5d0
[<0>] generic_file_buffered_read+0xf7/0x440
[<0>] blkdev_read_iter+0x44/0x60
[<0>] new_sync_read+0x10d/0x1b0
[<0>] vfs_read+0x14e/0x1b0
[<0>] ksys_read+0x5f/0xe0
[<0>] do_syscall_64+0x3d/0x80
[<0>] entry_SYSCALL_64_after_hwframe+0x61/0xc6
```

## attach nbd failed

使用下面命令attach nbd device:
```
[root@11-211-65-69 13:47:53 wangfuqiang49]# qemu-nbd -c /dev/nbd101 openEuler-24.03-LTS-SP2-x86_64.qcow2
qemu-nbd: Failed to set NBD socket
[pid 4025226] <... openat resumed>)     = 13
[pid 4025226] ioctl(13, NBD_SET_SOCK, 11) = -1 EBUSY (Device or resource busy)
[pid 4025226] newfstatat(2, "", {st_mode=S_IFIFO|0600, st_size=0, ...}, AT_EMPTY_PATH) = 0
```

而kernel dmesg中也有如下打印:
```
[root@11-211-65-69 13:49:09 wangfuqiang49]# echo ----------- > /dev/kmsg
[root@11-211-65-69 13:49:17 wangfuqiang49]# echo ----------- > /dev/kmsg
[root@11-211-65-69 13:49:17 wangfuqiang49]# echo ----------- > /dev/kmsg
[root@11-211-65-69 13:49:18 wangfuqiang49]# qemu-nbd -c /dev/nbd101 openEuler-24.03-LTS-SP2-x86_64.qcow2  > /dev/null 2>&1z

[root@11-211-65-69 13:49:27 wangfuqiang49]# dmesg |tail -100
[1704442.207544] -----------
[1704443.088276] -----------
[1704443.461220] -----------
[1704452.436427] print_req_error: 32 callbacks suppressed
[1704452.436428] blk_update_request: I/O error, dev nbd101, sector 0 op 0x0:(READ) flags 0x0 phys_seg 1 prio class 0
[1704452.437342] buffer_io_error: 31 callbacks suppressed
...
[1704452.449443]  nbd101: unable to read partition table
[1704452.480907] block nbd101: Device being setup by another task

[1704452.482656] blk_update_request: I/O error, dev nbd101, sector 0 op 0x0:(READ) flags 0x0 phys_seg 1 prio class 0
[1704452.483575] Buffer I/O error on dev nbd101, logical block 0, async page read
...
[1704452.491374] Dev nbd101: unable to read RDB block 0
[1704452.492160]  nbd101: unable to read partition table
```

报错代码:
```sh
__nbd_ioctl
=> case NBD_SET_SOCK:
   |-> nbd_add_socket()
   |-> if (!netlink && !nbd->task_setup &&
           !test_bit(NBD_RT_BOUND, &config->runtime_flags))
       |-> nbd->task_setup = current;
   |-> if (!netlink &&
            (nbd->task_setup != current ||
            test_bit(NBD_RT_BOUND, &config->runtime_flags))) {
       |-> dev_err(disk_to_dev(nbd->disk),
               "Device being setup by another task");
       |-> err = -EBUSY;
```

我们先找到该nbd设备
```
crash> dev -d |grep -E 'nbd101 |NAME '
MAJOR GENDISK            NAME       REQUEST_QUEUE      TOTAL ASYNC  SYNC
   43 ffff9cea93ade000   nbd101     ffff9d373d111ba8       0     0     0

crash> gendisk.private_data ffff9cea93ade000
  private_data = 0xffff9cf03d4bb400
crash> struct nbd_device.config 0xffff9cf03d4bb400
  config = 0xffff9d3620641d00

crash> struct nbd_config.socks,num_connections,live_connections,runtime_flags,dead_conn_timeout 0xffff9d3620641d00
  socks = 0xffff9d3584224000
  num_connections = 1311
  live_connections = {
    counter = 1310
  }
  runtime_flags = 12
  dead_conn_timeout = 0

```
可以看到runtime_Flags 是12:
```
#define NBD_RT_DISCONNECTED   2
#define NBD_RT_HAS_PID_FILE   3
```
并且`live_connections` 是 `num_connections - 1`, 表明有一个链接是disconnect的。

打印nbd_sock数组
```
crash> rd 0xffff9d3584224000 10
ffff9d3584224000:  ffff9cf680796d00 ffff9cf877849040   .my.....@..w....
ffff9d3584224010:  ffff9d3a8d305340 ffff9d3a5b83fa40   @S0.:...@..[:...
ffff9d3584224020:  ffff9cf8f0d26180 ffff9cf9e00aa680   .a..............
ffff9d3584224030:  ffff9cf67bd30d40 ffff9d3a75e0f540   @..{....@..u:...
ffff9d3584224040:  ffff9d3836b72700 ffff9cf880f21480   .'.68...........
```

**查看第一个sock**

```
crash> struct nbd_sock.sock,dead ffff9cf680796d00
  sock = 0xffff9d3a8e60ad80
  dead = true
crash> struct socket.state,type,sk 0xffff9d3a8e60ad80
  state = SS_CONNECTED
  type = 1
  sk = 0xffff9d35d31fd100

crash> struct unix_sock.addr,path,peer 0xffff9d35d31fd100
  addr = 0x0
  path = {
    mnt = 0x0,
    dentry = 0x0
  }
  peer = 0xffff9d35d31f8000
crash> struct unix_sock.addr,path,peer 0xffff9d35d31f8000
  addr = 0xffff9d37eb8b3000
  path = {
    mnt = 0x0,
    dentry = 0x0
  }
  peer = 0x0
crash> struct unix_address.name 0xffff9d37eb8b3000
  name = 0xffff9d37eb8b300c
crash> p (char *)0xffff9d37eb8b300e
$1 = 0xffff9d37eb8b300e "/export/jcloud-zbs/sock/gMgr-_vol-pw2h3n703v_i-kjmcfy5lto-_nbd101"
```

**第二个**

```
crash> struct nbd_sock.sock,dead ffff9cf877849040
  sock = 0xffff9d388aa857c0
  dead = false

crash> struct socket.state,type,sk 0xffff9d388aa857c0
  state = SS_CONNECTED
  type = 1
  sk = 0xffff9d35d3326780
crash> struct unix_sock.addr,path,peer 0xffff9d35d3326780
  addr = 0x0
  path = {
    mnt = 0x0,
    dentry = 0x0
  }
  peer = 0xffff9d35d3324800
```
我们看下对端有没有记录路径:
```
crash> struct unix_sock.addr,path,peer 0xffff9d35d3324800
  addr = 0xffff9d37bf151ea0
  path = {
    mnt = 0x0,
    dentry = 0x0
  }
  peer = 0x0

struct unix_address {
  refcnt = {
    refs = {
      counter = 1
    }
  },
  len = 68,
  hash = 256,
  name = 0xffff9d37bf151eac
}
crash> x/10cb 0xffff9d37bf151eac
0xffff9d37bf151eac:     1 '\001'        0 '\000'        47 '/'  101 'e' 120 'x' 112 'p' 111 'o' 114 'r'
0xffff9d37bf151eb4:     116 't' 47 '/'
crash>
crash> p (char *)0xffff9d37bf151eae
$1 = 0xffff9d37bf151eae "/export/jcloud-zbs/sock/gMgr-_vol-9vzkksccz1_i-lc9ky3p98b-_nbd101"
```
**第三个**
```
crash> struct nbd_sock.sock,dead ffff9d3a8d305340
  sock = 0xffff9d363c8a30c0
  dead = false
crash> struct socket.sk 0xffff9d363c8a30c0
  sk = 0xffff9d37ca3cda00
crash> struct unix_sock.peer 0xffff9d37ca3cda00
  peer = 0xffff9d37ca3cc800
crash> struct nbd_sock.sock ffff9d3a8d305340
  sock = 0xffff9d363c8a30c0
crash> struct socket.sk 0xffff9d363c8a30c0
  sk = 0xffff9d37ca3cda00
crash> struct unix_sock.peer 0xffff9d37ca3cda00
  peer = 0xffff9d37ca3cc800
crash> struct unix_sock.addr 0xffff9d37ca3cc800
  addr = 0xffff9d35d33925a0
crash> struct unix_address.name 0xffff9d35d33925a0
  name = 0xffff9d35d33925ac
crash> p (char*)0xffff9d35d33925ae
$2 = 0xffff9d35d33925ae "/export/jcloud-zbs/sock/gMgr-_vol-9vzkksccz1_i-lc9ky3p98b-_nbd101"
```

第一个sock`nbd_sock.dead`为true, 其他的sock估计都是false.

## dead nbd sock
下面路径会导致 nbd sock dead

**场景1**

```sh
sock_shutdown
=> if (test_and_set_bit(NBD_RT_DISCONNECTED, &config->runtime_flags))
   => return
=> for (i = 0; i < config->num_connections; i++)
   => nbd_mark_nsock_dead
      => if !nsock->dead
         => kernel_sock_shutdown(nsock->sock, SHUT_RDWR);
         => if (atomic_dec_return(&nbd->config->live_connections) == 0) 
            => if (test_and_clear_bit(NBD_RT_DISCONNECT_REQUESTED,
                  &nbd->config->runtime_flags))
               => set_bit(NBD_RT_DISCONNECTED,
                  &nbd->config->runtime_flags);
      => nsock->dead = true;
```

这个路径应该是, 因为`nbd_mark_nsock_dead()`是唯一可以将`nsock->dead -> true`, 并且
`sock_shutdown()`是可以无条件的设置`NBD_RT_DISCONNECTED`

但是这里有个问题, 这里会将所有sock，都disconnected, 那可能是进行了reconnect, 或
者是`nbd_add_socket()`

首先我们看reconnected:
```

=> nbd_reconnect_socket
   => for (i = 0; i < config->num_connections; i++) {
          nsock->dead = false;
      }
```
这里也会将所有 nsock dead都置为`false`, 并且将`NBD_RT_DISCONNECTED`
clear掉。但这不符合现场（现场是 只有sock[0] 是`true`，其他都是`false`
并且最终的state仍然是`NBD_RT_DISCONNECTED`

再来看下add_socket:
```sh
nbd_add_socket
=> nsock = kzalloc(sizeof(*nsock), GFP_KERNEL);
=> nsock->dead = false;
=> socks[config->num_connections++] = nsock;
=> atomic_inc(&config->live_connections);
```

可以发现和现场比较像。

**也就是说, 在shutdown sock后，又新链接了一些新的sock**

## 通过 live socket 找 socket owner ??

以第三个链接为例:
```
crash> struct socket.file 0xffff9d363c8a30c0
  file = 0xffff9d3866102580
```

查看该file被谁占用:
```
crash> foreach files |grep ffff9d3866102580
crash>
```

没有找到??

search结果:
```
ffff9d363c8a30d0: ffff9d3866102580
ffff9d37eb032b30: ffff9d3866102580
ffff9d3a74fc0a28: ffff9d3866102580
ffff9d3a83850b70: ffff9d3866102580
ffff9d3a8cb063b0: ffff9d3866102580

ffff9d3adb21d6f0: ffff9d3866102580

ffff9d3b4c78acc8: ffff9d3866102580
ffff9d3b4c78ad28: ffff9d3866102580
ffff9d3b4c78adc0: ffff9d3866102580
ffff9d3b4c78ae90: ffff9d3866102580
ffff9d3b55c0e9e8: ffff9d3866102580
```
仅这一个永久保存:
```
crash> kmem ffff9d363c8a30d0
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ffff9cfc7f949800      768       2217     18837    483    32k  sock_inode_cache
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  ffffe4ae63f22800  ffff9d363c8a0000     1     39          5    34
  FREE / [ALLOCATED]
  [ffff9d363c8a30c0]

      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffffe4ae63f228c0 78fc8a3000 dead000000000400        0  0 57ffffc0000000
```

FUCK! 这个是`socket`数据结构

## 查看正常的nbd设备
```
crash> dev -d |grep 'nbd100 '
   43 ffff9cf4ba288800   nbd100     ffff9d373d111270       1     0     1
crash> struct gendisk.private_data ffff9cf4ba288800
  private_data = 0xffff9cf5b9edc400
crash> struct nbd_device.config 0xffff9cf5b9edc400
  config = 0xffff9cea9f2de580

crash> struct nbd_config.socks,num_connections 0xffff9cea9f2de580
  socks = 0xffff9cf9533cdf10
  num_connections = 1
crash> rd 0xffff9cf9533cdf10 1
ffff9cf9533cdf10:  ffff9cea9914a080                    ........
crash> struct nbd_sock.sock ffff9cea9914a080
  sock = 0xffff9d37c6fca080
crash> struct socket.file 0xffff9d37c6fca080
  file = 0xffff9d3872ffbac0

crash> foreach files |grep -B 10 ffff9d3872ffbac0
...
PID: 118852  TASK: ffff9d35edde0000  CPU: 54  COMMAND: "nbd-client"
ROOT: /    CWD: /
 FD       FILE            DENTRY           INODE       TYPE PATH
  0 ffff9d3872ff88c0 ffff9cbe404670e0 ffff9cf6d6dad5a8 CHR  /dev/null
  1 ffff9d3872ff88c0 ffff9cbe404670e0 ffff9cf6d6dad5a8 CHR  /dev/null
  2 ffff9d3872ff88c0 ffff9cbe404670e0 ffff9cf6d6dad5a8 CHR  /dev/null
  3 ffff9d3872ffbac0 ffff9d39fec2d290 ffff9d37c6fca100 SOCK UNIX
```
可以看到，正常的为 num_connections 为1, 并且file 指针可以在进程中找到！

**疑似file被占用???但又不是进程占用**

<!--
## WHO USE the sock

我们来看下是谁尝试占用了这个sock， 查看环境发现有个KVM进程D住:
```
[root@11-211-65-69 15:08:03 381114]# ps aux -T |grep KVM
root      236131  236131  0.0  0.0  21832  1024 pts/8    S+   15:08   0:00 grep --color=auto KVM
root      381114  381160  0.4  0.0      0     0 ?        Dl   10月29  48:47 [CPU 5/KVM]
[root@11-211-65-69 15:08:06 381114]# ps aux -T |grep qemu |grep Z
root      381114  381114  0.0  0.0      0     0 ?        Zl   10月29   7:24 [qemu-system-x86] <defunct>
```

卡在了aio:
```
crash> bt 381160
PID: 381160  TASK: ffff9cfae9824180  CPU: 15  COMMAND: "CPU 5/KVM"
 #0 [ffffb6c3a0a87b68] __schedule at ffffffffa126f786
 #1 [ffffb6c3a0a87bf8] schedule at ffffffffa126fba6
 #2 [ffffb6c3a0a87c10] schedule_timeout at ffffffffa1272d90
 #3 [ffffb6c3a0a87cb0] wait_for_common at ffffffffa127054f
 #4 [ffffb6c3a0a87d10] exit_aio at ffffffffa0bcba2d
 #5 [ffffb6c3a0a87d68] mmput at ffffffffa08df238
 #6 [ffffb6c3a0a87d80] exit_mm at ffffffffa08e7d93
 #7 [ffffb6c3a0a87dc0] do_exit at ffffffffa08e7ffc
 #8 [ffffb6c3a0a87df0] do_group_exit at ffffffffa08e8293
 #9 [ffffb6c3a0a87e18] get_signal at ffffffffa08f5e08
#10 [ffffb6c3a0a87e68] arch_do_signal at ffffffffa0822c1a
#11 [ffffb6c3a0a87ef0] exit_to_user_mode_loop at ffffffffa097b139
#12 [ffffb6c3a0a87f10] exit_to_user_mode_prepare at ffffffffa097b1ef
#13 [ffffb6c3a0a87f28] syscall_exit_to_user_mode at ffffffffa1266aa2
#14 [ffffb6c3a0a87f38] do_syscall_64 at ffffffffa1262d1d
#15 [ffffb6c3a0a87f50] entry_SYSCALL_64_after_hwframe at ffffffffa1400099
    RIP: 00007f62d1be4bbb  RSP: 00007f62ca7fb4e8  RFLAGS: 00000246
    RAX: fffffffffffffffc  RBX: 000000000000ae80  RCX: 00007f62d1be4bbb
    RDX: 0000000000000000  RSI: 000000000000ae80  RDI: 000000000000002d
    RBP: 000055b2593b2320   R8: 000055b2578d6738   R9: 00000000000000ff
    R10: 0000000000000001  R11: 0000000000000246  R12: 0000000000000000
    R13: 000055b257d555e0  R14: 00007f62d00a9001  R15: 0000000000000000
    ORIG_RAX: 0000000000000010  CS: 0033  SS: 002b
```
-->
