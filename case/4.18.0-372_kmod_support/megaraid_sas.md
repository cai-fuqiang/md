# 网卡信息
|节点|网卡型号|device/vendor ID|4.18.0-372 是否支持|
|----|----|----|----|
|7      |RAID bus controller: Broadcom / LSI MegaRAID SAS 2208 [Thunderbolt]    |1000:005b|Y| 
|9      |RAID bus controller: Broadcom / LSI MegaRAID SAS 2208 [Thunderbolt]    |1000:005b|Y|
|11     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2008 [Falcon]         |1000:0073|Y|
|12     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2008 [Falcon]         |1000:0073|Y|
|13     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2008 [Falcon]         |1000:0073|Y|
|18     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2208 [Thunderbolt]    |1000:005b|Y|
|19     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2208 [Thunderbolt]    |1000:005b|Y|
|20     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2208 [Thunderbolt]    |1000:005b|Y|
|21     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2208 [Thunderbolt]    |1000:005b|Y|
|22     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2208 [Thunderbolt]    |1000:005b|Y|
|23     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2008 [Falcon]         |1000:0073|Y|
|24     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2008 [Falcon]         |1000:0073|Y|
|25     |RAID bus controller: Broadcom / LSI MegaRAID SAS 2008 [Falcon]         |1000:0073|Y|

# 通过modinfo查看是否支持该设备类型
查看4.18.0-372 kernel megaraid_sas 的 modinfo是否支持该设备
```
[root@node-1 ~]# modinfo megaraid_sas |grep alias |grep -E '73|5B'
alias:          pci:v00001000d0000005Bsv*sd*bc*sc*i*
alias:          pci:v00001000d00000073sv*sd*bc*sc*i*
```

# 结论
4.18.0-372支持该环境中的设备型号
