## 现有问题整理

1. 使用`anolis/hygon-cloud-kernel` 启动, dmesg中打印:
   ```
   [    6.531436] ccp 0000:05:00.2: CSV3: fail to set secure memory region, CSV3 support unavailable
   ```
2. 使用`anolis/hygon-cloud-kernel` + openEuler 24.03 sp1 qemu,edk2-ovmf, 
   并使用
   ```
   -object sev-guest,id=sev0,policy=0x5,cbitpos=47,reduced-phys-bits=5
                                    ^^^
   ```
   测试虚拟机内存加密，虚拟机寄存器加密，均没有问题.

   但是虚拟机dmesg 中并不打印CSV3
   ```
   [    0.426140]  HYGON CSV
   [    0.426141]  HYGON CSV2
   ```

   使用<sup>1</sup>
   ```
   -object sev-guest,id=sev0,policy=0x45,cbitpos=47,reduced-phys-bits=5
   ```
   测试，会报如下错误:
   ```
   2025-02-26T10:30:50.114156Z qemu-system-x86_64: sev_launch_start: LAUNCH_START ret=1 fw_error=24 'Part-specific integrity check failure'
   2025-02-26T10:30:50.114207Z qemu-system-x86_64: sev_kvm_init: failed to create encryption context
   2025-02-26T10:30:50.117151Z qemu-system-x86_64: failed to initialize kvm: Operation not permitted 
   ```
3. openEuler 24.03 sp1 qemu 不支持热迁移

4. 使用龙蜥全套的虚拟化组件测试, 包括:
   + anolis/hygon-qemu
   + anolis/hygon-edk2
   + anolis/hygon-cloud-kernel

   使用
   ```
   -object sev-guest,id=sev0,policy=0x5,cbitpos=47,reduced-phys-bits=5
                                    ^^^
   ```
   启动参数，dmesg中并无CSV3相关打印
   ```
   [    0.438524]  HYGON CSV
   [    0.438525]  HYGON CSV2
   ```

   使用
   ```
   -object sev-guest,id=sev0,policy=0x45,cbitpos=47,reduced-phys-bits=5
                                    ^^^^
   ```

   启动虚拟机会遇到:
   ```
   [root@11-241-212-143 18:38:26 openeuler]# sh test.sh
   2025-02-26T10:38:28.255829Z qemu-system-x86_64: sev_launch_start: LAUNCH_START ret=1 fw_error=24 'Part-specific integrity check failure'
   2025-02-26T10:38:28.255878Z qemu-system-x86_64: sev_kvm_init: failed to create encryption context
   ```

## 个人疑问
1. 在链接<sup>1</sup> 提到海光三号支持csv3，在链接<sup>2</sup>中提到
   `从4号CPU开始，支持CSV3安全虚拟化功能`, 那海光三号`Hygon C86 7360`,
   支持不支持 CSV3
2. 关于 guest policy的选值. 在 链接 <sup>2</sup>中给出的示例是使用`0x45`, 在AMD
   KM spec，第三章节`guest policy`给出的定义中，6 bit 是reserved。请问下，hygon
   使用这个bit来使能什么功能? (是csv3么)

## 参考链接

1. [hygon-sig 安全功能配置要求](https://openanolis.cn/sig/Hygon-Arch/doc/1076446632387394260)
2. [hygon-sig : 启动CSV3加密虚拟机](https://openanolis.cn/sig/Hygon-Arch/doc/984441308636882971)
3. [Secure Encrypted Virtualization API Version 0.24](https://www.amd.com/content/dam/amd/en/documents/epyc-technical-docs/programmer-references/55766_SEV-KM_API_Specification.pdf)
