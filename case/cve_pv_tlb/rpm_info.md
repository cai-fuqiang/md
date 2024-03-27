# 修复CVE 的相关rpm包信息以及部署方法
## 相关包
|包名|链接|
|---|---|
|kernel|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-core|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-core-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-modules|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-modules-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-modules-extra|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-modules-extra-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-modules-internal|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-modules-internal-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-tools|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-tools-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-tools-libs|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-tools-libs-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-headers|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-headers-4.18.0-147.5.1.es8_25.x86_64.rpm|
|kernel-devel|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/kernel-devel-4.18.0-147.5.1.es8_25.x86_64.rpm|
|python3-perf|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/python3-perf-4.18.0-147.5.1.es8_25.x86_64.rpm|
|perf|http://koji.easystack.io/kojifiles/packages/kernel/4.18.0/147.5.1.es8_25/x86_64/perf-4.18.0-147.5.1.es8_25.x86_64.rpm|

## md5
```
[root@node-25 147_25_all]# md5sum kernel-*
af4ca8a4187d7deb342e121771b8bb6a  kernel-4.18.0-147.5.1.es8_25.x86_64.rpm
85d81eea7e290491c2bda890ca8e0745  kernel-core-4.18.0-147.5.1.es8_25.x86_64.rpm
15694bbee523a662be2ae7c59cfa9924  kernel-devel-4.18.0-147.5.1.es8_25.x86_64.rpm
99eb9fc53fdc14821a96a49d1065bdc5  kernel-headers-4.18.0-147.5.1.es8_25.x86_64.rpm
d5c840982b0012974778644d864a844a  kernel-modules-4.18.0-147.5.1.es8_25.x86_64.rpm
3764792d11cbe333673e8f3596666790  kernel-modules-extra-4.18.0-147.5.1.es8_25.x86_64.rpm
868c6bfd406b88f96fc16389431d1c94  kernel-modules-internal-4.18.0-147.5.1.es8_25.x86_64.rpm
16829e0671008aa85816e831befc8756  kernel-tools-4.18.0-147.5.1.es8_25.x86_64.rpm
02d92e4752d662009b42adcd51818b76  kernel-tools-libs-4.18.0-147.5.1.es8_25.x86_64.rpm
```

## 安装方法
1. 为了确保/boot分区空间足够, 最好执行
   + 执行`rpm -qa |grep kernel-core |grep -v "kernel-core-4.18.0-147.5.1.es8_24.x86_64"`
     查看是否有其他版本的 kernel-core包, 如果有执行 `yum remove` 将其删除.
   + `rm -f /boot/initramfs-4.18.0-147.5.1.es8_24.x86_64kdump.img`
2. yum install + ALL_RPM_PATH
2. 重启后, 执行`uname -r`, 查看kernel版本是否更新

> NOTE
>
> 上面变更步骤仅限v611
