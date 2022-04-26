# ATS Specification

# 10.1 ATS Architectural Overview
Most contemporary system architectures make provisions for translating addresses from
DMA (bus mastering) I/O Functions. In many implementations, it has been common
practice to assume that the physical address space seen by the CPU and by an I/O
Function is equivalent. While in others, this is not the case. The address programmed into
an I/O Function is a “handle” that is processed by the Root Complex (RC). The result of 
this processing is often a translation to a physical memory address within the central complex. 
Typically, the processing includes access rights checking to insure that the DMA Function 
is allowed to access the referenced memory location(s).

<font color=gray face="黑体" size=2>
大部分现代系统架构对I/O Functions 的DMA (bus mastering)的地址转换作了准备.
在许多实现中, 通常的做法是假定CPU 和I/O Fucntions看到的是相同的物理地址空间.
而在其他情况下并非如此.地址以"handle"的方式被编程进I/O Function,该地址由Root 
Complex(RC)处理. 这个处理的结果通常是翻译为 central complex 中的physical memory
address.通常的,这个过程包括访问权限的检查,来月报DMA Function
允许访问指定的memory locations
</font>

The purposes for having DMA address translation vary and include:
* Limiting the destructiveness of a “broken” or miss-programmed DMA I/O Function
* Providing for scatter/gather
* Ability to redirect message-signaled interrupts (e.g., MSI or MSI-X) to 
different address ranges without requiring coordination with the underlying I/O Function
* Address space conversion (32-bit I/O Function to larger system address space)
* Virtualization support


<font color=gray face="黑体" size=2>
进行DMA address translation的不同的目的包括
<ul>
	<li>
	限制"broken"或者误编程的DMA I/O Function
	</li>
	<li>
	提供给 scatter/gather使用
	</li>
	<li>
	能够将message-signaled interrupts(例如:MSI/MSIx)重定向到不同的
	地址范围,并且不需要和下层的I/O Function协调
	</li>
	<li>
	地址空间的转换(将32-bit I/O Function转换成更大的系统地址空间)
	</li>
	<li>虚拟化支持</li>
</ul>
</font>

Irrespective of the motivation, the presence of DMA address translation 
in the host system has certain performance implications for DMA accesses.

<font color=gray face="黑体" size=2>
不管动机如何, 主机系统中的DMA address translation的存在会造成DMA
access时有一定的性能影响
</font>

Depending on the implementation, DMA access time can be significantly 
lengthened due to the time required to resolve the actual physical address. 
If an implementation requires access to a main-memory-resident translation table, the
access time can be significantly longer than the time for an untranslated access. 
Additionally, if each transaction requires multiple memory accesses (e.g., for a 
table walk), then the memory transaction rate (i.e., overhead) associated
with DMA can be high.

<font color=gray face="黑体" size=2>
根据实现情况,DMA access的时间会因为解析世纪的物理地址而显著增长. 
如果实现中需要访问main-memory-resident(主存中驻留)de地址转换表, 访问的时间可能
比不经过翻译的访问时间长的多.此外,如果每个transaction需要
multiple memory access(例如 for a table work), 则和DMA相关的内存事务率(即开销)
可能会很高
</font>

To mitigate these impacts, designs often include address translation 
caches in the entity that performs the address translation. In a CPU, 
the address translation cache is most commonly referred to as a translation 
look-aside buffer (TLB). For an I/O TA, the term address translation cache 
or ATC is used to differentiate it from the translation cache used
by the CPU.

<font color=gray face="黑体" size=2>
为了减轻这些影响, 设计时通常在执行地址转换的实体中包含地址转换缓存. 在CPU
中,地址转换缓存最常见之的是 translation look-aside buffer(TLB). 对于I/O TA, 
使用术语address translation cache 或者ATC 来区分CPU使用的地址转换缓存
</font>

