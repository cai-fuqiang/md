# 问题现象
在客户环境拔插网线后，bmc console会打印如下内容:

![ixgbe ecc err](pic/4-level-pgtable.png)

# 问题调查
## 查看kernel代码
查看kernel代码, 有两个函数会调用到分别是:
```
static irqreturn_t ixgbe_intr(int irq, void *data);
static irqreturn_t ixgbe_msix_other(int irq, void *data)
```
这两个函数都是中断的回调:
```
static int ixgbe_request_msix_irqs(struct ixgbe_adapter *adapter)
{
	...
	for (vector = 0; vector < adapter->num_q_vectors; vector++) {
		...
		err = request_irq(entry->vector, &ixgbe_msix_clean_rings, 0,
                	q_vector->name, q_vector);
		...
	}
	...
	err = request_irq(adapter->msix_entries[vector].vector,
	                ixgbe_msix_other, 0, netdev->name, adapter);
	...
}
static int ixgbe_request_irq(struct ixgbe_adapter *adapter)
{
	...
	if (adapter->flags & IXGBE_FLAG_MSIX_ENABLED)
	        err = ixgbe_request_msix_irqs(adapter);
	else if (adapter->flags & IXGBE_FLAG_MSI_ENABLED)
	        err = request_irq(adapter->pdev->irq, ixgbe_intr, 0,
	                          netdev->name, adapter);
	else
	        err = request_irq(adapter->pdev->irq, ixgbe_intr, IRQF_SHARED,
	                          netdev->name, adapter);
	...
}
```
我们只看下msix, 可以看到，这个中断不对应收队列, 而是属于其他类型的中断，
如果是收包中断的话，会调用clean rings。

我们只看下`ixgbe_msix_other`代码
### ixgbe_msix_other
```
static irqreturn_t ixgbe_msix_other(int irq, void *data)
{
        struct ixgbe_adapter *adapter = data;
        struct ixgbe_hw *hw = &adapter->hw;
        u32 eicr;

        /*
         * Workaround for Silicon errata.  Use clear-by-write instead
         * of clear-by-read.  Reading with EICS will return the
         * interrupt causes without clearing, which later be done
         * with the write to EICR.
         */
	//读这个寄存器相当于读EICR
        eicr = IXGBE_READ_REG(hw, IXGBE_EICS);

        /* The lower 16bits of the EICR register are for the queue interrupts
         * which should be masked here in order to not accidentally clear them if
         * the bits are high when ixgbe_msix_other is called. There is a race
         * condition otherwise which results in possible performance loss
         * especially if the ixgbe_msix_other interrupt is triggering
         * consistently (as it would when PPS is turned on for the X540 device)
         */
	//这里主要避免该流程对收包产生影响
	//commit : d87d830720a1446403ed38bfc2da268be0d356d1
        eicr &= 0xFFFF0000;
	//写1的相当于清空, 写0相当于没写
	IXGBE_WRITE_REG(hw, IXGBE_EICR, eicr);
	...
	switch (hw->mac.type) {
	case ixgbe_mac_82599EB:
	case ixgbe_mac_X540:
	case ixgbe_mac_X550:
	case ixgbe_mac_X550EM_x:
	case ixgbe_mac_x550em_a:
		...
		if (eicr & IXGBE_EICR_ECC) {
        		e_info(link, "Received ECC Err, initiating reset\n");
        		set_bit(__IXGBE_RESET_REQUESTED, &adapter->state);
        		ixgbe_service_event_schedule(adapter);
        		IXGBE_WRITE_REG(hw, IXGBE_EICR, IXGBE_EICR_ECC);
		}
		...
		break;
	default:
		break;
	}
	...
	return IRQ_HANDLED;
}
```
可以看到，在进入switch之前，就已经对IXGBE_EICR寄存器高16位清空，
但是在检测到IXGBE_EICR_ECC时，在执行完`ixgbe_service_event_schedule`
操作后，又对该位进行清空。我觉得这里可能是意识到在reset 过程中
可能会再次触发该类型中断，但是由于这个过程是屏蔽中断的，所以
硬件改了寄存器，但是还没发中断。这是reset完，就不需要再次处理了，
这次中断处理完成后，开中断，还是会收到此类型中断，但是由于该位
清空，就不用重复reset。

