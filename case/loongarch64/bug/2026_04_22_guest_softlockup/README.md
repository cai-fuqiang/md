## 问题现象
guest 触发softlockup, 由于softlockup时间过长，各个cpu触发的softlockup
堆栈已经毫无意义.

从hyp(`qemu-monitor-command`) 调试, 获取`info register -a`

[info_register_a](./info_register_a.txt)

执行多次, `info register -a`, 有以下共性:
* `PC`: `90000000015d3f40`
* `TVAL`: `0000ffffffffffff`
* `ESTAT`: `0000000000001808`

(还有一些其他的，由于对龙芯架构不熟悉，暂不贴出)

## ESTAT

`ESTAT`值为, `0x1808` :

使能为1 的bitmap:
* `bit 3`: 定时器中断 `TI`
* `bit 12, 11`: irq 8, irq 7


所以, 这里pending了定时器中断 (另外, TVAL 被使能为 `0000ffffffffffff`, 难道说明
hw timer 触发时钟中断后，`TVAL` 被初始化为`0000ffffffffffff`, 如果不restart，该值
一直不变?)

## loongarch 提供的patch

龙芯提供解决问题patch如下:

该patch的解决的问题，大概总结如下:
* 在负载较高的机器上, 会遇到`SIP0/IPI` 发送比较频繁
* 龙芯似乎引入了 vtimer, 也就是配置vtimer后，vtimer到期后，自己似乎能
  inject interrupt
* 如果vtimer到期后, 在kvm测, vtimer这时不会把中断直接打入host，而是更新`ESTAT/TVAL`，
  进入guest时，可能会自动注入interrupt, 或者手动注入(这块得在看下)
* 那问题是，如果hw vtimer到期，则自动修改guest `ESTAT`将`TI` bit置位, 而kvm模拟 `SIP0/IPI`
  注入过程可能会miss 刚刚被注入的`TI`

解决的patch在:
```diff
diff --git a/arch/loongarch/kvm/interrupt.c b/arch/loongarch/kvm/interrupt.c
index f67dddfec7d0..422515744dc0 100644
--- a/arch/loongarch/kvm/interrupt.c
+++ b/arch/loongarch/kvm/interrupt.c
@@ -27,6 +27,7 @@ static unsigned int priority_to_irq[EXCCODE_INT_NUM] = {
 static int kvm_irq_deliver(struct kvm_vcpu *vcpu, unsigned int priority)
 {
 	unsigned int irq = 0;
+	unsigned long old, new;
 
 	clear_bit(priority, &vcpu->arch.irq_pending);
 	if (priority < EXCCODE_INT_NUM)
@@ -41,7 +42,20 @@ static int kvm_irq_deliver(struct kvm_vcpu *vcpu, unsigned int priority)
 	case INT_IPI:
 	case INT_SWI0:
 	case INT_SWI1:
                //在更新 gcsr 时，先获取 TVAL
+		old = kvm_read_hw_gcsr(LOONGARCH_CSR_TVAL);
                //更新estat
 		set_gcsr_estat(irq);
                //再获取TVAL
+		new = kvm_read_hw_gcsr(LOONGARCH_CSR_TVAL);
+		/*
+		 * On some platforms, the passthrought HW timer interrupt
+		 * is lost when inject CPU interrupt by software if HW
+		 * timer interrupt is arriving.
+		 *
+		 * Here check whether there is inversion with timer tick
+		 * value, inject timer interrupt by SW if so.
+		 */
+
                //如果新的TVAL比old 大，说明其已经rollback, 那就意味着在这个过程中
                //有时钟中断到guest，则手动写入
+		if (new > old)
+			set_gcsr_estat(CPU_TIMER);
 		break;
 
 	case INT_HWI0 ... INT_HWI7:
@@ -58,6 +72,7 @@ static int kvm_irq_deliver(struct kvm_vcpu *vcpu, unsigned int priority)
 static int kvm_irq_clear(struct kvm_vcpu *vcpu, unsigned int priority)
 {
 	unsigned int irq = 0;
+	unsigned long old, new;
 
 	clear_bit(priority, &vcpu->arch.irq_clear);
 	if (priority < EXCCODE_INT_NUM)
@@ -72,7 +87,11 @@ static int kvm_irq_clear(struct kvm_vcpu *vcpu, unsigned int priority)
 	case INT_IPI:
 	case INT_SWI0:
 	case INT_SWI1:
+		old = kvm_read_hw_gcsr(LOONGARCH_CSR_TVAL);
 		clear_gcsr_estat(irq);
+		new = kvm_read_hw_gcsr(LOONGARCH_CSR_TVAL);
+		if (new > old)
+			set_gcsr_estat(CPU_TIMER);
 		break;
 
 	case INT_HWI0 ... INT_HWI7:
```

另外，个人感觉这其实是一个硬件bug, x86中有软硬件同时更新一个memory 地址的需求: `VMX PID`, 其可以规定好一个
写入协议，另外通过`atomic` 来保证写入race。但似乎loongarch并没有这么做。

另外，如果触发了该问题，会导致guest 看来，某个cpu迟迟收不到timer interrupt. 从而导致tick不会触发，也就没有办法
迭代rcu。所以出现了`rcu timeout`问题.

**但是, 当前环境遇到的问题是, timer 中断实际上是pending的。cpu 0 无响应。并且PC 始终为一个值, 个人判断不是
一个问题**

## 参考链接
1. [\[PATCH 0/3\] LoongArch: KVM: Small enhancement about timer](https://lore.kernel.org/all/20260414072313.3801110-1-maobibo@loongson.cn/)
