# 11 MEMORY CACHE CONTROL
This chapter describes the memory cache and cache control 
mechanisms, the TLBs, and the store buffer in Intel 64 and 
IA-32 processors. It also describes the memory type range 
registers (MTRRs) introduced in the P6 family processors 
and how they are used to control caching of physical memory 
locations.

<font color=gray face="黑体" size=2>
该章节描述了Intel 64 和 IA-32 处理器中的memory cache 和 
cache control机制， TLB和store buffer。也描述了P6 family processor
引进的memory type range register (MTRRs) 和他们如何用于控制physical
memory locations 的cache。
</font>

## 11.1 INTERNAL CACHES, TLBS, AND BUFFERS
The Intel 64 and IA-32 architectures support cache, translation
look aside buffers (TLBs), and a store buffer for temporary 
on-chip (and external) storage of instructions and data. 
(Figure 11-1 shows the arrangement of caches, TLBs, and the 
store buffer for the Pentium 4 and Intel Xeon processors.) 
Table 11-1 shows the characteristics of these caches and buffers
for the Pentium 4, Intel Xeon, P6 family, and Pentium processors.
The sizes and char- acteristics of these units are machine specific
and may change in future versions of the processor. The CPUID
instruction returns the sizes and characteristics of the caches and 
buffers for the processor on which the instruction is executed. See 
“CPUID—CPU Identification” in Chapter 3, “Instruction Set Reference, A-L,” 
of the Intel® 64 and IA-32 Architectures Software Developer’s Manual, 
Volume 2A. 

<font color=gray face="黑体" size=2>
Intel 64 和IA-32 架构支持cache, translation look aside buffers(TLBs),
和为 临时 on-chip (和外部的) 指令和数据的storage 建立的 store buffer
(Figure 11-1展示了对于Pentium 4, Intel Xeon, P6 family, 和 Pentium 处理器
的这些caches和buffers的特征）。这些单units 的大小和特征是machine specific
并且在之后的处理器版本中可能会改变。CPUID instruction 返回了对于运行该指令
的处理器上的caches和buffers 的sizes和特征。请查看Chapter 3 中的"CPUID - 
CPU Identification”章节。
</font>

![Figure-11-1](pic/Figure-11-1.png)

![Figure-11-2](pic/Figure-11-2.png)

Figure 11-2 shows the cache arrangement of Intel Core i7 processor.

<font color=gray face="黑体" size=2>
图片11-2 展示了Intel Core i7 处理器的cache 部署
</font>

Table 11-1. Characteristics of the Caches, TLBs, Store Buffer,
and Write Combining Buffer in Intel 64 and IA-32 Processors

<font color=gray face="黑体" size=6>
TABLE 11-1 (略)
</font>

Intel 64 and IA-32 processors may implement four types of caches:
the trace cache, the level 1 (L1) cache, the level 2 (L2) cache,
and the level 3 (L3) cache. See Figure 11-1. Cache availability
is described below:

<font color=gray face="黑体" size=2>
Intel 64 和 IA-32 处理器可以实现四种类型的caches: trace cache, level 
1 (L1) cache , level 2 (L2) cache和level 3 (L3) cache。 查看Figure 
11-1。Cache 可用性如下描述:
</font>

* Intel Core i7, i5, i3 processor family and Intel Xeon processor
family based on Nehalem microarchitecture and Westmere 
microarchitecture — The L1 cache is divided into two sections:
one section is dedicated to caching instructions (pre-decoded
instructions) and the other caches data. The L2 cache is a unified
data and instruction cache. Each processor core has its own L1 and L2.
The L3 cache is an inclusive, unified data and instruction cache,
shared by all processor cores inside a physical package. No trace
cache is implemented.
<br/>
<font color=gray face="黑体" size=2>
Intel Core i7, i5,i3 处理器 家族和基于 Nehalem microarchitecture 和
Westmere microarchitecture 的Intel Xeon 处理器家族:
<br/>
L1 cache 被分成两个部分:
一部分是用于cache instructions(预解码指令) ，另一部分是caches data。
L2 cache 是一个不区分data 和 instructions 的cache。每个处理器核心
有自己的L1和L2 cache 。L3 cache 是一个 inclusive(包括全部的?) 不区分
data 和 instructions cache, 并且被 physical package 内部中所有的
处理器核心共享。No trace cache 在该架构中实现。
</font>

* Intel® Core™ 2 processor family and Intel® Xeon® processor 
family based on Intel® Core™ microarchitecture — The L1 cache
is divided into two sections: one section is dedicated to caching
instructions (pre- decoded instructions) and the other caches data.
The L2 cache is a unified data and instruction cache located on the 
processor chip; it is shared between two processor cores in a 
dual-core processor implementation. Quad-core processors have two
L2, each shared by two processor cores. No trace cache is implemented.
<br/>
<font color=gray face="黑体" size=2>
Intel(R) Core(TM) 处理器家族和 基于Intel(R) Core(TM) microarchitecture
的Intel (R) Xeon(R) 处理器家族:
<br/>
L1 cache 被分为两部分: 一部分用于缓存指令(预解码指令)另一部分用于缓存
data。L2 cache 不区分data和 instruction cache , 这些cache 位于procoessor 
chip; 它在 dual-core 处理器实现中的两个 processor core 之间共享。Qual-core
处理器有两个L2, 每个在两个处理器中共享。没有trace cache 被实现。
</font>

* Intel Atom® processor — The L1 cache is divided into two sections:
one section is dedicated to caching instructions (pre-decoded 
instructions) and the other caches data. The L2 cache is a 
unified data and instruction cache is located on the processor 
chip. No trace cache is implemented.
<br/>
<font color=gray face="黑体" size=2>
Intel Atom(R) procssor:
L1 cache 被分为两部分: 一部分用于缓存指令(预解码指令), 另一部分用于缓存
data. L2 cache不区分data和指令，并且cache位于processor chip上。
没有trace cache 的实现。
<br/>
</font>

* Intel® Core™ Solo and Intel® Core™ Duo processors — The L1 
cache is divided into two sections: one section is dedicated
to caching instructions (pre-decoded instructions) and the other
caches data. The L2 cache is a unified data and instruction 
cache located on the processor chip. It is shared between two
processor cores in a dual-core processor implementation. No 
trace cache is implemented.
<br/>
<font color=gray face="黑体" size=2>
Intel® Core™ Solo and Intel® Core™ Duo processors :
<br/>
L1 cache 被分为两部分: 一部分用于缓存指令(预解码指令)另一部分
用于缓存数据。L2 cache 不区分data 和 instruction cache, 
并且位于processor chip。它在两个 dual-core processor 实现
的处理器核心之间共享。没有trace cache 的实现。
</font>

* Pentium® 4 and Intel® Xeon® processors Based on Intel 
NetBurst® microarchitecture — The trace cache caches decoded 
instructions (μops) from the instruction decoder and the L1 
cache contains data. The L2 and L3 caches are unified data and
instruction caches located on the processor chip.Dualcore
processors have two L2, one in each processor core. Note that 
the L3 cache is only implemented on some Intel Xeon processors.

* P6 family processors — The L1 cache is divided into two sections:
one dedicated to caching instructions (pre- decoded instructions)
and the other to caching data. The L2 cache is a unified data and
instruction cache located on the processor chip. P6 family 
processors do not implement a trace cache.

* Pentium® processors — The L1 cache has the same structure as
on P6 family processors. There is no trace cache. The L2 cache
is a unified data and instruction cache external to the processor
chip on earlier Pentium processors and implemented on the processor
chip in later Pentium processors. For Pentium processors where
the L2 cache is external to the processor, access to the cache
is through the system bus. 

For Intel Core i7 processors and processors based on Intel Core, 
Intel Atom, and Intel NetBurst microarchitectures, Intel Core
Duo, Intel Core Solo and Pentium M processors, the cache lines
for the L1 and L2 caches (and L3 caches if supported) are 64 
bytes wide. The processor always reads a cache line from system
memory beginning on a 64- byte **boundary.**<sup>1</sup> (A 64-byte 
aligned cache line begins at an address with its 6 least- 
**significant**<sup>2</sup> bits clear.) A cache line can be 
filled from memory with a **8-transfer burst** <sup>3</sup>transaction. The 
caches do not support partially-filled cache lines, so caching
even a single doubleword requires caching an entire line.
<br/>
<font color=gray face="黑体" size=2>
1. boundary: 界限分界线。这里指的是对齐的字节
2. significant : 有意义的; least-significant ，这里指的是最低的有效位
3. transfer: 转让调动，一次完整的DMA数据传输称为transfer, 可以包含多个
burst, burst数量被称为transfer数量

burst: 突发, DMA传输中的最小单元
<br/>
所以，这里应该指的是，一次从memory -> cacheline的transfer ,中有8个burst.
也就是8个bype数据(64 byte)。每个byte代表一个burst(以地址为单位)
</font>

The L1 and L2 cache lines in the P6 family and Pentium processors
are 32 bytes wide, with cache line reads from system memory 
beginning on a 32-byte boundary (5 least-significant bits of 
a memory address clear.) A cache line can be filled from memory
with a 4-transfer burst transaction. Partially-filled cache 
lines are not supported. The trace cache in processors based 
on Intel NetBurst microarchitecture is available in all 
execution modes: protected mode, system management mode (SMM),
and real-address mode. The L1,L2, and L3 caches are also available
in all execution modes; however, use of them must be handled 
carefully in SMM (see Section 31.4.2, “SMRAM Caching”).

The TLBs store the most recently used page-directory and page-table
entries. They speed up memory accesses when paging is enabled by
reducing the number of memory accesses that are required to read
the page tables stored in system memory. The TLBs are divided into
four groups: instruction TLBs for 4-KByte pages, data TLBs for
4-KByte pages; instruction TLBs for large pages (2-MByte, 4-MByte
or 1-GByte pages), and data TLBs for large pages. The TLBs are
normally active only in protected mode with paging enabled. When
paging is disabled or the processor is in real-address mode, 
the TLBs maintain their **contents**<sup>1</sup> until 
**explicitly**<sup>2</sup> or **implicitly**<sup>3</sup>
flushed (see Section 11.9, “Invalidating the Translation 
Lookaside Buffers (TLBs)”).

<font color=gray face="黑体" size=2>
四种TLB: 
<ul>
	<li>instruction TLB for 4-KByte page</li>
	<li>data TLB for 4-KByte page</li>
	<li>instruction TLB for large page</li>
	<li>data TLB for large page</li>
</ul>

1. content: 内容<br/>
2. explicitly: 明确而详细的<br/>
3. implicitly: 含蓄的，不直接表明的<br/>

这里指的是当paging 改变为disabled的状态，或者处理器正处于
real-address模式，TLBs 将会继续维护这些内容，直到被明确（直接）
或者间接的flush
</font>

Processors based on Intel Core microarchitectures implement one
level of instruction TLB and two levels of data TLB. Intel Core
i7 processor provides a second-level unified TLB. The store 
buffer is associated with the processors instruction execution
units. It allows writes to system memory and/or the internal 
caches to be saved and in some cases combined to optimize the
processor’s bus accesses. The store buffer is always enabled 
in all execution modes.

<font color=gray face="黑体" size=2>
store buffer 和处理器 指令执行单元联系起来。它允许将写入系统内存和
internal caches的数据存储下来，在某些情况下组合起来去优化processor's
bus 访问。在所有的执行模式中，strore buffer 总是 enabled
</font>

The processor’s caches are for the **most part**<sup>1</sup> transparent to 
software. When enabled, instructions and data **flow through** <sup>2</sup>
these caches without the need for explicit software control. 
However, knowledge of the behavior of these caches may be 
useful in optimizing software performance. For example, 
knowledge of cache dimensions<sup>3</sup> and replacement
<sup>4</sup> algorithms gives an indication of how large of 
a data structure can be operated on at once without causing 
cache thrashing<sup>5</sup>.

<font color=gray face="黑体" size=2>
1. most part: 很大程度上;大部分 <br/>
2. 流经; 流过; <br/>
3. 量度, 维度 <br/>
4. 替换 <br/>
5. 激烈的运动，移动

这些cache 维度和替换算法的知识可以表明一次操作多大的数据而不会导致
缓存抖动
</font>

In multiprocessor systems, maintenance of cache consistency may,
**in rare circumstances**<sup>1</sup>, require intervention<sup>2</sup> by system software.
For these rare cases, the processor provides privileged cache
control instructions for use in flushing caches and forcing 
**memory ordering**<sup>3</sup>.

<font color=gray face="黑体" size=2>
1. rare: 极少数 circumstances: 环境，情形，情况 <br/>
in rare circumstances: 在极少数的情况下<br/>
2. 介入
3. 内存访问排序 这里指的是一些内存屏障指令
</font>

There are several instructions that software can use to improve
the performance of the L1, L2, and L3 caches, including the 
PREFETCHh, CLFLUSH, and CLFLUSHOPT instructions and the **non-temporal**<sup>1</sup>
move instructions (MOVNTI, MOVNTQ, MOVNTDQ, MOVNTPS, and MOVNTPD).
The use of these instructions are discussed in Section 11.5.5,
“Cache Management Instructions.” 

<font color=red face="黑体" size=2>
1. !!!!!
</font>

## 11.2 CACHING TERMINOLOGY
IA-32 processors (beginning with the Pentium processor) and 
Intel 64 processors use the MESI (modified, exclu- sive, 
shared, invalid) cache protocol to maintain consistency with 
internal caches and caches in other processors (see Section 
11.4, “Cache Control Protocol”). 

When the processor recognizes that an operand<sup>1</sup> 
being read from memory is cacheable, the processor 
reads an entire cache line into the appropriate cache 
(L1, L2, L3, or all). This operation is called a cache 
line fill. If the memory location containing that operand is 
still cached the next time the processor attempts to access 
the operand, the processor can read the operand from the cache
instead of going back to memory. This operation is called a 
cache hit.

<font color=gray face="黑体" size=2>
1. operand: 操作数

cache line fill: 如果processor识别到 内存中读取的操作数是可缓存
,该操作被称为cache line fill
的，processor 读取整个的cache line 到合适的cache 中(L1,L2,L3 or all)

cache hit: 如果memory location 包含的操作数已经缓存了，下次处理器尝试
去访问这个操作数，处理器可以从cache 中读取该操作数，而不是回到内存中
读取。该操作被称为 a cache hit。
</font>

When the processor attempts to write an operand to a cacheable
area of memory, it first checks if a cache line for that memory
location exists in the cache. If a valid cache line does exist,
the processor (depending on the write policy<sup>1</sup> currently **in force**<sup>2</sup>)
can write the operand into the cache instead of writing it out
to system memory. This operation is called a **write hit**. If a 
write misses the cache (that is, a valid cache line is not 
present for area of memory being written to), the processor 
performs a cache line fill, write allocation. Then it writes 
the operand into the cache line and (depending on the write 
policy currently in force) can also write it out to memory. 
If the operand is to be written out to memory, it is written 
first into the store buffer, and then written from the store 
buffer to memory when the system bus is available. (Note that
for the Pentium processor, write misses do not result in a cache
line fill; they always result in a write to memory. For this 
processor, only read misses result in cache line fills.)

<font color=gray face="黑体" size=2>
1. policy: 策略<br/>
2. in force : 实施中的，现行的<br/>

当write 操作缓存miss, 处理器会执行一个cache line fill的操作。然后
write 这个操作数到 cachline 并且（根据当前的写入策略) 回写到内存。
如果操作数要回写到内存，现将其写入store buffer, 然后等system bus is available
时， 将store buffer 中的数据写入memory

在奔腾处理器中，write misses 不会导致 cache line fill; 他们
总是会写入到内存中。对于这些处理器，只有read misses会导致 cache line fill
</font>

When operating in an MP system, IA-32 processors (beginning 
with the Intel486 processor) and Intel 64 processors have the
ability to snoop other processor’s accesses to system memory 
and to their internal caches. They use this snooping ability 
to keep their internal caches consistent both with system memory
and with the caches in other processors on the bus. For example,
in the Pentium and P6 family processors, if through snooping 
one processor detects<sup>1</sup> that another processor intends<sup>2</sup> to write
to a memory location that it currently has cached in shared 
state, the snooping processor will invalidate its cache line 
forcing it to perform a cache line fill the next time it 
accesses the same memory location.

<font color=gray face="黑体" size=2>
1. detect : 发现察觉<br/>
2. intend: 打算

这里提到的某些处理器可能会有一定的snoop 行为。这个snoop 行为会使得
自己的internal cache 和system memory 以及其他cpu core 的 internal cache
有一些通信行为。 如果另一个处理器修改了 本处理器中cache 数据，
本处理器会snoop 到， 并且 invalidate internal cache line . 如果下
次要访问这块内存的话，需要去执行一个 cache line fill 的操作.
</font>

