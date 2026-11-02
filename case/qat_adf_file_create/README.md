# 问题现象

`kylin` 系统启动后，`/dev` 目录会自动创建`qat_adf_ctl` 文件，而 `openeuler` 并不会

# 调查过程

从直觉上看，`/dev/` 下的文件都是由`udev` 创建，首先怀疑是不是用户态创建的该文件。

我们直接在启动过程中修改 grub 增加 `rd.break=cmdline`, 结果启动后，还是发现创建
了该文件。所以该文件的创建是内核所为

# 代码查看
```

#define DEVICE_NAME "qat_adf_ctl"
adf_register_ctl_device_driver
=> adf_chr_drv_create
   => drv_device = device_create(adf_ctl_drv.drv_class, NULL,
         MKDEV(adf_ctl_drv.major, 0),
         NULL, DEVICE_NAME);

module_init(adf_register_ctl_device_driver);
```

该模块最终被编译为:
```
obj-$(CONFIG_CRYPTO_DEV_QAT) += intel_qat.o
ccflags-y += -DDEFAULT_SYMBOL_NAMESPACE=CRYPTO_QAT
intel_qat-objs := adf_cfg.o \
  adf_isr.o \
  adf_ctl_drv.o \
```

`intel_qat`

而device_create 相关路径:
```
device_create
=> device_create_groups_vargs
   => dev->devt = devt;
   => retval = device_add(dev)
	    => if (MAJOR(dev->devt))
         \> devtmpfs_create_node(dev);
```

最终会调用 `devtmpfs_create_node()`创建设备文件

**结论，该文件是由内核加载 `intel_qat`创建**

# 进一步验证

查看kylin和openeuler 两个系统的模块加载情况
```
# kylin
[root@11-212-1-78 17:09:55 ~]# ls /sys/module/|grep intel_qat
intel_qat
[root@11-212-1-78 17:09:59 ~]# modinfo intel_qat
name:           intel_qat
filename:       (builtin)
import_ns:      CRYPTO_INTERNAL
version:        0.6.0
alias:          crypto-intel_qat
alias:          intel_qat
description:    Intel(R) QuickAssist Technology
author:         Intel
license:        Dual BSD/GPL
file:           drivers/crypto/intel/qat/qat_common/intel_qat

# openeuler
[root@11-211-129-105 17:10:35 ~]# ls /sys/module/ |grep intel_qat
[root@11-211-129-105 17:24:13 ~]# modinfo intel_qat|grep filename
filename:       /lib/modules/6.6.0-jdcloud20.sp1.dad18702e.x86_64/kernel/drivers/crypto/intel/qat/qat_common/intel_qat.ko.xz
```

可以发现kylin是builtin，而openeuler 则是编译到模块中。

# 解决方法
在cmdline 中增加 `initcall_blacklist=adf_register_ctl_device_driver` , 将其加入黑名单

# 结论
kylin 系统将 `intel_qat`模块`builtin`, 而`intel_qat`模块加载会自动创建
`qat_adf_ctl`文件


