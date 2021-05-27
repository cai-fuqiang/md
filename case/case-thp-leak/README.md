# 基本信息

## 现象描述
在客户环境上出现了qemu重启的行为，经dmesg信息查看，qemu退出是
因为在qemu在运行过程中产生了OOM

## 环境mem信息获取

因为是客户的环境，如果想装一些debug包，得经过客户同意，在装debug包
之前，通过一些现有手段获取客户环境上的信息

首先获取的是`/proc/meminfo /proc/vmstat`
### meminfo

<details>
<summary><code>meminfo</code></summary>

```
MemTotal:       265856640 kB
MemFree:        20211840 kB
MemAvailable:     331200 kB
Buffers:             320 kB
Cached:          2025664 kB
SwapCached:            0 kB
Active:         237019008 kB
Inactive:        1838464 kB
Active(anon):   236843328 kB
Inactive(anon):  1749632 kB
Active(file):     175680 kB
Inactive(file):    88832 kB
Unevictable:      643904 kB
Mlocked:          643904 kB
SwapTotal:             0 kB
SwapFree:              0 kB
Dirty:               960 kB
Writeback:             0 kB
AnonPages:       2868800 kB
Mapped:           258176 kB
Shmem:           1759232 kB
Slab:            2927936 kB
SReclaimable:     248384 kB
SUnreclaim:      2679552 kB
KernelStack:      155008 kB
PageTables:        57280 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:    132928320 kB
Committed_AS:   16480704 kB
VmallocTotal:   133009637312 kB
VmallocUsed:           0 kB
VmallocChunk:          0 kB
HardwareCorrupted:     0 kB
AnonHugePages:         0 kB
ShmemHugePages:        0 kB
ShmemPmdMapped:        0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:     524288 kB
Hugetlb:               0 kB
```
</details>


### vmstat
<details>
<summary><code>vmstat</code></summary>

```
nr_free_pages 317912
nr_zone_inactive_anon 23120
nr_zone_active_anon 3700697
nr_zone_inactive_file 1845
nr_zone_active_file 4437
nr_zone_unevictable 10060
nr_zone_write_pending 10
nr_mlock 10060
nr_page_table_pages 919
nr_kernel_stack 154496
nr_bounce 0
nr_zspages 0
nr_free_cma 0
numa_hit 122846488589
numa_miss 3586893711
numa_foreign 3586893711
numa_interleave 5943
numa_local 27269479094
numa_other 99163903206
nr_inactive_anon 23120
nr_active_anon 3700697
nr_inactive_file 1845
nr_active_file 4437
nr_unevictable 10060
nr_slab_reclaimable 3840
nr_slab_unreclaimable 42011
nr_isolated_anon 0
nr_isolated_file 0
workingset_refault 53080910
workingset_activate 13701018
workingset_nodereclaim 1213876
nr_anon_pages 44721
nr_mapped 3983
nr_file_pages 29720
nr_dirty 10
nr_writeback 0
nr_writeback_temp 0
nr_shmem 23401
nr_shmem_hugepages 0
nr_shmem_pmdmapped 0
nr_anon_transparent_hugepages 0
nr_unstable 0
nr_vmscan_write 9864
nr_vmscan_immediate_reclaim 82895
nr_dirtied 81024187
nr_written 21367344
nr_dirty_threshold 1909
nr_dirty_background_threshold 954
pgpgin 25980853652
pgpgout 20310668181
pswpin 0
pswpout 0
pgalloc_dma32 2477737776
pgalloc_normal 124670218972
pgalloc_movable 0
allocstall_dma32 0
allocstall_normal 2087
allocstall_movable 46561
pgskip_dma32 0
pgskip_normal 0
pgskip_movable 0
pgfree 127148316526
pgactivate 69943612
pgdeactivate 70168040
pglazyfree 31588
pgfault 183606093635
pgmajfault 56797880
pglazyfreed 10039
pgrefill 198217414
pgsteal_kswapd 60711367
pgsteal_direct 1950137
pgscan_kswapd 156136767
pgscan_direct 5595263
pgscan_direct_throttle 0
zone_reclaim_failed 0
pginodesteal 158715
slabs_scanned 1165054788
kswapd_inodesteal 3422166
kswapd_low_wmark_hit_quickly 47131
kswapd_high_wmark_hit_quickly 3289588
pageoutrun 3368648
pgrotated 986794
drop_pagecache 2909
drop_slab 2909
oom_kill 23
numa_pte_updates 8532602053
numa_huge_pte_updates 977002
numa_hint_faults 538415344
numa_hint_faults_local 194550892
numa_pages_migrated 65707086
pgmigrate_success 65746270
pgmigrate_fail 664091368
compact_migrate_scanned 604087
compact_free_scanned 308588
compact_isolated 80289
compact_stall 48
compact_fail 48
compact_success 0
compact_daemon_wake 0
compact_daemon_migrate_scanned 0
compact_daemon_free_scanned 0
htlb_buddy_alloc_success 0
htlb_buddy_alloc_fail 0
unevictable_pgs_culled 462951
unevictable_pgs_scanned 0
unevictable_pgs_rescued 450638
unevictable_pgs_mlocked 1159848
unevictable_pgs_munlocked 450595
unevictable_pgs_cleared 698787
unevictable_pgs_stranded 698767
thp_fault_alloc 1205
thp_fault_fallback 939
thp_collapse_alloc 79
thp_collapse_alloc_failed 5477
thp_file_alloc 0
thp_file_mapped 0
thp_split_page 0
thp_split_page_failed 617424542
thp_deferred_split_page 5171
thp_split_pmd 0
thp_zero_page_alloc 1
thp_zero_page_alloc_failed 244
thp_swpout 0
thp_swpout_fallback 0
balloon_inflate 0
balloon_deflate 0
balloon_migrate 0
swap_ra 0
swap_ra_hit 0
```
</details>