Beginning with the P6 family processors, if a processor detects<sup>1</sup>
(through snooping) that another processor is trying to access
a memory location that it has modified in its cache, but has 
not yet written back to system memory, the snooping processor
will signal the other processor (**by means of**<sup>2</sup> the HITM# signal)
that the cache line is held in modified state and will perform
an implicit write-back of the modified data. The implicit 
write-back is transferred directly to the initial requesting 
processor and snooped by the memory controller to assure that
system memory has been updated. Here, the processor with the 
valid data may pass the data to the other processors without 
actually writing it to system memory; however, it is the 
responsibility of the memory controller to snoop this operation
and update memory. 

<font color=gray face="黑体" size=2>
1. detect : 发现察觉<br/>
2. be means of: 通过


这里讲的是如果有个处理器发现(通过snoop)另一个处理器尝试访问一个内存，
但是该内存在本处理器的缓存中已经修改了, 但是还没有写回到
系统内存中， snoop processor 会向其他的处理器发送一个信号(通过HITM#信号),
该信号表示该cache line 已经处于修改的状态，并且将会执行一个隐式
的modify data 的 write-back操作。该 隐式write-back 会直接传输到
发起request 的 processor 被嗯切被memory controller snoop 到，用于
保证系统内存已经更新。这里带有合法data的(正确的)processor 可能将输出直接
传输到其他的处理器，在数据实际没有写入系统内存的情况下;
但是, memory controller 负责 snoop 该行为并且更新内存。
</font>

## 11.3 METHODS OF CACHING AVAILABLE
The processor allows any area of system memory to be cached in
the L1, L2, and L3 caches. In individual pages or regions of 
system memory, it allows the type of caching (also called memory
type) to be specified (see Section 11.5). Memory types currently
defined for the Intel 64 and IA-32 architectures are (see Table
11-2):

<font color=gray face="黑体" size=2>
这里将的memory type 实际上指 独立的内存页或者内存区间的 cache type.
</font>

* Strong Uncacheable (UC) —System memory locations are not 
cached. All reads and writes appear on the system bus and are
executed in program order without reordering. No speculative<sup>1</sup>
memory accesses, page-table walks, or prefetches of speculated
branch targets are made. This type of cache-control is useful
for memory-mapped I/O devices. When used with normal RAM, it 
greatly reduces processor performance.

<font color=gray face="黑体" size=2>
1. speculative: 投机的; 推测出的

UC： 有以下几个特点：<br/>
1. 对system memory 访问不会触发cache <br/>
2. 任何读写访问不会被重新排序 <br/>
3. 没有投机内存访问，page-table walks, 以及预取推测的分支目标

</font>

* Uncacheable (UC-) — Has same characteristics as the strong 
uncacheable (UC) memory type, except that this memory type can
be overridden by programming the MTRRs for the WC memory type.
This memory type is available in processor families starting 
from the Pentium III processors and can only be selected 
through the PAT.

<font color=gray face="黑体" size=2>
基本于UC相同，除了 该memory type 可以通过编程MTRRs 值覆盖 成WC 
memory type.
</font>

* Write Combining (WC) — System memory locations are not cached
(as with uncacheable memory) and coherency is not enforced<sup>强制</sup> by
the processor’s bus coherency protocol. Speculative reads are
allowed. Writes may be delayed and combined<sup>组合;结合;合并</sup> in the write 
combining buffer (WC buffer) to reduce memory accesses. 
If the WC buffer is partially filled, the writes may be delayed
until the next occurrence<sup>事件，事情</sup> of a serializing<sup>序列化</sup> event; such as an 
SFENCE or MFENCE instruction, CPUID or other serializing instruction,
a read or write to uncached memory, an interrupt occurrence, 
or an execution of a LOCK instruction (including one with an 
XACQUIRE or XRELEASE prefix). In addition, an execution of the
XEND instruction (to end a transactional region) evicts<sup>逐出，赶出</sup> any 
writes that were buffered before the corresponding execution 
of the XBEGIN instruction (to begin the transactional region)
before evicting any writes that were performed inside the 
transactional region. This type of cache-control is appropriate
for video frame buffers, where the order of writes is unimportant
as long as the writes update memory so they can be seen on the
graphics display. See Section 11.3.1, “Buffering of Write 
Combining Memory Locations,” for more information about caching
the WC memory type. This memory type is available in the Pentium
Pro and Pentium II processors by programming the MTRRs; or in
processor families starting from the Pentium III processors by
programming the MTRRs or by selecting it through the PAT.

<font color=gray face="黑体" size=2>
1. enforced: 强制<br/>
2. 组合;结合
<br/><br/>
Write Combining (WC)有以下特点:<br/>
1. not cached (uncacheable memory)<br/>
2. 允许投机读取<br/>
3. 写操作将会被delayed 并且 combined(合并)在write combining buffer(WC buffer)
来减少 memory access<br/>
4. 如果WC buffer 被部分填充，writes操作将会delay, 知道发生了一些事件,例如 一些序列化指令，read or write to uncached memory , interrupt事件。
...<br/>
5. 某些寄存器可以通过 MTRRs 编程或者通过PAT 选择 enable 该memory type


(Note: Speculative read是指读之前并不验证内存的有效性，先冒险的读进来，
如果发现不是有效数据再取消读取操作，并更新内存后再读取. 比如说数据还是被
buffer在WC buffer中)
<br/>
来自: https://blog.csdn.net/chen1540524015/article/details/74219375
</font>

* Write-through (WT) — Writes and reads to and from system 
memory are cached. Reads come from cache lines on cache hits;
read misses cause cache fills. Speculative reads are allowed.
All writes are written to a cache line (when possible) and 
through to system memory. When writing through to memory, invalid
cache lines are never filled, and valid cache lines are either
filled or invalidated. Write combining is allowed. This type of
cache-control is appropriate for frame buffers or when there 
are devices on the system bus that access system memory, but 
do not perform snooping of memory accesses. It enforces coherency
between caches in the processors and system memory. 

<font color=gray face="黑体" size=2>
Write-through (WT) :<br/>
<ol>
<li>Write to 和 read from memory 会被cache.</li>
<li>read 会在cache hits 情况下从cache line 获取,在miss情况下
会造成cache fills</li>
<li> 允许 speculative read
<li> 所有的写操作会写入到一个cache line(如果可能的话）并且会写入system
memory.</li>
<li> 当写向内存时，无效的cache line 不会被filled, 有效的cache line 
可能会被fill, 或者被 无效(这里应该指的是other processor cpu)</li>
<li> 此类行的cache 使用于frame buffer 或者systemd bus 上的devices 
访问系统 内存</li>
<font color=red face="黑体" size=2>
<li> 不能执行snoop memory access.(???)</li>
</font>
<li> 会在processor cache 和 system memory 强制保持一致性.</li>
</ol>
</font>

* Write-back (WB) — Writes and reads to and from system memory
are cached. Reads come from cache lines on cache hits; read 
misses cause cache fills. Speculative reads are allowed. Write
misses cause cache line fills (in processor families starting
with the P6 family processors), and writes are performed entirely
in the cache, when possible. Write combining is allowed. The 
write-back memory type reduces bus traffic by eliminating<sup>清除</sup> many
unnecessary writes to system memory. Writes to a cache line are
not immediately forwarded to system memory; instead, they are
accumulated<sup>积累</sup> in the cache. The modified cache lines are written
to system memory later, when a write-back operation is performed.
Write-back operations are triggered when cache lines need to 
be deallocated<sup>解除分配;释放</sup>, such as when new cache lines are being allocated 
in a cache that is already full. They also are triggered by the
mechanisms used to maintain cache consistency. This type of 
cache-control provides the best performance, but it requires 
that all devices that access system memory on the system bus 
be able to snoop memory accesses to ensure system memory and 
cache coherency.

<font color=gray face="黑体" size=2>
同上1,2,3
<ol>
	<li>
	write操作将会导致cache line fill , 并且写操作将会全部在cache中进行。
	</li>
	<li>Write combining 将被允许</li>
	<li>
	write-back memory type 减少了bu通信，通过避免了许多不必要的写入内存操作。
	</li>
	<li>
	写入cache line 后，不会立即写入system memory， 相反的，他们会在
	cache 中积累。
	</li>
	<li>
	Write-back 操作将会在cache line 需要被释放的情况下触发.cache line
	需要释放的情形有: 分配new cache line 但是cache 已经满了;另外还可能
	由于 保持cache一致性出发。
	</li>
	<li>
	此种cache 类型提供了最好的性能，但是需要所有系统总线上的访问系统
	内存的设备可以 snoop memory access来确保系统内存和cache 的一致性
	</li>
</ol>
</font>

* Write protected (WP) — Reads come from cache lines when 
possible, and read misses cause cache fills. Writes are propagated <sup>传播，传送，传导</sup>
to the system bus and cause corresponding<sup>相一致;相应的;对应的</sup> cache lines on all 
processors on the bus to be invalidated. Speculative reads are
allowed. This memory type is available in processor families 
starting from the P6 family processors by programming the MTRRs 
(see Table 11-6).

Table 11-3 shows which of these caching methods are available
in the Pentium, P6 Family, Pentium 4, and Intel Xeon processors. 

<font color=gray face="黑体" size=2>
<ol>
	<li>读操作来自于cache line ,如果read miss 导致 cache line fill</li>
	<li>
	写操作将会传播到系统总线上，并且造成所有处理器上的相应的cache line 都被
	invalidated
	</li>
	<li>
	允许 Speculative read.
	</li>
</ol>
</font>

![Table-11-3](pic/Table-11-3.png)

### 11.3.1 Buffering of Write Combining Memory Locations
Writes to the WC memory type are not cached in the typical
sense<sup>语义</sup> of the word cached. They are retained in an internal 
write combining buffer (WC buffer) that is separate<sup>分离，分开</sup> from 
the internal L1, L2, and L3 caches and the store buffer. 
The WC buffer is not snooped and thus does not provide data 
coherency. Buffering of writes to WC memory is done to allow 
software a small window of time to supply<sup>(supply to)向...提供...</sup> more modified data 
to the WC buffer while remaining as non-intrusive<sup>入侵,侵入</sup> to software
as possible. The buffering of writes to WC memory also causes
data to be collapsed<sup>倒塌,坍塌;崩溃</sup>; that is, multiple writes to the same memory
location will leave the last data written in the location and
the other writes will be lost.

<font color=gray face="黑体" size=2>
Write to WC memory type 不会按照传统语义的缓存 进行缓存（可能缓存
但是缓存的含义和传统含义不一样）。它保留在一个内部的write combining buffer
(WC buffer), 该buffer 和内部的L1, L2 ,以及L3 caches 和store buffer是分开的。
WC buffer不支持snoop因此不提供数据一致性。写入WC buffer的缓冲是为了让软件
有一小段时间窗口用于提供更多的 modified data 给WC buffer，并且尽量向软件保持
no-instrusive。写入WC memory的缓冲也早造成了数据 collapsed. 这指的是，
对同一个内存 location 进行多个write操作将留下最后的 data written 到location中
并且其他的写入将会丢失。
</font>

The size and structure of the WC buffer is not architecturally
defined. For the Intel Core 2 Duo, Intel Atom, Intel Core Duo,
Pentium M, Pentium 4 and Intel Xeon processors; the WC buffer
is made up of several 64-byte WC buffers. For the P6 family 
processors, the WC buffer is made up of several 32-byte WC buffers.

<font color=gray face="黑体" size=2>
WC buffer 的size和structure 不是 architecturally defined.
</font>

When software begins writing to WC memory, the processor begins
filling the WC buffers one at a time. When one or more WC buffers
has been filled, the processor has the option of evicting the 
buffers to system memory. The protocol for evicting the WC buffers
is implementation dependent and should not be relied on by 
software for system memory coherency. When using the WC memory
type, software must be sensitive<sup>需要秘密或慎重处理的</sup> to the fact that the writing 
of data to system memory is being delayed and must
deliberately<sup>有意的，故意的</sup>
empty the WC buffers when system memory coherency is required.

<font color=gray face="黑体" size=2>
当软件开始写入WC memory时， processor 开始一次 fill 一个WC buffer。
当一个或者多个 WC buffer 已经fill时， processor 可以选择将 buffer 
evict 到 system memory。对于 evict WC buffer 的协议是 implementation
dependent 并且对于system memory coherency不应该依赖软件. 当使用WC memory
type 时， software 必须慎重考虑 写入数据到系统内存的延迟的这个事实
并且当系统内存一致性需要时，有意的清空WC buffer 
</font>

Once the processor has started to evict data from the WC buffer
into system memory, it will make a bus-transaction style 
decision<sup>决心，决定，决策</sup> based on how much of the buffer contains valid data. 
If the buffer is full (for example, all bytes are valid), the
processor will execute a burst-write transaction on the bus. 
This results in all 32 bytes (P6 family proces- sors) or 64 
bytes (Pentium 4 and more recent processor) being transmitted
on the data bus in a single burst transaction. If one or more
of the WC buffer’s bytes are invalid (for example, have not 
been written by software), the processor will transmit the data
to memory using “partial write” transactions (one chunk at a 
time, where a “chunk” is 8 bytes).

<font color=gray face="黑体" size=2>
一旦处理器开始 从WC buffer 中evict data 到system memory。
它将会根据buffer中包含了多少 valid data 作出 bus-transaction 
style(样式) 决策。(也就是说不同数量的valid data 产生的bus-transaction
不同). 如果buffer 是满的(例如所有的字节都是 valid), 处理器将会
向总线上发出一个burst-write transaction。这将导致整个32 bytes 
或者64 bytes数据存放在一个单独的burst transaction 传送到 data bus上。
如果WC buffer的一个或多个字节是 invalid(例如, 还没有被软件写入), 
处理器将会将数据以"partial write" transaction的方式传送到 memory。
（一次传送一个chunk ,每个chunk 是8 bytes)
</font>

This will result in a maximum of 4 partial write transactions
(for P6 family processors) or 8 partial write transactions 
(for the Pentium 4 and more recent processors) for one WC 
buffer of data sent to memory.

<font color=gray face="黑体" size=2>
这将会导致对于一个WC buffer 数据发送到memory 的最大值 是4 partial write
transactions或者 8 partial write translation.
</font>

The WC memory type is weakly ordered by definition. Once the 
eviction of a WC buffer has started, the data **is subject to**<sup>受...支配，受... 影响</sup>
the weak ordering semantics of its definition. Ordering is not
maintained between the successive<sup>连续的</sup> allocation/deallocation of
WC buffers (for example, writes to WC buffer 1 followed by 
writes to WC buffer 2 may appear as buffer 2 followed by buffer
1 on the system bus). When a WC buffer is evicted to memory as
partial writes there is no guaranteed ordering between 
successive partial writes (for example, a partial write for 
chunk 2 may appear on the bus before the partial write for 
chunk 1 or **vice versa**<sup>反之亦然</sup>).

<font color=gray face="黑体" size=2>
WC memory type 根据定义是弱排序的。一旦WC buffer eviction 操作开始 ,
data 收其定义的弱排序语义的影响。在WC buffers 连续的allocation/deallocation
不再保持顺序(例如，在写入WC buffer 2 后写入 WC buffer 1 可能会在总线上
呈现为在buffer 1 后面是 buffer 2。 当WC buffer 以partial write
的方式evict 到 memory , 不保障连续的 partial write 顺序（例如, 对于
chunk 1 的 partial write 可能会在 对于chunk 2 的partial write 之前出现在
bus上, 反之亦然。
</font>

The only elements of WC propagation<sup>繁殖;增殖;传播</sup> to the system bus that are
guaranteed are those provided by transaction atomicity. For 
example, with a P6 family processor, a completely full WC buffer
will always be propagated as a single 32-bit burst transaction
using any chunk order. In a WC buffer eviction where data will
be evicted as partials, all data contained in the same chunk 
(0 mod 8 aligned) will be propagated simultaneously<sup>同时</sup>. Likewise<sup>同样的</sup>,
for more recent processors starting with those based on Intel
NetBurst microarchitectures, a full WC buffer will always be 
propagated as a single burst transactions, using any chunk 
order within a transaction. For partial buffer propagations,
all data contained in the same chunk will be propagated 
simultaneously. 

<font color=gray face="黑体" size=2>
WC 传播到 system bus 保障的唯一原则是 提供 transaction atomicity.
例如，对于P6 family processor, 一个 完整的full WC buffer 总是会
以一个单独的, 使用任何顺序的chunk的32-bit burst transaction传播.
在WC buffer eviction 中，数据是 以partial evict ，所有的在相同chunk 中的
数据被同时传播. 对于更多近期的processor (从基于
NetBurst microarchitecture 开始), 一个full WC buffer 会以一个单独的
burst transaction 传播，并且在该transaction中 使用任意顺序的chunk。
对于 partial buffer 传播，在相同chunk中的所有的数据被同时传播。
</font>

### 11.3.2 Choosing a Memory Type
The simplest system memory model does not use memory-mapped 
I/O with read or write **side effects**<sup>负面影响</sup>, does not include a 
frame buffer, and uses the write-back memory type for all 
memory. An I/O agent can perform direct memory access (DMA) 
to write-back memory and the cache protocol maintains cache 
coherency.

<font color=gray face="黑体" size=2>
最简单的system memory 模型 不使用带有read or write 负面影响的
memory-mapped I/O, 不包括 frame buffer, 并且为所有的内存使用
write-back memory type。I/O agent 可以执行 DMA 到 write-back
memory 并且 cache protocol 会维持cache的一致性。
</font>

A system can use strong uncacheable memory for other memory-mapped I/O,
and should always use strong uncacheable memory for memory-mapped
I/O with read side effects.

<font color=gray face="黑体" size=2>
系统可以为其他的memory-mapped I/O 使用 strong uncacheable memory, 并且
对于带有 read副作用的memory-mapped I/O 总是使用strong uncacheable memory.
</font>

Dual-ported memory can be considered a write side effect, making 
relatively<sup>相对的</sup> prompt<sup>准时的;及时的</sup> writes 
desirable<sup>可取的，值得拥有的</sup>, because those writes cannot 
be observed at the other port until they reach the memory agent.
A system can use strong uncacheable, uncacheable, write-through, 
or write-combining memory for frame buffers or dual-ported memory 
that contains pixel values displayed on a screen. Frame buffer 
memory is typically large (a few megabytes) and is usually written
more than it is read by the processor. Using strong uncacheable
memory for a frame buffer generates<sup>产生</sup> very large amounts of bus
traffic, because operations on the entire buffer are implemented
using partial writes rather than line writes. Using write-through
memory for a frame buffer can displace almost all other useful
cached lines in the processor's L2 and L3 caches and L1 data cache.
Therefore, systems should use write-combining memory for frame
buffers whenever possible.

<font color=gray face="黑体" size=2>
Dual-port memory 可以认为是一个 write 副作用的内存，使得写操作
变得相对迅速, 因为这些写操作不能在其他的port 上被发现，直到
他们到达memory agent。system 可以为包含了screen上display的像素值的
或者 dual-ported memory或者frame buffer 使用strong uncacheable, 
uncacheable， write-though, 或者write-combining memory. Frame buffer
memory 很大（几兆B字节) 并且通常写比处理器进行的读操作多些。为frame 
buffer 使用 strong uncacheable 内存会产生大量的bus traffic,
因为对整个buffer 的操作是使用部分写入而不是行写入来实现的。为frame 
buffer使用 write-through memory 会提到处理器L2,L3 cache和L1 data cache
中的其他有用的cache line 。因此, 系统在任何可能的时候都应该为 
frame buffer 使用 write-combining memory。
</font>

Software can use page-level cache control, to assign appropriate
effective memory types when software will not access data structures
**in ways that**<sup>以某种方式</sup> benefit from write-back caching. For example, software
may read a large data structure once and not access the structure
again until the structure is rewritten by another agent. Such a
large data structure should be marked as uncacheable, or reading
it will evict cached lines that the processor will be referencing
again.

<font color=gray face="黑体" size=2>
当software 访问这些 data structure时，不会在从write-back cache中受益，
软件可以使用page-level cache 控制, 来分配合适有效的memory type。
例如software 可能一次性的读取一个大的数据结构并且直到另一个agent
重写这个数据结构之前都不会再去访问。像这样如此大的数据结构
应该被标记为 uncacheable, 否则读取它将 evict 处理器将再次引用
的缓存行。
</font>

A similar example would be a write-only data structure that 
is written to (to export the data to another agent), but never
read by software. Such a structure can be marked as uncacheable,
because software never reads the values that it writes (though
as uncacheable memory, it will be written using partial writes,
while as write-back memory, it will be written using line writes,
which may not occur until the other agent reads the structure 
and triggers implicit write-backs).

<font color=gray face="黑体" size=2>
一个相似的例子为 write-only data structure, 该数据结构回写入，
但是不会被软件读。这样的数据结构可以被标记为不可缓存，因为软件
从不会读其写入的值(虽然作为不可缓存的内存，它将使用部分写入来写入，
而作为回写内存，它将使用行写入来写入，直到其他代理读取结构并触发隐式
write-backs，才可能发生该操作(回写))
</font>

On the Pentium III, Pentium 4, and more recent processors, new
instructions are provided that give software greater control 
over the caching, prefetching, and the write-back characteristics
of data. These instructions allow software to use weakly ordered
or processor ordered memory types to improve processor performance,
but when necessary to force strong ordering on memory reads and/or
writes. They also allow software greater control over the caching
of data. For a description of these instructions and there intended<sup>打算</sup>
use, see Section 11.5.5, “Cache Management Instructions.” 

<font color=gray face="黑体" size=2>
在奔腾 3, 4 和最近更多的处理器，提供了更多的新的指令，用于给软件更好
的控制 caching, prefetching 和回写特性。这些指令允许软件使用弱排序
或者processor ordered memory type 来提升处理器性能，但在必要时强制
对内存写入和读取进行强排序。他们也允许软件更好的对 数据的缓存进行更好的
控制。关于这些指令和预期用途的描述，请参照 11.5.5 “Cache Management
Instructions"
</font>

### 11.3.3 Code Fetches in Uncacheable Memory
Programs may execute code from uncacheable (UC) memory, but the
implications are different from accessing data in UC memory. 
When doing code fetches, the processor never transitions from 
cacheable code to UC code speculatively. It also never 
speculatively fetches branch targets that result in UC code.

<font color=gray face="黑体" size=2>
程序从 uncacheable(UC) 内存中 执行代码，但是 含义不同于访问UC 
memory 中的data。 当执行 code fetch 时，处理器不会推测性的从
cacheable code 转换成 UC code。它也不会推测性的获取获取UC
代码的分支目标（现在有两个分支，一个分支是UC code , 他不会去
预取该分支)
</font>

The processor may fetch the same UC cache line multiple times 
in order to decode an instruction once. It may decode consecutive<sup>连续的</sup>
UC instructions in a cacheline without fetching between each 
instruction. It may also fetch additional cachelines from the
same or a consecutive 4-KByte page in order to decode one 
non-speculative UC instruction (this can be true even when the
instruction is contained fully in one line).

<font color=gray face="黑体" size=2>
处理器可以多次获取相同的 UC 高速缓存行，以便对指令一次完成解码。(?????)
它可以在高速缓存行中解码连续的 UC 指令，而无需在每条指令之
间进行fetch。 它还可以从相同或连续的 4 KB 页中获取额外的cache line
, 以解码一条非推测 UC 指令(即使指令完全包含在一个缓存行中，这也是
正确的)(?????)
</font>

Because of the above and because cacheline sizes may change in
future processors, software should avoid placing memory-mapped
I/O with read side effects in the same page or in a subsequent<sup>后来的随后的</sup>
page used to execute UC code. 

<font color=gray face="黑体" size=2>
因为上面的描述和cache line size 可能在未来的处理器中会改变, 软件
应该避免将具有读副作用的memory-mapped I/O 放置到相同的页面
或者后续的页面中。
<br/>
??????
</font>

## 11.4 CACHE CONTROL PROTOCOL

The following section describes the cache control protocol 
currently defined for the Intel 64 and IA-32 architectures.

<font color=gray face="黑体" size=2>
接下来的章节描述了当前 Intel 64 和IA-32 架构定义的 cache control
protocol
</font>

In the L1 data cache and in the L2/L3 unified caches, the MESI
(modified, exclusive, shared, invalid) cache protocol maintains
consistency with caches of other processors. The L1 data cache
and the L2/L3 unified caches have two MESI status flags per 
cache line. Each line can be marked as being in one of the 
states defined in Table 11-4. In general, the operation of the
MESI protocol is transparent to programs. 

<font color=gray face="黑体" size=2>
在L1 data cache 和 在L2/L3 无差别cache中（不区分data/instruction cache),
MESI cache 协议保持了和其他处理器中的cache的一致性。L1 data cache
和L2/L3 无差别cache 每个cache line 有两个MESI status flags, 每个cache line 
可以被编辑为 Table 11-4 中的某一个status。一般来说，MESI协议对
程序(对软件层)是透明的。
</font>

![Table-11-4](pic/Table-11-4.png)

The L1 instruction cache in P6 family processors implements 
only the “SI” part of the MESI protocol, because the
instruction cache is not writable. The instruction cache 
monitors changes in the data cache to maintain consistency
between the caches when instructions are modified. See Section
11.6, “Self-Modifying Code,” for more information
on the implications of caching instructions.

