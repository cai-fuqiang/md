# 客户侧现象

> NOTE
>
> 我们仅以node-9 ib0为例
```
[root@node-1 node9]# cat jour_xb.txt |grep -E '4b:00.0|ib0'
//=========(1)==========
Mar 18 10:29:41 node-9.domain.tld kernel: pci 0000:4b:00.0: [15b3:101b] type 00 class 0x020700
Mar 18 10:29:41 node-9.domain.tld kernel: pci 0000:4b:00.0: reg 0x10: [mem 0x5fff6000000-0x5fff7ffffff 64bit pref]
Mar 18 10:29:41 node-9.domain.tld kernel: pci 0000:4b:00.0: reg 0x30: [mem 0xdfe00000-0xdfefffff pref]
Mar 18 10:29:41 node-9.domain.tld kernel: pci 0000:4b:00.0: PME# supported from D3cold
Mar 18 10:29:41 node-9.domain.tld kernel: pci 0000:4b:00.0: reg 0x1a4: [mem 0x5fffbc00000-0x5fffbdfffff 64bit pref]
Mar 18 10:29:41 node-9.domain.tld kernel: pci 0000:4b:00.0: VF(n) BAR0 space: [mem 0x5fffbc00000-0x5ffff7fffff 64bit pref] (contains BAR0 for 30 VFs)
Mar 18 10:29:41 node-9.domain.tld kernel: DMAR: Hardware identity mapping for device 0000:4b:00.0
Mar 18 10:29:41 node-9.domain.tld kernel: iommu: Adding device 0000:4b:00.0 to group 30
//=========(2)==========
Mar 18 10:29:43 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: firmware version: 20.36.1010
Mar 18 10:29:43 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: 252.048 Gb/s available PCIe bandwidth (16 GT/s x16 link)
Mar 18 10:29:43 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: Port module event: module 0, Cable plugged
Mar 18 10:29:43 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: mlx5_fw_tracer_start:821:(pid 994): FWTracer: Ownership granted and active
Mar 18 10:29:53 node-9.domain.tld network[2914]: Bringing up interface ib0:  ERROR     : [/etc/sysconfig/network-scripts/ifup-eth] Device ib0 does not seem to be present, delaying initialization.
Mar 18 10:29:53 node-9.domain.tld /etc/sysconfig/network-scripts/ifup-eth[4014]: Device ib0 does not seem to be present, delaying initialization.
//=========(3)==========
Mar 18 10:31:13 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: MLX5E: StrdRq(0) RqSz(1024) StrdSz(256) RxCqeCmprss(0)
Mar 18 10:31:13 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: MLX5E: StrdRq(0) RqSz(1024) StrdSz(256) RxCqeCmprss(0)
//=========(4)==========
Mar 18 10:31:14 node-9.domain.tld systemd[1]: Started mlnx_interface_mgr - configure ib0.
-- Subject: Unit mlnx_interface_mgr@ib0.service has finished start-up
-- Unit mlnx_interface_mgr@ib0.service has finished starting up.
Mar 18 10:31:14 node-9.domain.tld mlnx_interface_mgr[13399]: Setting up Mellanox network interface: ib0
Mar 18 10:31:14 node-9.domain.tld mlnx_interface_mgr[13486]: INFO: ib0 does not support connected mode
Mar 18 10:31:14 node-9.domain.tld mlnx_interface_mgr[13548]: OS is booting, will not run ifup on ib0
//=========(5)==========
Mar 18 10:31:14 node-9.domain.tld mlnx_interface_mgr[13564]: Running: /bin/mlnx_conf_mgr.sh ib0
Mar 18 10:31:14 node-9.domain.tld systemd[1]: mlnx_interface_mgr@ib0.service: Succeeded.
-- The unit mlnx_interface_mgr@ib0.service has successfully entered the 'dead' state.
//=========(6)==========
Mar 22 16:56:38 node-9.domain.tld network[58003]: Bringing up interface ib0:  [  OK  ]
```
1. pci probe(启动初)
2. mlx5_core probe(启动初)
   > NOTE
   >
   > 启动初起大概在10:29:40+左右
3. ib probe(启动后过了一段时间, 大概在 10:31:13左右), 稍后我们分析这部分代码, 在这个过程中会创建netdev
4. 在`90-ib.rules`中, 会`start mlnx_interface_mgr service`, 稍后我们分析
5. 但是在该服务中, 打印了`OS is booting, will not run ifup on ib0`, 并没有去RUNNING 该网卡
6. 客户手动`ifup ib0`成功up该网卡

我们分两部分来看:
* kernel中create netdev
* mlnx_interface_mgr service

# create netdev

