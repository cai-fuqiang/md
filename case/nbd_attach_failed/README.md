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

## 自测

搭建nbd环境
```
[root@A06-R08-I134-73-919XB72 openEuler-2403]# nbd-server 10809 $(pwd)/disk.img

** (process:190053): WARNING **: 15:38:28.751: Specifying an export on the command line no longer uses the oldstyle protocol.
[root@A06-R08-I134-73-919XB72 openEuler-2403]#
[root@A06-R08-I134-73-919XB72 openEuler-2403]# nbd-client localhost 10809 /dev/nbd4
Warning: the oldstyle protocol is no longer supported.
This method now uses the newstyle protocol with a default export
Negotiation: ** Message: 15:38:32.488: Spawned a child process
..** Message: 15:38:32.489: virtstyle ipliteral
** Message: 15:38:32.489: connect from 127.0.0.1, assigned file is /export/wfq/vm/openEuler-2403/disk.img
** Message: 15:38:32.489: Can't open authorization file /usr/local/etc/nbd-server/allow (No such file or directory).
** Message: 15:38:32.489: Size of exported file/device is 10737418240
size = 10240MB
** Message: 15:38:32.489: Starting to serve
Connected /dev/nbd4
```
启动虚拟机使用`nbd4`:
```
-drive file=/dev/nbd4,if=virtio,format=raw,cache=none \
```

使用crash调试
```
crash> dev -d |grep nbd4
   43 ffff9e658bc74400   nbd4       ffff9e6591772250       0     0     0
crash> struct gendisk.private_data ffff9e658bc74400
  private_data = 0xffff9e658b10c600,
crash> struct nbd_config.socks,num_connections,live_connections,runtime_flags,dead_conn_timeout 0xffff9e65a66cf000
  socks = 0xffff9e65841af600,
  num_connections = 1,
  live_connections = {
    counter = 1
  },
  runtime_flags = 184,
  dead_conn_timeout = 0,
```

看下该socket被谁在使用:
```
crash> struct socket.file 0xffff9e7505768d00
  file = 0xffff9e65873898c0,
crash> foreach files |grep  ffff9e65873898d0
crash>
```
可以发现也找不到。并没有人打开这个sock. 而nbd-client
进程已经退出
```
crash> ps |grep nbd-client
crash>
```

我们此时让`nbd-server` stop。并在guest中发起io
```
 190112 root      20   0 6602360 850428  19000 S   0.0   0.6   0:01.21 CPU 6/KVM
 190113 root      20   0 6602360 850428  19000 S   0.0   0.6   0:00.83 CPU 7/KVM
 190128 root      20   0 6602360 850428  19000 S   0.0   0.6   0:00.00 kvm-nx-lpage-re
 190766 root      20   0 6602360 850428  19000 D   0.0   0.6   0:00.00 worker
```
可以发现worker D住了。

因为当时现场qemu变为僵尸进程。所以我们此时`kill -9 + qemu pid`, 也让其变僵尸
```
root      190093  190093  0.0  0.0      0     0 pts/2    Zl+  15:41   0:00 [qemu-system-x86] <defunct>
root      190093  190766  0.0  0.6 6602360 850428 pts/2  Dl+  15:54   0:00 /usr/bin/qemu-system-x86_64
```

此时qemu主进程变为僵尸进程，而D住的线程仍然保持D状态.
```
[<0>] submit_bio_wait+0x77/0xc0
[<0>] __blkdev_direct_IO_simple+0x107/0x240
[<0>] blkdev_write_iter+0x20e/0x330
[<0>] do_iter_readv_writev+0x14a/0x240
[<0>] vfs_writev+0x11c/0x2a0
[<0>] do_pwritev+0x8a/0xd0
[<0>] do_syscall_64+0x5d/0x250
[<0>] entry_SYSCALL_64_after_hwframe+0x76/0x7e
```

我们看看下此时sock状态:
```
crash> struct nbd_config.socks,num_connections,live_connections,runtime_flags,dead_conn_timeout 0xffff9e65a66cf000
  socks = 0xffff9e65841af600,
  num_connections = 1,
  live_connections = {
    counter = 1
  },
  runtime_flags = 184,
  dead_conn_timeout = 0,
```
发现并没有变化。

如果我们执行 :
```
[root@A06-R08-I134-73-919XB72 rocky-9.0]# nbd-client -d /dev/nbd4
```

则qemu d状态就会消失。

我们换另外一种方式测试:

