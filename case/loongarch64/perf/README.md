## 硬件说明

### CPU
| 架构        | cpu               | thread数量 | 主频   | 缓存L1(icache + dcache) | 缓存L2 | 缓存L3 |
|-------------|-------------------|------------|--------|-------------------------|--------|--------|
| loongarch64 | Loongson-3C6000/D | 128        | 2.1GHz | 4MiB+4MiB               | 16MiB  | 128MiB |
| hygon       | 7390              | 128        | 2.5GHz | 2MiB+4MiB               | 32MiB  | 128MiB |
| kunpeng     | Kunpeng 920 7262C | 128        | 2.5GHz | 8MiB+8MiB               | 64MiB  | 128MiB |

### memory
| 架构        | 大小 | 规格                     | numa |
|-------------|------|--------------------------|------|
| loongarch64 | 512G | 16 * 32GB DDR4 3200 MT/s | 4    |
| hygon       | 1T   | 32 *32GB DDR4 3200MT/s   | 2    |
| kunpeng     | 1T   | 16 * 32GB DDR4 3200MT/s  | 2    |

## CPU 性能测试

### UNIXBENCH

`UNIXBENCH`跑分用来衡量系统的综合性能,其中

* `dhry2reg`用来衡量字符串性能指标
* `whetstone-double`衡量浮点性能指标
* 总跑分数据用来衡量系统综合性能

#### dhry2reg

![dhrystone_2](./unixbench/unixbench_dhrystone_2_using_register_variables_detailed_comparison.png)

kunpeng强于龙芯 `14.3%`, hygon弱于龙芯`2.9%`, 龙芯虚拟化损耗`2.5%`, 可以忽略不计.

#### double precision whetstone

![double_precision_whetstone](./unixbench/unixbench_double_precision_whetstone_detailed_comparison.png)

龙芯和kunpeng几乎持平，hygon性能最好，高于龙芯鲲鹏`46%`

#### 综合跑分

![unixbench_overall_single_thread](./unixbench/unixbench_overall_single_thread.png)
![unixbench 16 thread](./unixbench/unixbench_overall_16_threads.png)

龙芯无论单线程或者多线程跑分均比较优秀，高于鲲鹏约`20%-25%`, 高于hygon约`25% ~30%`

#### 综合跑分虚拟化损耗

![unixbench 1 vm vs host](./unixbench/unixbench_vm_vs_host_unixbench1.png)
![unixbench 16 vm vs host](./unixbench/unixbench_vm_vs_host_unixbench16.png)

kunpeng非常优秀，综合跑分无论是单线程还是多线程，虚拟化损耗在`10%-15%`, 而龙芯在`25%-45%`,  其中
多线程性能损耗更明显。(单线程下降24.6%, 多线程下降43.2%)

### sysbench

#### sysbench 多核单核跑分

![sysbench 1](./sysbench/sysbench_cpu_single_thread.png)

![sysbench 16](./sysbench/sysbench_cpu_16_threads.png)

龙芯高于hygon `48%`, 低于kunpeng `30%`左右


#### 虚拟化损耗

![sysbench vm 1](./sysbench/sysbench_cpu_1_threads_vm_comparison.png)

![sysbench vm 16](./sysbench/sysbench_cpu_16_threads_vm_comparison.png)

sysbench 跑分，均无虚拟化损耗

## stream test(host)

stream 测试分为四项:

* copy: Copy操作最为简单，它先访问一个内存单元读出其中的值，再将值写入到另一个内存单元。
* scale: Scale操作先从内存单元读出其中的值，作一个乘法运算，再将结果写入到另一个内存单元。
* add: Add操作先从内存单元读出两个值，做加法运算， 再将结果写入到另一个内存单元。
* tride: Triad的中文含义是将三个组合起来，在本测试中表示的意思是将Copy、Scale、Add三种操作
  组合起来进行测试。具体操作方式是：先从内存单元中中读两个值a、b，对其进行乘加混合运算
（a + 因子 * b ） ，将运算结果写入到另一个内存单元。

结果如下:

### not cross numa

不跨numa测试:

![stream test diff arch](./stream/stream_architecture_comparison.png)

hygon性能最好, kunpeng 比龙芯高`65%~85`, hygon比kunpeng高`116~134%`

### cross-numa

![stream test diff arch cross-numa](./stream/stream_cross_numa_comparison.png)

cross-numa 测试中，kunpeng比龙芯高200%，hygon比龙芯高190%.

### 跨numa损耗

![stream test diff arch cross-numa](./stream/stream_numa_comparison.png)

龙芯损耗在`45%~55%`之间。

鲲鹏损耗在`10%~15%`之间。

hygon损耗在`35~40%`之间。

## TODO

* [X] 测试hygon vm 的unixbench, 目前还在测试中

## 综合说明

目前仅测试cpu和内存

### CPU性能测试结果

> NOTE
> 
>  * `+` 龙芯比该架构性能高出的百分比
>  * `-` 龙芯比该架构性能低出的百分比

各个架构之间的跑分对比:

|测试项|龙芯对比hygon|龙芯对比kunpeng|
|---|---|---|
|unixbench dhry2reg|+3%|14%|
|unixbench double precision whetstone|-46%| +- < %1|
|unixbench 综合跑分|+25%~30%|+20%~25%|
|sysbench 测试|+45%~50%|-30%-35%|

各个架构unixbench 综合跑分虚拟化损耗(其他测试项虚拟化损耗可忽略 < 5%):
|架构|损耗比单核|损耗比16核|
|----|----|----|
|loongarch|25%|43%|
|kunpeng|12%|12%|
|hygon|-|-|


### stream 内存压测

各个架构之间的跑分对比:
|是否跨numa|龙芯对比hygon|龙芯对比kunpeng|
|----|----|----|
|不跨numa|-115%~134%|-65%~83%|
|跨numa|-185%~200%|-200%~208%|

> NOTE
>
> 该测试中虚拟化带来的损耗可忽略 <5%, 所以下面仅对比跨numa带来的损耗:

各个架构跨numa损耗:

|龙芯|kunpeng|hygon|
|----|----|----|
|45%~55%|10%~15%|35%~40%|

## 其他细节图
### 内存延迟测试`lat_mem_rd`

![memory_latency](./lat_mem_rd/memory_latency_9lines.png)

### Perfbench

![perfbench1](./perfbench_sched/perfbench_combined.png)

### core-to-core latency

![core-to-core-latency](./core-to-core-latency/images/core_to_core_latency_loongarch.png)

## 龙芯目前存在的性能问题
1. 虚拟化损耗较高
   + perfbench
   + unixbench综合跑分
2. stream 测试性能低
3. 跨nume通信差
   + `core-to-core-latency`
   + `lat_mem_rd` 跨numa测试大步长
   + stream 跨numa测试