从上面的信息可以看出总的内存为270多G，而free的内存
为19G左右。还有一个信息是anon内存(尤其是Active anon)
占用特别高，这里其实就说明一个问题，这部分内存还存在
于cgroup lru链表上，另外该部分内存很可能是用户态内存。

通过一个脚本获取所有用户态进程的内存是用情况

<details>
<summary><code>get_all_pid_info.sh</code></summary>

```shell
#!/bin/sh
is_num_f() 
{
    expr $1 "+" 10 &> /dev/null
    if [ $? -eq 0  ];then
        #echo "$1 is number"
        echo 1
    else
        #echo "$1 not number"
        echo 0
    fi
}

get_meminfo()
{
    PID=$1
    mstat=`cat /proc/$PID/statm`
    comm=`cat /proc/$PID/comm`
    echo "PID($PID) comm($comm) mstat($mstat)"
    echo "BEG:get status"
    cat /proc/$PID/status
    echo "END:get status"
    echo "BEG:get maps"
    cat /proc/$PID/maps
    echo "END:get maps"
    echo "BEG:get smap"
    cat /proc/$PID/smaps
    echo "END:get smap"
    echo 
    echo
}

for PID in `ls /proc/`
do
    is_num=`is_num_f $PID`
    if [ $is_num -eq 1 ];then
        get_meminfo $PID
    fi
done
```
</details>

打印信息如下:
[get_all_pid_info](./get_all_pid_info.txt)

从上面的信息来看，进程占用的rss很小，进程虚拟内存区间也没有
占用大内存的情况，所以可以推测这部分内存是没有Map的。

### page-types

#### page-types介绍

在kernel 代码中，有个小工具源码(代码路径`tools/vm/page-types.c`)，
该源码可以编译成page-types工具，下面是该工具的介绍


1. **--help**

<details>
<summary><code>--help</code></summary>

```
[root@bogon 4.18.0-147-org]# /root/page-types --help
page-types [options]
            -r|--raw                   Raw mode, for kernel developers
            -d|--describe flags        Describe flags
            -a|--addr    addr-spec     Walk a range of pages
            -b|--bits    bits-spec     Walk pages with specified bits
            -c|--cgroup  path|@inode   Walk pages within memory cgroup
            -p|--pid     pid           Walk process address space
            -f|--file    filename      Walk file address space
            -l|--list                  Show page details in ranges
            -L|--list-each             Show page details one by one
            -C|--list-cgroup           Show cgroup inode for pages
            -N|--no-summary            Don't show summary info
            -X|--hwpoison              hwpoison pages
            -x|--unpoison              unpoison pages
            -F|--kpageflags filename   kpageflags file to parse
            -h|--help                  Show this usage message

```
</details>

解释下上面的参数:
* -C : 按照分组的形式栈是所有的pages，也是`page-types`默认的打印格式。
* -c ：打印一个cgroup 的所有pages，命令为`page-types -c ${CGROUP层级路径}`<br/>
	 eg: `page-types -c /sys/fs/cgroup/memory`
* -l : 打印每一个的页。
* -p : 打印一个进程的page，命令为`page-types -p ${PID}`

2. **打印格式**

以`page-types -C`为例:

<details>
<summary><code>page-types -C输出</code></summary>

```
            flags	page-count       MB  symbolic-flags			long-symbolic-flags
0x0000000000000000	     63375     3960  ___________________________________________	
0x0000000000100000	 105407700  6587981  ____________________n______________________	nopage
0x0000000004000000	       912       57  __________________________g________________	pgtable
0x0000000001000000	         1        0  ________________________z__________________	zero_page
0x000000000000400c	         2        0  __RU__________b____________________________	referenced,uptodate,swapbacked
0x0000000000000018	         2        0  ___UD______________________________________	uptodate,dirty
0x0000000000000020	         5        0  _____l_____________________________________	lru
0x0000000000000021	         3        0  L____l_____________________________________	locked,lru
0x0000000000000028	       574       35  ___U_l_____________________________________	uptodate,lru
0x000000000000002c	        84        5  __RU_l_____________________________________	referenced,uptodate,lru
0x0000000000000030	         2        0  ____Dl_____________________________________	dirty,lru
0x0000000000000038	         5        0  ___UDl_____________________________________	uptodate,dirty,lru
```
</details>

