## 现象

龙芯平台，源端热迁移，出现crash:

```
363768@[2026-02-25 18:32:33.780067] vmstate_save fw_cfg, fw_cfg
363768@[2026-02-25 18:32:33.780081] savevm_section_end fw_cfg, section_id 11 -> 0
363768@[2026-02-25 18:32:33.780088] savevm_section_start loongarch-ipi-kvm, section_id 12
363768@[2026-02-25 18:32:33.780095] vmstate_save loongarch-ipi-kvm, loongarch-ipi-kvm
Unexpected error in kvm_device_access() at ../accel/kvm/kvm-all.c:3217:
2026-02-25T10:32:33.789292Z qemu-system-loongarch64: KVM_GET_DEVICE_ATTR failed: Group 1073741825 attr 0x0000000000020000: Invalid argument
2026-02-25 10:32:34.249+0000: shutting down, reason=crashed
```

qemu直接挂死. kernel 中有如下报错:

```
[1128737.710138] kvm [363768]: kvm_loongarch_ipi_regs_access: invalid target cpu: 2
```

## 分析

查看kernel 代码:

```cpp
static int kvm_loongarch_ipi_regs_access(struct kvm_device *dev,
    struct kvm_device_attr *attr,
    bool is_write)
{
  ...
 vcpu = kvm_get_vcpu(dev->kvm, cpu);
 if (unlikely(vcpu == NULL)) {
  kvm_err("%s: invalid target cpu: %d\n", __func__, cpu);
  return -EINVAL;
 }
  ...
}
```

可以发现调用`kvm_get_vcpu()`执行失败. 未找到该cpu index 的cpu, 而该cpu index为`2`

查看虚拟机配置:

```
  <vcpu placement='static' cpuset='0-1' current='2'>128</vcpu>
```

配置两个cpu，但是支持cpu热插拔，最大到`128`

编写如下bpftrace做调试:

```
#include "kvm_host.h"
kprobe:kvm_loongarch_ipi_regs_access.isra.0
{
 $p = (struct kvm_device_attr *)arg1;
 printf("enter kvm_loongarch_ipi_regs_access cpu  = %d\n",
        ($p->attr >> 16) & 0x3ff);
 printf("enter kvm_loongarch_ipi_regs_access attr = %u\n",
        $p->attr >> 16);
}

uprobe:/usr/bin/qemu-system-loongarch64:kvm_device_access
{
 $group = arg1;
 $attr = arg2;
 printf("group %u attr %lu\n", $group, $attr);
}
```

执行热迁移后，bpftrace打印如下:

```
group 1073741825 attr 0
enter kvm_loongarch_ipi_regs_access cpu  = 0
enter kvm_loongarch_ipi_regs_access attr = 0
group 1073741825 attr 4
enter kvm_loongarch_ipi_regs_access cpu  = 0
enter kvm_loongarch_ipi_regs_access attr = 0
group 1073741825 attr 8
enter kvm_loongarch_ipi_regs_access cpu  = 0
enter kvm_loongarch_ipi_regs_access attr = 0
group 1073741825 attr 12
enter kvm_loongarch_ipi_regs_access cpu  = 0
enter kvm_loongarch_ipi_regs_access attr = 0
group 1073741825 attr 32
enter kvm_loongarch_ipi_regs_access cpu  = 0
enter kvm_loongarch_ipi_regs_access attr = 0
group 1073741825 attr 40
enter kvm_loongarch_ipi_regs_access cpu  = 0
enter kvm_loongarch_ipi_regs_access attr = 0
group 1073741825 attr 48
enter kvm_loongarch_ipi_regs_access cpu  = 0
enter kvm_loongarch_ipi_regs_access attr = 0
group 1073741825 attr 56
enter kvm_loongarch_ipi_regs_access cpu  = 0
enter kvm_loongarch_ipi_regs_access attr = 0
group 1073741825 attr 65536
enter kvm_loongarch_ipi_regs_access cpu  = 1
enter kvm_loongarch_ipi_regs_access attr = 1
group 1073741825 attr 65540
enter kvm_loongarch_ipi_regs_access cpu  = 1
enter kvm_loongarch_ipi_regs_access attr = 1
group 1073741825 attr 65544
enter kvm_loongarch_ipi_regs_access cpu  = 1
enter kvm_loongarch_ipi_regs_access attr = 1
group 1073741825 attr 65548
enter kvm_loongarch_ipi_regs_access cpu  = 1
enter kvm_loongarch_ipi_regs_access attr = 1
group 1073741825 attr 65568
enter kvm_loongarch_ipi_regs_access cpu  = 1
enter kvm_loongarch_ipi_regs_access attr = 1
group 1073741825 attr 65576
enter kvm_loongarch_ipi_regs_access cpu  = 1
enter kvm_loongarch_ipi_regs_access attr = 1
group 1073741825 attr 65584
enter kvm_loongarch_ipi_regs_access cpu  = 1
enter kvm_loongarch_ipi_regs_access attr = 1
group 1073741825 attr 65592
enter kvm_loongarch_ipi_regs_access cpu  = 1
enter kvm_loongarch_ipi_regs_access attr = 1
group 1073741825 attr 131072
enter kvm_loongarch_ipi_regs_access cpu  = 2
enter kvm_loongarch_ipi_regs_access attr = 2
```