PS: 
1. IXGBE_EICR
```
Writing a 1b to any bit in the register clears it. Writing a 0b to 
any bit has no effect. The EICR is also cleared on read if GPIE.OCD 
bit is cleared. When the GPIE.OCD bit is set, 
then only bits 16...29 are cleared on read. The later setting is useful
 for MSI-X mode in which the Tx and Rx and possibly the timer interrupts 
do not share the same interrupt with the other causes. Bits in the 
register can be auto cleared depending on the EIAC register setting 
(detailed in Section7.3.1.4).
```
在kernel中，支持MSIX的设备设置了GPIE.OCD
```cpp
static void ixgbe_setup_gpie(struct ixgbe_adapter *adapter)
{
	...
	if (adapter->flags & IXGBE_FLAG_MSIX_ENABLED) {
        	gpie = IXGBE_GPIE_MSIX_MODE | IXGBE_GPIE_PBA_SUPPORT |
        	      IXGBE_GPIE_OCD;
		...
	...
}
```
2. IXGBE_ICRS
```
This register enables software to initiate a hardware interrupt. 
Setting any bit on the EICS sets its corresponding bit in the 
EICR register while bits written to 0b have no 
impact. It then causes an interrupt assertion if enabled by the 
EIMS register. Setting any bit generates either LLI or throttled 
interrupt depending on the GPIE.EIMEN setting: 
When the EIMEN bit is set, then setting the EICS register causes 
an LLI interrupt; When the EIMEN bit is cleared, then setting 
the EICS register causes an interrupt after the 
corresponding interrupt throttling timer expires.
```
ps: GPIE.EIMEN没有设置

## reset流程
```cpp
static void ixgbe_service_event_schedule(struct ixgbe_adapter *adapter)
{
        if (!test_bit(__IXGBE_DOWN, &adapter->state) &&
            !test_bit(__IXGBE_REMOVING, &adapter->state) &&
            !test_and_set_bit(__IXGBE_SERVICE_SCHED, &adapter->state))
                queue_work(ixgbe_wq, &adapter->service_task);
}
```
必须满足上面三个条件，才会调到地下的`queue_work`

`adapter->service_task`在下面函数中初始化
```cpp
static int ixgbe_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	...
	INIT_WORK(&adapter->service_task, ixgbe_service_task);
	...
}
```
可以看到，初始化为`ixgbe_service_task` 函数

`ixgbe_service_task`该函数比较长，不一次分析，主要看下面:
```cpp
static void ixgbe_service_task(struct work_struct *work)
{
	...
	ixgbe_reset_subtask(adapter);
	...
}
static void ixgbe_reset_subtask(struct ixgbe_adapter *adapter)
{
        if (!test_and_clear_bit(__IXGBE_RESET_REQUESTED, &adapter->state))
                return;

        rtnl_lock();
        /* If we're already down, removing or resetting, just bail */
        if (test_bit(__IXGBE_DOWN, &adapter->state) ||
        ¦   test_bit(__IXGBE_REMOVING, &adapter->state) ||
        ¦   test_bit(__IXGBE_RESETTING, &adapter->state)) {
                rtnl_unlock();
                return;
        }

        ixgbe_dump(adapter);
        netdev_err(adapter->netdev, "Reset adapter\n");
        adapter->tx_timeout_count++;

        ixgbe_reinit_locked(adapter);
        rtnl_unlock();
}
```
`ixgbe_reset_subtask`理论上是可以走到的（虽然前面有分支会return，但是
应该走不到），走到`ixgbe_reset_subtask`时，必然会打`Reset adapter`,
可以从用户的console 日志中看到，并没有该打印。

再回到 interrupt handler的流程，该流程只是queue work, 并没有去等
这个work完成，所以可能会在reset的过程中重复调入。至于reset的流程，
没有走到`ixgbe_reset_subtask`的打印，可能有两点原因:
* 没有调入，可能和设备的`__IXGBE_DOWN`, `__IXGBE_REMOVING`, `__IXGBE_RESETTING`有关，
  鉴于客户环境是拔了光纤导致，不知道是否和`__IXGBE_REMOVING`有关。（这个状态会在
  pci device remove的时候调到，不知道把光纤是否会走该流程，**需要找环境测试**)
* 调用了，但是该task还没跑，或者跑的慢，interrupt handler没有等，导致多次
  调入, 产生多次打印，在这个过程中，不知道什么原因，导致系统挂了（没有vmcore/calltrace，
  不好分析为什么挂掉)

# ECC error interrupt

ECC error interrupt是硬件报的，在82599 datasheet中有描述(来自
7.14.1  Memory Integrity Protection)
```

All the 82599 internal memories are protected against soft 
errors. Most of them are covered by ECC that correct single 
error per memory line and detect double errors per memory line.
 Few of the smaller memories are covered by parity protection 
that detects a single error per memory line.

Single errors in memories with ECC protection are named also
 as correctable errors. Such errors are silently corrected. 
Two errors in memories with ECC protection or single error in
 memories with parity protection are also named as un-correctable
 errors. Uncorrectable errors are considered as fatal errors.
 If an un-correctable error is detected in Tx packet data, 
the packet is transmitted with a CRC error. If un-correctable 
error is detected in Rx packet data, the packet is reported 
to the host (or manageability) with a CRC error. If an un-correctable 
error is detected anywhere else, the 82599 halts the 
traffic and sets the ECC error interrupt. Software is then 
required to initiate a complete initialization cycle to 
resume nominal operation.
```
有两种错误，一种ECC机制能恢复，另一种(two errors) ecc不能恢复。

