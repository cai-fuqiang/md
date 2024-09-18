# 内核版本
```
[root@node-1 wangfuqiang]# uname -r
6.6.0-28.0.0.34.oe2403.aarch64
6.6.0-28.0.0.34.oe2403.aarch64
```

# 测试步骤
## 准备工作

1. 创建swapfile
```
[root@node-1 swap]# pwd
/swap
[root@node-1 swap]# dd if=/dev/zero of=swapfile_wfq bs=1M count=1000
1000+0 records in
1000+0 records out
1048576000 bytes (1.0 GB, 1000 MiB) copied, 0.226919 s, 4.6 GB/s
[root@node-1 swap]# mkswap swapfile_wfq
mkswap: swapfile_wfq: insecure permissions 0644, fix with: chmod 0600 swapfile_wfq
Setting up swapspace version 1, size = 1000 MiB (1048571904 bytes)
no label, UUID=da5adaa7-2aae-4909-8f81-cf1735c497f9
```

2. swapon
```
[root@node-1 swap]# swapon ./swapfile_wfq
swapon: /swap/swapfile_wfq: insecure permissions 0644, 0600 suggested.

[root@node-1 swap]# free -m
               total        used        free      shared  buff/cache   available
Mem:          256924       16288      206775          31       35246      240635
Swap:            999           0         999        # 有不到1G的swap空间
```

3. 开启`mm_vmscan_lru_shrink_inactive` tracepoint
```
[root@node-1 tracing]# echo mm_vmscan_lru_shrink_inactive > set_event
[root@node-1 tracing]# cat set_event
vmscan:mm_vmscan_lru_shrink_inactive
```
4. 创建test cgroup
```
[root@node-1 memory]# mkdir test
[root@node-1 memory]# cd test/
```
5. 设置`test` cgroup 内存限制为100M
```
[root@node-1 test]# echo $((100*1024*1024)) > memory.limit_in_bytes
```

6. 设置当前shell进程在 test cgroup
```
[root@node-1 test]# echo $$ > tasks
```

## swappness 设置为100进行测试(既回收匿名页又回收pagecache)

1. 设置swappness为100
```
[root@node-1 test]# echo 100 > memory.swappiness
```

2. 执行dd程序, 分配匿名页和pagecache
```
[root@node-1 tmp]# dd if=/dev/zero of=a.img bs=50M count=20
20+0 records in
20+0 records out
1048576000 bytes (1.0 GB, 1000 MiB) copied, 5.6004 s, 187 MB/s
```

3. 查看trace, 看看其是否能回收匿名页
```
[root@node-1 tmp]# cat /sys/kernel/debug/tracing/trace |grep ANON |wc -l
4848
[root@node-1 tmp]# cat /sys/kernel/debug/tracing/trace |grep FILE |wc -l
1841
[root@node-1 tmp]# cat /sys/kernel/debug/tracing/trace |grep ANON|head -1
              dd-157706  [027] ..... 304085.178485: mm_vmscan_lru_shrink_inactive: nid=1 nr_scanned=32 nr_reclaimed=9 
              nr_dirty=0 nr_writeback=0 nr_congested=0 nr_immediate=0 nr_activate_anon=0 nr_activate_file=0 
              nr_ref_keep=23 nr_unmap_fail=0 priority=5 flags=RECLAIM_WB_ANON|RECLAIM_WB_ASYNC
```

可以发现可以回收匿名页

## swappness 设置为0进行测试(不回收匿名页)
1. 设置`swappness`为0
```
[root@node-1 tmp]# echo 0 > /sys/fs/cgroup/memory/test/memory.swappiness
```
2. 执行dd程序, 分配匿名页和pagecache
```
[root@node-1 tmp]#  dd if=/dev/zero of=a.img bs=50M count=20
20+0 records in
20+0 records out
1048576000 bytes (1.0 GB, 1000 MiB) copied, 4.85546 s, 216 MB/s
```
3. 查看trace, 看看是否能回收匿名页
```
[root@node-1 tmp]# cat /sys/kernel/debug/tracing/trace |grep ANON
[root@node-1 tmp]# cat /sys/kernel/debug/tracing/trace |grep FILE |wc -l
14558
```
可以发现其不能回收匿名页.

# 结论
可以通过设置swappness来控制, 每个cgroup的内存回收流程是否扫描匿名页.
