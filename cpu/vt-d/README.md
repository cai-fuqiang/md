# 8. BIOS Consideration
BIOS负责检测平台中的重映射硬件功能,并且在host 地址
空间中定位基于内存映射的重映射硬件寄存器.BIOS通过接下
来描述的DMARemapping Reporting(DMAR) ACPI table来报告
到映射到系统软件remapping hardware units.

## 8.1 DMA Remapping Reporting Structure
<table>
    <tr>
		<th>Filed</th>
		<th>Byte Length</th>
		<th>Byte Offset</th>
		<th>Description</th>
    </tr>
    <tr>
		<td>Structure</td>
		<td>4</td>
		<td>0</td>
		<td>
			"DMAR".“DMAR”. Signature for the DMA Remapping
			Description table.
		</td>
    </tr>
    <tr>
		<td>Length</td>
		<td>4</td>
		<td>4</td>
		<td>
			Length, in bytes, of the description table
			including the length of the associated remapping
			structures.<br/>
			该描述符表的长度, byte为单位, 包括和 remapping 
			structures相关的长度
		</td>
    </tr>
    <tr>
		<td>Revision</td>
		<td>1</td>
		<td>8</td>
		<td>1</td>
    </tr>
    <tr>
		<td>Checksum</td>
		<td>1</td>
		<td>9</td>
		<td>Entire table must sum to zero</td>
    </tr>
    <tr>
		<td>OEMID</td>
		<td>6</td>
		<td>10</td>
		<td>OEMID</td>
    </tr>
    <tr>
		<td>OEM Table ID</td>
		<td>8</td>
		<td>16</td>
		<td>
			For DMAR description table, the Table ID is the
			manufacturer(生产商) model ID.
		</td>
    </tr>
    <tr>
		<td>OEM Revision</td>
		<td>4</td>
		<td>24</td>
		<td>OEM Revision of DMAR Table for OEM Table ID.</td>
    </tr>
    <tr>
		<td>Creator ID</td>
		<td>4</td>
		<td>28</td>
		<td>Vendor ID of utility that created the table.</td>
    </tr>
    <tr>
		<td>Creator Revision</td>
		<td>4</td>
		<td>32</td>
		<td>Revision of utility that created the table.</td>
    </tr>
    <tr>
		<td>Host Address Width</td>
		<td>1</td>
		<td>36</td>
		<td>
			This field indicates the maximum DMA physical
			addressability supported by this platform. The
			system address map reported by the BIOS
			indicates what portions of this addresses are
			populated.<br/><br/>
			The Host Address Width (HAW) of the platform is
			computed as (N+1), where N is the value
			reported in this field. For example, for a platform
			supporting 40 bits of physical addressability, the
			value of 100111b is reported in this field.<br/><br/>
			该字段表明平台DMA对物理地址的可寻址能力的支持。
			由bios提供的地址映射表明了这个地址的那些部分被
			填充<br/><br/>
			平台的Host Address Width(HAW)计算方式是 (N+1), 其中N是
			这个字段的实际的值。例如,对于一个平台支持 40bits 的
			物理地址寻址能力，那么这个字段中报告的值为100111(39)
		</td>
    </tr>
    <tr>
		<td>Flags</td>
		<td>1</td>
		<td>37</td>
		<td>
			* Bit 0: INTR_REMAP - If Clear, the platform
			does not support interrupt remapping. If Set,
			the platform supports interrupt remapping.<br/>
			* Bit 1: X2APIC_OPT_OUT - For firmware
			compatibility reasons, platform firmware
			may Set this field to request system
			software to opt out of enabling Extended
			xAPIC (X2APIC) mode. This field is valid only
			when the INTR_REMAP field (bit 0) is Set.
			Since firmware is permitted to hand off
			platform to system software in legacy xAPIC
			mode, system software is required to check
			this field as Clear as part of detecting
			X2APIC mode support in the platform.<br/>
			* Bits 2-7: Reserved.<br/><br/>
			* Bit 0:INTR_REMAP - 如果被清除，平台不支持
			interruptremapping.如果被设置，平台支持
			interrupt remapping
		</td>
    </tr>
    <tr>
		<td>Reserved</td>
		<td>10</td>
		<td>38</td>
		<td>Reserved(0)</td>
    </tr>
    <tr>
		<td>Remapping Structure[]</td>
		<td>-</td>
		<td>48</td>
		<td>
			A list of structures. The list will contain one or
			more DMA Remapping Hardware Unit Definition
			(DRHD) structures, and zero or more Reserved
			Memory Region Reporting (RMRR) and Root Port
			ATS Capability Reporting (ATSR) structures.
			These structures are described below.
			<br/>
			<br/>
			一系列的structures。这个列表包括:
			<table><th>数量</th><th>列表内容</th>
				<tr>
					<td>1/more</td>
					<td>
						DMA Remapping Hardware 
						Unit Definition structures(DRHD)
					</td>
				</tr>
				<tr>
					<td>0/more</td>
					<td>
						Reserved Memory Region Reporting 
						(RMRR) 
					</td>
				</tr>
				<tr>
					<td>0/more</td>
					<td>
						Root Port ATS Capability 
						Reporting (ATSR) structures.
					</td>
				</tr>
			</table>
		</td>
    </tr>