> NOTE
>
> 这里主要看下, 为什么netdev的初始化, 要比mlx5_core对 pci的初始化晚那么多.
```cpp
void mlx5e_init_rq_type_params(struct mlx5_core_dev *mdev,
                               struct mlx5e_params *params)
{
        params->log_rq_mtu_frames = is_kdump_kernel() ?
                MLX5E_PARAMS_MINIMUM_LOG_RQ_SIZE :
                MLX5E_PARAMS_DEFAULT_LOG_RQ_SIZE;

        mlx5_core_info(mdev, "MLX5E: StrdRq(%d) RqSz(%ld) StrdSz(%ld) RxCqeCmprss(%d)\n",
                       params->rq_wq_type == MLX5_WQ_TYPE_LINKED_LIST_STRIDING_RQ,
                       params->rq_wq_type == MLX5_WQ_TYPE_LINKED_LIST_STRIDING_RQ ?
                       BIT(mlx5e_mpwqe_get_log_rq_size(params, NULL)) :
                       BIT(params->log_rq_mtu_frames),
                       BIT(mlx5e_mpwqe_get_log_stride_size(mdev, params, NULL)),
                       MLX5E_GET_PFLAG(params, MLX5E_PFLAG_RX_CQE_COMPRESS));
}
```
上面日志中的打印来自于这里, 我们看下相关堆栈
```
ipoib_add_one
ipoib_add_port
ipoib_intf_alloc --- 创建netdev
rdma_init_netdev
rdma_init_netdev
mlx5_rdma_setup_rn
mlx5i_init
mlx5i_build_nic_params
```

而 `ipoib_add_one`是一个回调
```cpp
static struct ib_client ipoib_client = {
        .name   = "ipoib",
        .add    = ipoib_add_one,
        .remove = ipoib_remove_one,
        .get_net_dev_by_params = ipoib_get_net_dev_by_params,
};
```

可以看到是ib client, 比较符合这里的场景
```
ib_register_client
add_client_context
ipoib_add_one
```

而`ib_register_client`路径有多条, 需要在客户环境上调试下.

# mlnx_interface_mgr
> NOTE
>
> 这里主要看下, `90-ib.rules`为什么没有 RUNNING 网卡

`90-ib.rules`:
```
...
KERNEL=="ib[0-9]*\.*|*nfiniband[0-9]*\.*", DRIVERS=="", SUBSYSTEM=="net", ACTION=="add",RUN+="/bin/systemctl --no-block start mlnx_interface_mgr@$name.service"
KERNEL=="ib[0-9]*\.*|*nfiniband[0-9]*\.*", DRIVERS=="", SUBSYSTEM=="net", ACTION=="remove",RUN+="/bin/systemctl stop mlnx_interface_mgr@$name.service"
...
```

可以看到在检测到 `ib[0-9]` netdev时, 会启动 `mlnx_interface_mgr`服务.
```
[Unit]
Description=mlnx_interface_mgr - configure %I

[Service]
Type=simple
ExecStart=/bin/mlnx_interface_mgr.sh %I
ExecStop=/bin/true
```

该服务也很简单, 就是执行`mlnx_interface_mgr.sh`脚本, 传入 interface name

我们来看下该脚本, 在什么时候执行`ifup`动作
```sh
bring_up()
{
    ...
    if [ $OS_IS_BOOTING -eq 1 ]; then
        log_msg "OS is booting, will not run ifup on $i"
        unset $KNOWN_CONF_VARS
        return 6
    fi
    /sbin/ifup ${i}
    ...
}

bring_up $i
```

可以看到 如果检测到 `${OS_IS_BOOTING}`为真, 则跳过`ifup`

`OS_IS_BOOTING`表示系统在启动阶段,我们来看下该变量的初始化:
```sh
OS_IS_BOOTING=0

# 获取上一次的bootID
last_bootID=$(cat /var/run/mlx_ifc-${i}.bootid 2>/dev/null)
if [ "X$last_bootID" == "X" ] && [ -e /sys/class/net/${i}/parent ]; then
    parent=$(cat /sys/class/net/${i}/parent)
    last_bootID=$(cat /var/run/mlx_ifc-${parent}.bootid 2>/dev/null)
fi

# 获取本次bootid
bootID=$(cat /proc/sys/kernel/random/boot_id 2>/dev/null | sed -e 's/-//g')
# 将本次bootid 写入 /var/run/mlx-ifc-${i}.bootid
echo $bootID > /var/run/mlx_ifc-${i}.bootid
# 如果上一次为空, 或者本次和上一次不一样, 则认为是启动后, 第一次启动该脚本
# 则很有可能是创建netdev 后, 通过udev拉服务, 并首次执行该脚本
if [[ "X$last_bootID" == "X" || "X$last_bootID" != "X$bootID" ]]; then
    OS_IS_BOOTING=1
fi
# 查看 start_time, 查看现在时间和 /var/run/mlx_os_booting 时间
start_time=$(cat /var/run/mlx_os_booting 2>/dev/null)
if [ "X$start_time" != "X" ]; then
    let run_time=$(date +%s | tr -d '[:space:]')-${start_time}
    # 如果时间差小于300, 则认为还系统启动还未完成
    if [ $run_time -lt 300 ]; then
        OS_IS_BOOTING=1
    fi
fi
# If driver was loaded manually after last boot, then OS boot is over
last_bootID_manual=$(cat /var/run/mlx_ifc.manual 2>/dev/null)
if [[ "X$last_bootID_manual" != "X" && "X$last_bootID_manual" == "X$bootID" ]]; then
    OS_IS_BOOTING=0
fi
```