可以发现 `cpu 0, 1` 未报错，但是cpu 2 报错了。其实也好理解, 未热插拔的cpu
 (只有slot), 在kvm中 未 create vcpu。 所以会报错。

所以原因是，qemu `loongarch-ipi-kvm`, 不应该获取`cpu 2`的状态。查看相关qemu代码.

## qemu 代码分析

`loongarch_ipi` 注册了`vmstate` 用作热迁移状态保存:

```cpp
static const VMStateDescription vmstate_kvm_loongarch_ipi = {
    .name = TYPE_KVM_LOONGARCH_IPI,
    .version_id = 1,
    .minimum_version_id = 1,
    .pre_save = kvm_loongarch_ipi_pre_save,
    .post_load = kvm_loongarch_ipi_post_load,
    .fields = (VMStateField[]) {
        VMSTATE_STRUCT_VARRAY_POINTER_UINT32(cpu, KVMLoongArchIPI, num_cpu,
                         vmstate_kvm_ipi_core, IPICore),

        VMSTATE_END_OF_LIST()
    }
};

typedef struct IPICore {
    uint32_t status;
    uint32_t en;
    uint32_t set;
    uint32_t clear;
    /* 64bit buf divide into 2 32bit buf */
    uint32_t buf[IPI_MBX_NUM * 2];
    qemu_irq irq;
} IPICore;

```

其保存了每个cpu的IPI状态，所以其申请了一个数组，而需要注意，这个数组大小是固定的，
所以其数组大小直接设置为`KVMLoongArchIPI->num_cpu`大小。该大小为`maxcpus`.

其中`kvm_loongarch_ipi_pre_save` 负责保存:

```sh
kvm_loongarch_ipi_pre_save
=> KVMLoongArchIPI *ipi = (KVMLoongArchIPI *)opaque;
=> for (cpu_id = 0; cpu_id < ipi->num_cpu; cpu_id++)
   => attr = (cpu_id << 16) | CORE_STATUS_OFF;
   => kvm_ipi_access_regs(fd, attr, &cpu->status, false);
   => ...
   => //other attr
```

其会根据`num_cpu`, 对每个cpu进行获取（前面提到问题就出现在这）。所以应该修改为
仅在线的cpu。

## 解决方法

openeuler upstream 提供了patch:

```
commit e0bdec747ce4179ad1c0d0865a7d8f0ea4a3f380
Author: lizhanchun <lizhanchun@kylinos.cn>
Date:   Fri Dec 19 13:56:07 2025 +0800

    hw/intc/loongarch_ipi_kvm: Fix the bug where the VM would crash when performing save or resume operations.
```

大概的修改方法为，在循环时会判断该cpu是否在kvm中注册(也就是create vcpu), 如果没有，则跳过该cpu:

```diff
 static int kvm_loongarch_ipi_pre_save(void *opaque)
 {
 ...
     for (cpu_id = 0; cpu_id < ipi->num_cpu; cpu_id++) {
+        cpu_state = qemu_get_cpu(cpu_id);
+        if(!cpu_state || !cpu_state->kvm_fd) {
+            continue;
+        }

 ...
```

而post_load时也是类似.
