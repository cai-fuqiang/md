# 驱动
## drbd
nvr:kmod-drbd-kmodtool-9.1.7-7.es8_4.k372

http://koji.easystack.io/buildinfo?buildID=3713

## ice
nvr:kmod-ice-kmodtool-1.11.14-1.es8_0.k372

http://koji.easystack.io/buildinfo?buildID=3814
<!--
## ixgbe
nvr:kmod-ixgbe-kmodtool-5.15.2-0.es8_11.k372
-->
## megaraid_sas
nvr:kmod-megaraid_sas-kmodtool-07.724.02.00-1.es8_0.k372

http://koji.easystack.io/buildinfo?buildID=3812
## mlnx-ofa_kernel
nvr:kmod-mlnx-ofa_kernel-kmodtool-5.4-3.6.8.1.es8_4.k372

http://koji.easystack.io/buildinfo?buildID=3780
## ngbe
nvr:kmod-ngbe-kmodtool-1.2.4-1.es8_0.k372

http://koji.easystack.io/buildinfo?buildID=3835
## openvswitch
nvr:kmod-openvswitch-kmodtool-2.16.2-2.es8_2.k372

http://koji.easystack.io/buildinfo?buildID=3715
## qla2xxx
nvr:kmod-qla2xxx-kmodtool-10.02.09.00-1.es8_0.k372

http://koji.easystack.io/buildinfo?buildID=3813
## sfc
nvr:kmod-sfc-kmodtool-5.3.13.1006-1.es8_0.k372

http://koji.easystack.io/buildinfo?buildID=3840
## txgbe
nvr:kmod-txgbe-kmodtool-1.3.4-1.es8_0.k372

http://koji.easystack.io/buildinfo?buildID=3834

# 安装步骤
1. 卸载老的驱动: 
```
yum remove -y  \
kmod-drbd \
kmod-ice \
kmod-megaraid_sas \
kmod-mlnx-ofa \
kmod-ngbe \
kmod-openvswitch \
kmod-qla2xxx \
kmod-sfc \
kmod-txgbe
```
2. 预留够/boot分区空间:  rm -f /boot/initramfs-4.18.0-147.5.1.es8_24.x86_64kdump.img
3. 安装新kernel以及 mlnx 网卡驱动包， 包含:
nvr:kernel-4.18.0-372.19.1.es8_2:  kernel,  kernel-core, kernel-modules , kernel-modules-extra
4. 安装新的mlnx网卡驱动包以及其依赖包:
nvr:kmod-mlnx-ofa_kernel-kmodtool-5.4-3.6.8.1.es8_4.k372: kmod-mlnx-ofa_kernel, mlnx-ofa_kernel
nvr:mlnx-tools-5.2.0-0.es8_0: mlnx-tools

nvr:kmod-drbd-kmodtool-9.1.7-7.es8_4.k372
nvr:kmod-ice-kmodtool-1.11.14-1.es8_0.k372
nvr:kmod-megaraid_sas-kmodtool-07.724.02.00-1.es8_0.k372
nvr:kmod-ngbe-kmodtool-1.2.4-1.es8_0.k372
nvr:kmod-openvswitch-kmodtool-2.16.2-2.es8_2.k372
nvr:kmod-qla2xxx-kmodtool-10.02.09.00-1.es8_0.k372
nvr:kmod-sfc-kmodtool-5.3.13.1006-1.es8_0.k372
nvr:kmod-txgbe-kmodtool-1.3.4-1.es8_0.k372

5. 安装防止网卡重命名的包: http://koji.easystack.io/taskinfo?taskID=28273 :  rename_ethname
6. 安装完成后，请帮忙注意下有没有rpm命令报错，如果没有重启节点

> NOTE:
>
> 1. ice, sfc只有x86架构
>
> 2. 升级完成后，请帮忙提供x86, arm64, ft2500 环境用于测试kdump

