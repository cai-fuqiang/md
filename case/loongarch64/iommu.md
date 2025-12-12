## 环境描述
* cmdline
  ```
  BOOT_IMAGE=/vmlinuz-6.6.0-97.0.0.102.oe2403sp2.loongarch64 
  root=UUID=03d031db-430b-446b-af4e-5090765d85c7 
  ro resume=UUID=f578fdc3-d268-4440-b9bb-1f5d6704633b 
  video=efifb:off cgroup_disable=files apparmor=0 
  console=tty0 hugepagesz=32M hugepages=2048 
  vfio_iommu_type1.allow_unsafe_interrupts=1 nokaslr
  ```

dmesg打印:
```
[    0.784436] iommu: Default domain type: Passthrough
```
并没有iommu group相关打印, 看起来没有初始化iommu.

## 使用dpdk 绑定vfio-pci报错
```
[root@localhost devices]# /usr/local/bin/dpdk-devbind.py --bind=vfio-pci 0002:01:00.1
Error: IOMMU support is disabled, use --noiommu-mode for binding in noiommu mode
```

查看`dpdk-devbind.py`代码发现:
```python
def has_iommu():
    """Check if IOMMU is enabled on system"""
    return len(os.listdir("/sys/class/iommu")) > 0
```

是查看`/sys/class/iommu`目录，而在loongarch环境中该目录并没有文件:
```
[root@localhost devices]# ls /sys/class/iommu/
[root@localhost devices]#
```

> x86中:
>
> ```
> [root@A06-R08-I134-73-919XB72 rocky-9.0]# ls /sys/class/iommu/
> dmar0  dmar1
> ```
>
> 该目录是在使能`intel_iommu=on`  后才会加上

怀疑loongarch64 也有这样的参数.

## 查看代码
```sh
loongarch_iommu_driver_init
=> if loongarch_iommu_disable == 0
   => ret = loongarch_iommu_ivrs_init();
      => init_iommu_all(ivrs_base);
         => iommu = kzalloc(sizeof(struct loongarch_iommu),
               GFP_KERNEL);
         => init_iommu_one(iommu, h);
```
可以看到只有`loongarch_iommu_disable == 0` 时，才会初始化iommu
```cpp
static int __init la_iommu_setup(char *str)
{
    if (!str)
        return -EINVAL;
    while (*str) {
        if (!strncmp(str, "on", 2)) {
            loongarch_iommu_disable = 0;
            pr_info("IOMMU enabled\n");
        } else if (!strncmp(str, "off", 3)) {
            loongarch_iommu_disable = 1;
            pr_info("IOMMU disabled\n");
        }
        str += strcspn(str, ",");
        while (*str == ',')
            str++;
    }
    return 0;
}
__setup("loongarch_iommu=", la_iommu_setup);
```
通过设置`loongarch_iommu=on` 打开iommu , 但是设置`loongarch_iommu=on`并不能打开
iommu，原因是其所在的代码，是以模块编译的，在`include/init.h`中, 如果是以模块编译
则`__setup`宏为空:
```cpp
#define __setup_param(str, unique_id, fn)   /* nothing */
#define __setup(str, func)          /* nothing */
```

将`CONFIG_LOONGARCH_IOMMU` 设置为 `y` 重新编译内核。 启动后发现内核疑似挂死在早
期:

```
[Bds]Booting UEFI Hard Drive 2
 BlockSize : 512 
 LastBlock : 37E436AF 
 Valid efi partition table header
 Valid efi partition table header
 Valid primary and Valid backup partition table
 Partition entries read block success
 Number of partition entries: 128
 start check partition entries
 End check partition entries
 Index : 0
 Start LBA : 800
 End LBA : 12C7FF
 Partition size: 12C000
 Start : 100000 End : 258FFE00
 Index : 1
 Start LBA : 12C800
 End LBA : 32C7FF
 Partition size: 200000
 Start : 25900000 End : 658FFE00
 Index : 2
 Start LBA : 32C800
 End LBA : 222407FF
 Partition size: 21F14000
Loading Linux 6.6.0-98.0.0.103.loongarch64 ...
Loading initial ramdisk ...
Trying to terminate EFI services again
XhcClearBiosOwnership: called to clear BIOS ownership
DisableFloatExitBootServicesEventNotify
```
查看openeuler社区和 龙蜥社区，`CONFIG_LOONGARCH_IOMMU`都设置的`m`.

但是在下面commit 中，将 `loongarch_iommu_disable` 更改为1， 这样的修复方式真的
6.

```
commit 7e640142ed329d136ee5d8aad89fe14226330abb
Author: Xianglai Li <lixianglai@loongson.cn>
Date:   Tue Jun 24 17:32:27 2025 +0800

    drivers/iommu: Fix the initialization failure issue of the iommu module

    LoongArch inclusion
    category: feature
    bugzilla: https://gitee.com/openeuler/kernel/issues/ICOJKT

    ------------------------------------------

    Add support for the def_domain_type callback function.

    Link:https://gitee.com/openeuler/kernel/pulls/17271
    Signed-off-by: Xianglai Li <lixianglai@loongson.cn>

diff --git a/drivers/iommu/loongarch_iommu.c b/drivers/iommu/loongarch_iommu.c
index 038438b06281..8cfd36878e50 100644
--- a/drivers/iommu/loongarch_iommu.c
+++ b/drivers/iommu/loongarch_iommu.c
@@ -109,7 +109,7 @@ u16 la_iommu_last_bdf;                      /* largest PCI device id
                                                 *  we have to handle
                                                 */

-int loongarch_iommu_disable = 1;
+int loongarch_iommu_disable;
```
