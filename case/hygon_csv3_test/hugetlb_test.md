## CSV预留
使用kernel cmdline
```
csv_mem_percentage=50
```
预留50%内存分配csv虚拟机.

kernel 启动后, 观察dmesg
```
[root@11-241-212-108 10:44:34 vm]# dmesg |grep -i 'CSV-cma'
[    0.011283] CSV-CMA: Node 0 - reserve size 0x0000001000000000, (expected size 0x0000000ff9024000)
[    0.011400] CSV-CMA: Node 1 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
[    0.011515] CSV-CMA: Node 2 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
[    0.011631] CSV-CMA: Node 3 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
[    0.011750] CSV-CMA: Node 4 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
[    0.011873] CSV-CMA: Node 5 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
[    0.011995] CSV-CMA: Node 6 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
[    0.012117] CSV-CMA: Node 7 - reserve size 0x0000001000000000, (expected size 0x0000000fffec0000)
[    0.012118] CSV-CMA: SMR entry size is 0x800000
```
每个numa上预留64G内存


## 大页预留

修改kernel cmdline，为每个numa 预留 64G大页
```
hugepagesz=2M hugepages=262144
```

重启系统，kernel启动后, 执行下面命令查看内存情况:
* free -m
  ```
  [root@11-241-212-108 10:49:21 vm]# free -m
                 total        used        free      shared  buff/cache   available
  Mem:         1029458      531907      500076          19         548      497550
  Swap:              0           0           0
  ```
* cat /proc/meminfo
  ```
  [root@11-241-212-108 10:49:59 vm]# cat /proc/meminfo |grep -i huge
  AnonHugePages:     38912 kB
  ShmemHugePages:        0 kB
  FileHugePages:     45056 kB
  HugePages_Total:   262144
  HugePages_Free:    262144
  HugePages_Rsvd:        0
  HugePages_Surp:        0
  Hugepagesize:       2048 kB
  Hugetlb:        536870912 kB
  ```
* cat /proc/zoneinfo
  > NOTE
  >
  > 我们打算只在node-1分配内存，所以这里，只查看node-1的Normal zone
  ```
  Node 1, zone   Normal
    per-node stats
    pages free     16127918
          boost    0
          min      7058
          low      40085
          high     73112
          spanned  33553792
          present  33553792
          managed  33027391
          cma      16777216
          protection: (0, 0, 0, 0, 0)
        nr_free_pages 16127918
        nr_zone_inactive_anon 192
        nr_zone_active_anon 7
        nr_zone_inactive_file 7042
        nr_zone_active_file 992
        nr_zone_unevictable 0
        nr_zone_write_pending 0
        nr_mlock     0
        nr_bounce    0
        nr_zspages   0
        nr_free_cma  8095497
  ```
  <font color=red><strong>
  可以发现只消耗了大约一半的cma内存给大页
  </strong></font>

  cma 消耗多少呢:
  ```
  # echo $(((16777216-8095497)*4096/1024/1024/1024))
  ```
  33

  33G左右，我们在node-1上创建34G的虚拟机试试:

### 测试csv3机型
#### 创建34G内存虚拟机

xml:
```
[root@11-241-212-108 11:20:31 vm]# cat wang_sev_huge_34.xml |grep -i -E 'mem|policy'
  <memory unit='KiB'>35651584</memory>
  <currentMemory unit='KiB'>35651584</currentMemory>
  <memoryBacking>
  </memoryBacking>
    <memory mode='strict' nodeset='1'/>
    <memnode cellid='0' mode='strict' nodeset='1'/>
    <feature policy='require' name='topoext'/>
      <cell id='0' cpus='0-31' memory='35651584' unit='KiB'/>
    <policy>0x45</policy>
```
发现可以分配成功, 我们继续分配其他规格虚拟机

#### 创建其他规格虚拟机测试
|规格|启动成功|
|---|---|
|40G|Y|
|50G|Y|
|64G|Y|

最大规格虚拟机，居然也能创建?

创建64G虚拟机后，查看Node 1 normal zoneinfo
```
Node 1, zone   Normal
  per-node stats
  ...
  pages free     16047737
        boost    0
        min      7058
        low      40085
        high     73112
        spanned  33553792
        present  33553792
        managed  33027391
        cma      16777216
        protection: (0, 0, 0, 0, 0)
      nr_free_pages 16047737
      nr_zone_inactive_anon 37735
      nr_zone_active_anon 753
      nr_zone_inactive_file 4270
      nr_zone_active_file 7161
      nr_zone_unevictable 0
      nr_zone_write_pending 0
      nr_mlock     0
      nr_bounce    0
      nr_zspages   0
      nr_free_cma  8079813
      nr_unaccepted 0
      numa_hit     0
      numa_miss    0
      numa_foreign 0
      numa_interleave 0
      numa_local   0
      numa_other   0
```

