# 3
## 3.6 First-Level Translation
Extended-context-entries can be configured to translate 
requests-with-PASID through first-level translation. 
Extended-context-entries contain the PASID-table pointer 
and size fields used to reference the PASID-table. The 
PASID-number in a request-with-PASID is used to offset into 
the PASID-table. Each present PASID-entry contains a pointer 
to the base of the first-level translation structure for the 
respective process address space. Section 9.5 describes the 
exact format of the PASID-entry.

<font color=gray face="黑体" size=2>
Extended-context-entries 可以用来配置 来通过 first-level 
translation translate request-with-PASID。Extended-context-entries
包含PASID-table pointer 和size 字段用来引用PASID-table.request-with-PASID
中的PASID-number 用来在PASID-table中提供偏移。每个present PASID-entry 
包含一个指向first-level translation structure 的 基址，这些structure
分别对应process address space。Section 9.5描述了PASID-entry的详细
格式。
</font>

First-level translation restricts the input-address to a 48-bit
canonical addresses (i.e., address bits 63:48 has the same 
value as the address bit 47). A device may perform local check
for canonical address before it issues a request-with-PASID, 
and handle a violation in a device specific manner. 
Requests-with-PASID arriving at the remapping hardware are 
subject to canonical address checking, and a violation is 
treated as a translation-fault. Chapter 7 provides details 
of translation-fault conditions and how they are reported to 
software.

<font color=gray face="黑体" size=2>
First-level translation 将 input-address 限制为48-bit
规范address(例如, address 63:48位和bit 47 位有相同的值).
device 可以在提交 reuqest-with-PASID 之前， 对于规范的地
址执行一些本地的检查, 并且以设备特定的方式处理这些违规的地
址。Request-with-PASID到达remapping hardware 受到规范 
address  检查的影响, 并且 违规地址会被到嗯做translation fault 对待。
Chapter 7提供了translation-fault 情况的细节并且讲述了他们是怎么
报告给software的。
</font>

First-level translation supports the same paging structures 
as Intel® 64 processors when operating in 64-bit mode. Table 
3 gives the different names of the first-level translation 
structures, that are given based on their use in the translation
process. It also provides, for each structure, the source of 
the physical-address used to locate it, the bits in the 
input-address used to select an entry from the structure, 
and details of whether and how such an entry can map a page. 
Section 9.7 describes format of each of these paging structures
in detail. 

<font color=gray face="黑体" size=2>
First-level translation 支持 和Intel 64处理器运行于64-bit 模型
相同的paging structures。Table 3 给出了first-level translation 
不同的names, 这些名称是根据他们在translation 过程中的用途给出的。
它还为每一个structure 提供了，定位他们的物理地址的source , 用于从
结构中选择entry的input-address 的bits, 以及此类条目是否或者如何map page
的详细信息。Section 9.7 描述了每个paging structure 格式的细节。
</font>

![Table-3](pic/Table-3.png)

First-level translation may map input addresses to 4-KByte 
pages, 2-MByte pages, or 1-GByte pages. Support for 4-KByte 
pages and 2-MBytes pages are mandatory for first-level 
translation. Implementations supporting 1-GByte pages report 
it through the FL1GP field in the Capability Register (see 
Section 10.4.2). Figure 3-9 illustrates the translation process
when it produces a 4- KByte page; Figure 3-10 covers the case
of a 2-MByte page; Figure 3-11 covers the case of a 1-GByte page.

<font color=gray face="黑体" size=2>
First-level translation 可以将input address map到一个4-KByte
page, 2-MByte page, 或者1-GByte page。对4-KByte和2-MByte page
的支持，交给first-level translatin 去做。Implementation通过
Capability Register( Section 10.4.2) 中的FL1GP 字段报告是否支持
1-GByte pages。Figure3-9 图示了当产生了一个4-KByte page时的translation
过程; Figure 3-10 涉及2-MByte page 的情形; Figure 3-11 涉及
1-GByte page 的情形。
</font>

The following describe the first-level translation in more 
detail and how the page size is determined:

