## CPU features

CPU features 由cpuid 枚举，同时还会有一些控制寄存器来控制该功能是否开启，以及其
他的一些配置, 同时cpu 可能会增加一些指令，或者会修改一些cpu的逻辑。我们以vmx功能
为例，来简单看下:

![cpu_features_eg_vmx](./pic/cpu_features_eg_vmx.svg)

> NOTE
>
> VMX 不仅仅是增加了这些指令集，其在VMX-nonroot opeartion中增加了很多逻辑，例如
> 为了实现中断透传引入了对于虚拟机中断的中断评估, ack逻辑。

## host features && guest features

不是所有的host features都可以透传给guest, 大概的种类如下:

![host_features_types](./pic/host_features_types.svg)

1. 硬件支持虚拟化，并且不需要 vmm 模拟(这种可以无脑透传给guest)
2. 硬件支持虚拟化，但是需要 vmm 配置（这种需要看 host vmm 的支持情况)
3. 硬件不支持虚化（完全看 vmm 支不支持 emulate)


当然还有一些features host没有，例如半虚拟化features.

## 热迁移的约束

约束为:

* qemu: 版本相同
* host kernel: 版本相同
* xml配置: 完全相同
* guest cpu model和 features 相同

对于一个machine来说，主要的资源有三个:
* CPU
* memory
* IO device

memory 热迁移没有约束，IO device 是software emulate（除透传), 也没有问题。只有
cpu这边，需要vmm 控制cpuid emulate 在热迁移前后，报告给虚拟机相同的cpuid 列表.

因此, 我们需要保证，虚拟机在源端开启的features, 目的端均可以开启.

基于这个约束，完成下面两个需求

## 需求1: 支持在不同的 host上相互迁移

这个要求，在任意两个 机器上，虚拟机可以相互迁移，相当于双向迁移，我们的策略是采集
该集合的所有机器 features的最大子集。如下:

![migrate_both row](./pic/migrate_both_row.svg)

所以，该集合中所有的虚拟机均使用相同的配置.


## 需求2: 支持向超集 host上迁移

该需求, 仅需要单向迁移，用于旧机器上的虚拟机向新机器迁移，在新建虚拟机时，我们使
用 `host-model` 的方式来启动新的虚拟机。以尽量使用host feature.

![create_vm](./pic/create_vm.svg)

而在热迁移时，我们需要在目的端启动和源端相同的vcpu model 和 features, 结合vcpuid，
大概展示下流程:

![migrate_to_new_cpu](./pic/migrate_to_new_cpu.svg)

这样，经过多次迁移，host上可能存在多种类型的虚拟机:

![migrate_many_times](./pic/migrate_many_times.svg)


## 两者结合

需求1， 和需求2 均未使用`host-passthough`的方式, 可以结合使用如下:

![two set migrate](./pic/two_set_migrate.svg)

这样可以实现set A 到 set B的迁移。

可以用于下面场景:
1. 客户在2025年买了五种类型的机器(A)，现在想让虚拟机在所有机器上均可迁移
2. 客户在2030年，又购买了5种新型号的机器(B)，要做负载均衡，让老机器上的vm可以向
   新购买的机器迁移
3. 客户在2050年，又购买了5种新型号的机器. 和2030年有同样的需求.

不同物理机可运行的机器, 以及不同的虚拟机可调度的物理机如下:

![2025_2030_2050_example](./pic/2025_2030_2050_example.svg)
