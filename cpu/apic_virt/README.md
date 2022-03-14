# 29 APIC VIRTUALIZATION AND VIRTUAL INTERRUPTS
VMCS中包含一些控制字段来使能中断和APIC的虚拟化。

当这些控制字段被used, 处理器将会模拟大多数的对apic的访问，
跟踪virtual apic的状态，并且deliver 虚拟中断--这些都处于
non-root 模式并且没有任何VM-exit。

处理器通过使用一个virtual-APIC的页，该页是由VMM identified来跟踪
virtual apic状态。Section 29.1 描述了这个virtual-apic 页, 并且
描述了处理器如何使用它取跟踪virtual-APIC的状态

下面是和APIC虚拟化和虚拟中断相关的VM-execution controls:(24.6中
描述了这些控制字段的位置(也就是VMCS中相对偏移))

* **Virtual-interrupt delivery**. 这个控制字段enables evaluation 
 and delivery of pending virtual interrupt(Section 29.2). 它同时
 enabled the emulation of writes(memory-mapped or MSR-based, as 
 enabled) 对于APIC控制interrupt prioritization的寄存器.

* **Use TPR shadow**. 这个控制位使能对访问APIC's task-priority 寄存器(TPR)
 的模拟(Section 29.3), 访问方式： 通过memory-mapped or MSR-based 访问接口。

* **Virtualize APIC accesses**. 这个控制位enables 基于memory-based APIC的
  访问(29.4) 虚拟化。虚拟化方式为通过访问accesses to VMM-specified 
  APIC-access page造成VM exits。如果设置了某些控制位，某些访问会被模拟（硬件）
  而不是造成VM-exits

* **Virtualize x2APIC mode**. 这个控制位enables 基于MSR-based 访问 APIC的虚拟化。
  (Section 29.5)

* **APIC-register virtualization**.这个控制字段字段通过从virtual-APIC page 获取
  大多数的memory-mapped 和MSR-based APIC-register(as enabled) 寄存器信息。

* **Process posted interrupts**. 这个控制字段允许软件在一个数据结构中去
  post virtual interrupts并且对另一个logical processor发送一个notification;在
  收到这个notification之后, target processor 将会处理这个posted interrupts通过
  copy them into virtual-APIC page(Section 29.6)

`virtual APIC accesses`, `virtual x2APIC mode`, `virtual-interrupt delivery`和
`APIC-register virtualization`全部在secondary processor-based VM-execution 控制区。
如果primary processor-based VM-execution 的31位控制字段位0，processor会按照这些
控制字段为0去处理。See Section 24.6.2.


## 29.1 VIRTUAL APIC STATE
**virtual-APIC page** 是一个4-KByte的内存区域，处理器会用这个区域虚拟化对APIC某些
APIC register 访问并且管理虚拟中断
