# 解决方法

目前打算在客户环境上安装一个rpm包, 启动一个服务解决

## system service

目前使用systemd timer 实现. 没20秒启动一次服务:
```
[Unit]
Description=Monitor Memcg Timer

[Timer]
OnUnitActiveSec=20s
OnBootSec=5min

[Install]
WantedBy=timers.target
```

该服务就是拉一个脚本,并设置脚本超时时间为1min
```
[Unit]
Description=Monitor Memcg Service

[Service]
Type=oneshot
ExecStart=/usr/sbin/mon_memcg
TimeoutStartSec=1min
```

## 脚本逻辑
目前脚本逻辑主要是
1. 判断avail memory (total memory - inactive file - 低水位) 小于 4G(可配置)
   触发回收
2. 回收时, 采用的方法为`echo 1 > /sys/fs/cgroup/memory/user.slice/memory.force_empty`
3. 为了避免频繁的回收, 有增加以下逻辑:

   记录一个last mem(之前的 user.slice total_rss + total_cache) 的值, 如果本次的值比
   上一次大了不到1G(可配置), 本次就不用触发回收流程
4. 什么时候记录last mem呢? 有两种情况
   * 如果上一次触发了回收, 记录回收之后的内存值.
   * 在没有触发回收的情况下,如果当前的内存值 小于last mem, 则记录该值.

## 测试用例
我们测试如下场景:

### 持续触发内存回收

这个测试很简单, 我们预留200G左右空间做测试, 启动上面我们提到的测试程序, 由于
小程序复现特别快,所以20s一次其实不太够.不过足够覆盖客户环境场景
```
~/.../alloc_mem_cpu# cat /var/log/mon_memcg.log
Sat Sep 14 02:48:45 CST 2024     avalible_mem -300967919616 is less global thresh 4362076160, need reclaim /sys/fs/cgroup/memory//user.slice !!
Sat Sep 14 02:48:45 CST 2024     Before reclaim:
Sat Sep 14 02:48:45 CST 2024     mem usage: CURRENT 526179893248 LAST 915800064 CHANGE 525264093184
Sat Sep 14 02:48:45 CST 2024     After reclaim:
Sat Sep 14 02:48:45 CST 2024     mem usage total_rss+total_cache 2005598208


Sat Sep 14 02:50:05 CST 2024     avalible_mem -438700474368 is less global thresh 4362076160, need reclaim /sys/fs/cgroup/memory//user.slice !!
Sat Sep 14 02:50:05 CST 2024     Before reclaim:
Sat Sep 14 02:50:05 CST 2024     mem usage: CURRENT 663986438144 LAST 2005598208 CHANGE 661980839936
Sat Sep 14 02:50:06 CST 2024     After reclaim:
Sat Sep 14 02:50:06 CST 2024     mem usage total_rss+total_cache 1805385728

Sat Sep 14 02:47:45 CST 2024     338
Sat Sep 14 02:47:50 CST 2024     375
Sat Sep 14 02:47:55 CST 2024     413
Sat Sep 14 02:48:00 CST 2024     451
Sat Sep 14 02:48:05 CST 2024     490
Sat Sep 14 02:48:10 CST 2024     529
Sat Sep 14 02:48:15 CST 2024     567
Sat Sep 14 02:48:20 CST 2024     605
Sat Sep 14 02:48:25 CST 2024     642
Sat Sep 14 02:48:30 CST 2024     681
Sat Sep 14 02:48:35 CST 2024     720
Sat Sep 14 02:48:40 CST 2024     757
Sat Sep 14 02:48:45 CST 2024     303
Sat Sep 14 02:48:50 CST 2024     340
Sat Sep 14 02:48:55 CST 2024     379
Sat Sep 14 02:49:00 CST 2024     418
Sat Sep 14 02:49:05 CST 2024     458
Sat Sep 14 02:49:10 CST 2024     496
Sat Sep 14 02:49:15 CST 2024     535
Sat Sep 14 02:49:20 CST 2024     574
Sat Sep 14 02:49:25 CST 2024     614
Sat Sep 14 02:49:30 CST 2024     653
Sat Sep 14 02:49:35 CST 2024     691
Sat Sep 14 02:49:40 CST 2024     730
Sat Sep 14 02:49:45 CST 2024     769
Sat Sep 14 02:49:50 CST 2024     807
Sat Sep 14 02:49:55 CST 2024     846
Sat Sep 14 02:50:00 CST 2024     885
Sat Sep 14 02:50:05 CST 2024     302
```
可以看到, 在每次回收后, 都能回收大量内存

### 模拟实际占用大量内存场景
我们占用内存到3G
```
~/.../alloc_mem_cpu# free -g
              total        used        free      shared  buff/cache   available
Mem:            510         506           3           0           0           2
Swap:             0           0           0
```