While there are some similarities between TLB and ATC, there are important 
differences. A TLB serves the needs of a CPU that is nominally running one 
thread at a time. The ATC, however, is generally processing requests from 
multiple I/O Functions, each of which can be considered a separate thread. 
This difference makes sizing an ATC difficult depending
upon cost models and expected technology reuse across a wide range of system 
configurations.

<font color=gray face="黑体" size=2>
虽然对于TLB和ATC之间有一些相似点, 但是也有很大的不同之处. TLB 只服务于一个
同一时间只运行一个thread的CPU. 而ATC通常处理来自于muliple I/O Functions,他们
中的每个都可以被认为是一个单独的thread.这种差异是的根据成本模型确定ATC的大小
和在广泛的系统配置中预期的技术重用变得困难.(实际上就是难以根据之前的框架设计)
</font>

The mechanisms described in this specification allow an I/O Device to participate 
in the translation process and provide an ATC for its own memory accesses. The 
benefits of having an ATC within a Device include:

* Ability to alleviate TA resource pressure by distributing address translation 
caching responsibility (reduced probability of “thrashing” within the TA)
* Enable ATC Devices to have less performance dependency on a system’s ATC size
* Potential to ensure optimal access latency by sending pretranslated requests to central complex

<font color=gray face="黑体" size=2>
在本规范中描述的机制允许I/O Device来参与translation的处
理并且提供对于他们自己内存访问的ATC. 在Device中有ATC的好处包括:
<ul>
	<li>
	能够通过分配address trasnlation cache(ATC)责任来缓解
	TA资源方面的压力(减少TA中的"thrashing(抖动)"的可能性)
	</li>
	<li>
	使ATC devices减少对系统ATC size的性能依赖
	</li>
	<li>
	通过向central complex发送 pretranslated requests来
	保证最佳访问延迟的潜力
	</li>
</ul>
</font>

This specification will provide the interoperability that allows PCIe Devices 
to be used in conjunction with a TA, but the TA and its Address Translation 
and Protection Table (ATPT) are treated as implementation-specific and are outside the
scope of this specification. While it may be possible to implement ATS within 
other PCIe Components, this specification is confined to PCIe Devices and PCIe 
Root Complex Integrated Endpoints (RCiEPs).

<font color=gray face="黑体" size=2>
这个规范提供允许PCIe设备和TA结合使用的互操作性. 但是TA和他的Address Translation
以及Protection Table(ATPT)被作为implementation-specific(特定实现)对待并且在本规范
的范围之外. 虽然可以在其他的PCIe 组建中实现ATS, 但是本规范仅限于PCIe Devices和PCIe
Root Complex Integrated Endpoints. (RCiEPs)
</font>

Figure 10-1 illustrates an example platform with a TA and ATPT, along with a set 
of PCIe Devices and RC Integrated Endpoints with integrated ATC. A TA and an ATPT 
are implementation-specific and can be distinct or integrated components within a 
given system design.

<font color=gray face="黑体" size=2>
图10-1举例说明了一个带有TA和ATPT的平台, 并带有一些PCIe Devices和RC Integrated
Endpoints with integrated ATC.TA和ATPT是 implementation-specific并且在给定的系统
设计中是不同的或者集成的组件
</font>

![带TA的例子](pic/eg_platform_with_ta.png)

# 10.1.1 Address Translation Services (ATS) Overview
The ATS chapter provides a new set of TLP and associated semantics. ATS uses 
a request-completion protocol between a Device[^1] and a Root Complex (RC) to 
provide translation services. In addition, a new AT field is defined within the
Memory Read and Memory Write TLP. The new AT field enables an RC to determine 
whether a given request has been translated or not via the ATS protocol.

<font color=gray face="黑体" size=2>
ATS这个章节提供了一组新的TLP和相关的概念.ATS在Device[1]和Root Complex(RC)之间
使用一种request-completion协议来确保 translation services. 此外,在Memory Read和
Memory Write TLP中提供了一个新的AT字段. 这个新的AT字段使RC 确定给定的request是否
通过ATS协议进行了translate
</font>

Figure 10-2 illustrates the basic flow of an ATS Translation Request operation