<font color=gray face="黑体" size=2>
接下来描述first-level translation 的更多细节，并且page size 
是如何确定的。
</font>

* A 4-KByte naturally aligned PML4 table is located at the 
physical address specified in First-level- page-table-pointer
(FLPTPTR) field in the PASID-entry (see Section 9.5). A PML4 
table comprises 512 64-bit entries (PML4Es). A PML4E is 
selected using the physical address defined as follows:<br/>
	— Bits 2:0 are all 0. 	<br/>
	— Bits 11:3 are bits 47:39 of the input address. 	<br/>
	— Bits 12 and higher are from FLPTPTR field in the PASID-entry. <br/>
	Because a PML4E is identified using bits 47:39 of the input 
	address, it controls access to a 512-GByte region of the 
	input-address space. 
<br/>
<font color=gray face="黑体" size=2>
4-KByte naturally aligned(自然对齐, 有点像强制对齐) PML4 table位于
PASID-entry (请看section 9.5) 中的 First-level-page-table-pointer
(FLPTPTR) 字段指定的物理地址。PML4 table包含512 64-bit entries(PML4Es）。
使用下面定义物理地址选择PML4E。<br/>
&emsp;&emsp;- Bit 2:0 都是0 (3 位 地址对应64bit )<br/>
&emsp;&emsp;- Bit 11:3 是input address 的 47:39位<br/>
&emsp;&emsp;- Bit 12 更高的位来自PASID-entry 中的FLPTPTR 字段<br/>
&emsp;&emsp;因为PML4 使用input address 的47:39 bits定位PML4E，所以
它可以控制去访问input-address 空间的 512-GByte的region。
</font>

* A 4-KByte naturally aligned page-directory-pointer table 
is located at the physical address specified in address (ADDR)
field in the PML4E (see Table 18). A page-directory-pointer 
table comprises 512 64-bit entries (PDPEs). A PDPE is selected
using the physical address defined as follows:<br/>
	— Bits 2:0 are all 0.<br/>
	— Bits 11:3 are bits 38:30 of the input address.<br/>
	— Bits 12 and higher are from the ADDR field in the PML4E.<br/>
	Because a PDPE is identified using bits 47:30 of the input 
	address, it controls access to a 1-GByte region of the 
	input-address space. Use of the PDPE depends on its page-size (PS)
	field:
<br/>
<font color=gray face="黑体" size=2>
4-KByte naturally aligned(自然对齐) page-directory-pointer table
位于PML4E(see Table 18) 中的address(ADDR)字段指定的物理地址。
page-directory-pointer table 包含512 个 64-bit entries(PDPEs)。使用
接下来定义的物理地址选择PDPE:<br/>
&emsp;&emsp;- Bits 2:0 都是0 （同上）<br/>
&emsp;&emsp;- Bits 11:3 是input address 的 38:30位<br/>
&emsp;&emsp;- Bits 12 和更高的位来自PML4E的ADDR字段<br/>
&emsp;&emsp;因为PDPE通过input address 的47:30位指定PDPE, 所以它
控制访问input-address space的1-GByte region。PDPE的使用依赖他的
PS字段:
</font>

* If the PDPE’s PS field is 1, the PDPE maps a 1-GByte page 
(see Table 19). The final physical address is computed as 
follows:<br/>
	— Bits 29:0 are from the input address.<br/>
	— Bits 30 and higher are from the ADDR field in the PDPE.
<br/>
<font color=gray face="黑体" size=2>
如果PDPE的PS字段为战，PDPE 将会映射一个1-GByte的page (see Table 
19)。最终的物理地址由下面组成:<br/>
&emsp;&emsp;- Bits 29:0 来自于input address<br/>
&emsp;&emsp;- Bits 30和更高的位来自于PDPE的ADDR字段
</font>

* If the PDPE’s PS field is 0, a 4-KByte naturally aligned 
page directory is located at the physical address specified 
in the address (ADDR) field in the PDPE (see Table 20). A page
directory comprises 512 64-bit entries (PDEs). A PDE is 
selected using the physical address defined as follows:
	— Bits 2:0 are all 0.<br/>
	— Bits 11:3 are bits 29:21 of the input address.<br/>
	— Bits 12 and higher are from the ADDR field in the PDPE.<br/>
	Because a PDE is identified using bits 47:21 of the input 
	address, it controls access to a 2-MByte region of the 
	input-address space. Use of the PDPE depends on its 
	page-size (PS) field:
<br/>
<font color=gray face="黑体" size=2>
如果PDPE的PS字段为0, 4KByte 自然对齐的page directory 
位于PDPE（see Table 20)中的address(ADDR)字段指定的物理地址。
page directory 包含512 个 64 bit 的entries(PDEs)。通过
使用下面定义的物理地址选择PDE:
&emsp;&emsp;- Bits 2:0 都是0 <br/>
&emsp;&emsp;- Bits 11:3 是input address的29:21位。<br/>
&emsp;&emsp;- Bits 12和更高的位来自于PDPE的ADDR字段<br/>
&emsp;&emsp;因为PDE使用input address的47:21 bits指定，
所以它控制访问 input-address 2-MByte region。PDPE的使用
依赖他的page-size(PS)字段:
</font>