Node 1 cma内存消耗了 8 GB 左右，剩下的内存是否在cma中消耗的呢?

#### 查看内存其他信息

**free -g:**
+ 启动前:
  ```sh
  [root@11-241-212-108 15:59:08 kernel]# free -g
                 total        used        free      shared  buff/cache   available
  Mem:            1005         539         466           0           4         465
  Swap:              0           0           0
  ```
+ 启动后:
  ```sh
  [root@11-241-212-108 16:04:05 kernel]# free -g
                 total        used        free      shared  buff/cache   available
  Mem:            1005         599         406           0           3         406
  Swap:              0           0           0
  ```
  可以发现少了60多G内存。

也就是说明 guest 并未在hugetlb中分配内存。而是在cma 中分配的。


**查看hugetlb:**
```cpp
[root@11-241-212-108 16:10:55 hugepages-2048kB]# cat free_hugepages
229376
[root@11-241-212-108 16:10:58 hugepages-2048kB]# cat nr_hugepages
262144

# echo $(((262144-229376) *2048 /1024/1024))
64
```
而大页也减少了64 G，说明hugetlb也减少了64G。

由此可见，不能使用大页
#### 增加debug信息

经调试发现, kvm 在`csv3_set_guest_private_memory`分配guest内存，
增加如下调试信息
```diff
--- kernel/arch/x86/kvm/svm/csv.c       2025-06-25 23:13:39
+++ ../kernel/arch/x86//kvm/svm/csv.c   2025-07-15 16:09:20
@@ -1062,6 +1062,7 @@
static int csv3_set_guest_private_memory(struct kvm *kvm)
{
        u64 size = 0, nr_smr = 0, nr_pages = 0;
        u32 smr_entry_shift;
        int bkt;
+       int node;

        unsigned int flags = FOLL_HWPOISON;
        int npages;
@@ -1125,6 +1126,10 @@
                ALIGN(nr_pages * 9, 1UL << smr_entry_shift);
        nr_smr = size >> smr_entry_shift;
        remainder = nr_smr;
+
+    pr_info("set private page: nr_pages is %llu\n", nr_pages);
+    pr_info("Nodemask: %*pbl\n", nodemask_pr_args(nodemask_ptr));
+
        for (i = 0; i < nr_smr; i++) {
                smr = kzalloc(sizeof(*smr), GFP_KERNEL_ACCOUNT);
                if (!smr) {
@@ -1135,6 +1140,7 @@
                smr->hpa = csv_alloc_from_contiguous((1UL << smr_entry_shift),
                                                nodemask_ptr,
                                                get_order(1 << smr_entry_shift));
+               node = page_to_nid(phys_to_page(smr->hpa));
                if (!smr->hpa) {
                        kfree(smr);
                        ret = -ENOMEM;
```

打印如下:
```
[root@11-241-212-108 16:02:23 kernel]# dmesg |grep mask
[79060.167408] CSV: Nodemask: 0-7
[root@11-241-212-108 16:01:59 kernel]# dmesg |grep nr
[79060.162669] CSV: set private page: nr_pages is 16778240
[root@11-241-212-108 15:52:35 kernel]# dmesg |grep node |awk '{print $5}' |sort |uniq
0
1
2
3
4
5
6
7
```
可以发现, 本次分配内存在`node-[1-7]`中均有分配. 共分配16778240 page(64G)内存。

### csv2 机型

那普通虚拟机能创建多大容量呢?

### 创建csv2虚拟机

我们还是假设，csv2 只能分配非cma的内存，那么，我们计算下大页中，
有多少内存可以分配给csv2 虚拟机:

关闭虚拟机后, Node 1 zone info
```
  pages free     16084059
        boost    0
        min      7058
        low      40085
        high     73112
        spanned  33553792
        present  33553792
        managed  33027391
        cma      16777216
        protection: (0, 0, 0, 0, 0)
      nr_free_pages 16084059
      nr_zone_inactive_anon 28146
      nr_zone_active_anon 752
      nr_zone_inactive_file 4663
      nr_zone_active_file 7180
      nr_zone_unevictable 0
      nr_zone_write_pending 0
      nr_mlock     0
      nr_bounce    0
      nr_zspages   0
      nr_free_cma  8099519
```
计算如下:
```
cma 内存
16777216 -  8099519 = 8677697

```
大页中除去cma的大页数量
```
64 * 1024 * 1024 / 4 - 8677697
= 8099519 = 32398076KiB = 30.8G
```