<font color=gray face="黑体" size=2>
Figure 10-2 描述了ATS Translation Request主要的操作流程
</font>

![10-2](pic/ATS_T_R.png)

In this example, a Function-specific work request is received by a single-Function 
PCIe Device. The Function determines through an implementation-specific method 
that caching a translation within its ATC would be beneficial. There are a
number of considerations a Function or software can use in making such a 
determination; for example:
* Memory address ranges that will be frequently accessed over an extended 
period of time or whose associated buffer content is subject to a significant 
update rate
* Memory address ranges, such as work and completion queue structures, 
data buffers for low-latency communications, graphics frame buffers, host memory 
that is used to cache Function-specific content, and so forth

<font color=gray face="黑体" size=2>
在这个例子中,一个Singal-Function接收到一个 Function-specific 工作请求.
该Function确定了通过使用他自己的ATC缓存translation这样的 implementation-specific
方法是有效的.这里有几条注意事项,当一个Function或者software作出上述决定的时候, 
例如:
<ul>
	<li>
	该内存地址段在一段时间内被频繁访问,或者相关的buffer内容处于一个很高的更新频率
	</li>
	<li>
	该内存地址段是例如工作和完成队列数据结构,低延迟通信的data buffers, 图形帧缓冲区
	用于缓存 Function-specific内容的内存等等
	</li>
</ul>
</font>

The Function generates an ATS Translation Request which is sent upstream through 
the PCIe hierarchy to the RC which then forwards it to the TA. An ATS Translation 
Request uses the same routing and ordering rules as defined in this
specification. Further, multiple ATS Translation Requests can be outstanding at 
any given time; i.e., one may pipeline multiple requests on one or more TC. Each 
TC represents a unique ordering domain and defines the domain that must
be used by the associated ATS Translation Completion.

<font color=gray face="黑体" size=2>
Function生成了一个ATS Translation Request, 该 Request发送到upstream,该过程通过PCIe
层级到RC, 该RC接下来将转发到TA.一个ATS Translation Request使用同本规范中定义的路由
和排序规则. 此外, multiple ATS Translation Request 在任何时间都可以是
outstanding(这个需要在看下).例如,一个 pipeline multiple
requests可能需要一个或多个TC.每个TC代表一个唯一的顺序域, 并定义关联的ATS翻译完成
必须使用的域

</font>


Upon receipt of an ATS Translation Request, the TA performs the following basic steps:
1. Validates that the Function has been configured to issue ATS Translation Requests.
2. Determines whether the Function may access the memory indicated by the ATS 
Translation Request and has the associated access rights.
3. Determines whether a translation can be provided to the Function. 
If yes, the TA issues a translation to the Function.
	+ ATS is required to support a variety of page sizes to accommodate a 
	range of ATPT and processor implementations.
		+ Page sizes are required to be a power of two and naturally aligned.
		+ The minimum supported page size is 4096 bytes. ATS capable components 
		are required to support this minimum page size.
	+ A Function must be informed of the minimum translation or invalidate size 
	it will be required to support to provide the Function an opportunity to optimize 
	its resource utilization. The smallest minimum translation size must be 4096 bytes.
4. The TA communicates the success or failure of the request to the RC which 
generates an ATS Translation Completion and transmits via a Response TLP through 
a RP to the Function.
	+ An RC is required to generate at least one ATS Translation Completion per 
	ATS Translation Request;i.e., there is minimally a 1:1 correspondence 
	independent of the success or failure of the request.
		+ A successful translation can result in one or two ATS Translation 
		Completion TLPs per request. The Translation Completion indicates 
		the range of translation covered.
		+ An RC may pipeline multiple ATS Translation Completions; i.e., 
		an RC may return multiple ATS Translation Completions and these ATS 
		Translation Completions may be in any order relative to ATS 
		Translation Requests.
		+ The RC is required to transmit the ATS Translation Completion 
		using the same TC (Traffic Class) as the corresponding ATS 
		Translation Request.
	+ The requested address may not be valid. The RC is required to issue 
	a Translation Completion indicating that the requested address is 
	not accessible.