* If the PDE’s PS field is 1, the PDE maps a 2-MByte page 
(see Table 21). The final physical address is computed as 
follows:
	— Bits 20:0 are from the input address.
	— Bits 21 and higher are from the ADDR field in the PDE.
<br/>
<font color=gray face="黑体" size=2>
如果PDE的PS字段为1, PDE 会映射一个2-MByte page(see Table 21)。
最终的物理地址由以下组成:
&emsp;&emsp;-Bits 20:0 来自于input address。
&emsp;&emsp;-Bits 21 或者更高的位来自于PDE的ADDR字段。
</font>

* If the PDE’s PS field is 0, a 4-KByte naturally aligned page
table is located at the physical address specified in the 
address (ADDR) field in the PDE (see Table 22). A page table 
comprises 512 64-bit entries (PTEs). A PTE is selected using 
the physical address defined as follows:
	— Bits 2:0 are all 0.<br/>
	— Bits 11:3 are bits 20:12 of the input address.<br/>
	— Bits 12 and higher are from the ADDR field in the PDE.<br/>
	Because a PTE referenced by a PDE is identified using bits 
	47:12 of the input address, every such PTE maps a 4-KByte page
	(Table 23). The final page address is translated as follows:<br/>
	— Bits 11:0 are from the input address.<br/>
	— Bits 12 and higher are from the ADDR field in the PTE.
<br/>
<font color=gray face="黑体" size=2>
如果PDE的PS字段为0, 4-KByte自然对齐的page table 位于PDE中的address
(ADDR)字段中指定的物理地址。page table 由512个 64-bit的entries(PTEs）
组成。使用如下定义的物理地址选择PTE:
&emsp;&emsp;- Bits 2:0 都是0.<br/>
&emsp;&emsp;- Bits 11:3 是input address的20:12 bits<br/>
&emsp;&emsp;- Bits 12以及更高位来自PDE的ADDR字段。<br/>
&emsp;&emsp;因为PDE引用的(指向的)PTE是使用input address 47:12位
确定的，所以每个PTE maps 一个4-KByte page （Table 23)。最终翻译
的页面地址如下:<br/>
&emsp;&emsp;-Bits 11:0 来自于 input address 的11:0<br/>
&emsp;&emsp;-Bits 12和更高的字节来自于PTE的ADDR字段。
</font>

If a paging-structure entry’s Present (P) field (bit 0) is 0 
or if the entry sets any reserved field, the entry is used 
neither to reference another paging-structure entry nor to 
map a page. A reference using a input address whose translation 
would use such a paging-structure entry causes a translation
fault (see Chapter 7).