但是经测试, 基本遇不到这种情况, 通过k8s提供的方法检测到的avaible的内存剩余量总是>4G,
用户态这边再继续申请内存, 会OOM, 所以,我们调整阈值-> 10G 来测试
```
## 这个地方我们释放了pagecache, 所以 last usage memory , 有很大的内存变化, 所以这个也得到了验证

Sat Sep 14 03:02:05 CST 2024     mem usage decline, change last usage mem 2689925120 -> 932184064


## 由于占用内存变大, 所以这里会触发一次回收

Sat Sep 14 03:20:55 CST 2024     avalible_mem 8240496640 is less global thresh 10804527104, need reclaim /sys/fs/cgroup/memory//user.slice !!
Sat Sep 14 03:20:55 CST 2024     Before reclaim:
Sat Sep 14 03:20:55 CST 2024     mem usage: CURRENT 216618106880 LAST 932184064 CHANGE 215685922816
Sat Sep 14 03:20:55 CST 2024     After reclaim:
Sat Sep 14 03:20:55 CST 2024     mem usage total_rss+total_cache 216604540928

## 可以看到, 如果两次之间的差值比较小(<1G), 不再会触发reclaim.

Sat Sep 14 03:22:05 CST 2024     avalible_mem 8249278464 is less global thresh 10804527104, need reclaim /sys/fs/cgroup/memory//user.slice !!
Sat Sep 14 03:22:05 CST 2024     Before reclaim:
Sat Sep 14 03:22:05 CST 2024     mem usage: CURRENT 216604540928 LAST 216604540928 CHANGE 0
Sat Sep 14 03:22:05 CST 2024     change is too small (less than 1073741824 ), so skip reclaim

## 停止内存分配程序, 这里也打印了一条,但是是因为usage mem 见效, 虽然变化不大

Sat Sep 14 03:23:05 CST 2024     mem usage decline, change last usage mem 216604540928 -> 205825900544
```

## 超时测试
我们为脚本设置了1min超时. 我们来测试下
```
~/.../alloc_pagecache# cat cat.sh
set -e
for FILE in `find /lib/modules -name "*" -type f`
do
        _FILE=$(echo $FILE |sed 's/\//_/g')
        cp $FILE  /var/lib/containerd/kernel/$_FILE
done

~/.../alloc_pagecache# cat loop_cat.sh
NUM=0

while [ 1 ]
do
        if [ $NUM -lt 64 ];then
                nohup sh cat.sh &
        fi
        NUM=`ps aux |grep cat.sh |grep -v grep  |wc -l`
done
```
我们在shell中执行上面脚本, 该脚本会位于`user.slice`下的某个session, 
所以执行到force_empty 时, 会触发对这些pagecache的内存回收.另外, 
为了让这些cgroup活跃, 这里并行64 个进程, 同时产生pagecache.

并且我们执行`mem.sh`, 来完成rss的错误统计.

这种情况下, 可能会遇到一些极端情况, 导致dying的cgroup无法回收, 我们
只展示下这种情况:
```
Sat Sep 14 05:14:55 CST 2024     avalible_mem -941129007104 is less global thresh 4362076160, need reclaim /sys/fs/cgroup/memory//user.slice !!
Sat Sep 14 05:14:55 CST 2024     Before reclaim:
Sat Sep 14 05:14:55 CST 2024     mem usage: CURRENT 1169024221184 LAST 1202323456 CHANGE 1167821897728
Sat Sep 14 05:15:55 CST 2024     avalible_mem -816127213568 is less global thresh 4362076160, need reclaim /sys/fs/cgroup/memory//user.slice !!
Sat Sep 14 05:15:55 CST 2024     Before reclaim:
Sat Sep 14 05:15:55 CST 2024     mem usage: CURRENT 1048440340480 LAST 1202323456 CHANGE 1047238017024
Sat Sep 14 05:16:55 CST 2024     avalible_mem -988091842560 is less global thresh 4362076160, need reclaim /sys/fs/cgroup/memory//user.slice !!
Sat Sep 14 05:16:55 CST 2024     Before reclaim:
Sat Sep 14 05:16:55 CST 2024     mem usage: CURRENT 1218668658688 LAST 1202323456 CHANGE 1217466335232

Sat Sep 14 05:14:55 CST 2024     1389
Sat Sep 14 05:15:00 CST 2024     1403
Sat Sep 14 05:15:05 CST 2024     1418
Sat Sep 14 05:15:10 CST 2024     1350
Sat Sep 14 05:15:15 CST 2024     1283
Sat Sep 14 05:15:20 CST 2024     1214
Sat Sep 14 05:15:25 CST 2024     1199
Sat Sep 14 05:15:30 CST 2024     1209
Sat Sep 14 05:15:35 CST 2024     1224
Sat Sep 14 05:15:40 CST 2024     1237
Sat Sep 14 05:15:45 CST 2024     1250
Sat Sep 14 05:15:50 CST 2024     1264
Sat Sep 14 05:15:55 CST 2024     1277
Sat Sep 14 05:16:00 CST 2024     1291
Sat Sep 14 05:16:05 CST 2024     1305
Sat Sep 14 05:16:10 CST 2024     1320
Sat Sep 14 05:16:15 CST 2024     1334
Sat Sep 14 05:16:20 CST 2024     1349
Sat Sep 14 05:16:25 CST 2024     1362
Sat Sep 14 05:16:30 CST 2024     1375
Sat Sep 14 05:16:35 CST 2024     1390
Sat Sep 14 05:16:40 CST 2024     1401
Sat Sep 14 05:16:45 CST 2024     1411
Sat Sep 14 05:16:50 CST 2024     1422
Sat Sep 14 05:16:55 CST 2024     1435
```

可以看到, 整个过程内存都是一直增长的(单位是G), 没有回收内存. 
所以这种情况算脚本失效了, 但是超时机制正常执行.

这种情况是因为, `force_empty`流程总能从我们ssh 所在的session 的cgroup
中回收到足量内存, 导致dying 的cgroup 一直没有机会回收.

和于宸, 倪振讨论这样的极端场景, 发现这种情况在客户环境下很难遇到,
并且也没有特别完美的解决方法, 目前先略过这种情况