将nbd-server杀死:

查看qemu状态:
```
 191017 root      20   0 6500812   1.5g  19020 S   0.0   1.2   0:23.79 CPU 7/KVM
 191023 root      20   0 6500812   1.5g  19020 S   0.0   1.2   0:00.00 kvm-nx-lpage-re
 191155 root      20   0 6500812   1.5g  19020 D   0.0   1.2   0:00.67 worker
```

可以发现qemu D住, 虚拟机中有IO报错.
```
[  288.848329][    C0] I/O error, dev vda, sector 1025968 op 0x1:(WRITE) flags 0x104000 phys_seg 254 prio class 2
[  288.850547][    C0] I/O error, dev vda, sector 1028000 op 0x1:(WRITE) flags 0x104000 phys_seg 254 prio class 2
[  288.852449][    C0] I/O error, dev vda, sector 1030032 op 0x1:(WRITE) flags 0x104000 phys_seg 254 prio class 2
[  288.854703][    C0] I/O error, dev vda, sector 1032064 op 0x1:(WRITE) flags 0x104000 phys_seg 254 prio class 2
[  288.856621][    C0] I/O error, dev vda, sector 1034096 op 0x1:(WRITE) flags 0x104000 phys_seg 254 prio class 2
```
直接读取nbd设备
```
[root@A06-R08-I134-73-919XB72 rocky-9.0]# xxd -l 64 /dev/nbd4
xxd: Input/output error

[1293267.696676] Dev nbd4: unable to read RDB block 0
[1293267.696969] I/O error, dev nbd4, sector 0 op 0x0:(READ) flags 0x0 phys_seg 1 prio class 2
[1293267.697272] Buffer I/O error on dev nbd4, logical block 0, async page read
[1293267.697565] I/O error, dev nbd4, sector 0 op 0x0:(READ) flags 0x0 phys_seg 1 prio class 2
[1293267.697877] Buffer I/O error on dev nbd4, logical block 0, async page read
[1293267.698178]  nbd4: unable to read partition table
[1293267.699750] I/O error, dev nbd4, sector 0 op 0x0:(READ) flags 0x0 phys_seg 1 prio class 2
[1293267.705056] Buffer I/O error on dev nbd4, logical block 0, async page read
```
dmesg中报错和之前的异常现场很像！

我们将qemu杀死，和之前一样。
```
 190999 root      20   0       0      0      0 Z   0.0   0.0   0:09.65 qemu-system-x86
 191155 root      20   0 6500812   1.5g  19020 D   0.0   1.2   0:00.67 worker
```

此时我们查看sock状态:
```
crash> struct nbd_config.socks,num_connections,live_connections,runtime_flags,dead_conn_timeout 0xffff9e65a66cf000
  socks = 0xffff9e65841af600,
  num_connections = 1,
  live_connections = {
    counter = 0
  },
  runtime_flags = 4,
  dead_conn_timeout = 0,
```

可以发现和异常现场很像，就是`num_connections` 没有那么大

如果我们此时在使用nbd-client attach则会有如下报错:
```
[1293418.959247] nbd: nbd4 already in use
[1293440.582097] block nbd4: Possible stuck request 000000006372588b: control (read@843272192,4096B). Runtime 330 seconds
```
和现场很像。

## 为什么会有这样的现象

因为sock会在`nbd_release`时被清理:
```cpp
static void nbd_release(struct gendisk *disk)
{
        struct nbd_device *nbd = disk->private_data;

        if (test_bit(NBD_RT_DISCONNECT_ON_CLOSE, &nbd->config->runtime_flags) &&
                        disk_openers(disk) == 0)
                //here
                nbd_disconnect_and_put(nbd);

        nbd_config_put(nbd);
        nbd_put(nbd);
}
```

而`nbd_release()`时该nbd块设备的`release`回调，其会在fput()流程中执行:
```cpp
static const struct block_device_operations nbd_fops =
{
        .owner =        THIS_MODULE,
        .open =         nbd_open,
        .release =      nbd_release,
        .ioctl =        nbd_ioctl,
        .compat_ioctl = nbd_ioctl,
        .free_disk =    nbd_free_disk,
};
```

如果qemu一直D住，就不会执行到该release函数释放资源，所以再次attach时，nbd
会报错被占用.
## 结论
疑似nbd-server被清理导致.

## 参考链接
1. [The Network Block Device](https://www.linuxjournal.com/article/3778)