<font color=gray face="黑体" size=2>
如果page-structure entry中的Present(P) field(bit 0)是0,或者
如果entry 设置了任何的保留字段，entry 不再用作引用另一个
paging-structure entry 或者map a page. 使用这样的input address
的reference(引用?)会导致translation fault(see Chapter 7),
该input address他的翻译会使用上述的paging-structure entry
</font>

The following bits are reserved with first-level translation:

<font color=gray face="黑体" size=2>
下面的bits将在first-level translation中保留。
</font>

* If the P field of a paging-structure entry is 1, bits 51:HAW
(Host Address Width) are reserved.
<br/>
<font color=gray face="黑体" size=2>
如果paging-structure entry 中的P 字段为1, bit 51:HAW(Host Address
Width) 将被保留
</font>

* If the P field of a PML4E is 1, the PS field is reserved.
<br/>
<font color=gray face="黑体" size=2>
如果PML4E中的P字段为1, PS字段被保留
</font>

* If 1-GByte pages are not supported and the P field of a 
PDPE is 1, the PS field is reserved.
<br/>
<font color=gray face="黑体" size=2>
如果1-GByte page 不被支持，并且PDPE中的P字段为战， PS字段
将被保留。
</font>

* If the P field and PS field of a PDPE are both 1, bits 29:13
are reserved.
<br/>
<font color=gray face="黑体" size=2>
如果P字段和PDPE中的PS字段都为1, bits 29:13字段将被保留。
</font>

* If the P field and PS field of a PDE are both 1, bits 20:13
are reserved.
<br/>
<font color=gray face="黑体" size=2>
如果PDE和PS字段都为1, 20:13字段被保留。
</font>

* If Extended-Accessed flag is not supported, the EA field in
the paging entries are ignored.
<br/>
<font color=gray face="黑体" size=2>
如果 Extended-Accessed flags 不支持，paging entires中的EA字段
将被忽略。
</font>

* If No-Execute-Enable (NXE) field is 0 in the extended-context-entry 
and the P field of a paging-structure entry is 1, the Execute-Disable 
(XD) field is reserved
<br/>
<font color=gray face="黑体" size=2>
如果extended-context-entry 中的No-Execute-Enable(NXE)字段为0， 并且
paging-structure中的P字段为1,则Execute-Disable(XD)字段被预留。
</font>

### 3.6.1 Translation Faults
Requests-with-PASID can result in first-level translation 
faults for either of two reasons: (1) there is no valid 
translation for the input address; or (2) there is a valid 
translation for the input address, but its access rights do 
not permit the access. There is no valid translation if any 
of the following are true:

* The Root Table Type (RTT) field in Root-table Address 
register (RTADDR_REG) is 0.
* The input address in the request is not canonical (i.e., 
address bits 63:48 not same value as address bit 47).
* Hardware attempt to access a translation entry (extended-root-entry, 
extended-context-entry, PASID-table entry, or a first-level 
paging-structure entry) resulted in error.
* The extended-root-entry used to process the request (as noted
in Section 3.4.2) has the relevant present field as 0, has 
invalid programming, or has a reserved bit set.
* The extended-context-entry used to process the request 
(as noted in Section 3.4.3) has the P field as 0, PASIDE 
field as 0, ERE field as 0 (for requests with Execute-Requested (ER)
field Set), SMEP field as 1 (for requests with Execute-Requests (ER)
and Privileged-mode-Requested (PR) fields Set), has invalid programming,
T field is programmed to block requests-with-PASID, or has a reserved 
bit set.
* The PASID-entry used to process the request (as noted in 
Section 3.6) has the P field as 0, or has the SRE field as 0 
(for requests with Privileged-mode-Requested (PR) field Set).
* The translation process for that address (as noted in Section 3.6)
used a paging-structure entry in which the P field is 0 or one 
that sets a reserved bit.

If there is a valid translation for an input address, its 
access rights are determined as described in Section 3.6.2.

Depending on the capabilities supported by remapping hardware 
units and the endpoint device, translations faults may be 
treated as non-recoverable errors or recoverable page faults. 
Chapter 7 provides detailed hardware behavior on translation 
faults and reporting to software. 
