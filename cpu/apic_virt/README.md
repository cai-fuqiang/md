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

VMM可以虚拟化CR8-based APIC access通过设置`CR8-load exiting`和`CR8-store exiting`VM-execution 控制位，
确保这些访问可以导致VM exits(Section 25.1.3). 或者，这里有一些方法用来虚拟化某些CR8-based APIC accesses
并且没有VM exits.

正常来说，执行MOV from CR8或者MOV to CR8不会造成fault or cause a VM exit accesses the
APIC's TPR.但是，如果设置了`use TPR shadow` VM-execution 控制位为1， 这个执行将会
被特殊处理。下面介绍这些细节：
* MOV from CR8. 这个指令使用 源操作数VTPR 的 7:4 加载目的操作数的3:0 (Section 29.1.1)
目的操作数的63:4位将会被清空
* MOV to CR8. 这个指令使用源操作数0:3 加载到VTPR的7:4; VTPR的剩余部分(3:0 和 31:8)将会被
清空。接下来，处理器将会进行TPR virtualization(Section 29.1.2)


## 29.4 VIRTUALIZING MEMORY-MAPPED APIC ACCESSES
(略)

## 29.5 VIRTUALIZING MSR-BASED APIC ACCESSES
(略)

## 29.6 POSTED-INTERRUPT PROCESSING
Posted-interrupt processing是处理器通过pending on the virtual-APIC page
记录虚拟中断的一个特性。

Posted-interrupt processing在设置`process posted interrupt` VM-execution
控制字段后使能。这个过程的执行是为了回应当一个带着`posted-interrupt nofication 
vector`的中断到达。在回应这样一个中断时，处理器处理记录在一个数据结构的virtual 
interrupts, 该数据结构称作`posted-interrupt descriptor`. posted-interrupt nofication
vector和posted-interrupt descriptor的地址都时VMCS中的字段。See Section 24.6.8


如果`process posted interrupts`VM-execution控制字段为1，逻辑处理器使用一个64-byte
的posted-interrupt descriptor，该描述符记录在posted-interrupt descriptor address.
这个posted-interrupt descriptor有以下的格式:
|Bit Position(s)|Name|Description|
|----|----|----|
|255:0|Posted-interrupt requestes|(1)|
|256|Outstanding nofication|(2)|
|511:257|Reserved for software and other agents|(3)|

(1) 对于每个interrupt vector有一个bit。如果某一位为1，则表明这里有一个对应
vector的posted-interrupt request<br/>
(2) 如果改为被设置，则表明在bit 255:0 中存在一个或多个posted interrupts为完成
notification<br/>
(3) 这些位用于软件和系统中其他的agents(例如: chipset). 处理器不会修改这些bits

the notation PIR(posted-interrupt requests)指的是在posted-interrupt descriptor
中的256 posted-interrupt bits

使用posted-interrupt descriptor不同于VMCS中其他引用的数据结构。这里有一个一般
性的需求：软件会确保只有在没有逻辑处理器的current VMCS在VMX non-root操作模式
下被引用的时候才可以修改该其中的数据结构。这个需求不适用于posted-interrupt descriptor.
当然，这有一个另外的需求，这些modifications必须在locked read-modify-write指令的下完成。

如果`external-interrupt exiting` VM-execution控制字段为1，任何未屏蔽的外部中断将
导致一个VM exit(see Section 25.2).如果`process posted interrupts`VM-execution 控制字段也
是1，这个行为将会改变，处理器处理一个外部中断的过程如下：
1. 如果local APIC is acknowledaged;它提供给处理器core一个interrupt vector, 这里成这个vector
为`physical vector`
2. 如果physical vector 和posted-interrupt notification vector相等，逻辑处理器将
进行下一个步骤。否则，VM exit将会以接收到外部中断的情况触发VM exit;该vector被保存在
VM-exit interruption-information 字段
3. 处理器清空posted-interrupt descriptor 中的outstanding-nofication bit。该处理过程
是原子的以确保描述符剩余部分是不会被修改的。
4. 处理器向local APIC中的EOI register写入0; 取消来自local APIC的posted-interrupt
nofication vector
5. logical processor将PIR逻辑或到VIRR并且清空PIR。没有其他的agent可以在这次的操作过程中
(逻辑或和清空)read or write a PIR bit(or group of bits)
6. 逻辑处理器将RVI设置成老的RVI和PIR中最高级别的位的最大值；如果PIR中没有设置任何bit，
RVI不会被修改。
7. 逻辑处理器如在Section 29.2.1 描述那样去evaluates pending virtual interrupts.

逻辑处理器在不可中断的状态下执行上面的步骤，如果步骤#7 会recognition of a virtual
interrupt, 处理器将立刻deliver  the interrupt

步骤#1 到 #7 发生在interrupt controller delivers 一个未屏蔽的外部中断到CPU core。
下面的条目考虑了interrupt delivery几种情况:
* interrupt delivery 可以发生在REP-prefixed指令的iterations中(至少在某个iteration已经
完成后，在所有的iterations完成之前)。如果这发生了这种情况, 下面的条目描述了在
posted-interrupt processing完成后, guest执行恢复前的一些操作:
```
    -- RIP指向REP-prefixed指令
    -- RCX, RSI ,RDI都更新到本次iterations 完成后的情况
    -- RFLAGS.RF=1
``` 
* interrupt delivery可以发生在当处理其处于active, HLT, 或者MWAIT的状态。
如果逻辑处理器在收到中断到达之前已经在active 或者 MWAIT的状态，在完成#7步骤
后将处于active 状态；如果当时处于HLT状态，他将在完成步骤#7后转会HLT状态(如果
一个pending virtual interrupts被recognized, 逻辑处理器将会立即从HLT状态中
唤醒)

* interrupt delivery可以发生在逻辑处理器处于enclave状态。如果逻辑处理器在收到
中断之前已经处于enclave状态, Asynchronous  Enclave Exit(AEX)将会在step #1到#7之间
发生（Chapter 36).如果在step #1之前没有发生AEX并且在step #2之前没有发生 VM-exit，
AEX将发生在VM exit delivered之前。

