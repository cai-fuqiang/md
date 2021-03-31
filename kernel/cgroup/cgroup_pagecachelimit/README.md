# page cache limit

## 背景与现状

### 背景

* page cache是linux内核的一种文件缓存机制，它主要是用来缓存一些磁盘文件。
  通过把磁盘文件缓存到内存中，减少系统对磁盘IO的操作，从而提高系统的读写
  性能。但是，过多的文件缓存，会占用大量的系统内存，可能会导致系统剩余内
  存不足，反而会影响业务的性能或使用。因此，在某些场景下，我们需要限制page 
  cache的使用率，尽量使系统剩余的内存能够满足业务的需求。

* 现有的linux内核中已有回收page cache的机制：进程分配内存时，如果系统的剩
  余内存低于一定值(与/proc/sys/vm/min_free_kbytes相关)时，就会进入slow 
  path，唤醒kswapd进程回收一些page cache，如果kswapd没有回收成功或者回收
  较慢，则进入到direct_reclaim进行深度的回收，直到系统剩余内存满足要求，如
  果还是不行，那么只能OOM了。这样的机制是否足够了呢？

* 在一些原子操作的场景，比如中断上下文中，是不能睡眠的，因此在这种场景下的
  内存分配时，是不会进入到direct_reclaim环节，甚至也不会去唤醒kswapd进程的。
  比如在网卡收包的软中断处理函数中，可能存在因为page cache占用过多，系统剩
  余内存不足无法为收到的数据包分配内存，而直接丢包的现象。这也就是page 
  cache limit要解决的问题。

  NOTE: 上面引用自https://github.com/Tencent/TencentOS-kernel#page-cache-limit%E7%89%B9%E6%80%A7

### 现状


Memory cgroup stats for /kubepods/burstable/pod0bcd15d8-e967-4263-adec-b7440f94527c/
70f66974fa4319ae6e34ca122a998b620c61064eb58ad67edacdad13a0d35d4a: cache:1663744KB 
rss:436736KB rss_huge:0KB shmem:0KB mapped_file:0KB dirty:1695936KB writeback:6336KB 
swap:0KB inactive_anon:0KB active_anon:423936KB inactive_file:447296KB active_file:1225728KB 
unevictable:0KB

### 相关链接
https://lkml.org/lkml/2014/6/16/142