### 测试不同规格虚拟机
我们首先测试
|规格|启动成功|
|---|---|
|31G|Y|
|32G|N|
|35G|N|


### 32G csv2 虚拟机启动失败信息
qemu 打印
```
[root@11-241-212-108 11:34:39 csv2]# virsh start wang_sev
error: Failed to start domain 'wang_sev'
error: internal error: QEMU unexpectedly closed the monitor (vm='wang_sev'): configure accelerator pc-q35-6.2 start
2025-07-14T03:34:50.433653Z qemu-system-x86_64: sev_ram_block_added: failed to register region (0x7fabafe00000+0x800000000) error 'Cannot allocate memory'
```

dmesg打印
```
[ 3631.277369] kvm_amd: SEV: Failure locking 8388608 pages.
```

所以可以认为，csv2虚拟机基本上不能分配cma 中预留的内存。

### 31G csv2 虚拟机启动成功后,zoneinfo 
```
  pages free     15959768
        boost    0
        min      7058
        low      40085
        high     73112
        spanned  33553792
        present  33553792
        managed  33027391
        cma      16777216
        protection: (0, 0, 0, 0, 0)
      nr_free_pages 15959768
      nr_zone_inactive_anon 33638
      nr_zone_active_anon 1775
      nr_zone_inactive_file 4470
      nr_zone_active_file 7442
      nr_zone_unevictable 0
      nr_zone_write_pending 0
      nr_mlock     0
      nr_bounce    0
      nr_zspages   0
      nr_free_cma  8048214
```
发现cma内存，几乎并未被消耗, 说明csv2 会优先找非cma预留内存.

## 测试非csv虚拟机
|规格|启动成功|
|---|---|
|31G|Y|
|32G|Y|
|64G|Y|

和csv2不同的是，普通虚拟机，可以使用cma 预留的内存

## 使用free page 分配匿名页测试

我们再来测试下，buddy中的cma 内存是否可以分配给普通程序使用。

编写程序，直接分配50G内存做如下测试.
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#define GB (1024UL * 1024 * 1024)
#define MEM_SIZE (50UL * GB)

int main() {
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t num_pages = MEM_SIZE / page_size;

    printf("Page size: %zu bytes\n", page_size);
    printf("Total pages: %zu\n", num_pages);

    uint8_t *mem = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    printf("Memory allocated at %p\n", mem);

    for (size_t i = 0; i < num_pages; ++i) {
        uint8_t *addr = mem + i * page_size;
        uint8_t value = (uint8_t)(i & 0xFF);

        *addr = value;  // 写入一个字节

        if (*addr != value) {  // 校验
            printf("Data mismatch at page %zu: got %u, expected %u\n",
                   i, *addr, value);
            munmap(mem, MEM_SIZE);
            return 2;
        }

        if ((i+1) % 10000 == 0) {
            printf("Checked %zu pages...\n", i+1);
        }
    }

    printf("All pages written and verified successfully!\n");

    //munmap(mem, MEM_SIZE);
    while(1) sleep(60);
    return 0;
}
```

编译执行:
```
numactl --membind 1 ./test
```
发现可以执行，验证通过。

程序执行后，查看zoneinfo，发现cma内存确实被占用。

```
  pages free     2963158
        boost    0
        min      7058
        low      40085
        high     73112
        spanned  33553792
        present  33553792
        managed  33027391
        cma      16777216
        protection: (0, 0, 0, 0, 0)
      nr_free_pages 2963158
      nr_zone_inactive_anon 13129108
      nr_zone_active_anon 750
      nr_zone_inactive_file 4447
      nr_zone_active_file 7432
      nr_zone_unevictable 0
      nr_zone_write_pending 0
      nr_mlock     0
      nr_bounce    0
      nr_zspages   0
      nr_free_cma  1481723
      nr_unaccepted 0
      numa_hit     0
      numa_miss    0
      numa_foreign 0
      numa_interleave 0
      numa_local   0
      numa_other   0
```

说明，非虚拟机程序，可以占用cma 内存。

## 结论
* 如果在开启了`csv_mem_percentage`参数的系统中预留了hugetlb，hugetlb会从
  buddy中获取csv_cma预留内存和非csv_cma 内存
* csv3 不能使用大页预留的内存，只能使用buddy中的csv_cma内存
* 即使csv3虚拟机内存指定大页启动，但是其private page仍然会在csv_cma内存中分配,
  而hugetlb 仍然被占用（public page) 造成双倍的内存消耗
* csv1,csv2 虚拟机不能使用cma预留内存
* 非csv虚拟机，可以使用cma 内存


所以综上所述:
<font color=red><strong> 不建议在使用了`csv_mem_percentage`机器上使用hugetlb 创建虚拟机</strong>
</font>
