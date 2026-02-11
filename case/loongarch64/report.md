## loongarch测试

* 性能
  * 目前unixbench 测试，在开启pv-ipi的情况下，损耗仍较高:
    * 单核7%, 多核25%
  * [unixbench性能测试细节](./perf/README.md)  中的
    `UNIXBENCH/综合跑分虚拟化损耗` 章节
  * TODO
    * [ ] 目前龙芯工程师发现是openeuler 2403 并未合入 `irqchip in kvm` 相关patch，可能会导致性能问题,
          待合入patch 再次做测试。
* 稳定性问题
  * 内核
    * [ ] 多次重启后，重启卡住，进不去任何系统（包括bios setup)
    * [ ] 更换kernel 为 `kernel-6.6.0-117.0.0.122.oe2403sp2` 后，仍然出现了 `megaraid_sas`报错 [details](./bug/2026_02_09_kernel__megaraid_sas.md)
    * [X] KVM ioeventfd bug, 现象是疑似 多个vm之间io eventfd batch (de)assign 出现race [details](./bug/2026_02_09_kernel__ioeventfd.md)
    * [ ] crash 命令解析 指令地址和所在的函数符号不匹配(龙芯已经定位到问题，待出包解决)
  * qemu
    * [ ] 热迁移后，源端异常，virsh 命令卡住. [details](./bug/2026_02_09_qemu__migrate_hung_source.md)
      * [X] 问题定位(定位到是openeuler上游bug，upstream已经解决)
      * [ ] 反馈openeuler
    * [ ] 热迁移后，源端异常后，目的端也异常, virsh 命令卡住. [details](./bug/2026_02_09_qemu__migrate_hung_dest.md)
  * 其他
    * [ ] gcore 命令获取的coredump堆栈信息打印不全

## 目前采用的软件版本

* **kernel** : kernel-6.6.0-117.0.0.122.oe2403sp2.src.rpm
* **qemu**: qemu-8.2.0-37.oe2403sp2.src.rpm