</table>


## 8.2 Remapping Structure Types
接下来描述了定义的remapping structure 的type。所有
的remapping struct 开始于一个`Type`字段，接下来是一个
`Length`字段，该字段表明这个structure的大小

<table>
	<th>value</th>
	<th>Description</th>
	<tr>
		<td>0</td>
		<td>
			DMA Remapping Hardware Unit 
			Definition (DRHD) Structure
		</td>
	</tr>
	<tr>
		<td>1</td>
		<td>
			Reserved Memory Region Reporting 
			(RMRR) Structure
		</td>
	</tr>
	<tr>
		<td>2</td>
		<td>
			Root Port ATS Capability Reporting 
			(ATSR) Structure
		</td>
	</tr>
	<tr>
		<td>3</td>
		<td>
			Remapping Hardware Static 
			Affinity (RHSA) Structure
		</td>
	</tr>
	<tr>
		<td>4</td>
		<td>
			ACPI Name-space Device 
			Declaration (ANDD) Structure
		</td>
	<tr>
		<td>>4</td>
		<td>
			>4 Reserved for future use. 
			For forward compatibility, 
			software skips structures 
			it does not comprehend by skipping 
			the appropriate number of bytes 
			indicated by the Length field
		</td>
	</tr>
</table>

BIOS的实现必须按照type数值顺序report 这些remmaping table(从小到大)
所有type 0(DRHD)的remapping structures 需要在type 1(RMRR)
的remapping structure 之前被枚举.

## 8.3 DMA Remapping Hardware Unit Definition Structure

一个DMA-remapping hardware unit definition(DRHD) structure
唯一表示平台中存在的remapping hardware unit .在平台中每个
pci segment至少存在一个该structure的实例
<table>
	<th>Field</th>
	<th>Byte Length</th>
	<th>Byte Offset</th>
	<th>Description</th>
	<tr>
		<td>Type</td>
		<td>2</td>
		<td>0</td>
		<td>
		0 - DMA Remapping Hardware 
		Unit Definition(DRHD) structure
		</td>
	</tr>
	<tr>
		<td>Length</td>
		<td>2</td>
		<td>2</td>
		<td>
		Varies (16 + size of Device Scope Structure)
		</td>
	</tr>
	<tr>
		<td>Flags</td>
		<td>1</td>
		<td>4</td>
		<td>
		Bit 0: INCLUDE_PCI_ALL
		• If Set, this remapping hardware unit has under
		its scope all PCI compatible devices in the
		specified Segment, except devices reported
		under the scope of other remapping hardware
		units for the same Segment<sup>1</sup>
		If a DRHD structure
		with INCLUDE_PCI_ALL flag Set is reported for a
		Segment, it must be enumerated by BIOS after
		all other DRHD structures for the same
		Segment. A DRHD structure with
		INCLUDE_PCI_ALL flag Set may use the ‘Device
		Scope’ field to enumerate I/OxAPIC and HPET
		devices under its scope.
		• If Clear, this remapping hardware unit has under
		its scope only devices in the specified Segment
		that are explicitly identified through the ‘Device
		Scope’ field.
		Bits 1-7: Reserved
		* 如果设置，这个重映射硬件单元在它的范围内指定
		  段内的所有PCI兼容设备，除了在同一段的其他重映射硬
		  件单元范围内报告的设备。如果一个Segment中有带着
		  INCLUDE_PCI_ALL标记的DRHD structures被报告，
		  他必须在所有其他的的在相同的Segment的DRHD 
		  structure 之后被枚举.一个带有INCLUDE_PCI_ALL
		  的DRHD structure 可能使用`Device Scope`字段，
		  来枚举他范围内的I/O xAPIC和HPET 设备。
		* 如果该字段清空，这个remapping hareware 只能
		  拥有通过`Device Scope`中明确指定的设备
		</td>
	</tr>
	<tr>
		<td>Reserved</td>
		<td>1</td>
		<td>5</td>
		<td>Reserved(0)</td>
	</tr>
	<tr>
		<td>Segment Number</td>
		<td>2</td>
		<td>6</td>
		<td>
		The PCI Segment associated with this unit
		</td>
	</tr>
	<tr>
		<td>Register Base Address</td>
		<td>8</td>
		<td>8</td>
		<td>
		Base address of remapping hardware register-set for this unit.
		</td>
	</tr>
	<tr>
		<td>Device Scope</td>
		<td>-</td>
		<td>16</td>
		<td>
		The Device Scope structure contains zero or more
		Device Scope Entries that identify devices in the
		specified segment and under the scope of this
		remapping hardware unit.
		<br/><br/>
		The Device Scope structure is described below.
		Device Scope structure包含0个或多个Device Scope
		Entries，这些Entries标识指定段中以及在这个重映
		射硬件单元范围内的设备。
		<br/><br/>
		Device Scope 将会在下面描述
		</td>
	</tr>