<font color=gray face="黑体" size=2>
P6 family processor 中的 L1 instruction cache 只实现了MESI协议
中的”SI“部分，因为 instruction cache 是不可写的。instruction
cache 监视器在instruction 改变的的时候，去 改变 data cache 
来维护两个cache 之间的一致性。更多cache instruction 的实现的
相关信息请查看 "11.6" Self-Modifying Code。
</font>

## 11.5 CACHE CONTROL
The Intel 64 and IA-32 architectures provide a variety<sup>多样的</sup> of 
mechanisms for controlling the caching of data and instructions
and for controlling the ordering of reads and writes between 
the processor, the caches, and memory. These mechanisms can be
divided into two groups:

<font color=gray face="黑体" size=2>
Intel 64和IA-32 架构提供了多样的机制用于控制数据和指令缓存并且用于
控制处理器, 缓存和内存之间的读取和写入顺序。这些机制可以分为两组:
</font>

* Cache control registers and bits — The Intel 64 and IA-32 
 architectures define several dedicated<sup>专用的</sup> 
 registers and various<sup>各种, 各个</sup>
 bits within control registers and page- and directory-table 
 entries that control the caching system memory locations in 
 the L1, L2, and L3 caches. These mechanisms control the 
 caching of virtual memory pages and of regions of physical 
 memory.
<br/>
<font color=gray face="黑体" size=2>
Cache control register and bit :<br/>
Intel 64 和IA-32 架构定义了一些专用的寄存器和control register
和 page directory-table entries 中的多个控制 在L1,L2,L3 cache
的缓存system memory location 的 各个bits。这些机制控制 virtual
memory pages 和physical memory region 的缓存行为。
</font>

* Cache control and memory ordering instructions — The Intel 
 64 and IA-32 architectures provide several instructions 
 that control the caching of data, the ordering of memory 
 reads and writes, and the prefetching of data. These 
 instructions allow software to control the caching of specific
 data structures, to control memory coherency for specific 
 locations in memory, and to force strong memory ordering at 
 specific locations in a program.
</br>
<font color=gray face="黑体" size=2>
Cache control and memory ordering **instruction**:
intel 64 和 IA-32 架构提供了一些用于控制 缓存数据，内存读写顺序，
数据预取行为的指令。这些指令允许软件控制特定数据结构，内存中
特定位置的内存一致性，以及强制 在程序中的特定位置memory 
强排序ordering 。
</br>
</font>

The following sections describe these two groups of cache 
control mechanisms.

<font color=gray face="黑体" size=2>
接下来的章节描述缓存控制的这两种机制:
</font>

### 11.5.1 Cache Control Registers and Bits
Figure 11-3 depicts<sup>描绘，描画</sup> cache-control mechanisms in IA-32 
processors. Other than for the matter of memory address 
space, these work the same in Intel 64 processors.

<font color=gray face="黑体" size=2>
Figure 11-3 描述了 IA-32 处理器中的cache-control 机制。除了内存地址空间
的问题，其他与 Intel 64处理器中工作方式相同。
</font>

The Intel 64 and IA-32 architectures provide the following 
cache-control registers and bits for use in enabling or 
restricting caching to various pages or regions in memory:

<font color=gray face="黑体" size=2>
Intel 64 和 IA-32 架构提供了下面的 cache-control 寄存器
和控制位用于 enable 或者限制 各种page 或memory region 的
caching 行为。
</font>