<font color=gray face="黑体" size=2>
当受到一个ATS Translation Request, TA 执行下面的主要步骤:
<ol>
	<li>
	验证这个Function已经被配置了,可以用来提交 ATS Translation Requests.
	</li>
	<li>
	确定 Function 是否可以访问 ATS Translation Request 指示的内存并具
	有相关的访问权限。
	</li>
	<li>
	确定可以向该Function 提供 translation.如果可以, TA 将对该Function
	提交一个 translation
	<ul>
		<li>
		ATS 需要支持各种页面大小以适应一系列 ATPT 和处理器实现。
		<ul>
			<li>页面大小必须是2的幂,并自然对齐</li>
			<li>
			支持的page size的最小值是4096字节. ATS capable 组件需要
			支持此最小页面的大小
			</li>
		</ul>
		</li>
		<li>
		为了优化Function的资源利用率,该Function必须被告知他需要支持的
		minimun translation和invalidate size. 最小的minimun translation 
		size必须是4096 bytes
		</li>
	</ul>
	</li>
	<li>
	TA需要告诉RC 该请求的结果是成功还是失败, 该RC会产生一个ATS Translation
	Completion并且通过RP 向Fucntion发送一个 Response TLP
	<ul>
		<li>
		RC需要为每个ATS Translation Request 生成至少一个ATS Translation 
		Completion; 也就是说, 至少存在1:1的对应关系, 与request的成功失败无关
		</li>
		<ul>
			<li>
			一个成功执行的translation可以为每个request回应一条或两条
			ATS Translation Completion. Translation Completion表明了
			translation 的范围.
			</li>
			<li>
			RC可能pipeline multiple ATS Translation Completions. 也就是说
			RC可能返回muliple ATS Translation Completions并且这些ATS Translation
			Completions 打乱了原有ATS Translation Request的顺序
			</li>
		</ul>
	</ul>
	</li>
</ol>
</font>


When the Function receives the ATS Translation Completion and either updates its ATC to reflect the translation or notes
that a translation does not exist. The Function proceeds with processing its work request and generates subsequent
requests using either a translated address or an untranslated address based on the results of the Completion.
a. Similar to Read Completions, a Function is required to allocate resource space for each completion(s) without
causing backpressure on the PCIe Link.
b. A Function is required to discard Translation Completions that might be “stale”. Stale Translation Completions
can occur for a variety of reasons.
As one can surmise, ATS Translation Request and Translation Completion processing is conceptually similar and, in
many respects, identical to PCIe Read Request and Read Completion processing. This is intentional to reduce design
complexity and to simplify integration of ATS into existing and new PCIe-based solutions. Keeping this in mind, ATS
requires the following:
• ATS capable components must interoperate with [PCIe-1.1] compliant components.
• ATS is enabled through a new Capability and associated configuration structure. To enable ATS, software must
detect this Capability and enable the Function to issue ATS TLP. If a Function is not enabled, the Function is
required not to issue ATS Translation Requests and is required to issue all DMA Read and Write Requests with
the TLP AT field set to “untranslated”.
• ATS TLPs are routed using either address-based or Requester ID (RID) routing.
• ATS TLPs are required to use the same ordering rules as specified in this specification.
• ATS TLPs are required to flow unmodified through [PCIe-1.1] compliant Switches.
• A Function is permitted to intermix translated and untranslated requests.
• ATS transactions are required not to rely upon the address field of a memory request to communicate
additional information beyond its current use as defined by the PCI-SIG.

[^1]:All references within this  chapter to a Device apply equally to 
a PCIe Device or a Root Complex Integrated  Endpoint. ATS does 
not delineate between these two types in terms of requirements, 
semantics, configuration, error handling, etc. From a software perspective, an 
ATS-capable Root Complex Integrated Endpoint must behave the same as an 
ATS-capable non-integrated Device. 