所以, `OS_IS_BOOTING`为真, 表明是系统启动后, 在初始化阶段(udev 检测到 netdev), 第一次执行
执行脚本. 那为什么这种情况下, 不去ifup 该接口呢?

我们需要看另一个服务, `openibd`

> NOTE
>
> 需要注意的是,该服务在咱们编译的时候,默认没有启动, 因为
> 一般的mlx_en网卡不需要该服务管理.

```
[Unit]
Description=openibd - configure Mellanox devices
Documentation=file:/etc/infiniband/openib.conf
After=wickedd.service wickedd-nanny.service local-fs.target
Before=network.target network.service networking.service remote-fs-pre.target
RefuseManualStop=false
DefaultDependencies=false

[Service]
Type=oneshot
TimeoutSec=180
RemainAfterExit=yes
ExecStart=/etc/init.d/openibd start bootid=%b
ExecStop=/etc/init.d/openibd stop
ExecReload=/etc/init.d/openibd restart bootid=%b

[Install]
WantedBy=sysinit.target
```

该服务在 install 时, 会被 sysinit.target 弱依赖. 并且在 network.target之前启动.
我们enable该服务后, 查看该服务的启动日志


我们先看下, 开启了 `openibd`服务后, netdev创建时机:
```
Apr 02 11:09:34 node-9.domain.tld openibd[2319]: + /sbin/modprobe ib_ipoib
Apr 02 11:09:34 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: MLX5E: StrdRq(0) RqSz(1024) StrdSz(256) RxCqeCmprss(0)
Apr 02 11:09:34 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: MLX5E: StrdRq(0) RqSz(1024) StrdSz(256) RxCqeCmprss(0)
```

可以看到在 夹在 `ib_ipoib`驱动后, 创建了netdev, 那么这里我们就可以补全,上面的堆栈了.
```
module_init(ipoib_init_module);
ipoib_init_module
ib_register_client
```

我们看下该接口所在文件的MAKEFILE
```
ib_ipoib-y                                      := ipoib_main.o \ ----> 该文件
                                                   ipoib_ib.o \
                                                   ipoib_multicast.o \
                                                   ipoib_verbs.o \
                                                   ipoib_vlan.o \
                                                   ipoib_ethtool.o \
                                                   ipoib_netlink.o \
                                                   ipoib_genetlink.o
```

那么这里, 我们来下`openibd`中, 该服务是如何加载该模块的.
```sh
start()
{
    ...
    if [ "X${UMAD_LOAD}" == "Xyes" ]; then
        load_module ib_umad
        RC=$[ $RC + $? ]
    fi

    if [ "X${UVERBS_LOAD}" == "Xyes" ]; then
        load_module ib_uverbs
        RC=$[ $RC + $? ]
    fi

    if [ $IPOIB -eq 1 ]; then
        load_module ib_ipoib
        RC=$[ $RC + $? ]

        ipoib_send_queue_size=`cat /sys/module/ib_ipoib/parameters/send_queue_size 2> /dev/null`
        if [ ! -z $ipoib_send_queue_size ]; then
            if [ $ipoib_send_queue_size -gt 1024 ]; then
                if (lspci -n | grep -qw 15b3:1011); then
                    log_msg "IPoIB: Failed to bring up interface for Connect-IB device"
                    log_msg "Please set ib_ipoib send_queue_size to be <= 1024 and restart driver"
                    echo_failure $"Loading IPoIB driver for Connect-IB device:"
                    echo "Please set ib_ipoib send_queue_size to be <= 1024 and restart driver"
                fi
            fi
        fi
    fi
    ...
}
```

该脚本比较大, 我们这里只列出和 `ib_ipoib`相关的脚本代码.
可以看到和 `IPOIB` 变量相关, 该变量初始化:

```sh
IPOIB=0
if [ "X${IPOIB_LOAD}" == "Xyes" ]; then
    IPOIB=1
fi
```