* **CD flag, bit 30 of control register CR0** — Controls caching 
of system memory locations (see Section 2.5, “Control 
Registers”). If the CD flag is clear, caching is enabled for
the whole of system memory, but may be restricted for 
individual<sup>个别的，单独的</sup> pages or regions of memory by other cache-control
mechanisms. When the CD flag is set, caching is restricted in
the processor’s caches (cache hierarchy) for the P6 and more 
recent processor families and prevented for the Pentium processor
(see note below). With the CD flag set, however, the caches 
will still respond<sup>回应</sup> to snoop traffic. Caches should be explicitly
flushed to ensure memory coherency. For highest processor 
performance, both the CD and the NW flags in control register
CR0 should be cleared. Table 11-5 shows the interaction<sup>合作;配和</sup> of 
the CD and NW flags.
<br/>
<font color=gray face="黑体" size=2>
CD flag, CR0 的 30位:<br/>
控制system memory locations 的cache 行为(请看 Section 2.5 "Control
Register")。如果CD flag 是clean，对于整个系统内存，caching 都是
enabled, 但是对于某些单独的page和 内存中的regions 可能会被通过其他的
cache control 机制限制。当CD flag 设置，对于P6 和更新的处理器家族 在 processor
‘caches( cache hierarchy)的缓存行为 被限制，并且对于 Pentium 处理器，
caching 被阻止。但是当CD flag 设置时，caches 仍然会反馈snoop traffic。
Caches 应该显示的被flush 保证内存一致性。为了达到更高的处理器性能，
CR0 控制寄存器中的CD和NW flags都应该被clear。Table 11-5 展示了
CD 和 NW flags的配合使用。
</font>
<br/>
The effect of setting the CD flag is somewhat<sup>有点;稍微</sup> different 
for processor families starting with P6 family than the Pentium
processor (see Table 11-5). To ensure memory coherency after 
the CD flag is set, the caches should be explicitly flushed 
(see Section 11.5.3, “Preventing Caching”). Setting the CD 
flag for the P6 and more recent processor families modifies 
cache line fill and update behavior. Also, setting the CD 
flag on these processors do not force strict ordering of memory
accesses unless the MTRRs are disabled and/or all memory is 
referenced as uncached (see Section 8.2.5, “Strengthening 
or Weakening the Memory-Ordering Model”). 
<br/>
<font color=gray face="黑体" size=2>
在P6 family以及之后的处理器家族中设置CD flags 和 Pentium
processor 有些不同（请查看Table 11-5) 。在设置了CD flags 后，
为了保证内存一致性, cache 应该被显式的flush（请查看Section 11.5.3，
“Preventing Caching")。对于P6 和最近的一些处理器家族，设置了CD flasg
将会改变 cache line fill 和 update 的一些行为。另外，在这些处理器
上设置CD flags 不会强制的 严格的对memory access 进行排序，除非MTRRs 
是disabled 或者 所有的memory 都被引用为uncached (请查看 Section 8.2.5
"Strengthening or Weakening the Memory-Ordering Model”)
</font>

![Figure-11-3](pic/Figure-11-3.png)

<font color=gray face="黑体" size=2>
1. designate:指定
</font>

![Table-11-5](pic/Table-11-5.png)

* **NW flag, bit 29 of control register CR0** — Controls the write
policy for system memory locations (see Section 2.5, “Control
Registers”). If the NW and CD flags are clear, write-back is 
enabled for the whole of system memory, but may be restricted 
for individual pages or regions of memory by other cache-control
mechanisms. Table 11-5 shows how the other combinations<sup>结合,组合</sup> of CD 
and NW flags affects caching.
<br/>
<font color=gray face="黑体" size=2>
NW flags, control register CR0的29 bit:<br/>
控制对于system memory locations 的写策略(请查看Section 2.5 "control
Register"). 如果NW 和 CD flags被清空，write-back 对于整个的系统内存
来说是enabled，但是可能会通过其他的缓存控制机制限制一些单独的页或者
内存中的一些区域。Table 11-5 表明了 CD和NW flags的其他组合怎么影响caching
行为。
</font>

<font color=Blue face="黑体" size=4>
NOTES
</font>
<br/>
<font color=gray face="黑体" size=2>
For the Pentium 4 and Intel Xeon processors, the NW flag
is a don’t care flag; that is, when the CD flag is set, the 
processor uses the no-fill cache mode, regardless of the setting
of the NW flag. For Intel Atom processors, the NW flag is a don’t
care flag; that is, when the CD flag is set, the processor 
disables caching, regardless of the setting of the NW flag.

对于 Pentium 4和Intel Xeon processors, NW flags是一个不需要关心的位
（无意义的位），因为，当CD flags设置的时候，无论NW 为是否设置,
processor都使用no-fill cache mode。对于Intel Atom 处理器，NW flags
也是一个不需要关心的位，因为如果CD flags设置了，不管NW flags是否设置，
processor 都将disables caching。

For the Pentium processor, when the L1 cache is disabled (the
CD and NW flags in control register CR0 are set), external 
snoops are accepted in DP (dual-processor) systems and 
inhibited in uniprocessor systems.

对于 Pentium 处理器而言，当L1 cache 是 disabled ( CR0 中的CD 和NW 
flags都被设置），额外的snoop 在DP （dual-processor) system 中被接受，
在 uniprocessor(单处理器) system 中被 inhibited(抑制)

When snoops are inhibited, address parity is not checked and 
APCHK# is not asserted for a corrupt address; however, when 
snoops are accepted, address parity is checked and APCHK# is 
asserted for corrupt addresses. 

当snoop 被 inhibited, address parity(地址奇偶 ap) 将不会被校验，
并且对于 corrupt address (错误地址) APCHK# 信号也不会 assert;
但是当snoop 被 accepted, AP 将会被检查，并且 对于错误地址 APCHK# 
信号会 assert.
</font>

* **PCD and PWT flags in paging-structure entries** — Control 
the memory type used to access paging structures and pages 
(see Section 4.9, “Paging and Memory Typing”).
<br/>
<font color=gray face="黑体" size=2>
paging-structure entries 中的PCD 和 PWD flags: <br/>
控制访问 paging structures 和 其pages 的 memory type (请看
Section 4.9 "Paging and Memory Type")
</font>

* **PCD and PWT flags in control register CR3** — Control the 
memory type used to access the first paging structure of 
the current paging-structure hierarchy (see Section 4.9, 
“Paging and Memory Typing”). 
<br/>
<font color=gray face="黑体" size=2>
CR3 中的 PCD 和 PWT flags: <br/>
控制用于访问当前 paging-structure 层级中的first paging structure
的memory type (请查看 Section 4.9 "Paging and Memory Typing"
</font>

* **G (global) flag in the page-directory and page-table entries
(introduced to the IA-32 architecture in the P6 family 
processors)** — Controls the flushing of TLB entries for 
individual pages. See Section 4.10, “Caching Translation 
Information,” for more information about this flag.
<br/>
<font color=gray face="黑体" size=2>
page-directory 和 page-table entries中的 G (global) flags (在P6
family processor 中被引进):<br/>
为单独的page 控制其TLB entries 的flush 行为。请看4.10, "Caching
Translation Information"， 了解关于此flag 的更多信息
</font>

* **PGE (page global enable) flag in control register CR4** — 
Enables the establishment of global pages with the G flag. 
See Section 4.10, “Caching Translation Information,” for more
information about this flag. 
<br/>
<font color=gray face="黑体" size=2>
CR4控制寄存器中的PGE (page global enable) flags:<br/>
使在全局建立pages时，带有G flag。请查阅 Section 4.10 "Caching
Translation Information" 了解关于此flags的更多信息
</font>

* **Memory type range registers (MTRRs) (introduced in 
P6 family processors)** — Control the type of 
caching used in specific regions of physical memory. 
Any of the caching types described in Section 11.3, “Methods 
of Caching Available,” can be selected. See Section 11.11, 
“Memory Type Range Registers (MTRRs),” for a detailed 
description of the MTRRs.
<br/>
<font color=gray face="黑体" size=2>
Memory type ranger registers (MTRRs) 在P6 family中被引进:<br/>
控制 在物理内存中指定regions的memory type。所有可以选择的caching type 
在Section 11.3 "Methods of Caching Availble" 描述。在Section 11.11
"Memory Type Range Registers(MTRRs) 详细描述了MTRRs。
</font>

* **Page Attribute Table (PAT) MSR (introduced in the Pentium 
III processor)** — Extends the memory typing capabilities of 
the processor to permit memory types to be assigned on a 
page-by-page basis<sup>基础</sup> (see Section 11.12, “Page Attribute Table 
(PAT)”).
<br/>
<font color=gray face="黑体" size=2>
Page Attribute Table (PAT) MSR (在奔鹏3处理器中引进):<br/>
扩展 processor 的 memory type capabilities 允许分配 page-by-page
基础的memory type。（请查看Section 11.12 "Page Attribute Table (PAT)")
</font>

* **Third-Level Cache Disable flag, bit 6 of the IA32_MISC_ENABLE
MSR (Available only in processors based on Intel NetBurst 
microarchitecture)** — Allows the L3 cache to be disabled 
and enabled, independently of the L1 and L2 caches.
<br/>
<font color=gray face="黑体" size=2>
Third-level Cache Disable flag, IA32_MISC_ENABLE MSR的第6 bits:<br/>
允许L3cache disabled / enabled , 独立于L1 L2 caches.
</font>

* **KEN# and WB/WT# pins (Pentium processor)** — Allow external 
hardware to control the caching method used for specific areas 
of memory. They perform similar (but not identical<sup>完全
相同的;一模一样的</sup>) functions to the MTRRs in the P6 family 
processors.
<br/>
<font color=gray face="黑体" size=2>
KEN# 和 WB/WT# pins （奔腾处理器):<br/>
允许外部硬件控制用于内存特定内存区域的caching method( memory type)。
它的功能和P6 family processors的MTRRs 很相似（但不完全一样)
</font>

* **PCD and PWT pins (Pentium processor)** — These pins (which 
are associated with the PCD and PWT flags in control register
CR3 and in the page-directory and page-table entries) permit 
caching in an external L2 cache to be controlled on a 
page-by-page basis, consistent with the control exercised on 
the L1 cache of these processors. The P6 and more recent 
processor families do not provide these pins because the L2 
cache in internal to the chip package.
<br/>
<font color=gray face="黑体" size=2>
PCD 和PWT pins(奔腾处理器）:<br/>
这些pins(这些pins 和CR3 /page-directory / page-table entries 中的
PCD 和PWT 相关联） 允许在外部的L2 cache 中缓存以page-by-pages为基础
进行控制, 与对这些的L1 缓存执行的控制一致。P6 和更新的处理器系列
不提供这些引脚，因为L2缓存芯片封装在内部。
</font>

### 11.5.2 Precedence of Cache Controls
The cache control flags and MTRRs operate hierarchically for 
restricting caching. That is, if the CD flag is set, caching 
is prevented globally (see Table 11-5). If the CD flag is clear,
the page-level cache control flags and/or the MTRRs can be used
to restrict caching. If there is an overlap<sup>重叠</sup> of page-level and
MTRR caching controls, the mechanism that prevents caching 
has precedence<sup>优先</sup>. For example, if an MTRR makes a region of 
system memory uncacheable, a page-level caching control 
cannot be used to enable caching for a page in that region. The
converse<sup>反过来说</sup> is also true; that is, if a page-level caching control
designates<sup>清楚的标出指出</sup> a page as uncacheable, an MTRR cannot be used to 
make the page cacheable.

<font color=gray face="黑体" size=2>
缓存控制flags 和 MTRRs 分层管理 以限制缓存行为。也就是说，如果设置了
CD flags， caching 会被全面的阻止( 请看Table 11-5).如果清空
CD flags, page-level cache control flag 和MTRRs 可以被用来
限制缓存行为。如果 page-level 和 MTRR 缓存控制发生了重叠，
阻止缓存的行为机制优先。 举个例子，如果 某个MTRRs 表名某系统内存区域
是uncacheable, page-level control 对于该region中的page 不能enable caching。
反过来说也是一样: 如果page-level caching control 指明一个pages 是uncacheable,
MTRR 不能使 page cacheable。
</font>

In cases where there is a overlap in the assignment of the 
write-back and write-through caching policies to a page and 
a region of memory, the write-through policy takes precedence.
The write-combining policy (which can only be assigned through
an MTRR or the PAT) takes precedence over either write-through
or write-back. 

<font color=gray face="黑体" size=2>
在 对一个page 或者memory regions的 write-back 和write-through 
缓存策略分配发生重叠的情况下，write-through 策略优先级更高。
write-combining策略(只能通过MTRR或者PAT分配) 优先级要高于
write-though 和write-back。
</font>

The selection of memory types at the page level
varies depending on whether PAT is being used to select memory
types for pages, as described in the following sections.

<font color=gray face="黑体" size=2>
page level 的 memory types 选择取决于用于PAT是否用来给page选择
memory type， 如下面的章节所述:
</font>

On processors based on Intel NetBurst microarchitecture, the 
third-level cache can be disabled by bit 6 of the 
IA32_MISC_ENABLE MSR. Using IA32_MISC_ENABLE[bit 6] takes 
precedence over the CD flag, MTRRs, and PAT for the L3 cache 
in those processors. That is, when the third-level cache disable
flag is set (cache disabled), the other cache controls have no
affect on the L3 cache; when the flag is clear (enabled), the
cache controls have the same affect on the L3 cache as they 
have on the L1 and L2 caches.

<font color=gray face="黑体" size=2>
在基于 Intel NetBurst microarchitecture的处理器来说，third-level
cache 可以通过 IA32_MISC_ENABLE MSR的第6位 disable。对于这些处理器中
的L3 cache来说，使用 IA32_MISC_ENABLE [bit 6] 比CD flags , 
MTRRs, 以及PAT 优先级要高。也就是说，当third-level cache disable
flags 被设置时( cache disabled) , 其他的cache controls 对L3 cache
不起作用; 当flags 被清空( enabled) ，cache controls 在L3 cache上的
影响和其在L1, L2 cache上的影响一样。
</font>

IA32_MISC_ENABLE[bit 6] is not supported in Intel Core i7 
processors, nor processors based on Intel Core, and Intel 
Atom microarchitectures. 

<font color=gray face="黑体" size=2>
IA32_MISC_ENABLE[bit 6] 在Intel Core i7, Intel core和Intel 
Atom microarchitectures 等处理器中不支持。
</font>

#### 11.5.2.1 Selecting Memory Types for Pentium Pro and Pentium II Processors
The Pentium Pro and Pentium II processors do not support the 
PAT. Here, the effective memory type for a page is selected 
with the MTRRs and the PCD and PWT bits in the page-table or 
page-directory entry for the page. Table 11-6 describes the 
mapping of MTRR memory types and page-level caching attributes
to effective memory types, when normal caching is in effect 
(the CD and NW flags in control register CR0 are clear). 
Combinations that appear in gray are implementation-defined 
for the Pentium Pro and Pentium II processors. System designers
are encouraged<sup>鼓励，支持</sup> to avoid these implementation-defined 
combinations.

<font color=gray face="黑体" size=2>
Pentium Pro 和 Pentium 2 处理器不支持PAT。 这里，对于一个page 的
通过 MTRRs和该page 的 page-table 或者page-directory entry 中的 PCD
和PWT 位去选择 有效的memory type. Table 11-6 描述了 MTRR memory type 
和 page-level caching 属性到有效的 memory type 的映射，在normal cache 
生效的情况下(CR0 中的CD 和 NW flags 是clear状态）。图中的灰颜色的组合
对于Pentium Pro 和 Pentium II 处理器来说是implementation-defined。
系统设计时尽量去避免这些 implementation-defined 组合。
</font>

![Table-11-6](pic/Table-11-6.png)

<font color=blue face="黑体" size=3>
NOTE:
</font>
<br/>
<font color=gray face="黑体" size=2>
1. These effective memory types also apply to the Pentium 4, 
Intel Xeon, and Pentium III processors when the PAT bit is not
used (set to 0) in page-table and page-directory entries.
<br/>
当PAT bit 没有在page-table 和page-directory entries中使用时，(set to 0)
这些有效的memory types 也是用于 Pentium 4, Intel Xeon, 和
Pentium III 处理器。
</font>

When normal caching is in effect, the effective memory type 
shown in Table 11-6 is determined using the following
rules:

<font color=gray face="黑体" size=2>
当normal caching 是生效的状态，memory type 的有效性在Table 11-6
中展示，memory type 的有效性使用下面的规则确定。
</font>

1. If the PCD and PWT attributes for the page are both 0, then
the effective memory type is identical<sup>完全相同的，一模一样的</sup> 
to the MTRR-defined memory type.
2. If the PCD flag is set, then the effective memory type 
is UC.
3. If the PCD flag is clear and the PWT flag is set, the 
effective memory type is WT for the WB memory type and
the MTRR-defined memory type for all other memory types.
4. Setting the PCD and PWT flags to opposite<sup>相反的</sup> values is 
considered model-specific for the WP and WC memory
types and architecturally-defined for the WB, WT, and UC 
memory types.

<font color=gray face="黑体" size=2>
1. 如果 对于page 的 PCD 和 PWT attribute 都是0, 有效
memory type 和 MTRR-defined memory type 保持一致 <br/>
2. 如果设置了PCD flag, 有效memory type 为UC。<br/>
3. 如果PCD flags 是clear 状态，并且设置了PWT flags,
对于WB memory type 来说， 有效memory type 为 WT, 
并且对于其他所有 memory type来说， 有效memory 
type 为 MTRR-defined memory type。<br/>
4. 将 PCD 和 PWT 标志设置为相反的值被认为是针对 WP 和 WC 
内存类型的模型特定和针对 WB、WT 和 UC 内存类型的体系结构
定义的。
(说明了WC, WP 中灰色部分)
</font>

#### 11.5.2.2 Selecting Memory Types for Pentium III and More Recent Processor Families
The Intel Core 2 Duo, Intel Atom, Intel Core Duo, Intel Core 
Solo, Pentium M, Pentium 4, Intel Xeon, and Pentium III 
processors use the PAT to select effective page-level memory 
types. Here, a memory type for a page is selected by the MTRRs
and the value in a PAT entry that is selected with the PAT, 
PCD and PWT bits in a page-table or page- directory entry (see
Section 11.12.3, “Selecting a Memory Type from the PAT”). 
Table 11-7 describes the mapping of MTRR memory types and PAT
entry types to effective memory types, when normal caching is
in effect (the CD and NW flags in control register CR0 are clear). 

<font color=gray face="黑体" size=2>
在Intel Core 2 Duo, Intel Atom, Intel Core Duo, Intel Core Solo,
Pentium M, Pentium 4, Intel Xeon, 和 Pentium III 处理器中，使用PAT
来选择page-level memory type。 这里, 一个page 的memory type 通过MTRRs
和PAT entry中的value选择，而PAT entry value 通过 page-table或者
page-directory entry 中的PCD 和PWT 位选择（请查看 Section 11.12.3 
"Selecting a Memory Type from PAT"). Table 11-7 描述了 当 normal 
caching 是有效时(CR0 中的CD 和 NW 是clear状态) MTRR memory type 和 
PAT entry types 到 有效memory type 的映射。
</font>

![Table-11-7](pic/Table-11-7.png)

![Table-11-7-2](pic/Table-11-7-2.png)

<font color=blue face="黑体" size=4>
NOTES:
</font>

1. The UC attribute comes from the MTRRs and the processors
are not required to snoop their caches since the data could
never have been cached. This attribute is preferred<sup>选择某物，更喜欢某物</sup> for
performance reasons.
2. The UC attribute came from the page-table or page-directory
entry and processors are required to check their caches because
the data may be cached due to page aliasing, which is not 
recommended.
3. These combinations were specified as “undefined” in previous
editions of the Intel® 64 and IA-32 Architectures Software 
Developer’s Manual. However, all processors that support both 
the PAT and the MTRRs determine the effective page-level memory
types for these combinations as given. 

<font color=gray face="黑体" size=2>

1. UC属性来自于MTRRs ，并且处理器不需要snoop 其缓存，因此数据永远
不会被cache. 出于性能原因，该属性是首选的。
2. UC 属性来自于page-table 或者 page-directory entry 并且处理器不需要
去检查其缓存，因为数据可能因为page aliasing被缓存，不推荐这样做。
3. 在之前的spec手册中，这些组合被指定为 "undefined"。但是，所有同时支持
PAT和MTRRs的处理器会根据给定的这些组合确定有效的 page-level memory
type。

</font>

#### 11.5.2.3 Writing Values Across Pages with Different Memory Types 
If two adjoining<sup>相邻的</sup> pages in memory have different memory types,
and a word or longer operand is written to a memory location 
that crosses the page boundary between those two pages, the 
operand might be written to memory twice<sup>两次</sup>. This action does not
present a problem for writes to actual memory; however, if a 
device is mapped the memory space assigned to the pages, the 
device might malfunction<sup>运行不正常，发生故障</sup>. 

<font color=gray face="黑体" size=2>
如果内存中两个相邻的pages有不同的memory type., 并且向一个memory location
中写入一个 word 或者更长字节的数据，并且该memory location 穿过了
两个page的之间的边界（跨page访问), 这个操作可能得分两次写入到内存。
该操作不会对写入实际内存造成问题; 但是如果设备映射到分配给该pages
的内存空间，设备可能运行不正常。
</font>

### 11.5.3 Preventing Caching
To disable the L1, L2, and L3 caches after they have been 
enabled and have received cache fills, perform the following 
steps:

<font color=gray face="黑体" size=2>
在L1, L2, L3 cache 已经enabled 并且已经收到了一些cache fills(已经
fill了一些cache) 之后，想要去 disable L1, L2, L3 caches，执行下面
的步骤:
</font>

1. Enter the no-fill cache mode. (Set the CD flag in control 
register CR0 to 1 and the NW flag to 0.

2. Flush all caches using the WBINVD instruction.

3. Disable the MTRRs and set the default memory type to 
uncached or set all MTRRs for the uncached memory type (see 
the discussion of the discussion of the TYPE field and the 
E flag in Section 11.11.2.1, “IA32_MTRR_DEF_TYPE MSR”).

<font color=gray face="黑体" size=2>

1. 进入 no-fill cache 模式(设置CR0 中的CDflags为1，并且NW
flag 为0。
2. 通过WBINVND指令 flush所有的缓存
3. disable MTRRs 并且设置默认的memory type 成uncached或者
设置所有的 MTRRs 为 uncached memory type (请参阅 Section 11.11.2.1
"IA32_MTRR_DEF_TYPE MSR" 中的TYPE field和E flag)

</font>

The caches must be flushed (step 2) after the CD flag is set 
to ensure system memory coherency. If the caches are not 
flushed, cache hits on reads will still occur and data will 
be read from valid cache lines.

<font color=gray face="黑体" size=2>
在CD flags被设置后为了保证系统内存一致性 caches 必须被flush。
如果cache 没有被flush, 在read时，仍然会发生cache hits 并且
数据也将从valid cache line中读取。
</font>

The intent<sup>目的</sup> of the three separate<sup>独立的; 分别的</sup> 
steps listed above address <sup>处理...问题</sup> three distinct<sup>不同的</sup>
requirements: (i) discontinue<sup>停止</sup> new data 
replacing existing data in the cache (ii) ensure data already
in the cache are evicted to memory, (iii) ensure subsequent<sup>随后的</sup>
memory references observe<sup>遵守</sup> UC memory type semantics. Different
processor implementation of caching control hardware may allow
some variation<sup>变化</sup> of software implementation of these three 
requirements. See note below. 

<font color=gray face="黑体" size=2>
上面列出的这些独立的步骤 的目的是解决三个不同的要求:

1. 停止新数据替换现有cache中的数据
2. 保证现有cache中的数据会被 evict 到 内存中
3. 保证随后的内存引用遵守UC memory type 语义。

cache control hardware 的不同处理器实现可能允许这
三个要求的软件实现有一些变化。请看下面的Note
</font>

<font color=blue face="黑体" size=4>
NOTES:
</font>

Setting the CD flag in control register CR0 modifies the 
processor’s caching behavior as indicated in Table 11-5, 
but setting the CD flag alone may not be sufficient<sup>足够; 充足</sup> across 
all processor families to force the effective memory type 
for all physical memory to be UC nor does it force strict 
memory ordering, due to hardware implementation variations 
across different processor families. To force the UC memory 
type and strict memory ordering on all of physical memory, 
it is sufficient to either program the MTRRs for all physical
memory to be UC memory type or disable all MTRRs. 

<font color=gray face="黑体" size=2>
设置控制寄存器CR0中的CD flags 修改了处理器中的caching 行为像
Table 11-5描述的那样, 但是由于硬件在不同处理器家族中实现不同，
单独设置CD flags不足以让所有的处理器家族去强制其所有物理内存
的有效memory type为 UC , 也不会强制严格的内存排序。为了强制UC
memory type 并且在所有的物理内存都严格的memory ordering，
将所有物理内存的MTRR编程为 UC memory type 或者disable所有的
MTRRs 就可以满足上面的要求。
</font>

For the Pentium 4 and Intel Xeon processors, after the sequence of 
steps given above has been executed, the cache lines containing
the code between the end of the WBINVD instruction and before
the MTRRS have actually been disabled may be retained<sup>保留; 保存</sup> in the 
cache hierarchy. Here, to remove code from the cache completely,
a second WBINVD instruction must be executed after the MTRRs 
have been disabled.

<font color=gray face="黑体" size=2>
对于 Pentium 4 和 Intel Xeon 处理器,在上面给定的步骤顺序执行后,
执行WBINVD 指令后和MTRRs 在实际的被disabled 之前的code 的缓存行仍然会
保留在 cache 层级中。这里，要从cache 层级中移除这些code, 
必须在MTRRs 被disabled 之后 执行第二个 WBINVD 指令
</font>

For Intel Atom processors, setting the CD flag forces all 
physical memory to observe UC semantics (without requiring 
memory type of physical memory to be set explicitly). 
Consequently<sup>因此</sup>, software does not need to issue a second 
WBINVD as some other processor generations might require. 

<font color=gray face="黑体" size=2>
对于Intel Atom 处理器，设置CD flag 会强制所有的物理内存去
遵守UC语义(不需要物理内存的memory type 被显示的设置)。
因此，软件不需要像其他代的CPU 需要的那样去提交第二个 WBINVD指令。
</font>

### 11.5.4 Disabling and Enabling the L3 Cache
On processors based on Intel NetBurst microarchitecture, the
third-level cache can be disabled by bit 6 of the IA32_MISC_ENABLE
MSR. The third-level cache disable flag (bit 6 of the IA32_MISC_ENABLE
MSR) allows the L3 cache to be disabled and enabled, independently
of the L1 and L2 caches. **Prior to**<sup>在...之前</sup> using this control to disable
or enable the L3 cache, software should disable and flush all
the processor caches, as described earlier in Section 11.5.3,
“Preventing Caching,” to prevent of loss of information stored
in the L3 cache. After the L3 cache has been disabled or enabled,
caching for the whole processor can be restored<sup>恢复</sup>. 

<font color=gray face="黑体" size=2>
在基于 NetBurst microarchitecture 处理器上，third-level cache 可以通过
IA32_MISC_ENABLE MSR 中的bit 6 disable。third-level cache disable
flag( IA32_MISC_ENABLE MSR 中的 bit 6)允许 L3 cache 去disable 或enable,
并且该操作独立于L1和L2 cache, 软件应该像前面的章节 11.5.3 "Preventing Caching"
描述的那样去disable 并且 flush 所有的处理器缓存, 来防止L3 cache中存储
的数据丢失。在L3 cache 被disable 或者enable 后，可以恢复整个处理器的缓存。
</font>

Newer Intel 64 processor with L3 do not support IA32_MISC_ENABLE[bit 6], 
the procedure<sup>程序;过程;步骤</sup> described in Section 11.5.3, “Preventing Caching,” 
apply to the entire cache hierarchy.

<font color=gray face="黑体" size=2>
更加新的带有L3 cache 的 Intel 64 processor 不支持 IA32_MISC_ENABLE[bit 6],
Section 11.5.3 "Preventing Caching" 描述的流程适用于整个cache层级。
</font>

### 11.5.5 Cache Management Instructions
The Intel 64 and IA-32 architectures provide several instructions
for managing the L1, L2, and L3 caches. The INVD and WBINVD 
instructions are privileged instructions and operate on the L1,
L2 and L3 caches as a whole. The PREFETCHh, CLFLUSH and CLFLUSHOPT
instructions and the non-temporal move instructions (MOVNTI,
MOVNTQ, MOVNTDQ, MOVNTPS, and MOVNTPD) offer more granular <sup>颗粒;粒状</sup>
control over caching, and are available to all privileged levels.

<font color=gray face="黑体" size=2>
Intel 64 和 IA-32 架构提供了一系列用于管理L1,L2,L3 cache的指令。
INVD 和 WBINVD 指令时特权级指令并且对L1, L2 L3 cache 做一个整体的
操作。PREFETCHh, CLFLUSH 和 CLFLUSHOPT 指令和 non-temporal mov
指令(MOVNTI, MOVNTQ, MOVNTDQ, MOVNTPS以及MOVNTPD) 对cache提供了
更精细的控制，并且对提供给所有的特权级使用。
</font>

The INVD and WBINVD instructions are used to invalidate the 
contents<sup>内容</sup> of the L1, L2, and L3 caches. The INVD instruction 
invalidates all internal cache entries, then generates a 
special-function bus cycle that indicates that external caches
also should be invalidated. The INVD instruction should be 
used with care. It does not force a write-back of modified 
cache lines; therefore, data stored in the caches and not 
written back to system memory will be lost. Unless there is 
a specific requirement or benefit to invalidating the caches
without writing back the modified lines (such as, during testing
or fault recovery where cache coherency with main memory is 
not a concern), software should use the WBINVD instruction.

<font color=gray face="黑体" size=2>
INVD和WBINVD指令用于无效 L1,L2,L3 cache内容。INVD指令无效所有的 internal
cache entries, 然后生成一个 special-function bus cycle, 该bus
cycle 表明外部的cache 也应该被 invalidate. INVD指令应该被小心使用。
他不会强制回写修改过的缓存行; 因此，数据存储在 caches中并且没有被
会写到系统内存将会丢失。除非在没有write back 修改过的缓存行就invalidate 
cache 有特殊需求，或者收益点(能从该行为中获益)（例如, 处于test 流程
或者 在做一个fault recovery 这里并不关注 main memory 中的cache 一致性),
否则软件应该使用WBINVD 指令。
</font>

The WBINVD instruction first writes back any modified lines 
in all the internal caches, then invalidates the contents of
both the L1, L2, and L3 caches. It ensures that cache coherency
with main memory is maintained regardless of the write policy
in effect (that is, write-through or write-back). Following 
this operation, the WBINVD instruction generates one (P6 family
processors) or two (Pentium and Intel486 processors) 
special-function bus cycles to indicate to external cache 
controllers that write-back of modified data followed by 
invalidation of external caches should occur. The amount of 
time or cycles for WBINVD to complete will vary due to the size
of different cache hierarchies and other factors. **As a 
consequence**<sup>因此</sup>, the use of the WBINVD instruction can have an impact
on interrupt/event response time.

<font color=gray face="黑体" size=2>
WBINVD 指令首先会write back internal cache 中的任何modified line,
然后无效L1,L2,L3 cache中的内容。他会保证 main memory 的一致性,
无论write policy 是否生效(这里指的是， write-through 或write-back).
在这个操作之后，WBINVD指令生成一个(P6 family 处理器) 或者两个
(Pentium 或者Intel486 处理器) special-function bus cycle 来指示
外部 cache controllers write-back modified data, 然后使外部缓存
失效。对于WBINVD 完成的time 或者cycle将会不同，由于 不同cache
层级的size 或者其他影响。因此，使用 WBINVD指令可以对 interrupt/event
回应产生影响。(可能会造成延迟)
</font>

The PREFETCHh instructions allow a program to suggest to the
processor that a cache line from a specified location in system
memory be prefetched into the cache hierarchy (see Section 11.8,
“Explicit Caching”). 

<font color=gray face="黑体" size=2>
PREFETCHh 指令允许程序 建议 来自系统内存中指定位置的cache line 可以
被预取到 cache 层级中。（请查看11.8 "Explicit caching").
</font>

The CLFLUSH and CLFLUSHOPT instructions allow selected cache
lines to be flushed from memory. These instructions give a 
program the ability to explicitly free up cache space, when 
it is known that cached section of system memory will not be
accessed in the near future.

<font color=gray face="黑体" size=2>
CLFLUSH 和CLFLUSHOPT 指令允许从内存中 选择要刷新的cache line。
这些指令提供给软件一个能力(方法) 在当他们知道 系统内存中的 
cached section 在近期不会在访问的时候, 去显式的free cache 空间，
</font>

The non-temporal move instructions (MOVNTI, MOVNTQ, MOVNTDQ,
MOVNTPS, and MOVNTPD) allow data to be moved from the 
processor’s registers directly into system memory without being
also written into the L1, L2, and/or L3 caches. These 
instructions can be used to prevent cache pollution<sup>污染</sup> when 
operating on data that is going to be modified only once before
being stored back into system memory. These instructions operate
on data in the general-purpose<sup>通用目的; purpose:目的</sup>, 
MMX, and XMM registers. 

<font color=gray face="黑体" size=2>
non-temporal mov instruction(MOVNTI, MOVNTQ, MOVNTDQ,
MOVNTPS,和 MOVNTPD) 允许数据数据从处理器中的寄存器直接移动到
系统内存，并且不会写入到 L1, l2 和/或 L3 caches。这些指令用于
防止cache污染，操作的始于在存储到system memory 之前，只会修改一次。
这些指令操作的对象可以是 general-purpose（通用目的)， MMX和
XMM等寄存器。
</font>

### 11.5.6 L1 Data Cache Context Mode
L1 data cache context mode is a feature of processors based 
on the Intel NetBurst microarchitecture that support
Intel Hyper-Threading Technology. When CPUID.1:ECX[bit 10] = 1,
the processor supports setting L1 data cache context mode 
using the L1 data cache context mode flag ( IA32_MISC_ENABLE[bit 24] ). 
Selectable modes are adaptive mode (default) and shared mode.

<font color=gray face="黑体" size=2>
L1 data cache context mode是基于 Intel NetBurst microarchitecture
处理器的一个 feature, 该处理器支持 Intel Hyper-Threading Technology。
当 CPUID.1:ECX[bit 10] = 1, 处理器支持通过使用L1 data cache context
mode flag( IA32_MISC_ENABLE[bit 24]) 设置 L1 data cache context mode.
可选择的mode为 adaptive(default) 和 shared mode.
</font>

The BIOS is responsible for configuring the L1 data cache 
context mode.

<font color=gray face="黑体" size=2>
BIOS 负责配置L1 data cache context mode.
</font>

#### 11.5.6.1 Adaptive Mode
Adaptive mode facilitates<sup>帮助；促进</sup> L1 data cache sharing between logical
processors. When running in adaptive mode, the L1 data cache
is shared across logical processors in the same core if:

<font color=gray face="黑体" size=2>
Adaptive mode 帮助L1 data cache 在逻辑处理器中共享。当运行于
adaptive mode, L1 data cache 在同一个core 中的逻辑处理器共享
的条件为:
</font>

* CR3 control registers for logical processors sharing the 
cache are identical.
<br/>
<font color=gray face="黑体" size=2>
对于sharing cache 的逻辑处理器中的CR3 control register 是一样的。
</font>
* The same paging mode is used by logical processors sharing the cache.
<br/>
<font color=gray face="黑体" size=2>
sharing cache 的逻辑处理器使用相同的paging mode.
</font>

In this situation, the entire L1 data cache is available to 
each logical processor (instead of being competitively<sup>有竞争力的</sup>
shared).

<font color=gray face="黑体" size=2>
对于这种情况，每个逻辑处理器都可以使用整个的L1 data cache.
(而不是竞争性的分享)
</font>

If CR3 values are different for the logical processors sharing
an L1 data cache or the logical processors use different paging
modes, processors compete<sup>竞争</sup> for cache resources. This reduces the
effective size of the cache for each logical processor. Aliasing
of the cache is not allowed (which prevents data thrashing
<sup>多次反复; 屡次反复</sup>).

<font color=gray face="黑体" size=2>
如果sharing L1 data cache 的logical processor 使用不同的CR3 value 或者
使用不同的paging modes, 处理器将竞争 cache 资源。这将减少
每一个逻辑处理器的 有效size。不允许缓存别名(防止数据抖动)
</font>

#### 11.5.6.2 Shared Mode
In shared mode, the L1 data cache is competitively shared 
between logical processors. This is true even if the logical
processors use identical CR3 registers and paging modes.

<font color=gray face="黑体" size=2>
在share mode 下, L1 data cache 在逻处理器中是竞争性共享。即使
逻辑处理器使用相同的CR3 寄存器和 paging modes。
</font>

In shared mode, linear addresses in the L1 data cache can be 
aliased, meaning that one linear address in the cache can 
point to different physical locations. The mechanism for 
resolving aliasing can lead to thrashing. For this reason, 
IA32_MISC_ENABLE[bit 24] = 0 is the preferred configuration 
for processors based on the Intel NetBurst microarchitecture
that support Intel Hyper-Threading Technology. 

<font color=gray face="黑体" size=2>
在shared mode中，L1 data cache 中的线性地址可以被别名，
意味着cache中的一个线性地址可以指向不同的physical location.
对于解析别名这个机制可能会带来抖动。因为此原因，对于基于
Intel NetBurst microarchitecture并支持 Intel Hyper-Threading
Technology 的处理器来说 IA32_MISC_ENABLE[bit 24] = 0 是配置首选。
</font>

## 11.6 SELF-MODIFYING CODE
A write to a memory location in a code segment that is currently
cached in the processor causes the associated cache line (or lines)
to be invalidated. This check is based on the physical address
of the instruction. In addition, the P6 family and Pentium processors
check whether a write to a code segment may modify an instruction
that has been prefetched for execution. If the write affects a
prefetched instruction, the prefetch queue is invalidated. This
latter<sup>后期的; 末期的;后面的</sup> 
check is based on the linear address of the instruction.
For the Pentium 4 and Intel Xeon processors, a write or a snoop
of an instruction in a code segment, where the target instruction
is already decoded and resident<sup>定居;常驻;驻留</sup> in the trace cache, invalidates
the entire trace cache. The latter behavior means that programs
that self-modify code can cause severe<sup>严格的;严重;剧烈
</sup> degradation<sup>降低</sup> of performance
when run on the Pentium 4 and Intel Xeon processors.

<font color=gray face="黑体" size=2>
对当前cache存在的处理器中的代码段中的memory location 的写操作会导致
相关的cache line (或者lines) 无效。该检查基于指令的物理地址。另外,
P6 family 和 Pentium 处理器检查对代码段的写入是否会修改已经执行预取
的指令。如果这个write 操作影响了一个已经预取的指令，预取队列将会被无效。
后面的检查是基于指令的线性地址。对于 Pentium 4和Intel Xeon 处理器，
在代码段中的指令进行write 或者 snoop , 并且该代码段中的指令已经 decoded
和 驻留在了trace cache， 无效整个的 trace cache。后面的行为意味着
编写self-modify code 会严重减低性能，当运行在 Pentium 4 和Intel 
Xeon processors
</font>

**In practice**<sup>实际上</sup>, the check on linear addresses should not create
compatibility problems among IA-32 processors. Applications
that include self-modifying code use the same linear address
for modifying and fetching the instruction. Systems software,
such as a debugger, that might possibly modify an instruction
using a different linear address than that used to fetch the
instruction, will execute a serializing operation, such as a
CPUID instruction, before the modified instruction is executed,
which will automatically<sup>自动的</sup> resynchronize the instruction cache
and prefetch queue. (See Section 8.1.3, “Handling Self- and 
Cross-Modifying Code,” for more information about the use of
self-modifying code.)

<font color=gray face="黑体" size=2>
实际上来说，在IA-32的这些处理器之间, 对线性地址的检查不应该新增
一些兼容性的问题。包含self-modifying code 的应用使用相同的线性地址
来修改和预取指令。系统软件，例如调试其，可能会使用和预取指令时不同
线性地址来修改指令,在修改指令执行之前，执行执行一个序列化指令，例如
CPUID指令，这将会自动重新同步指令缓存和预取队列（请查看Section 8.1.3
"Handing Self- and Cross-Modify Code 了解更多和self-modifying code 
使用方法更多信息)
</font>