详细说下各个列:

* **flags** <br/>
在kernel`include/linux/mm_types.h`代码中定义的page结构体如下,

```C/C++
struct page {
	unsigned long flags;
	...
};
```

而这里的flags，**并不指的就是**`struct page.flags`，实际上是会比
page.flags多一些信息，例如是否map，而`page-types -C`实际上将page
按照flags进行分组，将相同flags的page的数量和大小输出出来，

* **其他列** <br/>

|列名|解释|
|----|----|
|page-count         |指的该flag的page有多少页|
|MB                 |指的该页总量大小|
|symbolic-flags     |将flags按照某种格式打印出来,方便查看|
|long-symbolic-flags|以文字形式打印flags|

后两个字段相关内容可以在`vm/page-types.c`文件中的
`static const char *const page_flag_name[]`数组中找到所有的
对应关系。

打印格式可以见`tools/vm/page-types.c`中的
`static const char * const page_flag_names[]`数组


#### 使用page-types客户环境上获取的信息

[page-types -C](./pages-C-node-16.txt) <br/>
[page-types -l](./pages-l-node-16.txt)

其中`page-types -C`中有两种类型的页面需要关注下

```
0x000000000040507c         447       27  __RUDlA_____a_b_______t____________________    referenced,uptodate,dirty,lru,active,anonymous,swapbacked,thp
0x0000000000401000     3661377   228836  ____________a_________t____________________    anonymous,thp
```


`__RUDIA____a_b__t__`类型的页面实际上是透明大页中的head, `_a___t_`
类型的页面是透明大页中的tail页面。
可以看出这部分内存是lru, 但是不是map的，所以这部分内存是连接到某个
cgroup中，但是没有进程映射该内存。这就很像透明大页被泄露了。

实际上在获取信息之后，我们查阅了thp解除映射的相关代码，主要是`nummap`，
这个流程在解除映射后，如果page->refcount == 0，就将该page free掉。
(在之后的章节中会介绍这部分内容)

`page-types -l` 中`__RUDIA____a_b__t__`page的打印情况
（列举一小部分）

```
10000	1	__RUDlA_____a_b_______t____________________
10001	1fff	____________a_________t____________________
12000	1	__RUDlA_____a_b_______t____________________
12001	1fff	____________a_________t____________________
14000	1	__RUDlA_____a_b_______t____________________
14001	1fff	____________a_________t____________________
```

### crash工具信息获取
在安装完debuginfo包后，可以使用crash工具对内存进行查看,
其中一个重要的命令是`kmem -p `

* **help kmem**

这里我们直看kmem -p 的help
```
...

 -p  displays basic information about each page structure in the system
     mem_map[] array, made up of the page struct address, its associated
     physical address, the page.mapping, page.index, page._count and
     page.flags fields.
...
```

大概意思就是说`kmem -p`命令可以获取struct page 的address(虚拟地址)以及对应
的物理地址, mapping, index count flags等等信息, 但是flags的信息会比
`page-types`中的flags打印的要少。所以，可以利用`page-types -l`获取到想
要的页面的页框，然后再使用`kmem -p`获取第一个`struct page`的虚拟地址，
再通过计算可以得出需要的`struct page`的地址。

* **信息获取**
```
crash> kmem -p|head
      PAGE         PHYSICAL      MAPPING       INDEX CNT FLAGS
ffff7fe000000000          0                0        0  0 0
ffff7fe000000040      10000                0        0  1 7ffff8000000000
ffff7fe000000080      20000                0        0  1 7ffff8000000000
ffff7fe0000000c0      30000                0        0  1 7ffff8000000000
ffff7fe000000100      40000                0        0  1 7ffff8000000000
ffff7fe000000140      50000                0        0  0 7ffff8000000000
ffff7fe000000180      60000                0        0  0 7ffff8000000000
ffff7fe0000001c0      70000                0        0  0 7ffff8000000000
ffff7fe000000200      80000                0        0  1 7ffff8000000800 reserved
```
可以看出`struct page`的首地址为`0xffff7fe000000000`, 一个`struct page`的大小
为`0x40`, 所以上面`page-types -`中`__RUDIA____a_b__t__`page的首地址为:
`ffff7fe000000000+0x40*10000=ffff7fe000400000`
然后就可以通过crash命令`struct page ffff7fe000400000`来获取该page的信息了。

这里我们获取了一个page，信息如下:

[一个struct page的信息](./struct_page_one.txt)