和 `$IPOIB_LOAD`变量相关
```sh
OPENIBD_CONFIG=${OPENIBD_CONFIG:-"/etc/infiniband/openib.conf"}
CONFIG=$OPENIBD_CONFIG
export LANG="C"

if [ ! -f $CONFIG ]; then
    echo No InfiniBand configuration found
    exit 0
fi

. $CONFIG
```
在脚本初期,会加载`/etc/infiniband/openib.conf`中的变量
```
Apr 02 11:09:33 node-9.domain.tld openibd[2319]: + '[' '!' -f /etc/infiniband/openib.conf ']'
Apr 02 11:09:33 node-9.domain.tld openibd[2319]: + . /etc/infiniband/openib.conf
...
Apr 02 11:09:33 node-9.domain.tld openibd[2319]: ++ IPOIB_LOAD=yes
...
```
我们再找包中的文件确认下:
```
➜  mlnx-ofa_kernel cat ./etc/infiniband/openib.conf |grep IPOIB_LOAD
IPOIB_LOAD=yes
```

所以, 可以得出, 在openibd服务中会加载`ib_ipoib`驱动, 创建netdev设备(ib0,ib1), 从而udev
检测到该netdev, 通过`90-ib.rules`, 执行`mlnx_interface_mgr.sh`网卡, 而该脚本并不会执行
ifup的动作, 而是通过后续的network 服务执行ifup, 所以正确流程如下:
```
openibd
 -> modprobe ib_ipoib
  -> create netdev :ib0
   -> mlnx_interface_mgr(不启作用)
     -> network --> ifup
```

> NOTE
>
> 正确初始化的日志如下:
> ```
> # PCI probe
> Apr 02 11:09:27 node-9.domain.tld kernel: pci 0000:4b:00.0: [15b3:101b] type 00 class 0x020700
> Apr 02 11:09:27 node-9.domain.tld kernel: pci 0000:4b:00.0: reg 0x10: [mem 0x5fff6000000-0x5fff7ffffff 64bit pref]
> Apr 02 11:09:27 node-9.domain.tld kernel: pci 0000:4b:00.0: reg 0x30: [mem 0xdfe00000-0xdfefffff pref]
> Apr 02 11:09:27 node-9.domain.tld kernel: pci 0000:4b:00.0: PME# supported from D3cold
> Apr 02 11:09:27 node-9.domain.tld kernel: pci 0000:4b:00.0: reg 0x1a4: [mem 0x5fffbc00000-0x5fffbdfffff 64bit pref]
> Apr 02 11:09:27 node-9.domain.tld kernel: pci 0000:4b:00.0: VF(n) BAR0 space: [mem 0x5fffbc00000-0x5ffff7fffff 64bit pref] (contains BAR0 for 30 VFs)
> Apr 02 11:09:27 node-9.domain.tld kernel: DMAR: Hardware identity mapping for device 0000:4b:00.0
> Apr 02 11:09:27 node-9.domain.tld kernel: iommu: Adding device 0000:4b:00.0 to group 30
>
> # MLX probe
> Apr 02 11:09:28 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: firmware version: 20.36.1010
> Apr 02 11:09:28 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: 252.048 Gb/s available PCIe bandwidth (16 GT/s x16 link)
> Apr 02 11:09:29 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: Port module event: module 0, Cable plugged
> Apr 02 11:09:29 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: mlx5_fw_tracer_start:821:(pid 5): FWTracer: Ownership granted and active
>
> # insmod  ib_ipoib
> Apr 02 11:09:34 node-9.domain.tld openibd[2319]: + /sbin/modprobe ib_ipoib
>
> # create netdev
> Apr 02 11:09:34 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: MLX5E: StrdRq(0) RqSz(1024) StrdSz(256) RxCqeCmprss(0)
> Apr 02 11:09:34 node-9.domain.tld kernel: mlx5_core 0000:4b:00.0: MLX5E: StrdRq(0) RqSz(1024) StrdSz(256) RxCqeCmprss(0)
>
> # mlnx_interface_mgr 判断 OS is booting , 不去 ifup
> Apr 02 11:09:34 node-9.domain.tld mlnx_interface_mgr.sh[2861]: + log_msg 'OS is booting, will not run ifup on ib0'
>
> # network 服务 bring up ib0
> Apr 02 11:09:43 node-9.domain.tld network[3256]: Bringing up interface ib0:  [  OK  ]
> ```

而现在的错误是由于下面的顺序
```
openibd --- disable
  network ---> ifup (没有找到ib0网络设备, 因为这时候还未加载驱动)

    某个时刻: modprobe ipoib_ib
       create netdev: ib0
         mlnx_interface_mgr(仍然判断是 OS_IS_BOOTING 所以不会去RUNNING 网卡)
           客户手动 RUNNING
```

所以该问题的根本原因是, 没有enable oepnibd服务
