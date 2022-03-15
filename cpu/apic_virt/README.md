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
`virtual-APIC page` 是一个4-KByte的内存区域，处理器会用这个区域虚拟化对APIC某些
APIC register 访问并且管理虚拟中断。这个virtual-APIC page的物理地址是`virtual-APIC
address`, 一个在VMCS中的64-bit VM-execution控制字段(Section 24.6.8)。

根据某些VM-execution控制字段的设置，处理器可能虚拟化virtual-APIC page上的某些字段，
其功能类似于在local APIC执行的效果。Section 29.1.1指出并定义了这些fields.Section 29.1.2,
Section 29.1.3, Section 29.1.4和Section 29.1.5细节描述了虚拟化某些字段的更新所采取的操作。

### 29.1.1 Virtual APIC Registers

根据VM-execution控制字段的设置，一个逻辑处理器可能会根据下面的virtual-APIC page
中的某些字段虚拟化访问APIC registers流程。

* Virtual task-priority register(VTPR)
* Virtual processor-pirority register(VPPR)
* Virtual end-of-interrupt register(VEOI)
* Virtual interrupt-service register(VISR)
* Virtual interrupt-request registrer(VIRR)
* Virtual interrupt-command register(ICR_LO)
* Virtual interrupt-command register(ICR_HI)

29.1.2 TPR Virtualization
处理器在下列操作中需要进行`TPR虚拟化`:
(1) 对MOV to CR8指令的虚拟化
(2) 对APIC-access page 的80H偏移的写操作虚拟化
(3) 对WRMSR指令并且ECS=808H
Section 29.3, Section 29.4.3和Section 29.5细节描述了上述行为.

下面位代码细节描述了TPR虚拟化过程：

```
IF “virtual-interrupt delivery” is 0
	THEN
		IF VTPR[7:4] < TPR threshold (see Section 24.6.8)
			THEN cause VM exit due to TPR below threshold;
		FI;
	ELSE
		perform PPR virtualization (see Section 29.1.3);
		evaluate pending virtual interrupts (see Section 29.2.1);
FI;
```

任何有TPR 虚拟化造成的VM exit是类似于trap的方式：在VM exit发生之前，导致TPR虚拟化的指令
会completes（举例来说，在VMCS中的guest-state area 的CS:RIP保存的值为下一条指令的地址）

### 29.1.3 PPR Virtualization
处理器在下列操作中需要进行`PPR 虚拟化`:
(1) VM entry
(2) TPR virtualization
(3) EOI virtualization
Section 26.3.2.5 Section 29.1.2和Section 29.1.4细节描述了上述行为.

PPR虚拟化使用guest interrupt status(确切的说:SVI, Section 24.4.2)和VTPR。下面的位代码
细节描述了PPR 虚拟化的行为:
```
IF VTPR[7:4] ≥ SVI[7:4]
    THEN VPPR ← VTPR & FFH;
    ELSE VPPR ← SVI & F0H;
FI;
```

PPR虚拟化总是清空VPPR[3,1]
PPR虚拟化只由TPR虚拟化，EOI虚拟化和VM entry触发。delivery 虚拟中断也会修改VPPR，
但是是另一种方式。(Section 29.2.2).其他操作均不会修改VPPR，即使修改SVI, VISR或者
VTPR。

29.1.4 EOI Virtualization
处理器进行在下面的操作中进行`EOI 虚拟化`:
(1) 对APIC-access page偏移BOH的写操作
(2) WRMSR指令并且ECX= 80BH
Section 29.4.3和Section 29.5 描述了上述过程的细节。EOI虚拟化进发生在`virtual-interrupt
delivery` VM-execution 控制字段为1:

```
Vector ← SVI;
VISR[Vector] ← 0; (see Section 29.1.1 for definition of VISR)
	IF any bits set in VISR
	THEN SVI ← highest index of bit set in VISR
	ELSE SVI ← 0;
FI;
perform PPR virtualiation (see Section 29.1.3);
IF EOI_exit_bitmap[Vector] = 1 (see Section 24.6.8 for definition of EOI_exit_bitmap)
	THEN cause EOI-induced VM exit with Vector as exit qualification;
	ELSE evaluate pending virtual interrupts; (see Section 29.2.1)
FI;
```
和TPR虚拟化一样，由EOI虚拟化造成的VM-exit是类似于trap的方式。

### 29.1.5 Self-IPI Virtualization
处理器在下面的操作中进行`self-IPI 虚拟化`:
(1) 对APIC-access page偏移300H的写操作
(2) 执行WRMSR指令并且ECX=83FH
Section 29.4.3和29.5细节描述了上述行为。Self-IPI虚拟化只发生在`virtual-interrupt 
delivery`控制字段为1。

