# 8. BIOS Consideration
BIOS负责检测平台中的重映射硬件功能,并且在host 地址
空间中定位基于内存映射的重映射硬件寄存器.BIOS通过接下
来描述的DMARemapping Reporting(DMAR) ACPI table来报告
到映射到系统软件remapping hardware units.

## 8.1 DMA Remapping Reporting Structure
<table>
    <tr>
		<td>Filed</td>
		<td>Byte Length</td>
		<td>Byte Offset</td>
		<td>Description</td>
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
			value of 100111b is reported in this field.
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
			* Bits 2-7: Reserved.
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
		</td>
    </tr>
</table>