<!--
http://koji.easystack.io/kojifiles/packages/kmod-drbd-kmodtool/9.1.7/7.es8_4.k372/aarch64/kmod-drbd-9.1.7-7.es8_4.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-ixgbe-kmodtool/5.15.2/0.es8_11.k372/aarch64/kmod-ixgbe-5.15.2-0.es8_11.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-megaraid_sas-kmodtool/07.719.04.00/2.es8_10.k372/aarch64/kmod-megaraid_sas-07.719.04.00-2.es8_10.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-mlnx-ofa_kernel-kmodtool/5.4/3.6.8.1.es8_4.k372/aarch64/kmod-mlnx-ofa_kernel-5.4-3.6.8.1.es8_4.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-mlnx-ofa_kernel-kmodtool/5.4/3.6.8.1.es8_4.k372/aarch64/mlnx-ofa_kernel-5.4-3.6.8.1.es8_4.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-ngbe-kmodtool/1.2.2/1.es8_11.k372/aarch64/kmod-ngbe-1.2.2-1.es8_11.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-openvswitch-kmodtool/2.16.2/2.es8_2.k372/aarch64/kmod-openvswitch-2.16.2-2.es8_2.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-qla2xxx-kmodtool/10.02.06.200/0.es8_12.k372/aarch64/kmod-qla2xxx-10.02.06.200-0.es8_12.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-txgbe-kmodtool/1.3.2/1.es8_15.k372/aarch64/kmod-txgbe-1.3.2-1.es8_15.k372.aarch64.rpm

http://koji.easystack.io/kojifiles/packages/kmod-drbd-kmodtool/9.1.7/7.es8_4.k372/aarch64/kmod-drbd-numa64-9.1.7-7.es8_4.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-ixgbe-kmodtool/5.15.2/0.es8_11.k372/aarch64/kmod-ixgbe-numa64-5.15.2-0.es8_11.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-megaraid_sas-kmodtool/07.719.04.00/2.es8_10.k372/aarch64/kmod-megaraid_sas-numa64-07.719.04.00-2.es8_10.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-mlnx-ofa_kernel-kmodtool/5.4/3.6.8.1.es8_4.k372/aarch64/kmod-mlnx-ofa_kernel-numa64-5.4-3.6.8.1.es8_4.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-mlnx-ofa_kernel-kmodtool/5.4/3.6.8.1.es8_4.k372/aarch64/mlnx-ofa_kernel-5.4-3.6.8.1.es8_4.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-ngbe-kmodtool/1.2.2/1.es8_11.k372/aarch64/kmod-ngbe-numa64-1.2.2-1.es8_11.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-openvswitch-kmodtool/2.16.2/2.es8_2.k372/aarch64/kmod-openvswitch-numa64-2.16.2-2.es8_2.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-qla2xxx-kmodtool/10.02.06.200/0.es8_12.k372/aarch64/kmod-qla2xxx-numa64-10.02.06.200-0.es8_12.k372.aarch64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-txgbe-kmodtool/1.3.2/1.es8_15.k372/aarch64/kmod-txgbe-numa64-1.3.2-1.es8_15.k372.aarch64.rpm


http://koji.easystack.io/kojifiles/packages/kmod-drbd-kmodtool/9.1.7/7.es8_4.k372/x86_64/kmod-drbd-9.1.7-7.es8_4.k372.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-ice-kmodtool/1.9.11/1.es8_20.k372/x86_64/kmod-ice-1.9.11-1.es8_20.k372.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-ixgbe-kmodtool/5.15.2/0.es8_11.k372/x86_64/kmod-ixgbe-5.15.2-0.es8_11.k372.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-megaraid_sas-kmodtool/07.719.04.00/2.es8_10.k372/x86_64/kmod-megaraid_sas-07.719.04.00-2.es8_10.k372.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-mlnx-ofa_kernel-kmodtool/5.4/3.6.8.1.es8_4.k372/x86_64/kmod-mlnx-ofa_kernel-5.4-3.6.8.1.es8_4.k372.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-mlnx-ofa_kernel-kmodtool/5.4/3.6.8.1.es8_4.k372/x86_64/mlnx-ofa_kernel-5.4-3.6.8.1.es8_4.k372.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-ngbe-kmodtool/1.2.2/1.es8_11.k372/x86_64/kmod-ngbe-1.2.2-1.es8_11.k372.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-txgbe-kmodtool/1.3.2/1.es8_15.k372/x86_64/kmod-txgbe-1.3.2-1.es8_15.k372.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-openvswitch-kmodtool/2.16.2/2.es8_2.k372/x86_64/kmod-openvswitch-2.16.2-2.es8_2.k372.x86_64.rpm
http://koji.easystack.io/kojifiles/packages/kmod-qla2xxx-kmodtool/10.02.06.200/0.es8_12.k372/x86_64/kmod-qla2xxx-10.02.06.200-0.es8_12.k372.x86_64.rpm
-->
