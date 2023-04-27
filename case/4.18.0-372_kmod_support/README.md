# 编译驱动列表如下:
|驱动名称|rpm build link|
|----|----|
|openvswitch|[here](http://koji.easystack.io/buildinfo?buildID=3715)|
|mlnx-ofa_kernel|[here](http://koji.easystack.io/buildinfo?buildID=3719)|
|qla2xxx|[here](http://koji.easystack.io/buildinfo?buildID=3722)|
|megaraid_sas|[here](http://koji.easystack.io/buildinfo?buildID=3710)|
|ngbe|[here](http://koji.easystack.io/buildinfo?buildID=3712)|
|txgbe|[here](http://koji.easystack.io/buildinfo?buildID=3718)|
|ixgbe|[here](http://koji.easystack.io/buildinfo?buildID=3714)|
|ice|[here](http://koji.easystack.io/buildinfo?buildID=3708)|
|drbd|[here](http://koji.easystack.io/buildinfo?buildID=3713)|


# es-up
```
@all 申请更新
# kmod
nvr:kmod-openvswitch-kmodtool-2.16.2-2.es8_2.k372
nvr:kmod-mlnx-ofa_kernel-kmodtool-5.4-3.6.8.1.es8_2.k372
nvr:kmod-qla2xxx-kmodtool-10.02.06.200-0.es8_12.k372
nvr:kmod-megaraid_sas-kmodtool-07.719.04.00-2.es8_10.k372
nvr:kmod-ngbe-kmodtool-1.2.2-1.es8_11.k372
nvr:kmod-txgbe-kmodtool-1.3.2-1.es8_15.k372
nvr:kmod-ixgbe-kmodtool-5.15.2-0.es8_11.k372
nvr:kmod-ice-kmodtool-1.9.11-1.es8_20.k372
nvr:kmod-drbd-kmodtool-9.1.7-7.es8_4.k372
repo: es8
套件更新需求者: @fuqiang.wang
包的来源:
|openvswitch|[here](http://koji.easystack.io/buildinfo?buildID=3715)|
|mlnx-ofa_kernel|[here](http://koji.easystack.io/buildinfo?buildID=3719)|
|qla2xxx|[here](http://koji.easystack.io/buildinfo?buildID=3722)|
|megaraid_sas|[here](http://koji.easystack.io/buildinfo?buildID=3710)|
|ngbe|[here](http://koji.easystack.io/buildinfo?buildID=3712)|
|txgbe|[here](http://koji.easystack.io/buildinfo?buildID=3718)|
|ixgbe|[here](http://koji.easystack.io/buildinfo?buildID=3714)|
|ice|[here](http://koji.easystack.io/buildinfo?buildID=3708)|
|drbd|[here](http://koji.easystack.io/buildinfo?buildID=3713)|

jira: [:jira: EAS-108942](https://easystack.atlassian.net/browse/EAS-108942)
导入验证case: 无
对应包的repo依赖测试： 无
需要补充依赖包：无
导入时限: 明天
导入风险: 无

# kernel
:gift: [nvr:kernel-4.18.0-372.19.1.es8_2](http://koji.easystack.io/b/kernel-4.18.0-372.19.1.es8_2) | [:receipt:](http://koji.easystack.io:3000/rpms/kernel/src/branch/4.18.0) [:technologist:](http://koji.easystack.io:3000/rpms/kernel/issues) | :bug: 问题支援 ~escl-upgrade / @shawn.wang  / @dexin.zhang
repo: es8
套件更新需求者: @fuqiang.wang
包的来源: http://koji.easystack.io/buildinfo?buildID=3618
jira: 无
导入验证case: 无
对应包的repo依赖测试： 无
需要补充依赖包：无
导入时限: 明天
导入风险: 无

# kexec-tools
:gift: [nvr:kexec-tools-2.0.20-68.es8](http://koji.easystack.io/b/kexec-tools-2.0.20-68.es8) | [:receipt:](http://koji.easystack.io:3000/rpms/kexec-tools/src/branch/2.0.20) [:technologist:](http://koji.easystack.io:3000/rpms/kexec-tools/issues) | :bug: 问题支援 ~escl-upgrade / @shawn.wang  / @dexin.zhang
repo: es8
套件更新需求者: @fuqiang.wang
包的来源: http://koji.easystack.io/buildinfo?buildID=3696
jira: http://koji.easystack.io/buildinfo?buildID=3696
导入验证case: 无
对应包的repo依赖测试： 无
需要补充依赖包：无
导入时限: 明天
导入风险: 无

# crash
:gift: [nvr:crash-7.3.1-5.es8](http://koji.easystack.io/b/crash-7.3.1-5.es8) | [:receipt:](http://koji.easystack.io:3000/rpms/crash/src/branch/7.3.1) [:technologist:](http://koji.easystack.io:3000/rpms/crash/issues) | :bug: 问题支援 ~escl-upgrade / @shawn.wang  / @dexin.zhang
repo: es8
套件更新需求者: @fuqiang.wang
包的来源: http://koji.easystack.io/buildinfo?buildID=3695
jira: 无
导入验证case: 无
对应包的repo依赖测试： 无
需要补充依赖包：无
导入时限: 明天
导入风险: 无

# mlnx-tools
nvr:mlnx-tools-5.2.0-0.es8_0
repo: es8
套件更新需求者: @王富强
包的来源:http://koji.easystack.io/koji/buildinfo?buildID=3115
jira: 无
导入验证case: 无
对应包的repo依赖测试： 无
需要补充依赖包：无
导入时限: 明天
导入风险: 无

> NOTE
>
> :gift: [nvr:kmod-mlnx-ofa_kernel-kmodtool-k372-5.4-3.6.8.1.es8_1](http://koji.easystack.io/b/kmod-mlnx-ofa_kernel-kmodtool-k372-5.4-3.6.8.1.es8_1) | [:receipt:](http://koji.easystack.io:3000/rpms/kmod-mlnx-ofa_kernel-kmodtool-k372/src/branch/5.4) [:technologist:](http://koji.easystack.io:3000/rpms/kmod-mlnx-ofa_kernel-kmodtool-k372/issues) | :bug: 问题支援 ~escl-upgrade / @shawn.wang  / @dexin.zhang 需要进两个包
>
> kmod-mlnx-ofa_kernel && mlnx-ofa_kernel
>
> 并且依赖mlnx-tools
```