</table>
1. 在带有多个PCI segment的平台上，其中任意一个segments可能有
一个设置了INCLUDE_PCI_ALL flags的DRHD

## 8.3.1 Device Scope Structure
Device Scope Structure由 Device Scope Entries组成.
每个Device Scope Entry可能被用作代表一个PCI endpoint device,
a PCI sub-hierarchy, 或者 I/Ox APICs 或者 HPET(High Precision
Event Timer)

在这一节, 通用术语`PCI`被用作描述传统的PCI, PCI-X和PCI-Express
设备. 相似的,通用术语`PCI-PCI bridge`被用作指作传统的PCI bridge,
PCI-X bridges, PCI-Express root ports, 或者downstream ports of
a PCI-Express switch.

PCI sub-hierarchy被定义为PCI 控制器的集合,这些PCI 控制器位于
特定PCI-PCI bridge的downstream.为了识别一个PCI sub-hierarchy,
Device Scope Entry 只需要识别sub-hierarchy的parent PCI-PCI bridge.

<table style="background-color:red">
	<th>Filed</th>
	<th>Byte Length</th>
	<th>Byte Offset</th>
	<th>Description</th>
	<tr>
		<td>Type</td>
		<td>1</td>
		<td>0</td>
		<td>
		The following values are defined for this field.
		<dd>
		• 0x01: PCI Endpoint Device - The device
		identified by the ‘Path’ field is a PCI endpoint
		device. This type must not be used in Device
		Scope of DRHD structures with
		INCLUDE_PCI_ALL flag Set.
		</dd>
		<dd>
		• 0x02: PCI Sub-hierarchy - The device identified
		by the ‘Path’ field is a PCI-PCI bridge. In this
		case, the specified bridge device and all its
		downstream devices are included in the scope.
		This type must not be in Device Scope of DRHD
		structures with INCLUDE_PCI_ALL flag Set.
		</dd>
		<dd>
		• 0x03: IOAPIC - The device identified by the
		‘Path’ field is an I/O APIC (or I/O SAPIC) device,
		enumerated through the ACPI MADT I/O APIC
		(or I/O SAPIC) structure.
		</dd>
		<dd>
		• 0x04: MSI_CAPABLE_HPET 1 - The device
		identified by the ‘Path’ field is an HPET Timer
		Block capable of generating MSI (Message
		Signaled interrupts). HPET hardware is reported
		through ACPI HPET structure.
		</dd>
		<dd>
		• 0x05: ACPI_NAMESPACE_DEVICE - The device
		identified by the ‘Path’ field is an ACPI name-
		space enumerated device capable of generating
		DMA requests.
		</dd>
		Other values for this field are reserved for future
		use.
		<br/>
		<br/>
		<font color=gray face="黑体" size=2>
		接下来描述该字段的定义值:
		<dd>
		• 0x01: PCI Endpoint Device - `Path` 字段标识了
		该设备是一个PCI endpoint device.
		这个类型不能用在带有设置INCLUDE_PCI_ALL flags
		的DRHD structures的Device Scope
		</dd>
		<dd>
		• 0x02: PCI sub-hierarchy - `Path`字段标识了
		该设备是一个PCI-PCI bridge. 在这种情况中,
		这个被指定的bridge device 和它的所有的downstream
		device都被包括在这个scope中. 
		这个类型不能用在带有设置INCLUDE_PCI_ALL flags
		的DRHD structures的Device Scope
		</dd>
		<dd>
		• 0x03: IO-APIC- `Path`字段标识了
		该设备是一个IO-APIC.(or I/O-SAPIC device),通过ACPI
		MADT I/O APIC( or I/O-SAPIC)structure枚举
		</dd>
		<dd>
		• 0x04: MSI_CAPABLE_HPET - <sup>1</sup>
		`Path`字段标识了该设备是一个HPET Timer Block,并且
		具有生成MSI的能力(Message Signaled interrupts).
		HPET hardware 通过ACPI HEPT structure被报告.
		</dd>
		<dd>
		• 0x05:ACPI_NAMESPACE_DEVICE -
		`Path`字段标识该设备是能够生成 DMA 
		请求的 ACPI 命名空间枚举设备。
		</dd>
		其他值被预留用作未来使用.
		</font>
	</tr>
	<tr>
		<td>Length</td>	
		<td>1</td>	
		<td>1</td>	
		<td>
		Length of this Entry in Bytes. (6 + X), where X is the
		size in bytes of the “Path” field.
		</br></br>
		这个条目的长度,以Byte为单位.(6+X),其中X是`Path`字段长度
		(以Byte为单位)
		</td>	
	</tr>
	<tr>
		<td>Reserved</td>	
		<td>2</td>	
		<td>2</td>	
		<td>Reserved(0)</td>
	</tr>
	<tr>
		<td>Enumeration ID</td>	
		<td>1</td>	
		<td>4</td>	
		<td>
		When the ‘Type’ field indicates ‘IOAPIC’, this field
		provides the I/O APICID as provided in the I/O APIC
		(or I/O SAPIC) structure in the ACPI MADT (Multiple
		APIC Descriptor Table).
		</br>
		When the ‘Type’ field indicates ‘MSI_CAPABLE_HPET’,
		this field provides the ‘HPET Number’ as provided in
		the ACPI HPET structure for the corresponding Timer
		Block.
		</br>
		When the ‘Type’ field indicates
		‘ACPI_NAMESPACE_DEVICE’, this field provides the
		“ACPI Device Number” as provided in the ACPI
		Name-space Device Declaration (ANDD) structure for
		the corresponding ACPI device.
		</br>
		This field is treated reserved (0) for all other ‘Type’
		fields.
		</br></br>
		<font color=gray face="黑体" size=2>
		<table>
			<th>`Type` field value</th>
			<th>含义</th>	
			<tr>
				<td>IOAPIC</td>
				<td>
				该字段提供I/O APICID ,该ID在ACPI MADT(Multiple APIC
				Descriptor Table)结构中被提供.
				</td>
			</tr>
			<tr>
				<td>MSI_CAPABLE_HPET</td>
				<td>
				该字段提供了`HPET` number, 该字段在相应的 Timer 
				Block中的 ACPI HEPT 结构体中被提供.
				</td>
			</tr>
			<tr>
				<td>ACPI_NAMESPACE_DEVICE</td>
				<td>
				这个字段提供了`ACPI Device Number`, 该字段在
				相应的APCI device中的ACPI Name-space Device 
				Declaration(ANDD)结构体中被提供.
				</td>
			</tr>
		</table>
		该字段在其他值的`Type`中按照预留对待(值为0)
		</font>
		</td>
	</tr>
	<tr>
		<td>Start Bus Number</td>
		<td>1</td>
		<td>5</td>
		<td>
		This field describes the bus number (bus number of
		the first PCI Bus produced by the PCI Host Bridge)
		under which the device identified by this Device
		Scope resides.
		</br></br>
		<font color=gray face="黑体">
		这个字段描述了由该Device Scope 表示的设备所在的
		bus号(由PCI Host Bridge提供的第一个PCI Bus的bus 
		号),
		</font>
		</td>
	</tr>
	<tr>
		<td>Path</td>
		<td>2 * N</td>
		<td>6</td>
		<td>
		Describes the hierarchical path from the Host Bridge
		to the device specified by the Device Scope Entry.
		For example, a device in a N-deep hierarchy is
		identified by N {PCI Device Number, PCI Function
		Number} pairs, where N is a positive integer. Even
		offsets contain the Device numbers, and odd offsets
		contain the Function numbers.
		</br></br>
		The first {Device, Function} pair resides on the bus
		identified by the ‘Start Bus Number’ field. Each
		subsequent pair resides on the bus directly behind
		the bus of the device identified by the previous pair.
		The identity (Bus, Device, Function) of the target
		device is obtained by recursively walking down these
		N {Device, Function} pairs.
		</br></br>
		If the ‘Path’ field length is 2 bytes (N=1), the Device
		Scope Entry identifies a ‘Root-Complex Integrated
		Device’. The requester-id of ‘Root-Complex
		Integrated Devices’ are static and not impacted by
		system software bus rebalancing actions.
		</br></br>
		If the ‘Path’ field length is more than 2 bytes (N >
		1), the Device Scope Entry identifies a device behind
		one or more system software visible PCI-PCI
		bridges. Bus rebalancing actions by system software
		modifying bus assignments of the device’s parent
		bridge impacts the bus number portion of device’s
		requester-id.
		</br></br>
		</br></br>
		<font color=gray face="黑体" size=2>
		描述了从Host Bridge到 Device Scope Entry 所要识别的
		device 的层级性质的路径.举个例子, 一个N-hierarchy的
		device可以被认为N个{PCI Device Number, PCI Function
		Number}的pair, 其中N 是正整数,偶数便宜包含Device Number,
		基数偏移包含 Function Number
		</br></br>
		第一个{Device, Function} pair位于由`Start Bus Number`
		标识的总线向.每个下一级的pair 在前一对标识的设备的总线
		后面的总线上. target device的identity(Bus, Device, Function)
		可以通过递归的遍历N个{Device Function} pairs获取.
		</br></br>
		如果`Path`字段长度是2 bytes(N=1),这个 Device  Scope
		Entry 表明是一个`Root-Complex Integrated Device`.
		`Root-Complex Integrated Devices`的requester-id是静态的,
		并且不受系统软件bus rebalancing 行为的影响.
		</br></br>
		如果`Path`字段的长度大于2 bytes(N>1).这个Device Scope
		Entry 表明这个设备位于一个或多个系统软件可见的PCI-PCI
		bridges. 通过系统软件修改bus assignments 的Bus 
		rebalancing行为影响device requester-id的bus number部分.
		</font>
		</td>
	</tr>
