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