每个导致self-IPI虚拟化的操作提供了一个8-bit的vector(Section 29.4.3 和29.5).
Self-IPI虚拟化guest interrupt status (RVI)。下面的位代码细节描述了self-IPI虚拟化的
行为:
```
VIRR[Vector] ← 1; (see Section 29.1.1 for definition of VIRR)
RVI ← max{RVI,Vector};
evaluate pending virtual interrupts; (see Section 29.2.1)
```

## 29.2 EVALUATION AND DELIVERY OF VIRTUAL INTERRUPTS

如果`virtual-interrupt delivery`VM-execution控制字段为1, 某些在VMX non-root的操作或者
在VM entry过程中将导致处理器去模拟并delivery 虚拟中断。

对虚拟中断的evaluation 是由对virtual-APIC page中的某些字段改变的行为导致的。在29.2.1中
描述了这些行为。这些evaluation可能导致识别一个virtual interrupt.一旦虚拟中断被识别，处理器
可能deliver it在VMX non-root操作模式下，并且没有VM exit。Virtual-interrupt delivery 在29.2.2
中描述。

### 29.2.1 Evaluation of Pending Virtual Interrupts

如果`virtual-interrupt delivery` VM-execution 控制字段为1，某些行为将导致逻辑处理器去
`evalute pending virtual interrupts`.

下面的这些行为将导致evalute pending virtual interrupt:
* VM entry 						(Section 26.3.2.5)
* TPR virtualization 			(Section 29.1.2)
* EOI virtualization			(Section 29.1.4)
* self-IPI virtualization		(Section 29.1.5)
* posted-interrupt processing	(Section 29.6)

没有其他的操作可以导致evaluation of pending virtual interrupts, 即使修改RVI or VPPR。

Evaluation of pending virtual interrupts使用guest itnerrupt status(RVI).下面是
evaluation of pending virtual interrupts的伪代码:
```
IF “interrupt-window exiting” is 0 AND
RVI[7:4] > VPPR[7:4] (see Section 29.1.1 for definition of VPPR)
		THEN recognize a pending virtual interrupt;
	ELSE
		do not recognize a pending virtual interrupt;
FI;
```
一旦被识别，虚拟中断可能在VMX non-root 操作模式下delivered. Section 29.2.2

逻辑处理器在delivery 一个虚拟中断后，停止recognition of a pending virtual 
interrupt

### 29.2.2 Virtual-Interrupt Delivery

一个虚拟中断被识别后(Section 29.2.1),  当下列所有的条件被满足时， 它将被
deliverd在一个intstruction boundary(一个指令周期？) :
(1) RFLAGS.IF=1
(2) there is no blocking by STI
(3) there is no blocking by MOV SS or by POP SS
(4) the `interrupt-window exiting` VM-execution control is 0

Virtual-interrupt delivery和由于设置了`interrupt-window exiting`VM-execution 
控制字段为1产生VM-exit具有想统的优先级。因此，NMIs和更高优先级的事件比起delivery
虚拟中断有更高的优先级，delivery 一个虚拟中断将比外部中断和更低优先级事件有更高的
优先级。

Virtual-interrupt delivery可以像外部中断一样将inactive activity状态逻辑处理器环境。
确切的说，它唤醒一个逻辑处理器从使用HIL和MWAIT指令进入的状态。它不能唤醒一个逻辑处理器
在shutdown state或者wait-for-SIPI state. 

Virtual-interrupt delivery更新了guest interrupt status(both RVI 和SVI)并且delivers一个event
在VMX non-root操作模式下并且不会导致VM exit.下面是这一行为的解码：
```
Vector <← RVI;
VISR[Vector] <← 1;
SVI <← Vector;
VPPR <← Vector & F0H;
VIRR[Vector] <- 0;
IF any bits set in VIRR
	THEN RVI <← highest index of bit set in VIRR
	ELSE RVI <← 0;
FI;
deliver interrupt with Vector through IDT;
cease recognition of any pending virtual interrupt;
```

如果一个逻辑处理器是enclave mode，AEX 将会在delivery of virtual interrupt之前发生(Chapter 39)

### 29.3 VIRTUALIZING CR8-BASED TPR ACCESSES
在64-bit模式中，软件可以通过CR8来访问local APIC's task-priority register(TPR). 确切来说，软件使用
MOV from CR8和MOV to CR8 instruction(Section 10.8.6). 本章节将描述这些访问如何被虚拟化的。


