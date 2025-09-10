## 构建chroot qemu 运行环境
### base chroot build
1. 创建出一个合适的repo文件: `openeuler.repo`
2. 向chroot目录安装软件
   ```
   yum --installroot=$(pwd)/mnt \
     --disablerepo="*" --enablerepo="my*" \
     -c openeuler.repo install yum make gcc
   ```
3. 覆盖chroot 目录中的`repo`, `hosts`, `resolv.conf`文件
   ```
   [~]# cp openeuler.repo mnt/etc/yum.repos.d/openEuler.repo
   cp: 是否覆盖 'mnt/etc/yum.repos.d/openEuler.repo'？y
   [~]# cp /etc/hosts ./mnt/etc/
   cp: 是否覆盖 './mnt/etc/hosts'？y
   [~]# cp /etc/resolv.conf  ./mnt/etc/
   ```
4. 编写如下`mount.sh`脚本:
   ```sh
   if [ "$1" == "mount" ];then
           mount -t proc none ./mnt/proc/
           mount -t sysfs none ./mnt/sys
           mount -o bind /dev/ ./mnt/dev
   else
           umount ./mnt/proc
           umount ./mnt/sys
           umount ./mnt/dev
   fi
   ```
   执行该脚本

## 安装包准备
* 编译相关
  + make
  + gcc
  + rpm-build
  + ninja-build
  + meson
  + m4
  + autoconf
  + automake
* 虚拟化相关
  + qemu
  + libvirt
  + edk2-ovmf
* 其他工具
  + pciutils
* 编译依赖
  + dpdk
    + numactl-devel
    + python3-pyelftools
  + ovs
    + libtool
    + groff
* 软件依赖
  + ovs:
    + hostname
## 编译包  dpdk, ovs
### dpdk<sup>2</sup>
1. 下载解压包:
   ```
   tar xf dpdk-24.11.2.tar.xz
   cd dpdk-stable-24.11.2
   ```
2. 编译
   ```
   export DPDK_BUILD=$(pwd)/build
   meson build
   ninja -C build
   ninja -C build install
   ldconfig
   ```
3. check
Check if libdpdk can be found by pkg-config:
```
bash-5.2# pkg-config --modversion libdpdk
Package libdpdk was not found in the pkg-config search path.
Perhaps you should add the directory containing `libdpdk.pc'
to the PKG_CONFIG_PATH environment variable
Package 'libdpdk', required by 'virtual:world', not found
```
执行下面命令:
```
bash-5.2# export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig

```
再次检查:
```
bash-5.2# pkg-config --modversion libdpdk
24.11.2
```
### ovs<sup>2</sup>
```
sh build.sh
./configure --with-dpdk=static 
make -j 100
make install
```
## 临时开启大页
开启50G个2M大页(为虚拟机大页作准备)
```
bash-5.2# cat /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
0
bash-5.2# echo 25600 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
bash-5.2#
```

挂载大页
```
bash-5.2# mount -t hugetlbfs none /dev/hugepages/
```
## insmod vfio驱动
host执行
```
modprobe vfio-pci
/usr/bin/chmod a+x /dev/vfio
/usr/bin/chmod 0666 /dev/vfio/*
```
## 其他
```
/usr/local/bin/dpdk-devbind.py --bind=vfio-pci eth1
/usr/local/bin/dpdk-devbind.py --status
```
打印如下信息，说明网卡已经绑定`vfio-pci`驱动
```
Network devices using DPDK-compatible driver
============================================
0000:01:00.1 '82599ES 10-Gigabit SFI/SFP+ Network Connection 10fb' numa_node=0 drv=vfio-pci unused=
```
## 启动ovs
```
export PATH=$PATH:/usr/local/share/openvswitch/scripts
export DB_SOCK=/usr/local/var/run/openvswitch/db.sock

mkdir -p /usr/local/var/run/openvswitch
mkdir -p /usr/loca/var/log/openvswitch
ovsdb-tool create /usr/local/etc/openvswitch/conf.db
ovs-vsctl --no-wait init
ovsdb-server --remote=punix:/usr/local/var/run/openvswitch/db.sock --remote=db:Open_vSwitch,Open_vSwitch,manager_options --private-key=db:Open_vSwitch,SSL,private_key --certificate=db:Open_vSwitch,SSL,certificate --bootstrap-ca-cert=db:Open_vSwitch,SSL,ca_cert  --pidfile --detach --log-file
ovs-vsctl --no-wait set Open_vSwitch . other_config:dpdk-init=true
ovs-ctl --no-ovsdb-server --db-sock="$DB_SOCK" start
```
验证ovs服务是否启动成功:
```
bash-5.2# ovs-vsctl get Open_vSwitch . dpdk_initialized
true
bash-5.2# ovs-vswitchd --version
ovs-vswitchd (Open vSwitch) 3.5.90
DPDK 24.11.2
bash-5.2# ovs-vsctl get Open_vSwitch . dpdk_version
"DPDK 24.11.2"
```

创建网桥`br0`, 绑定dpdk端口:
```
bash-5.2# ovs-vsctl add-br br0 -- set bridge br0 datapath_type=netdev
bash-5.2# ovs-vsctl add-port br0 myportnameone -- set Interface myportnameone type=dpdk options:dpdk-devargs=0000:01:00.1
```

执行命令验证是否正确:
```
bash-5.2# ovs-vsctl show
ab8ecae6-aa90-4a0d-8503-506a068750a0
    Bridge br0
        datapath_type: netdev
        Port br0
            Interface br0
                type: internal
        Port myportnameone
            Interface myportnameone
                type: dpdk
                options: {dpdk-devargs="0000:01:00.1"}
```
增加`ovs`端口:
```

```

## 参考链接
1. [Using Open vSwitch with DPDK](https://github.com/openvswitch/ovs/blob/main/Documentation/howto/dpdk.rst)
2. [Open vSwitch with DPDK](https://docs.openvswitch.org/en/latest/intro/install/dpdk/)
3. [DPDK+OVS+QEMU搭建vhost-user实验环境](https://blog.csdn.net/yangye2014/article/details/78064735/)
