## loongarch测试

* 性能
  * 目前unixbench 测试，在开启pv-ipi的情况下，损耗仍较高:
    * 单核7%, 多核25%
  * [unixbench性能测试细节](./perf/README.md)  中的
    `UNIXBENCH/综合跑分虚拟化损耗` 章节
* 稳定性问题
  * 内核
    * [ ] 多次重启后，重启卡住，进不去任何系统（包括bios setup)
    * [ ] 更换kernel 为 `kernel-6.6.0-117.0.0.122.oe2403sp2` 后，仍然出现了 `megaraid_sas`报错 [details](./bug/2026_02_09_kernel__megaraid_sas.md)
    * [X] KVM ioeventfd bug, 现象是疑似 多个vm之间io eventfd batch (de)assign 出现race [details](./bug/2026_02_09_kernel__ioeventfd.md)
  * qemu
    * [ ] 热迁移后，源端目的端均异常，virsh 命令卡住. [details](./bug/2026_02_09_qemu__migrate_hung.md)

## 目前采用的软件版本

* **kernel** : kernel-6.6.0-117.0.0.122.oe2403sp2.src.rpm
* **qemu**: qemu-8.2.0-37.oe2403sp2.src.rpm
