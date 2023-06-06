# 611
## 驱动列表
```
kmod-drbd.x86_64                      9.0.21-14.es8_2                    @anaconda
kmod-ice.x86_64                       1.5.8-1.es8_15                     @anaconda
kmod-megaraid_sas.x86_64              07.719.04.00-2.es8_8               @anaconda
kmod-mlnx-ofa.x86_64                  5.5-1.0.3.es8_8                    @anaconda
kmod-ngbe.x86_64                      1.1.1-1.es8_8                      @anaconda
kmod-openvswitch.x86_64               2.16.2-3.es8_0                     @anaconda
kmod-qla2xxx.x86_64                   10.02.06.02_k-0.es8_1              @anaconda
kmod-sfc.x86_64                       4.15.6.1007-2.es8_11               @anaconda
kmod-txgbe.x86_64                     1.2.2-1.es8_12                     @anaconda
```

## 611未装驱动
1. lpfc, elx-lpfc

# 621
|module_name                        |version                                |是否需要(都更新下最新驱动)|
|---|----|----|
|drbd                               |                                       |Y,self mod|
|ice                                |0.8.2-k                                |Y,和611版本差别有点大(更新到最新)|
|qla2xxx                            |10.02.06.200-k                         |再议，因为版本差别不大|
|mlx5_core                          |4.18.0-372.19.1.es8_2.aarch64          |Y, 推荐合入, 因为mlnx网卡驱动更新频率较高(测试下5.8-2.0.3.0)|
|megaraid_sas                       |07.719.03.00-rh1(ps, 在147 kernel中,版本为:07.707.51.00-rc1)|还需要在商量，因为版本和611 kmod差不多(看下官方新版本)|
|openvswitch                        |无                                     |再商量|
|txgbe, sfc,ngbe                    |                                       |驱动必须要，因为372 kernel中没有|
|ixgbe                              |                                       |N,611没有合入|

## 相关case
1. megaraid_sas
https://easystack.atlassian.net/browse/ECS-5102
https://easystack.atlassian.net/browse/ECSDESK-16981

2. openvswitch 
https://easystack.atlassian.net/browse/EAS-96189

3. qla2xxx
https://easystack.atlassian.net/browse/EPS-495


## 相关链接
1. megaraid_sas
https://docs.broadcom.com/doc/12353253
https://www.broadcom.com/products/storage/raid-controllers/megaraid-9670-24i