</table>

1. An HPTE Timer Block is capable of MSI interrupt generation 
if any of the Timers in the Timer Block reports 
FSB_INTERRUPT_DELIVERY capability in the Timer Configuration 
and Capability Registers. HPET Timer Blocks not capable of 
MSI interrupt generation (and instead have their interrupts 
routed through I/OxAPIC)are not reported in the Device Scope.

<font color=gray face="黑体" size=2>
如果在任何的在Timer Block 的Timers在Timer Configuration和
Capability Registers中报告了FSB_INTERRUPT_DELIVERY capability
该HPET Timer Block具有能产生MSI中断的能力. Device Scope中不报告
HPET Timer Blocks没有产生MSI interrupt 能力的HPET Timer Blocks
(让它们通过I/OxAPIC路由)
</font>

The following pseudocode describes how to identify the device 
specified through a Device Scope structure:

<font color=gray face="黑体" size=2>
接下来的伪代码描述了通过Device Scope structure识别指定的设备.
</font>

```
n = (DevScope.Length - 6) / 2;		// number of entries in the ‘Path’ field
type = DevScope.Type;				// type of device
bus = DevScope.StartBusNum;			// starting bus number
dev = DevScope.Path[0].Device;		// starting device number
func = DevScope.Path[0].Function;	// starting function number
i = 1;
while (--n) {
	// secondary bus# from config reg.
	bus = read_secondary_bus_reg(bus, dev, func);
	// read next device number
	dev = DevScope.Path[i].Device;
	func = DevScope.Path[i].Function;
	// read next function number
	i++;
}
source_id = {bus, dev, func};
// if ‘type’ indicates ‘IOAPIC’, DevScope.EnumID
// provides the I/O APICID as reported in the ACPI MADT
target_device = {type, source_id};   
```

#### 8.3.1.1 Reporting Scope for I/OxAPICs