For Intel486 processors, a write to an instruction in the cache
will modify it in both the cache and memory, but if the instruction
was prefetched before the write, the old version of the instruction
could be the one executed. To prevent the old instruction from
being executed, flush the instruction prefetch unit by coding
a jump instruction immediately after any write that modifies
an instruction. 

<font color=gray face="黑体" size=2>
对于Intel486 处理器来说，对cache中的一个指令进行写操作将会同时
修改cache 和memory， 但是如果一个指令在写之前预取了，执行的指令
可能是旧版本的指令（旧的指令)。为了防止旧的指令被执行, 在执行
任何修改指令的写操作后，通过编程一个 jump 指令来flush 指令预取单元。
</font>

## 11.7 IMPLICIT CACHING (PENTIUM 4, INTEL XEON, AND P6 FAMILY PROCESSORS)
Implicit caching occurs when a memory element is made potentially<sup>潜在的</sup>
cacheable, although the element may never have been accessed 
in the normal von Neumann sequence. Implicit caching occurs on
the P6 and more recent processor families due to aggressive <sup>气盛，激进的</sup>
prefetching, branch prediction, and TLB miss handling. Implicit
caching is an extension of the behavior of existing Intel386,
Intel486, and Pentium processor systems, since software running
on these processor families also has not been able to 
deterministically<sup>确切的</sup> predict the behavior of instruction prefetch.

<font color=gray face="黑体" size=2>
隐式缓存发生在memory element 被潜在的缓存时, 尽管该元素可能从来
没有在正常的 von Neumann(冯诺依曼) 序列中被访问。隐式缓存发生在P6
和更新的传力气家族, 由于激进的 prefetching, branch prediction,
和 TLB miss handling。隐式的caching 在现有的Intel386 , Intel486,
和 Pentium 处理器系统的行为扩展，因为在这些处理器家族上运行的软件
不能确切的预测 指令预取的行为。
</font>

To avoid problems related to implicit caching, the operating 
system must explicitly invalidate the cache when changes are 
made to cacheable data that the cache coherency mechanism does
not automatically handle. This includes writes to dual-ported
or physically aliased memory boards that are not detected<sup>检测，发现</sup> by 
the snooping mechanisms of the processor, and changes to 
page-table entries in memory.

<font color=gray face="黑体" size=2>
为了避免与隐式cache相关的问题，当缓存一致性基质不能自动的处理
的可缓存数据发生更改时，操作系统必须显式的无效该缓存。这些包括写入 dual-port
或者写入 不能被处理器snooping 机制检测到的 physical aliased memory boards
以及对内存中page-table entries 的修改。
</font>

The code in Example 11-1 shows the effect of implicit caching 
on page-table entries. The linear address F000H points to 
physical location B000H (the page-table entry for F000H contains
the value B000H), and the page-table entry for linear address
F000 is PTE_F000. 

<font color=gray face="黑体" size=2>
Example 11-1 中的代码展示了 在page-table entires上的隐式缓存的影响。
线性地址F000H 指向物理位置 B000H (对于F000H 的page-table entry包含了
B000H 值),对于线性地址F000的page-table entry 是 PTE_F000。
</font>

Example 11-1. Effect of Implicit Caching on Page-Table Entries
```
mov EAX, CR3; Invalidate the TLB
mov CR3, EAX; by copying CR3 to itself
mov PTE_F000, A000H; Change F000H to point to A000H
mov EBX, [F000H];
```

Because of speculative execution in the P6 and more recent 
processor families, the last MOV instruction performed would
place the value at physical location B000H into EBX, rather 
than the value at the new physical address A000H. This 
situation is remedied<sup>纠正;矫正</sup> by placing a TLB invalidation between 
the load and the store. 

<font color=gray face="黑体" size=2>
因为在P6和更新处理器家族中的投机执行，最后的MOV 指令执行
可能把 在 physical location B000H 中的值 赋给了EBX， 而不是
新的物理地址A000H处的值。这种情况通过在load和store中间放置一个TLB
无效指令避免该问题。
</font>

## 11.8 EXPLICIT CACHING
The Pentium III processor introduced four new instructions, 
the PREFETCHh instructions, that provide software with 
explicit control over the caching of data. These instructions
provide “hints” to the processor that the data requested by a
PREFETCHh instruction should be read into cache hierarchy now
or as soon as possible, in anticipation<sup>预期</sup> of its use. The 
instructions provide different variations<sup>变化;变异，变种，变体</sup> of the hint that 
allow selection of the cache level into which data will be 
read.

<font color=gray face="黑体" size=2>
Pentium III 处理器引进了4个全新指令, PREFETCHh 指令，这些指令
提供给穿件对数据缓存的显示的控制。这些指令提供给处理器"hints(提示)",
通过 PREFETCHh 指令数据需要现在就要被读到cache 层级中, 或者是
尽可能的快，以供预期使用。这些指令提供了 hint 的不同变体，允许
选择数据被读到的缓存级别。
</font>

The PREFETCHh instructions can help reduce the long latency 
typically associated with reading data from memory and thus 
help prevent processor “stalls.” However, these instructions 
should be used judiciously. Overuse can lead to resource 
conflicts and hence reduce the performance of an application.
Also, these instructions should only be used to prefetch data
from memory; they should not be used to prefetch instructions.
For more detailed information on the proper<sup>适当的</sup> use of the 
prefetch instruction, refer to Chapter 7, “Optimizing Cache 
Usage,” in the Intel® 64 and IA-32 Architectures Optimization
Reference Manual.

<font color=gray face="黑体" size=2>
PREFETCHh 指令 会有助于减少通常于内存读取数据相关的长延迟
并且有助于防止处理器"stalls(停顿)". 但是这些指令应该被谨慎使用。
过度使用会导致资源冲突并且因此减少应用的性能。同时，这些指令
应该使用在预取内存中的data, 而不是使用在预取指令。
关于是当时用预取指令的细节，请参考 Intel(R)64 和IA-32 Architectures
Optimization Reference Manual 中的Chapter 7 " Optimizing Cache Usage" 
</font>

## 11.9 INVALIDATING THE TRANSLATION LOOKASIDE BUFFERS (TLBS)
The processor updates its address translation caches (TLBs) 
transparently to software. Several mechanisms are available,
however, that allow software and hardware to invalidate the 
TLBs either explicitly or as a side effect of another operation.
Most details are given in Section 4.10.4, “Invalidation of 
TLBs and Paging-Structure Caches.” In addition, the following
operations invalidate all TLB entries, irrespective<sup>不顾;不考虑</sup> of the 
setting of the G flag: 

<font color=gray face="黑体" size=2>
处理器更新他的 address transcation caches(TLBs) 对于软件来说
是透明的。但是，有几种机制可用，他们允许软件和硬件显式的或者
作为另一个操作的副作用来使 TLB 无效。在Section 4.10.4 "Invalidation
of TLBs and Paging-Structure Caches" 中给出了更多细节。
另外, 下面的操作会invalidate 所有的TLB entries, 不考虑
设置G flags
</font>

* Asserting or de-asserting the FLUSH# pin.
* (Pentium 4, Intel Xeon, and later processors only.) Writing
to an MTRR (with a WRMSR instruction).
* Writing to control register CR0 to modify the PG or PE flag.
* (Pentium 4, Intel Xeon, and later processors only.) Writing
to control register CR4 to modify the PSE, PGE, or PAE flag.
* Writing to control register CR4 to change the PCIDE flag from 1 to 0.
<br/>
<font color=gray face="黑体" size=2>
* assert 或者 de-assert FLUSH# 引脚
* (只适用 Pentium 4, Intel Xeon 或者更新的处理器) 写入MTRR（使用WRMSR指令)
* 写入CR0 去修改PG 或PE flags 
* (只适用 Pentium 4, Intel Xeon 或者更新的处理器) 写入CR4去修该
PSE,PGE或者PAE flag
* 写入CR4 将PCIDE flags 从1 修改为0
</font>

See Section 4.10, “Caching Translation Information,” for 
additional information about the TLBs.

<font color=gray face="黑体" size=2>
请查看Section 4.10 "Caching Translation Information " 了解关于TLBs
额外的更多信息
</font>

## 11.10 STORE BUFFER
Intel 64 and IA-32 processors temporarily store each write 
(store) to memory in a store buffer. The store buffer improves
processor performance by allowing the processor to continue 
executing instructions without having to wait until a write 
to memory and/or to a cache is complete. It also allows 
writes to be delayed for more efficient<sup></sup> use of memory-access 
bus cycles.

<font color=gray face="黑体" size=2>
Intel 64 和 IA-32 处理器临时在 store buffer 中 store 每一个
memory 写入(store). store buffer 通过 允许处理器在没有写入memory
或者 cache 的操作完成的情况下，不需要去等待 去提升了处理器性能。
它还允许延迟写入，为了更有效的使用内存访问总线周期。
</font>

In general, the existence of the store buffer is transparent
to software, even in systems that use multiple processors. 
The processor ensures that write operations are always **carried
out**<sup>实施，贯彻，完成</sup> in program order. It also ensures that the contents of the
store buffer are always drained<sup>排水;排泄;外流</sup> to memory in the following 
situations: 

<font color=gray face="黑体" size=2>
通常来说，现有的store buffer 对于软件来说是透明的，即使在使用
multiple processor 的系统中。处理器确保始终按照程序执行顺序去
进行写操作。它也保证store buffer 中的内容在下面的情况下，始终
可以更新到 memory中。
</font>

* When an exception or interrupt is generated.
* (P6 and more recent processor families only) When a 
serializing instruction is executed.
* When an I/O instruction is executed.
* When a LOCK operation is performed.
* (P6 and more recent processor families only) When a BINIT 
operation is performed.
* (Pentium III, and more recent processor families only) When
using an SFENCE instruction to order stores.
* (Pentium 4 and more recent processor families only) When 
using an MFENCE instruction to order stores.

<font color=gray face="黑体" size=2>
* 当获取到一个异常/中断
* (只适用于P6 和更新的处理器家族) 当执行序列化指令时
* 当执行I/O instruction
* 当执行LOCK 操作时。
* (只适用于P6 和更新的处理器家族) 当之行BINIT操作时.
* (只适用于Pentium III, and 更新的processor families only)当
使用 SFENCE指令来排序stores时
* (只适用于Pentium III, and 更新的processor families only)当
使用 MFENCE 指令来排序stors时
</font>

The discussion of write ordering in Section 8.2, “Memory 
Ordering,” gives a detailed description of the operation of
the store buffer.

<font color=gray face="黑体" size=2>
关于写入排序的讨论在Section 8.2 "Memory Ordering", 并各处了
关于store buffer 的操作细节的描述。
</font>

## 11.11 MEMORY TYPE RANGE REGISTERS (MTRRS)
The following section pertains<sup>符合;与某事物有关联</sup>
only to the P6 and more recent processor families.

<font color=gray face="黑体" size=2>
接下来的章节只适用于 P6 和更新的处理器家族。
</font>

The memory type range registers (MTRRs) provide a mechanism 
for associating the memory types (see Section 11.3, “Methods 
of Caching Available”) with physical-address ranges in system
memory. They allow the processor to optimize operations for 
different types of memory such as RAM, ROM, frame-buffer memory,
and memory-mapped I/O devices. They also simplify system 
hardware design by eliminating<sup>清除</sup> the memory control pins used 
for this function on earlier IA-32 processors and the 
external logic needed to drive them.

<font color=gray face="黑体" size=2>
MTRRs提供了关联memory types(请查看Section 11.3 "Methods
of Caching Available")和系统内存中物理地址区间的机制。
他允许处理器针对不同类型的内存进行优化操作，这些不同类型
的内存例如 RAM, ROM frame-buffer memory和memory-mapped I/O
device。它也通过清除memory control pins 简化了硬件设计, 这些
pins也用于这个功能并且在更早期的IA-32 processor中使用，并且
驱动它们需要额外的逻辑。
</font>

The MTRR mechanism allows multiple ranges to be defined in 
physical memory, and it defines a set of model-specific 
registers (MSRs) for specifying the type of memory that is 
contained in each range. Table 11-8 shows the memory types 
that can be specified and their properties; Figure 11-4 shows
the mapping of physical memory with MTRRs. See Section 11.3, 
“Methods of Caching Available,” for a more detailed description
of each memory type. 

<font color=gray face="黑体" size=2>
MTRR 机制允许定义多个区间被定义在物理内存中，并且它定义了一系列的
MSRs用于指定每个range 中包含的memory type。Table 11-8 展示了
可以指定的memory type和他们的属性值(encodings); Figure 11-4展示了
物理内存和MTRR之间的映射关系。Section 11.3 "Method of Caching 
Available", 描述了每个memory type的更多细节。
</font>

Following a hardware reset, the P6 and more recent processor
families disable all the fixed and variable MTRRs, which in 
effect makes all of physical memory uncacheable. Initialization
software should then set the MTRRs to a specific, system-defined
memory map. Typically, the BIOS (basic input/output system) 
software configures the MTRRs. The operating system or 
executive is then free to modify the memory map using the 
normal page-level cacheability attributes.

<font color=gray face="黑体" size=2>
随着硬件reset, P6 和更新的处理器家族disable 所有fixed和 variable
MTRRs，这实际上使所有的物理内存都为 uncacheable。初始化软件
应该将MTRR设置成特定的值，system-defined 内存映射。通常BIOS
(basic input/output system) 软件配置MTRRs。操作系统或者执行程序
可以使用 normal page-level cacheability attributes 来自由的修改
memory map。
</font>

In a multiprocessor system using a processor in the P6 family
or a more recent family, each processor MUST use the identical
MTRR memory map so that software will have a consistent view 
of memory. 

<font color=gray face="黑体" size=2>
在多处理器系统中，使用P6 family或更近的处理器家族的处理器，每个
处理器必须使用一致的MTRR memory map, 这样软件可以有一个一致性
的内存视图。
</font>
<br/><br/>
<font color=blue face="黑体" size=3>
NOTE:
</font>

In multiple processor systems, the operating system must 
maintain MTRR consistency between all the processors in the 
system (that is, all processors must use the same MTRR values).
The P6 and more recent processor families provide no hardware
support for maintaining this consistency.

<font color=gray face="黑体" size=2>
在多处理器系统中，操作系统必须在系统中所有处理器之间保持MTRR 
一致性（也就是说，所有处理器必须使用相同的MTRR values). P6 和
更新的处理器家族对保持上述的一致性不提供硬件支持。
</font>
<br/><br/>

![Table-11-8](pic/Table-11-8.png)

<font color=blue face="黑体" size=3>
NOTE:
</font>

