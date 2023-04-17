# 问题现象
替换紫光网卡后，出现了ifup 不起来，并且网卡运行着可能突然down掉,
dmesg中有如下报错:
![ixgbe_firmware_error](pic/ixgbe_firmware_error.png)

并且网络不稳定，可能出现丢包的情况.
查看驱动固件版本:

![ethtool_i](pic/ethtool_i.png)

其中厂商给的回复，希望更新驱动版本, 厂商参考链接如下:

https://forum.proxmox.com/threads/pve-6-0-7-ixgbe-firmware-errors.58592/


# 查看厂商给出的链接
大概如下:
```
-- Oct 1, 2019
   dm15用户反馈, 在 dmesg中发现了如下报错:
   [44650.577580] ixgbe 0000:05:00.0: Warning firmware error detected FWSM: 0x00000000
   [44650.741545] ixgbe 0000:05:00.1: Warning firmware error detected FWSM: 0x00000000
   ...
   并且在没有任何state change 的情况下，网卡偶尔hung住。并且ip link set ...up 可以让链路
   正常，无需重启 {不知道翻译的对不对，原文如下:  The NICs (05:00.0 Ethernet controller: Intel 
   Corporation 82599ES 10-Gigabit SFI/SFP+ Network Connection (rev 01)) randomly hang without 
   any state changes, `ip link set ... up` brings them back without reboot.}
	
   他发现当前kernel 自带的ixgbe driver 不如官网上的新:
   他那kernel自带的:
     # ethtool -i enp5s0f0
     driver: ixgbe
     version: 5.1.0-k
     firmware-version: 0x000118b3 
   官网: 5.6.3

   给出了一个链接，在centos的bug tracer上，有相同的报错，并且给出了解释, 我们一会再看
   https://bugs.centos.org/view.php?id=16495

   随后他表示，将更新  "最新"版本ixgbe驱动, (为什么最新打引号呢，我们之后分析), 有进展在
   该页面更新

-- Oct 10,  2019
   dm15用户表示，该问题，已经通过更新驱动解决，他的环境已经好了
```

# 查看 centos bug tracer 链接
```
-- 2019-09-30 :
   kadafax报告了该问题, 表示在Dell PowerEdge R510(fully firmware updated) 情况下，也遇到了
   这个问题，他表示: 网卡看起来仍在继续工作。
-- 2019-09-30 :
   nixma 更新，说在Debian上也遇到了该问题，表示网络突然被当了，并且dmesg中出现了该打印，
   通过重启暂时解决(原文如下: Not a CentOS user, but searched for this on Google because 
   it happened today to me on Debian. Suddenly network was down and that dmesg spam, 
   soft reboot has resolved the problem.)

   因为他使用的kernel是5.0.21-1-pve, 猜测，和kernel version 关系不大

-- 2019-09-30 
   pgreco(deveplor) 表示，让其提供固件版本，表示某些driver可能依赖特定的固件版本.

-- 2019-10-01
   kadafax(reporter) 提供版本如下:
      Here it is:
      driver: ixgbe
      version: 5.1.0-k-rh7.6
      firmware-version: 0x800001a5, 16.5.20 


-- 2019-10-01
   nixma 提供如下:
       # ethtool -i enp5s0f0
       driver: ixgbe
       version: 5.1.0-k
       firmware-version: 0x000118b3
       expansion-rom-version:
       bus-info: 0000:05:00.0
       supports-statistics: yes
       supports-test: yes
       supports-eeprom-access: yes
       supports-register-dump: yes
       supports-priv-flags: yes

-- 2021-07-29
   toracat(manager) 表示, 该问题需要通过将固件更新为最新解决，参考如下:
   https://access.redhat.com/solutions/5068871
```

# 查看红帽知识库

# 安装5.12.5驱动
```
[root@localhost ixgbe-5.12.5]# ethtool -i enp5s0f1
driver: ixgbe
version: 5.12.5
firmware-version: 0x546c0001, 255.65535.255
expansion-rom-version:
bus-info: 0000:05:00.1
supports-statistics: yes
supports-test: yes
supports-eeprom-access: yes
supports-register-dump: yes
supports-priv-flags: yes
```

# kernel本身驱动
```
[root@localhost ixgbe]# ethtool -i enp5s0f1
driver: ixgbe
version: 5.1.0-k-rh8.1.0
firmware-version: 0x0001546c
expansion-rom-version:
bus-info: 0000:05:00.1
supports-statistics: yes
supports-test: yes
supports-eeprom-access: yes
supports-register-dump: yes
supports-priv-flags: yes
```