* Use of these encodings<sup>编码</sup> results in a general-protection 
exception (#GP).
<br/>
<font color=blue face="黑体" size=3>
使用这些编码将导致 general-protection exception (#GP)。
</font>

![Figure-11-4](pic/Figure-11-4.png)
### 11.11.1 MTRR Feature Identification
The availability of the MTRR feature is model-specific. 
Software can determine if MTRRs are supported on a
processor by executing the CPUID instruction and reading 
the state of the MTRR flag (bit 12) in the feature 
information register (EDX).

<font color=gray face="黑体" size=2>
MTRR feature的可用是model-specific。软件可以通过执行CPUID指令
并且在feature information register (EDX) 中读取MTRR flags(bit 12)
确定该处理器是否支持MTRRs
</font>

If the MTRR flag is set (indicating that the processor implements
MTRRs), additional information about MTRRs can be obtained from
the 64-bit IA32_MTRRCAP MSR (named MTRRcap MSR for the P6 family
processors). The IA32_MTRRCAP MSR is a read-only MSR that can be
read with the RDMSR instruction. Figure 11-5 shows the contents
of the IA32_MTRRCAP MSR. The functions of the flags and field in
this register are as follows:

<font color=gray face="黑体" size=2>
如果MTRR flags被设置了(表明处理器实现了 MTRRs), 关于MTRRs的额外的
信息可以通过64-bit IA32_MTRRCAP MSR获取(对于P6 家族的处理器来说
命名为MTRRcap)。IA32_MTRRCAP MSR 是一个只读的MSR， 该MSR可以通过
RDMSR 指令读取。Figure 11-5展示了 IA32_MTRRCAP MSR 的内容。
该register 中flags 和 field的功能如下:
</font>

* **VCNT (variable range registers count) field, bits 0 through
7** — Indicates the number of variable ranges implemented on 
the processor.
<br/>
<font color=gray face="黑体" size=2>
VCNT:<br/>
指明处理器上实现的 variable ranges 的数量
</font>
<br/>
* **FIX (fixed range registers supported) flag, bit 8** — Fixed 
range MTRRs (IA32_MTRR_FIX64K_00000 through IA32_MTRR_FIX4K_0F8000)
are supported when set; no fixed range registers are supported when
clear.
<br/>
<font color=gray face="黑体" size=2>
Fix flags:<br/>
当设置时，支持Fixed range MTRRs; 当clear时，不支持 fixed range 
registers。
</font>
<br/>
* **WC (write combining) flag, bit 10** — The write-combining (WC)
memory type is supported when set; the WC type is not supported
when clear.
<br/>
<font color=gray face="黑体" size=2>
WC:<br/>
当设置时，支持 WC memory type; 当clear时，不支持 WC type。
</font>
<br/>
* **SMRR (System-Management Range Register) flag, bit 11** — The 
system-management range register (SMRR) interface is supported
when bit 11 is set; the SMRR interface is not supported when 
clear. 
<br/>
<font color=gray face="黑体" size=2>
SMRR:<br/>
当设置bit 11 时，支持 system-management range register (SMRR) 接口;
当clear时，不支持 SMRR interface 。
</font>

Bit 9 and bits 12 through 63 in the IA32_MTRRCAP MSR 
are reserved. If software attempts to write to the IA32_MTRRCAP
MSR, a general-protection exception (#GP) is generated.

<font color=gray face="黑体" size=2>
IA32_MTRRCAP MSR 中的 Bit 9 以及 Bit 12 到 Bit 32是被保留的。
如果软件尝试对IA32_MTRRCAP MSR 进行写操作的话(因为该寄存器是一个
只读寄存器，会生成一个 general-protection exception (#GP)。
</font>

Software must read IA32_MTRRCAP VCNT field to determine the 
number of variable MTRRs and query<sup>查询</sup> other feature bits in 
IA32_MTRRCAP to determine additional capabilities that are 
supported in a processor. For example, some processors may 
report a value of ‘8’ in the VCNT field, other processors may
report VCNT with different values. 

<font color=gray face="黑体" size=2>
软件必须读取 IA32_MTRRCAP VCNT 字段来确定 variable MTRRs 的
数量并查询其他的 IA32_MTRRCAP 中的 其他 feature bits 以确定
该处理器支持的额外的 capabilities。例如, 某些寄存器可能在
VCNT 字段中报告了8 这个值，而其他处理器则报告了VCNT 为其他
值。
</font>

![Figure-11-5](pic/Figure-11-5.png)

### 11.11.2 Setting Memory Ranges with MTRRs
The memory ranges and the types of memory specified in each 
range are set by three groups of registers: the IA32_MTRR_DEF_TYPE
MSR, the fixed-range MTRRs, and the variable range MTRRs. 
These registers can be read and written to using the RDMSR and
WRMSR instructions, respectively. The IA32_MTRRCAP MSR indicates
the availability of these registers on the processor (see Section
11.11.1, “MTRR Feature Identification”).
<br/>
<font color=gray face="黑体" size=2>
memory ranges和指定在每个range中的memory type 可以有三组寄存器设置:
IA32_MTRR_DEF_TYPE MSR, fixed-range MTRRs和 variable range MTRRs。
这些寄存器可以通过RDMSR 和 WRMSR指令分别进行读和写操作。IA32_MTRRCAP
MSR指示处理器上的 这些register 的可用性(请查看Section 11.11.1 "MTRR
Feature Identification”)。
</font>

#### 11.11.2.1 IA32_MTRR_DEF_TYPE MSR
The IA32_MTRR_DEF_TYPE MSR (named MTRRdefType MSR for the P6 
family processors) sets the default properties of the regions
of physical memory that are not encompassed<sup>包含</sup> by MTRRs. The 
functions of the flags and field in this register are as follows:

<font color=gray face="黑体" size=2>
IA32_MTRR_DEF_TYPE MSR（在P6 家族处理器中命名MTRRdefType) 设置 MTRRs
没有包含的物理内存区间的默认属性。在这个register 中的flags 和
field 的龚恩嗯如下所述:
</font>

* **Type field, bits 0 through 7** — Indicates the default memory
type used for those physical memory address ranges that do not
have a memory type specified for them by an MTRR (see Table 
11-8 for the encoding of this field). The legal<sup>合法的;
法律许可的</sup> values for this field are 0, 1, 4, 5, 
and 6. All other values result in a general-protection 
exception (#GP) being generated.
<br/>
<font color=gray face="黑体" size=2>
Type field, bits 0 through 7 :<br/>
对于那些没有通过MTRR 指定 的memory type的 内存区间 , 指定默
认的 memory type。(查看Table 11-8 了解这个字段的编码)。
对于这些字段合法的值有0,1,4,5和6。所有其他的值将导致 产生general-
protection exception(#GP)。
</font>
<br/>
Intel recommends<sup>推荐</sup> the use of the UC (uncached) memory type for
all physical memory addresses where memory does not exist. 
To assign the UC type to nonexistent memory locations, it can
either be specified as the default type in the Type field or 
be explicitly assigned with the fixed and variable MTRRs.
<br/>
<font color=gray face="黑体" size=2>
Intel 推荐 对所有不存在的物理内存地址使用 UC(uncached) 
内存类型。为了给这些不存在的 memory location分配UC 类型，
可以通过指定 Type field 中的 default type 指定或者显式的
通过fixed 和 variable MTRRs 分配。
</font>

![Figure-11-6](pic/Figure-11-6.png)

* **FE (fixed MTRRs enabled) flag, bit 10** — Fixed-range MTRRs 
are enabled when set; fixed-range MTRRs are disabled when 
clear. When the fixed-range MTRRs are enabled, they take 
priority over the variable-range MTRRs when overlaps in ranges
occur. If the fixed-range MTRRs are disabled, the variable-range 
MTRRs can still be used and can map the range ordinarily <sup>正常的;通常的</sup>
covered by the fixed-range MTRRs.
<br/>
<font color=gray face="黑体" size=2>
FE:<br/>
当设置时，enable fixed-range MTRRs; 当clear 时，fixed-range MTRRs
disable。当 enable fixed-range MTRRs，如果variable 和 fixed range 
发生了重叠, fixed-range 的优先级超过了 variable-range MTRRs 。如果
fixed-range MTRRs 被disabled, variable-range MTRRs仍然可用并且
可以映射正常情况下由 fixed-range MTRRs 覆盖的range.
</font>
* **E (MTRRs enabled) flag, bit 11** — MTRRs are enabled when set;
all MTRRs are disabled when clear, and the UC memory type is
applied to all of physical memory. When this flag is set, 
the FE flag can disable the fixed-range MTRRs; when the flag
is clear, the FE flag has no affect. When the E flag is set,
the type specified in the default memory type field is used 
for areas of memory not already mapped by either a fixed or 
variable MTRR. 
<br/>
<font color=gray face="黑体" size=2>
E: <br/>
当设置时，MTRRs enable, 当clear, 所有的 MTRRs disable，并且
所有的物理内存都是 UC memory type。当该设置flags, FE flags可以
disable fixed-range MTRRs;当flags 被clear时，FE flags则不会
生效（flags 值没有意义)。当设置E flags, 指定在 default memory
type 中的 type 用于还没有被fixed或者 variable MTRR 映射的内存
区域。
</font>

Bits 8 and 9, and bits 12 through 63, in the IA32_MTRR_DEF_TYPE
MSR are reserved; the processor generates a general-protection
exception (#GP) if software attempts to write nonzero values 
to them.

<font color=gray face="黑体" size=2>
IA32_MTRR_DEF_TYPE MSR中的Bits 8 和 9, 和 bit2 到 bit63是reserved;
当软件尝试对他们写入一个非0值时，处理器会生成一个 general-protection
exception。
</font>

#### 11.11.2.2 Fixed Range MTRRs
The fixed memory ranges are mapped with 11 fixed-range registers
of 64 bits each. Each of these registers is divided into 8-bit
fields that are used to specify the memory type for each of the
sub-ranges the register controls:
<font color=gray face="黑体" size=2>
fixed memory ranges 被11 个 64 bits 的 fixed-range registers 映射。
这些寄存器中的每个都被分割成 8-bits 字段，这些字段用于指定该register
控制的每个sub-ranges。
</font>

* **Register IA32_MTRR_FIX64K_00000** — Maps the 512-KByte address
range from 0H to 7FFFFH. This range is divided into eight 64-KByte
sub-ranges.
<br/>
<font color=gray face="黑体" size=2>
IA32_MTRR_FIX64K_00000:<br/>
映射 从0H 到 7FFFFH 的512-KByte 地址范围。该范围空间备分割为每个
64-KByte 的 sub-ranges
</font>
<br/>
* **Registers IA32_MTRR_FIX16K_80000 and IA32_MTRR_FIX16K_A0000** —
Maps the two 128-KByte address ranges from 80000H to BFFFFH.
This range is divided into sixteen 16-KByte sub-ranges, 8 
ranges per register.
<br/>
<font color=gray face="黑体" size=2>
IA32_MTRR_FIX16K_80000 和 IA32_MTRR_FIX16K_A0000:<br/>
映射从80000H到BFFFFH 这两个128-KByte 地址范围。该范围空间
被分为16个 16KByte 的sub-ranges, 每个register 有 8个 ranges.
</font>
<br/>
* **Registers IA32_MTRR_FIX4K_C0000 through IA32_MTRR_FIX4K_F8000** —
Maps eight 32-KByte address ranges from C0000H to FFFFFH. This
range is divided into sixty-four 4-KByte sub-ranges, 8 ranges
per register.

<font color=gray face="黑体" size=2>
IA32_MTRR_FIX4K_C0000 到 IA32_MTRR_FIX4K_F8000 :<br/>
映射从 C0000H到 FFFFFH 8 个32-KByte 的地址范围。该range
被分为64个4KByte 的sub-ranges, 每个register 有8个ranges。
</font>

Table 11-9 shows the relationship between the fixed physical-address
ranges and the corresponding fields of the fixed-range MTRRs;Table 
11-8 shows memory type encoding for MTRRs.

<font color=gray face="黑体" size=2>
Table 11-9 展示了 fixed physical-address和fixed-range MTRR 对应字段
之间的关系; Table 11-8 展示了 对于MTRRs 的memmory type 的编码值。
</font>

For the P6 family processors, the prefix for the fixed range 
MTRRs is MTRRfix. 

<font color=gray face="黑体" size=2>
对于P6 family processor来说，fixed range MTRRs 的前缀是 MTRRfix。
</font>

![Table-11-9](pic/Table-11-9.png)

#### 11.11.2.3 Variable Range MTRRs
The Pentium 4, Intel Xeon, and P6 family processors permit 
software to specify the memory type for m variable- size 
address ranges, using a pair of MTRRs for each range. The 
number m of ranges supported is given in bits 7:0 of the 
IA32_MTRRCAP MSR (see Figure 11-5 in Section 11.11.1).

<font color=gray face="黑体" size=2>
Pentium 4, Intel Xeon,和P6 家族处理器允许软件对 m 个variable-size
的address ranges 指定其memory type, 通过对每个range使用一对
MTRRs达成上面的目的。ranges的数量 m 由 IA32_MTRRCAP MSR( 请看
Section 11.11.1 中的Figure 11-5) 的bits 7:0 给定。
</font>

The first entry in each pair (IA32_MTRR_PHYSBASEn) defines the
base address and memory type for the range; the second entry 
(IA32_MTRR_PHYSMASKn) contains a mask used to determine the 
address range. The “n” suffix is in the range 0 through m–1 
and identifies a specific register pair.

<font color=gray face="黑体" size=2>
每个pair中的first entry (IA32_MTRR_PHYBASEn) 定义了对于该range 的
base address和memory type；second entry (IA32_MTRR_PHYMASKn)
包含了一个用于确定 address range 的 mask。"n" 后缀在 [0, m-1]
范围内，并且用于确定一个指定的 register pair。
</font>

For P6 family processors, the prefixes for these variable range
MTRRs are MTRRphysBase and MTRRphysMask. 

<font color=gray face="黑体" size=2>
对于P6 family 处理器来说，对于这些可变range MTRR 的前缀是
MTRRphyBase和MTRRphysMask。
</font>

Figure 11-7 shows flags and fields in these registers. The 
functions of these flags and fields are:

<font color=gray face="黑体" size=2>
图11-7 展示了 这些 register 中的flags 和field。这些flags 和
field 的功能如下:
</font>

* **Type field, bits 0 through 7** — Specifies the memory type for 
the range (see Table 11-8 for the encoding of this field). 
<br/>
<font color=gray face="黑体" size=2>
Type 字段:<br/>
指定该range 的memory type (对于这些字段的编码，请查看Table 11-8)
</font>
* **PhysBase field, bits 12 through (MAXPHYADDR-1)** — Specifies 
the base address of the address range. This 24-bit value, in 
the case where MAXPHYADDR is 36 bits, is extended by 12 bits 
at the low end to form the base address (this automatically 
aligns the address on a 4-KByte boundary). 
<br/>
<font color=gray face="黑体" size=2>
PhyBase 字段:<br/>
指定了该address range 的base address。此24-bit 值，在
MAXPHYADDR 是36 bits 情况下，通过低端扩展12位生成基地值。
(这会自动的将地址在4-KByte 边界上对齐)
</font>
* PhysMask field, bits 12 through (MAXPHYADDR-1) — Specifies 
a mask (24 bits if the maximum physical address size is 36 bits,
28 bits if the maximum physical address size is 40 bits). The
mask determines the range of the region being mapped, according
<sup>据...所述</sup> to the following relationships:
<br/>
<font color=gray face="黑体" size=2>
PhysMask field:<br/>
指定了一个mask(如果物理地址最大大小为36 bits的话，该mask是24
bits, 如果物理地址最大大小为40 bits的话，该mask 是28 bits)。
根据下面关系所述，mask 确定 region 被映射的范围:
</font>
	+ Address_Within_Range AND PhysMask = PhysBase AND PhysMask
	<br/>
	<font color=gray face="黑体" size=2>
	(例如: PhysBase = fe000, Physical Mask fef00, 那么, 
	Address_Within_Range的值可能为 fe011, fe022, 不能为fe100)
	</font>
	+ This value is extended by 12 bits at the low end to form 
	the mask value. For more information: see Section 11.11.3, 
	“Example Base and Mask Calculations.”
	<br/>
	<font color=gray face="黑体" size=2>
	该值通过低12bits扩展形成mask value。关于更多信息，请查看
	Section 11.11.3 “Example Base and Mask Calculations.”
	</font>
	+ The width of the PhysMask field depends on the maximum 
	physical address size supported by the processor.
	<br/><br/>
	CPUID.80000008H reports the maximum physical address size 	
	supported by the processor. If 	CPUID.80000008H is not 
	available, software may assume that the processor supports 
	a 36-bit physical 	address size (then PhysMask is 24 bits 
	wide and the upper 28 bits of IA32_MTRR_PHYSMASKn are 	
	reserved). See the Note below. 
	<font color=gray face="黑体" size=2>
	CPUID.80000008H报告了处理器支持的 maximum physical address 
	大小。如果 CPUID.80000008H 没有 available, 软件可以假设处理器
	支持36-bits 的物理地址空间大小(也就是说 PhysMask 是24 bits宽，
	而 IA32_MTRR_PHYSMASKn 的高28 bits 是保留的)。请看下面的Note
	部分
	</font>
* **V (valid) flag, bit 11** — Enables the register pair when set;
disables register pair when clear.
<font color=gray face="黑体" size=2>
V flags:<br/>
当设置时， enable register pair, 当clear时，disable register pair。
</font>

![Figure-11-7](pic/Figure-11-7.png)

All other bits in the IA32_MTRR_PHYSBASEn and IA32_MTRR_PHYSMASKn
registers are reserved; the processor generates a general-protection 
exception (#GP) if software attempts to write to them.

<font color=gray face="黑体" size=2>
在 IA32_MTRR_PHYBASEn 和 IA32_MTRR_PHYMASKsn 寄存器中的所有其他 bits
都是保留的，如果软件尝试去写这些位的话， 处理器则会生成一个
general-protection exception(#GP)。
</font>

Some mask values can result in ranges that are not
continuous<sup>继续不停的,不间断的</sup>.
In such ranges, the area not mapped by the mask value is set 
to the default memory type, unless some other MTRR specifies 
a type for that range. Intel does not encourage the use of 
“discontinuous” ranges.

<font color=gray face="黑体" size=2>
某些mask 值可能会造成这些range 是不连续的。在这些ranges中，
未被掩码映射的area 会被设置成默认的memory type,除非一些其他的
MTRR指定了该ranges的type 。Intel 不鼓励（不推荐）使用" discontinue"
ranges.
</font>
<br/>
<font color=blue face="黑体" size=3>
NOTE:
</font>

It is possible for software to parse the memory descriptions 
that BIOS provides by using the ACPI/INT15 e820 interface 
mechanism. This information then can be used to determine how
MTRRs are initialized (for example: allowing the BIOS to define
valid memory ranges and the maximum memory range supported by
the platform, including the processor).

<font color=gray face="黑体" size=2>
通过使用 ACPI/INT15 e820 接口，软件可以解析BIOS 提供的内存描述符。
然后可以使用这个信息去确定如何初始化MTRRs(例如: 允许 BIOS 
去定义平台，包括处理器支持的valid memory ranges 和 maximum 
memory range。
</font>

See Section 11.11.4.1, “MTRR Precedences<sup>t1</sup>,” for information on
overlapping variable MTRR ranges. 

<font color=gray face="黑体" size=2>
t1: precedence: (时间，顺序，行列等上）领先于某人，某事物的权利

请查看Section 11.11.4.1 "MTRR Precedences",了解更多
在 variable MTRR ranges发生覆盖的一些信息。
</font>

#### 11.11.2.4 System-Management Range Register Interface
If IA32_MTRRCAP[bit 11] is set, the processor supports the 
SMRR interface to restrict access to a specified memory 
address range used by system-management mode (SMM) software 
(see Section 31.4.2.1). If the SMRR interface is supported, 
SMM software is strongly encouraged<sup>鼓励;支持</sup> to use it to protect the 
SMI code and data stored by SMI handler in the SMRAM region.
<br/>
<font color=gray face="黑体" size=2>
如果设置了 IA32_MTRRCAP[bit 11] 位，处理器支持SMRR 接口来限制
对用于system-manage mode(SMM) 软件的指定的内存地址范围的访问。
(请查看Section 31.4.2.1)。如果支持SMRR 接口，强烈支持SMM 软件
使用它去保护SMI 代码和使用SMI handler 在SMRAM region中store的
数据。
</font>

The system-management range registers consist of a pair of MSRs
(see Figure 11-8). The IA32_SMRR_PHYSBASE MSR defines the base
address for the SMRAM memory range and the memory type used to
access it in SMM. The IA32_SMRR_PHYSMASK MSR contains a valid
bit and a mask that determines the SMRAM address range protected
by the SMRR interface. These MSRs may be written only in SMM;
an attempt to write them outside of SMM causes a general-protection 
exception. 
<br/>
<font color=gray face="黑体" size=2>
system-management range register 包含了一对MSRs。(see Figure 11-8)。
IA32_SMRR_PHYSBASE MSR 定义了SMRAM memory range 的 base address和
在SMM 下用于访问它的memroy type。IA32_SMRR_PHYSMASK MSR 包含了一个
有效位bit和一个mask, 该mask用于确定有SMRR interface 保护的 SMRAM 
address range。这些MSRs只可以在SMM中被写入; 在SMM 之前尝试对它们
执行写入操作会造成 general-protection exception。
</font>

Figure 11-8 shows flags and fields in these registers. The 
functions of these flags and fields are the following:
<br/>
<font color=gray face="黑体" size=2>
Figure 11-8 展示了这些register 中的flags 和 fields。这些
flags 和 fields 的功能如下:
</font>

* **Type field, bits 0 through 7** — Specifies the memory type 
for the range (see Table 11-8 for the encoding of this field).
<br/>
<font color=gray face="黑体" size=2>
Type field <br/>
指定了range的memery type(请查看Table 11-8 了解该字段的编码)
</font>
* **PhysBase field, bits 12 through 31** — Specifies the base 
address of the address range. The address must be less than 4
GBytes and is automatically aligned on a 4-KByte boundary.
<br/>
<font color=gray face="黑体" size=2>
PhysBase field<br/>
指定了address range 的 base address。该地址必须比4Gbyte 要小
并且在4-KByte 边界上，自动对齐。
</font>
* **PhysMask field, bits 12 through 31** — Specifies a mask that 
determines the range of the region being mapped, according to
the following relationships:
<br/>
<font color=gray face="黑体" size=2>
PhysMask field<br />
指定一个掩码,根据以下关系确定被映射区域的范围:
</font>
	+ Address_Within_Range AND PhysMask = PhysBase AND PhysMask
	+ This value is extended by 12 bits at the low end to form 
	the mask value. For more information: see Section11.11.3, 
	“Example Base and Mask Calculations<sup>计算</sup>.”
	<br/>
	<font color=gray face="黑体" size=2>
	该值通过低12 bits扩展 形成mask value。了解更多信息:请查看
	Section 11.11.3 “Example Base and Mask Calculations.”
	</font>
* **V (valid) flag, bit 11** — Enables the register pair when set;
disables register pair when clear.
<br/>
<font color=gray face="黑体" size=2>
V:<br/>
当设置时，enable register pair, 当clear时，disable register
pair。
</font>

Before attempting to access these SMRR registers, software 
must test bit 11 in the IA32_MTRRCAP register. If SMRR is 
not supported, reads from or writes to registers cause 
general-protection exceptions. When the valid flag in the 
IA32_SMRR_PHYSMASK MSR is 1, accesses to the specified 
address range are treated as follows:
<br/>
<font color=gray face="黑体" size=2>
在尝试访问这些SMRR 寄存器之前，软件必须test(感觉这里像是test
指令的意思)IA32_MTRRCAP 寄存器中的bit 11(SMRR flags)。如果 
SMRR 不支持，对这些寄存器的读取或写入会导致 generic-protection
exceptions。当 IA32_SMRR_PHYSMASK MSR 中的valid flags为1,
对这个指定 地址范围的访问会如下对待:
</font>

* If the logical processor is in SMM, accesses uses the memory
type in the IA32_SMRR_PHYSBASE MSR.
<br/>
<font color=gray face="黑体" size=2>
如果logical processor 在SMM中，使用 IA32_SMRR_PHYSBASE MSR 中
的memory type 进行访问
</font>
* If the logical processor is not in SMM, write accesses are 
ignored and read accesses return a fixed value for each
byte. The uncacheable memory type (UC) is used in this case.
<br/>
<font color=gray face="黑体" size=2>
如果logical processor 没有在SMM中，write访问被忽略，read 
访问对于每个byte 返回一个固定的值。在这种情况下使用 uncacheable
memory type(UC)
</font>

The above items apply even if the address range specified 
overlaps with a range specified by the MTRRs.
<br/>
<font color=gray face="黑体" size=2>
即使 指定的address range 和MTRRs 中指定的range 重叠，上面的
条目也适用。
</font>

![Figure-11-8](pic/Figure-11-8.png)

### 11.11.3 Example Base and Mask Calculations
The examples in this section apply to processors that support
a maximum physical address size of 36 bits. The base and mask
values entered in variable-range MTRR pairs are 24-bit values
that the processor extends to 36-bits.
<br/>
<font color=gray face="黑体" size=2>
该section 中的例子适用于支持 最大 phyiscal address size 为36 bits
的处理器。variable-range MTRRs pair 中的base 和mask 值是24-bit,
并且处理器将其扩展为36-bits
</font>

For example, to enter<sup>输入</sup> a base address of 2 MBytes (200000H) in
the IA32_MTRR_PHYSBASE3 register, the 12 least- significant 
bits are truncated and the value 000200H is entered in the 
PhysBase field. The same operation must be performed on mask 
values. For example, to map the address range from 200000H to
3FFFFFH (2 MBytes to 4 MBytes), a mask value of FFFE00000H is
required. Again, the 12 least-significant bits of this mask 
value are truncated, so that the value entered in the PhysMask
field of IA32_MTRR_PHYSMASK3 is FFFE00H. This mask is chosen 
so that when any address in the 200000H to 3FFFFFH range is 
AND’d with the mask value, it will return the same value as 
when the base address is AND’d with the mask value (which is 
200000H).
<br/>
<font color=gray face="黑体" size=2>
举个例子, 要在 IA32_MTRR_PHYSBASE3 寄存器中输入一个2MByte(200000H)
的基地值，低12有效位需要被 truncate 并且 PhysBase 输入的值为
200H。相同的操作也需要在 mask值上执行。例如, 去map 一个 从
20000H 到3FFFFFH 的address range(2MByte 到 4MByte), 需要FFFE00000H
mask value。同样的，mask value的低12有效bits 被truncate，因此
IA32_MTRR_PHYSMASK3 中的PhysMask 字段输入为FFFE00H。这样选择mask是为了
落在200000H到3FFFFFH范围中任何地址与mask values进行AND(与)操作时,
其返回值和 base address 和mask value AND操作的值相同
</font>

To map the address range from 400000H to 7FFFFFH (4 MBytes to
8 MBytes), a base value of 000400H is entered in the PhysBase
field and a mask value of FFFC00H is entered in the PhysMask 
field. 
<br/>
<font color=gray face="黑体" size=2>
为了map 一个从 400000H到 7FFFFFH的地址范围(4Byte 到 8Byte),
需要向PhysBase字段中输入一个 400H的 base value 并且向PhysMask
字段中输入一个 FFFC00H 的mask value
</font>


<font color=blue face="黑体" size=3>
Example 11-2. Setting-Up Memory for a System
</font>

Here is an example of setting up the MTRRs for an system. 
Assume that the system has the following characteristics:
<br/>
<font color=gray face="黑体" size=2>
这里有一个为一个系统初始化MTRRs的例子。假设系统中有以下特征:
</font>

* 96 MBytes of system memory is mapped as write-back memory 
(WB) for highest system performance.
<br/>
<font color=gray face="黑体" size=2>
系统内存中的96 MByte 被映射为 write-back memory(WB)，为了
获取更高的系统性能。
</font>
* A custom<sup>定制</sup> 4-MByte I/O card is mapped to uncached memory (UC)
at a base address of 64 MBytes. This restriction forces the 
96 MBytes of system memory to be addressed from 0 to 64 MBytes
and from 68 MBytes to 100 MBytes, leaving a 4-MByte hole for 
the I/O card.
<br/>
<font color=gray face="黑体" size=2>
一个定制的 4MByte I/O 卡被映射到了一个base address 为64 MByte的
uncache memory (UC)。此限制强制96MB的系统内存寻址为从0~64 Mbyte
和68~100 MByte, 为I/O card留下了一个4-MByte的空洞。
</font>
* An 8-MByte graphics card is mapped to write-combining memory
(WC) beginning at address A0000000H.
<br/>
<font color=gray face="黑体" size=2>
一个 8-Mbyte graphics card 被map到一个从地址 A0000000H开始的
write-combining memory (WC) 。
</font>
* The BIOS area from 15 MBytes to 16 MBytes is mapped to UC 
memory.
<br/>
<font color=gray face="黑体" size=2>
从15 MByte 到 16 MByte 的BIOS 空间被映射为UC memory。
</font>

The following settings for the MTRRs will yield<sup>产生</sup> 
the proper <sup>适当的</sup> mapping of the physical address 
space for this system configuration.

<font color=gray face="黑体" size=2>
下面对于MTRRs的设置将为系统配置会产生正确的物理地址空间映射。
</font>

```
IA32_MTRR_PHYSBASE0 = 0000 0000 0000 0006H
IA32_MTRR_PHYSMASK0 = 0000 000F FC00 0800H
Caches 0-64 MByte as WB cache type.
IA32_MTRR_PHYSBASE1 = 0000 0000 0400 0006H
IA32_MTRR_PHYSMASK1 = 0000 000F FE00 0800H
Caches 64-96 MByte as WB cache type.
IA32_MTRR_PHYSBASE2 = 0000 0000 0600 0006H
IA32_MTRR_PHYSMASK2 = 0000 000F FFC0 0800H
Caches 96-100 MByte as WB cache type.
IA32_MTRR_PHYSBASE3 = 0000 0000 0400 0000H
IA32_MTRR_PHYSMASK3 = 0000 000F FFC0 0800H
Caches 64-68 MByte as UC cache type.
IA32_MTRR_PHYSBASE4 = 0000 0000 00F0 0000H
IA32_MTRR_PHYSMASK4 = 0000 000F FFF0 0800H
Caches 15-16 MByte as UC cache type.
IA32_MTRR_PHYSBASE5 = 0000 0000 A000 0001H
IA32_MTRR_PHYSMASK5 = 0000 000F FF80 0800H
Caches A0000000-A0800000 as WC type.
```

This MTRR setup uses the ability to overlap any two memory 
ranges (as long as the ranges are mapped to WB and UC memory 
types) to minimize the number of MTRR registers that are 
required to configure the memory environment. This setup 
also fulfills<sup>满足</sup> the requirement that two register pairs are left
for operating system usage.

<font color=gray face="黑体" size=2>
该MTRR 初始化使用了从重叠任意两个memory range的能力（只要
ranges被映射为WB和UC memory type) 来最小化配置内存环境所需的
MTRR 寄存器的数量。该初始化同时也满足了操作系统保留两个register
pairs的需求。
</font>

#### 11.11.3.1 Base and Mask Calculations for Greater-Than 36-bit Physical Address Support
For Intel 64 and IA-32 processors that support greater than 36
bits of physical address size, software should query CPUID.80000008H 
to determine the maximum physical address. See the example.

<font color=gray face="黑体" size=2>
对于支持超过36 bits 物理地址大小的Intel 64 和 IA-32 processor 处理器，
软件应该查询  CPUID.80000008H 来确定物理地址的最大值。请看这个例子
</font>

Example 11-3. Setting-Up Memory for a System with a 40-Bit Address Size

If a processor supports 40-bits of physical address size, then
the PhysMask field (in IA32_MTRR_PHYSMASKn registers) is 28 
bits instead of 24 bits. For this situation, Example 11-2 
should be modified as follows: 

<font color=gray face="黑体" size=2>
如果一个处理器支持40-bits 物理地址大小，其 PhysMask 字段(在 IA32_MTRR_PHYSMASKn
寄存器)是28 bits，而不是24 bites. 在这种情况下，Example 11-2
应该被修改如下:
</font>
```
IA32_MTRR_PHYSBASE0 = 0000 0000 0000 0006H
IA32_MTRR_PHYSMASK0 = 0000 00FF FC00 0800H
Caches 0-64 MByte as WB cache type.
IA32_MTRR_PHYSBASE1 = 0000 0000 0400 0006H
IA32_MTRR_PHYSMASK1 = 0000 00FF FE00 0800H
Caches 64-96 MByte as WB cache type.
IA32_MTRR_PHYSBASE2 = 0000 0000 0600 0006H
IA32_MTRR_PHYSMASK2 = 0000 00FF FFC0 0800H
Caches 96-100 MByte as WB cache type.
IA32_MTRR_PHYSBASE3 = 0000 0000 0400 0000H
IA32_MTRR_PHYSMASK3 = 0000 00FF FFC0 0800H
Caches 64-68 MByte as UC cache type.
IA32_MTRR_PHYSBASE4 = 0000 0000 00F0 0000H
IA32_MTRR_PHYSMASK4 = 0000 00FF FFF0 0800H
Caches 15-16 MByte as UC cache type.
IA32_MTRR_PHYSBASE5 = 0000 0000 A000 0001H
IA32_MTRR_PHYSMASK5 = 0000 00FF FF80 0800H
Caches A0000000-A0800000 as WC type.
```

### 11.11.4 Range Size and Alignment Requirement
A range that is to be mapped to a variable-range MTRR must 
meet the following “power of 2” size and alignment rules:
<br/>
<font color=gray face="黑体" size=2>
要映射到 variable-range MTRR 的 range 必须满足以"2的幂"
大小和对齐规则
</font>

1. The minimum range size is 4 KBytes and the base address 
of the range must be on at least a 4-KByte boundary.
<br/>
<font color=gray face="黑体" size=2>
最小的range size为 4KByte 并且其范围的基址必须在至少4-KByte的
边界上。
</font>
2. For ranges greater than 4 KBytes, each range must be of 
length 2<sup>n</sup> and its base address must be aligned on a 2<sup>n</sup>
boundary, where n is a value equal to or greater than 12. 
The base-address alignment value cannot be less than its 
length. For example, an 8-KByte range cannot be aligned on 
a 4-KByte boundary. It must be aligned on at least an 8-KByte
boundary.
<br/>
<font color=gray face="黑体" size=2>
对于大于4KByte的 range来说，每个range 必须是2的n次幂的长度,
并且他的基地址必须以2的n次幂为边界对齐，这里的n >= 2。
这些基地址 alignment value 不能小于他的长度。例如 8-KByte 的range
不能以4-KByte 边界对齐。它必须以至少8-KByte 的边界对齐。
</font>

#### 11.11.4.1 MTRR Precedences
If the MTRRs are not enabled (by setting the E flag in the 
IA32_MTRR_DEF_TYPE MSR), then all memory accesses are of the 
UC memory type. If the MTRRs are enabled, then the memory 
type used for a memory access is determined as follows:
<br/>
<font color=gray face="黑体" size=2>
如果MTRRs没有被enabled(通过在 IA32_MTRR_DEF_TYPE MSR 设置E flags),
然后所有的内存访问都是UC memory type。如果MTRRs是enabled状态，
用于内存访问的memory type 如下描述确定:
</font>

1. If the physical address falls within the first 1 MByte of 
physical memory and fixed MTRRs are enabled, the processor 
uses the memory type stored for the appropriate fixed-range 
MTRR.
<br/>
<font color=gray face="黑体" size=2>
如果物理地址落在了物理内存的前1M空间，并且fixed MTRRs 是enabled，
处理器使用 stored在合适的 fixed-range MTRR 中的 memory type。
</font>
2. Otherwise, the processor attempts to match the physical 
address with a memory type set by the variable-range MTRRs:
<br/>
<font color=gray face="黑体" size=2>
其他的，处理器尝试匹配物理地址和 由 variable-range MTRRs设置
的memory type
</font>
	+ If one variable memory range matches, the processor uses 
	the memory type stored in the IA32_MTRR_PHYSBASEn register 
	for that range.
	<br/>
	<font color=gray face="黑体" size=2>
	如果匹配了一个 variable memory range, 对于这个range处理器
	使用 store 在 IA32_MTRR_PHYBASEn range中的memory type。
	</font>
	+ If two or more variable memory ranges match and the 
	memory types are identical, then that memory type is 
	used.
	<br/>
	<font color=gray face="黑体" size=2>
	如果两个或多个vaiable memory range 都 匹配到并且其memory
	types 都是一致的, 然后使用该memory type
	</font>
	+ If two or more variable memory ranges match and one of 
	the memory types is UC, the UC memory type used.
	<br/>
	<font color=gray face="黑体" size=2>
	如果两个或多个 variable memory range 都匹配到了，
	其中一个memory type 是UC, 使用 UC memory type。
	</font>
	+ If two or more variable memory ranges match and the 
	memory types are WT and WB, the WT memory type is used.
	<br/>
	<font color=gray face="黑体" size=2>
	如果两个或多个 variable memory range 匹配到，其中memory
	type 是WT 和WB, 使用 WT memory type.
	</font>
	+ For overlaps not defined by the above rules, processor 
	behavior is undefined.
	<br/>
	<font color=gray face="黑体" size=2>
	如果覆盖的情况没有在上面的规则中定义，则处理器的行为是
	undefined
	</font>
3. If no fixed or variable memory range matches, the processor
uses the default memory type.
<br/>
<font color=gray face="黑体" size=2>
如果没有 fixed 或者 variable memory range 匹配到，处理器使用默认
的memory type.
</font>

### 11.11.5 MTRR Initialization
On a hardware reset, the P6 and more recent processors clear 
the valid flags in variable-range MTRRs and clear the E flag 
in the IA32_MTRR_DEF_TYPE MSR to disable all MTRRs. All other
bits in the MTRRs are undefined.
<br/>
<font color=gray face="黑体" size=2>
在处理器reset 时， P6和更新的处理器clear variable-range MTRR中的
valid flags并且清空 IA32_MTRR_DEF_TYPE MSR 中的Eflags，来 disable
所有的MTRRs。 MTRRs中的其他bits 都是 undefined。
</font>

Prior<sup>在...之前</sup> to initializing the MTRRs, software (normally the system
BIOS) must initialize all fixed-range and variable-range MTRR
register fields to 0. Software can then initialize the MTRRs 
according to known types of memory, including memory on devices
that it auto-configures. Initialization is expected to occur 
prior to booting the operating system.
<br/>
<font color=gray face="黑体" size=2>
在初始化MTRRs之前，软件(正常来说是 system BIOS) 必须初始化 fixed-range
和 variable-range MTRR register 中的字段为0。软件之后可以根据内存的已知
类型初始化MTRRs， 包括 auto-configures的 设备上的内存。初始化动作
应在操作系统引导之前进行。
</font>

See Section 11.11.8, “MTRR Considerations in MP Systems,” for
information on initializing MTRRs in MP (multiple- processor)
systems. 
<br/>
<font color=gray face="黑体" size=2>
请查看11.11.8 "MTRRs Considerations in MP Systems", 了解更多
在MP系统上初始化 MTRRs的信息。
</font>

### 11.11.6 Remapping Memory Types
A system designer may re-map memory types to tune<sup>调整</sup> performance
or because a future processor may not imple- ment all memory 
types supported by the Pentium 4, Intel Xeon, and P6 family 
processors. The following rules support coherent memory-type 
re-mappings:
<br/>
<font color=gray face="黑体" size=2>
系统设计人员可能会 re-map memory type 来调整性能，或者因为将来的
处理器可能不会实现 在 Pentium 4, Intel Xeon 和P6 family 处理器
所有支持的 memory types。 下面的规则支持 一致性 memory-type
重新映射。
</font>
1. A memory type should not be mapped into another memory type
that has a weaker memory ordering model. For example, the 
uncacheable type cannot be mapped into any other type, and the
write-back, write-through, and write-protected types cannot be
mapped into the weakly ordered write-combining type.
<br/>
<font color=gray face="黑体" size=2>
memory type 不应该映射到具有弱一致模型的 另一个memory type 中。
例如 uncacheable type 不能映射到另一个 type。 WB, WT 和WP type
不能映射到 弱排序的 write-combining type。
</font>
2. A memory type that does not delay writes should not be mapped
into a memory type that does delay writes, because applications
of such a memory type may rely on its write-through behavior.
Accordingly<sup>因此</sup>, the write- back type cannot be mapped into the 
write-through type.
<br/>
<font color=gray face="黑体" size=2>
一个不能delay write 的memory type 不应该map 在 可能delay write
的memory type 上，因为这样的一个memory type 的应用可能会依赖
它的write-through 行为。因此, write-back type 不能 map 到
write-through type。
</font>
3. A memory type that views write data as not necessarily 
stored and read back by a subsequent read, such as the 
write-protected type, can only be mapped to another type 
with the same behavior (and there are no others for the 
Pentium 4, Intel Xeon, and P6 family processors) or to the 
uncacheable type. 
<br/>
<font color=gray face="黑体" size=2>
一个将写入数据视为 不一定stored并且可以通过连续读取read back的
memory type ,例如write-protected type, 只能影响到具有相同行为
的 memory type ( 在 Pentium4 , Intel Xeon, 和 P6 家族处理器）
没有其他memory type) 或者映射到 uncacheable type。
</font>

In many specific cases, a system designer  can have additional
information about how a memory type is  used, allowing 
additional mappings. For example, write-through memory 
with no associated write side effects can be mapped into 
write-back memory. 
<br/>
<font color=gray face="黑体" size=2>
在许多特定的例子中，系统设计人员可以有更多的额外信息关于memory
type 如何使用，允许额外的 mappings。例如 可以将没有副作用的write-through
memory 映射到 write-back memory。
</font>

### 11.11.7 MTRR Maintenance Programming Interface
The operating system maintains the MTRRs after booting and 
sets up or changes the memory types for memory-mapped devices.
The operating system should provide a driver and application 
programming interface (API) to access and set the MTRRs. The 
function calls MemTypeGet() and MemTypeSet() define this 
interface.
<br/>
<font color=gray face="黑体" size=2>
操作系统在启动后维护 MTRRs并且初始化或者为memory-mapped devices
改变memory type。操作系统应该系统一个driver和一些API 来 访问
并设置这些MTRRs。function call MemTypeGet()和 MemTypeSet()定义
了这个接口。
</font>

#### 11.11.7.1 MemTypeGet() Function
#### 11.11.7.2 MemTypeSet() Function

### 11.11.8 MTRR Considerations in MP Systems
In MP (multiple-processor) systems, the operating systems must
maintain MTRR consistency between all the processors in the 
system. The Pentium 4, Intel Xeon, and P6 family processors 
provide no hardware support to maintain this consistency. 
In general, all processors must have the same MTRR values.
<br/>
<font color=gray face="黑体" size=2>
在MP系统中, 操作系统必须维持系统中所有处理器的MTRRs一致。Pentium
4, Intel Xeon, 和 P6 家族处理器没有提供硬件支持来维持这个一致性
的行为。通常的，这些处理器必须有相同的MTRRs值。
</font>

This requirement implies<sup>意味着</sup> that when the operating system 
initializes an MP system, it must load the MTRRs of the 
boot processor while the E flag in register MTRRdefType is 0.
The operating system then directs<sup>指挥，指示</sup> other processors to load 
their MTRRs with the same memory map. After all the processors
have loaded their MTRRs, the operating system signals them 
to enable their MTRRs. Barrier synchronization is used to 
prevent further memory accesses until all processors indicate
that the MTRRs are enabled. This synchronization **is likely 
to**<sup>很有可能</sup> be a shoot-down style algorithm, with shared variables 
and interprocessor interrupts.
<br/>
<font color=gray face="黑体" size=2>
这个需求意味着当操作系统初始化MP 系统时，他必须将 boot processor
的 MTRRs的 MTRRdefType(IA32_MTRR_DEF_TYPE) 中的 E flags 设置为0。
操作系统之后会只是其他的处理器去将他们的MTRRs  load为相同的 memory
map。 在所有的处理器load 他们的MTRRs完成，操作系统向他们发信号
去enable 他们的MTRRs。Barrier 同步用于防止之后的内存访问，直到
所有的处理器表明 MTRRs已经enabled。 **这个同步很有可能是一个 shoot-down
style 算法，具有共享变量和处理器间中断。**???
</font>

Any change to the value of the MTRRs in an MP system requires
the operating system to repeat the loading and enabling 
process to maintain consistency, using the following procedure: 
<br/>
<font color=gray face="黑体" size=2>
任何对MP系统中的MTRRs值的改变都需要操作系统重复 load和enable
的过程来维持一致性，使用下面的流程:
</font>

1. Broadcast to all processors to execute the following code 
sequence.
2. Disable interrupts.
3. Wait for all processors to reach this point.
4. Enter the no-fill cache mode. (Set the CD flag in control 
register CR0 to 1 and the NW flag to 0.)
5. Flush all caches using the WBINVD instructions. Note on a
processor that supports self-snooping, CPUID feature flag bit
27, this step is unnecessary.
<br/>
<font color=gray face="黑体" size=2>
* 广播到所有的处理器去执行下面的代码序列。
* disable interrputs
* 等待所有处理器到达该点(等待所有处理器完成disable interrupts)
* 进入 no-fill cache mode (设置CR0 CD flags为1, 并设置NW flags为0)
* 使用指令WBINVD flush所有的cache。注意在支持self-snooping处理器上,
 CPUID feature flag bit 27, 该步骤是没有必要的)
</font>
6. If the PGE flag is set in control register CR4, flush all 
TLBs by clearing that flag.
7. If the PGE flag is clear in control register CR4, flush 
all TLBs by executing a MOV from control register CR3 to
another register and then a MOV from that register back to CR3.
8. Disable all range registers (by clearing the E flag in 
register MTRRdefType). If only variable ranges are being 
modified, software may clear the valid bits for the affected
register pairs instead.
<br/>
<font color=gray face="黑体" size=2>
* 如果CR4中的PGE flags设置，通过清空该flags flush 所有的
TLB。
* 如果CR4中的PGE flags已经clear, 通过执行MOV CR3 to OTH register,
MOV OTH register to CR3 来flush所有的TLBs。
* disable 所有的range  registers( 通过clear MTRRdefType 
中的E flags）。如果只修改 variable range, 软件可以受影响寄存器
对的 valid位.
</font>
9. Update the MTRRs.
10. Enable all range registers (by setting the E flag in 
register MTRRdefType). If only variable-range registers were
modified and their individual valid bits were cleared, then 
set the valid bits for the affected ranges instead.
11. Flush all caches and all TLBs a second time. (The TLB 
flush is required for Pentium 4, Intel Xeon, and P6 family
processors. Executing the WBINVD instruction is not needed 
when using Pentium 4, Intel Xeon, and P6 family
processors, but it may be needed in future systems.)
<br/>
<font color=gray face="黑体" size=2>
* 更新MTRRs
* Enable所有的range register(通过设置 MTRRdefType中的E flag)。
如果只有 variable-range register 需要修改并且他们valid bit
都被clear, 通过设置受影响range的 valid bits.
* 再次Flush所有的caches和所有的TLBS。(Pentium 4,Intel Xeon,
P6 family processors需要TLB flush。在Pentium 4, Intel Xeon
和P6 family 处理器上不需要执行 WBINVD指令，但是在未来的系统中
可能需要)。
</font>
12. Enter the normal cache mode to re-enable caching. (Set 
the CD and NW flags in control register CR0 to 0.)
13. Set PGE flag in control register CR4, if cleared in Step 
6 (above).
14. Wait for all processors to reach this point.
15. Enable interrupts.
<br/>
<font color=gray face="黑体" size=2>
* enable normal cache mode来再次enable caching(设置CR0 中
的CD 和 NW flags为0)
* 设置CR4 中的 PGE flags, 如果在前面的步骤六清空了该flags的话。
* 等待所有的处理器执行到该点。
* Enable interrupts
</font>

### 11.11.9 Large Page Size Considerations
The MTRRs provide memory typing for a limited number of 
regions that have a 4 KByte granularity<sup>粒度</sup> (the same granularity
as 4-KByte pages). The memory type for a given page is cached
in the processor’s TLBs. When using large pages (2 MBytes, 
4 MBytes, or 1 GBytes), a single page-table entry covers 
multiple 4-KByte granules, each with a single memory type. 
Because the memory type for a large page is cached in the TLB,
the processor can behave in an undefined manner if a large page
is mapped to a region of memory that MTRRs have mapped with 
multiple memory types.
<br/>
<font color=gray face="黑体" size=2>
MTRRs 为具有4 KByte 粒度的有限大小的region 提供 memory type。(和
4-KByte page 有相同的粒度)。对于一个给定page的memory type 被
cached到了 处理器的 TLBs中。当使用large pages时（2 MByte,
4MByte, 或者1 GBytes), 一个page-table entry 会覆盖多个4-KByte 
颗粒, 每个带有单独的memory type。因为对于一个缓存在TLB中的large 
page 只有一个memory type, 如果large page 被映射到一个内存区间，
MTRR 将该区间映射成多个memory types, 处理器会以未定义的方式运行。
</font>

Undefined behavior can be avoided by insuring that all MTRR 
memory-type ranges within a large page are of the same type. 
If a large page maps to a region of memory containing different
MTRR-defined memory types, the PCD and PWT flags in the 
page-table entry should be set for the most conservative<sup>保守的</sup> memory
type for that range. For example, a large page used for memory
mapped I/O and regular memory is mapped as UC memory. 
Alternatively, the operating system can map the region using 
multiple 4-KByte pages each with its own memory type. 
<br/>
<font color=gray face="黑体" size=2>
未定义的行为可以通过保证 large page 中所有的memory-type range 
都具有相同的值避免。如果large page 映射到一个内存区间包含了不同的
MTRR-defined memory type, page-table entry中的PCD和PWT flags
应该为该range 设置最保守的 memory type。例如, 用于memory mapped
I/O 的large page和常规的内寸被映射为 UC memory 。或者，操作系统
可以使用多个 4 KB页映射该区域，每个页都有自己的内存类型。
</font>

The requirement that all 4-KByte ranges in a large page are of the
same memory type implies that large pages with different memory
types may suffer a performance penalty, since they must be 
marked with the lowest common denominator memory type. The same
consideration apply to 1 GByte pages, each of which may consist
of multiple 2-Mbyte ranges.
<br/>
<font color=gray face="黑体" size=2>
大页面中所有 4 KB 范围都属于相同内存类型的要求意味着具有不同内存
类型的大页面可能会遭受性能损失，因为它们必须使用最低公分母内存类
型进行标记。相同的考虑适用于 1 GByte 页面，每个页面可能包含多个 
2 MB 范围
</font>

The Pentium 4, Intel Xeon, and P6 family processors provide 
special support for the physical memory range from 0 to 4 MBytes,
which is potentially<sup>可能存在,出现的;</sup> 
mapped by both the fixed and variable MTRRs.
This support is invoked when a Pentium 4, Intel Xeon, or P6 family
processor detects a large page overlapping the first 1 MByte of
this memory range with a memory type that conflicts with the 
fixed MTRRs. Here, the processor maps the memory range as multiple
4-KByte pages within the TLB. This operation ensures correct 
behavior at the cost of performance. To avoid this performance
penalty<sup>处罚，刑罚</sup>, operating-system software should reserve the large page
option for regions of memory at addresses greater than or equal
to 4 MBytes. 
<br/>
<font color=gray face="黑体" size=2>
Pentium 4, Intel Xeon, 和 P6 family processors 对于从0~4MByte 的
phyiscal memory range 提供了特殊的支持, 这个内存区间可能被 fixed
和variable MTRRs同时映射。当 Pentium 4 , intel Xeon,或者P6 家族
处理器检测到在这个一个大页内存覆盖了此内存范围的前1MByte, 并且其
memory type 和fixed MTRRs 中的memory type 冲突。这里，处理器将这个
memory ranges 映射为多个带有TLB的4-KByte pages。次操作以性能作为
代价确保正确的行为。为了避免性能处罚，操作系统软件应该为大于等于
4 MByte 的地址所在的 memory regions 保留 large page 选项。
</font>

## 11.12 PAGE ATTRIBUTE TABLE (PAT)
The Page Attribute Table (PAT) extends the IA-32 architecture’s
page-table format to allow memory types to be assigned to 
regions of physical memory based on linear address mappings. 
The PAT is a companion<sup>伙伴;成双成对成套的物品之一</sup>
feature to the MTRRs; that is, the MTRRs
allow mapping of memory types to regions of the physical address
space, where the PAT allows mapping of memory types to pages 
within the linear address space. The MTRRs are useful for 
statically describing memory types for physical ranges, and 
are typically set up by the system BIOS. The PAT extends the 
functions of the PCD and PWT bits in page tables to allow all
five of the memory types that can be assigned with the MTRRs 
(plus one additional memory type) to also be assigned dynamically
to pages of the linear address space.
<br/>
<font color=gray face="黑体" size=2>
PAT 扩展了 IA-32 architecture’s page-table 格式允许给基于线性地址
映射的物理内存regions 分配 memory type。PAT是MTRRs 的配套功能;
也就是说, MTRRs 允许 memory type 映射到 物理地址空间ranges, 而
PAT 允许memory type 映射到线性地址空间pages上。MTRRs对于静态描述
phyiscal range 的memory type 是有用的，并且通常的由 system BIOS
初始化。PAT 扩展了page table 中的PCD和PWT bits允许5个中的所有
memory types, 这些memory types 可以用于MTRRs分配（外加一种额外的
memory type) 也能用于动态的分配给线性地址空间的pages。
</font>

The PAT was introduced to IA-32 architecture on the Pentium III
processor. It is also available in the Pentium 4 and Intel Xeon
processors.
<br/>
<font color=gray face="黑体" size=2>
PAT在 Pentium III 处理器的IA-32 架构上引进。同时在Pentium 4 和
Intel Xeon 处理器上也可以获取该功能。
</font>

### 11.12.1 Detecting Support for the PAT Feature
An operating system or executive can detect the availability 
of the PAT by executing the CPUID instruction with a value of
1 in the EAX register. Support for the PAT is indicated by 
the PAT flag (bit 16 of the values returned to EDX register).
If the PAT is supported, the operating system or executive can
use the IA32_PAT MSR to program the PAT. When memory types have
been assigned to entries in the PAT, software can then use of
the PAT-index bit (PAT) in the page-table and page-directory 
entries along with the PCD and PWT bits to assign memory types
from the PAT to individual pages. 
<br/>
<font color=gray face="黑体" size=2>
操作系统或执行程序可以通过执行CPUID instruction，其中EAX =1 ,
检测PAT 的可用性。通过PAT flags(有EDX返回的值的bit 16)指示PAT
是否支持。如果PAT 支持，操作系统或执行程序可以使用 IA32_PAT MSR
编程PAT, 软件可以使用page-table 和page-directory entries中的 
PAT-index bit(PAT) , 以及entries 中的PCD和 PWT bits 来分配给各自的
内存页分配PAT中的memory  types。
types 
</font>

Note that there is no separate flag or control bit in any of 
the control registers that enables the PAT. The PAT is always
enabled on all processors that support it, and the table lookup
always occurs whenever paging is enabled, in all paging modes.
<br/>
<font color=gray face="黑体" size=2>
注意这里在任何的controls registers 中没有一个单独的flags或者control 
bits enables PAT。 PAT 总是在所有支持它的处理器上enable，并且在所有
分页模式下，只要启用分页，就会始终进行table lookup 。
</font>

### IA32_PAT MSR
The IA32_PAT MSR is located at MSR address 277H (see Chapter 2,
“Model-Specific Registers (MSRs)” in the Intel® 64 and IA-32 
Architectures Software Developer’s Manual, Volume 4). Figure 
11-9. shows the format of the 64-bit IA32_PAT MSR.
<br/>
<font color=gray face="黑体" size=2>
IA32_PAT MSR在MSR address 227H处(请查看Intel sdm Volume 4, 
Chapter 2, "Model-Specific Registers(MSRs)" , Figure 11-9
图示了64-bit IA32_PAT MSR 的格式。
</font>

The IA32_PAT MSR contains eight page attribute fields: PA0 
through PA7. The three low-order bits of each field are
used to specify a memory type. The five high-order bits of 
each field are reserved, and must be set to all 0s. Each
of the eight page attribute fields can contain any of the 
memory type encodings specified in Table 11-10.
<br/>
<font color=gray face="黑体" size=2>
IA32_PAT MSR 包含了8个page attribute 字段: PA0到 PA7。每个字段
的第三位用于指定一个memory type.每个字段的高5位是保留的，
并且必须设置为0。8个page attribute 子读那中的每个可以包含
Table 11-10 中指定的任意编码的memory type。
</font>

![Figure-11-9](pic/Figure-11-9.png)

Note that for the P6 family processors, the IA32_PAT MSR is 
named the PAT MSR.
<br/>
<font color=gray face="黑体" size=2>
注意对于P6 family processor, IA32_PAT MSR 被命名为 PAT MSR。
</font>

![Table-11-10](pic/Table-11-10.png)

### 11.12.3 Selecting a Memory Type from the PAT
To select a memory type for a page from the PAT, a 3-bit index
made up of the PAT, PCD, and PWT bits must be encoded in the 
page-table or page-directory entry for the page. Table 11-11 
shows the possible encodings of the PAT, PCD, and PWT bits and
the PAT entry selected with each encoding. The PAT bit is bit 7
in page-table entries that point to 4-KByte pages and bit 12 
in paging-structure entries that point to larger pages. The PCD
and PWT bits are bits 4 and 3, respectively, in paging-structure
entries that point to pages of any size.
<br/>
<font color=gray face="黑体" size=2>
为了从PAT 所在的page 中选择memory type, 由 PAT, PCD, PWT 位组成的
3-bits的index 必须在page所在的 page-table或者page-directory 中编码。
Table 11-11 展示了 PAT PCD和PWT bits可能的编码以及使用每种比那码选择的
PAT entry。PAT bit在 在指向4-KByte pages的page-table entry 中的 第7位,
在指向更大page的paging-structures entries的第12位。PCD和PWT bits是
在指向任意大小page 的page-structures entries中的第3位和第4位。
</font>

The PAT entry selected for a page is used in conjunction<sup>结合</sup> with
the MTRR setting for the region of physical memory in which 
the page is mapped to determine the effective memory type for
the page, as shown in Table 11-7. 
<br/>
<font color=gray face="黑体" size=2>
为page 选择的PAT entry与 该page 映射到的物理内存所在region 设置的
MTRR结合使用，用来确认对于该page的有效memory type, 如Table 11-7 所示.
</font>

![Table-11-11](pic/Table-11-11.png)

### 11.12.4 Programming the PAT

Table 11-12 shows the default setting for each PAT entry 
following a power up or reset of the processor. The setting
remain unchanged following a soft reset (INIT reset).
<br/>
<font color=gray face="黑体" size=2>
Table 11-12展示了在处理器power up 或者 reset后，每个PAT entry
默认的设置。软复位(INIT reset)后该设置保持不变。
</font>

![Table-11-12](pic/Table-11-12.png)

The values in all the entries of the PAT can be changed by 
writing to the IA32_PAT MSR using the WRMSR instruction. 
The IA32_PAT MSR is read and write accessible (use of the 
RDMSR and WRMSR instructions, respectively) to software 
operating at a CPL of 0. Table 11-10 shows the allowable 
encoding of the entries in the PAT. Attempting to write an 
undefined memory type encoding into the PAT causes a 
general-protection (#GP) exception to be generated.
<br/>
<font color=gray face="黑体" size=2>
素有entries 中的PAT 可以通过使用WRMSR 指令写入IA_PAT MSR
改变其值。IA32_PAT MSR 对于运行在CPL 0上的软件来说，read
和write 操作是有权限访问的。Table 11-10 展示了 PAT的可允许
的编码值。尝试去写一个向PAT中 写入undefined memory type 编码
会导致生成一个GP 异常。
</font>

The operating system is responsible for insuring that changes
to a PAT entry occur **in a manner**<sup>在一定程度上</sup> 
that maintains the consistency
of the processor caches and translation lookaside buffers (TLB).
This is accomplished<sup>完成;做成功</sup> by following the procedure as specified 
in Section 11.11.8, “MTRR Considerations in MP Systems,” for 
changing the value of an MTRR in a multiple processor system.
It requires a specific sequence of operations that includes 
flushing the processors caches and TLBs.
<br/>
<font color=gray face="黑体" size=2>
操作系统负责保证当PAT entry发生了一定程度上的更改时，维护
processor cache 和TLB的一致性。有下面章节11-11-8"MTRR Considerations 
in MP Systems"描述 完成对多处理器中改变MTRRs 值的过程。
它需要一个指定的操作序列，包括flush processors caches 和TLBs。
</font>

The PAT allows any memory type to be specified in the page 
tables, and therefore it is possible to have a single physical
page mapped to two or more different linear addresses, each 
with different memory types. Intel does not support this 
practice<sup>实践</sup> because it may lead to undefined operations that can
result in a system failure. In particular, a WC page must never
be aliased to a cacheable page because WC writes may not check
the processor caches.
<br/>
<font color=gray face="黑体" size=2>
PAT 允许在page tables 中指定任何memory type，因此可能会出现一个
physical page 映射到两个或多个线性地址，每个有不同的memory type。
Intel不支持这样的做法，因为它可能导致undefined operations，这样
可能会导致system failure(系统出错) 。尤其是，WC page 不能对
cacheable page 别名因为WC write 不会检查processor caches。
</font>

When remapping a page that was previously mapped as a cacheable
memory type to a WC page, an operating system can avoid this 
type of aliasing by doing the following: 
<br/>
<font color=gray face="黑体" size=2>
当将以前映射为cacheable memory type 重新映射到WC page时，操作系统
可以通过做下面事情来避免这种别名的类型。
</font>

1. Remove the previous mapping to a cacheable memory type in 
the page tables; that is, make them not present.
2. Flush the TLBs of processors that may have used the mapping,
even speculatively.
3. Create a new mapping to the same physical address with a 
new memory type, for instance, WC.
4. Flush the caches on all processors that may have used the 
mapping previously. Note on processors that support 
self-snooping, CPUID feature flag bit 27, this step is 
unnecessary.
<br/>
<font color=gray face="黑体" size=2>
* 在所在的page tables中, 解除之前的cacheable memory type 映射关系。
也就是说，使他们为 not present。
* flush 用于该映射的的处理器的TLBs，甚至是 speculatively。
* 为相同的physical address 创建一个新的映射，同时带有new memory type,
例如WC
* flush 所有处理器上的caches，该caches用于之前的 mapping。注意在
支持self-snooping 的处理器上 ,CPUID feature flags bit 27, 该步骤是不
必要的
</font>

Operating systems that use a page directory as a page table 
(to map large pages) and enable page size extensions must 
carefully scrutinize<sup>仔细的彻查，检查某事务</sup> the 
use of the PAT index bit for the 4-KByte page-table entries. 
The PAT index bit for a page-table entry (bit 7)
corresponds to the page size bit in a page-directory
entry. Therefore, the operating system can only use PAT entries
PA0 through PA3 when setting the caching type for a page table
that is also used as a page directory. If the operating system
attempts to use PAT entries PA4 through PA7 when using this 
memory as a page table, it effectively sets the PS bit for the
access to this memory as a page directory.
<br/>
<font color=gray face="黑体" size=2>
使用page directory 作为 page table (为了映射更大的page) 并且 enable
了 page size extensions 的操作系统必须仔细的检查 4-KByte page-table
entries的PAT index bit。page-table entry中的 PAT index bits(bit 7)
对应于page-directory entry 中的页面大小bits。因此，当为用于page directory
的page table 设置cache type 时，操作系统只能使用PAT entries的PA0到
PA3。如果操作系统在使用该memory 作为page table时尝试使用PA4 到PA7 PAT 
entries, 它会为这次作为page directory 访问该memory有效的设置PS bit 。
</font>

For compatibility with earlier IA-32 processors that do not
support the PAT, care should be taken in selecting the encodings
for entries in the PAT (see Section 11.12.5, “PAT Compatibility 
with Earlier IA-32 Processors”). 
<br/>
<font color=gray face="黑体" size=2>
为了兼容更早的不支持PAT的 IA-32 processors, 应注意选择PAT条目中
的编码(请查看Section 11.12.5 "PAT Compatibility with Earlier IA-32
Processors").
</font>

### 11.12.5 PAT Compatibility with Earlier IA-32 Processors

For IA-32 processors that support the PAT, the IA32_PAT MSR 
is always active. That is, the PCD and PWT bits in page-table
entries and in page-directory entries (that point to pages) 
are always select a memory type for a page indirectly by 
selecting an entry in the PAT. They never select the memory 
type for a page directly as they do in earlier IA-32 processors
that do not implement the PAT (see Table 11-6).  
<br/>
<font color=gray face="黑体" size=2>
对于支持PAT的 IA-32 processors，IA32_PAT MSR 总是 active。也
就是说，page-table entries 和 page-directory entries(指向page)
中的PCD和PWT bits总是间接的通过选择PAT中的entry 选择 page的
memory type。它从不像之前IA-32处理器那样直接选择页面的内存类型，
因为IA-32处理器没有实现PAT。
</font>

To allow compatibility for code written to run on earlier 
IA-32 processor that do not support the PAT, the PAT mechanism
has been designed to allow backward compatibility to earlier 
processors. This  compatibility is provided through the 
ordering of the PAT, PCD, and PWT bits in the 3-bit PAT 
entry index. For processors that do not implement the 
PAT, the PAT index bit (bit 7 in the page-table entries 
and bit 12 in the page-directory entries) is reserved 
and set to 0. With the PAT bit reserved, only the
first four entries of the PAT can be selected with the PCD and
PWT bits. At power-up or reset (see Table 11-12), these first
four entries are encoded to select the same memory types as the
PCD and PWT bits would normally select directly in an IA-32 processor
that does not implement the PAT. So, if encodings of the first
four entries in the PAT are left unchanged following a power-up
or reset, code written to run on earlier IA-32 processors that
do not implement the PAT will run correctly on IA-32 processors
that do implement the PAT.
<br/>
<font color=gray face="黑体" size=2>
为了兼容在不支持PAT的早期IA-32处理器上运行的代码, PAT 机制被
设计为允许向后兼容早期的处理器。这里的兼容性通过PAT entry index中的
PAT,PCD和PWT bits 的排序来提供的。对于没有实现PAT的处理器来说，
PAT index bit( page-table entries中的 bit7, page-directory entries
中的bit 12) 被保留并且设置为0。如果保留PAT位，只有PAT的前4个表项
可以通过PCD和PWT bits选择。当power-up 或者 reset 时（请看Table 11-12),
前四个表项被编码为和(1)相同的memory type.这里(1)指的是在没有实现
PAT 的IA-32处理器中，使用PCD和PWT bits 通常直接选择的memory type。
所以如果PAT前四个entries中的编码在power-up 或reset后保持不变，
编写于运行在早期没有实现PAT的IA-32 处理器的代码可以在支持PAT的IA-32
processor中运行。
</font>
