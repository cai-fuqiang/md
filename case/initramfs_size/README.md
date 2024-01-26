# 调研kernel以及其安装包，所占用的size

在调研之前，发现了node-1, node-2 initramfs 的size不同,
我们在下面看下:
## node-1 node-2 size不同
```
[root@node-1 ~]# ls -lh /boot/initramfs-4.18.0-147.5.1.es8_24.aarch64.img
-rw------- 1 root 67M May 11 12:28 /boot/initramfs-4.18.0-147.5.1.es8_24.aarch64.img

[root@node-2 ~]# ls -lh /boot/initramfs-4.18.0-147.5.1.es8_24.aarch64.img
-rw------- 1 root 30M May 16 16:06 /boot/initramfs-4.18.0-147.5.1.es8_24.aarch64.img
```
解压看下，发现两者的`/usr/lib/firmware/`目录大小有着较大的差别
```
[root@node-1 fs]# du -h --max-depth 1 usr/lib/
124K    usr/lib/dracut
82M     usr/lib/firmware
4.5M    usr/lib/kbd
16K     usr/lib/modprobe.d
20M     usr/lib/modules
20K     usr/lib/sysctl.d
6.1M    usr/lib/systemd
12K     usr/lib/tmpfiles.d
432K    usr/lib/udev
113M    usr/lib/

[root@node-2 fs]# du -h --max-depth 1 usr/lib/
20K     usr/lib/dracut
200K    usr/lib/modules
220K    usr/lib/
```

> NOTE 
>
> 这个是根据当时安装kernel时（或者kmod) 时环境定，我们
> 下面会举例说明下.

# 相关流程
查看kernel.spec, 发现生成initramfs是通过下面的命令生成。
```
%{_sbindir}/weak-modules --add-kernel %{KVERREL}%{?1:+%{1}} || exit $?\
```

这里并没有细看，`weak-modules`脚本的具体流程, 只是简单的调试了下，发现
该命令通过`dracut` 命令 生成 initramfs, 具体命令如下:
```
/usr/bin/dracut -f /boot/initramfs-4.18.0-147.5.1.es8_24.aarch64.img 4.18.0-147.5.1.es8_24.aarch64 
```

## 查看dracut程序

> NOTE
>
> 这里需要说明下，node-1 / node-2 上面dracut的版本不同，导致
> 脚本流程不太一样。这里我们以es8的dracut 版本`049-201.git20220131.es8`
> (node-2)为例，讲述
```sh
#============(1)==============
for moddir in "$dracutbasedir/modules.d"/[0-9][0-9]*; do
    _d_mod=${moddir##*/}; _d_mod=${_d_mod#[0-9][0-9]}
    #==============(3)=============
    [[ "$mods_to_load" == *\ $_d_mod\ * ]] || continue
    if [[ $show_modules = yes ]]; then
        printf "%s\n" "$_d_mod"
    else
        dinfo "*** Including module: $_d_mod ***"
    fi
    if [[ $kernel_only == yes ]]; then
        module_installkernel "$_d_mod" "$moddir" || {
            dfatal "installkernel failed in module $_d_mod"
            exit 1
        }
    else
        #============(2)==============
        module_install "$_d_mod" "$moddir"
        if [[ $no_kernel != yes ]]; then
            module_installkernel "$_d_mod" "$moddir" || {
                dfatal "installkernel failed in module $_d_mod"
                exit 1
            }
        fi
    fi
    #==============(3)=============
    mods_to_load=${mods_to_load// $_d_mod /}
    modules_loaded+="$_d_mod "

    #print the module install size
    if [ -n "$printsize" ]; then
        _isize_new=$(du -sk ${initdir}|cut -f1)
        _isize_delta=$((_isize_new - _isize))
        printf "%s\n" "$_d_mod install size: ${_isize_delta}k"
        _isize=$_isize_new
    fi
done
```
1. 在该处会去查看`/usr/lib/dracut/modules.d/`的各个modules，然后循环
2. 在该处调用 `module_install` 函数, 参数为 `mod_name` `mod_dir`
(以00bash 为例，mod_name为bash, mod_dir /usr/lib/dracut/modules.d/00bash)
3. 在开头部分回去检查modules是否在`mods_to_load`列表中, 如果没有直接continue
(这个地方比较关键，我们一会还会看到），而在最后，则将处理完成的mod,
在`mods_to_load`列表中删除，在`modules_loaded`中加上该mod


这里比较熬重要的是`module_install` 函数:
该函数是一个通用函数在`/usr/lib/dracut/dracut-init.sh`中定义:
```
module_install() {
    local _moddir=$(echo ${dracutbasedir}/modules.d/??${1} | { read a b; echo "$a"; })
    local _ret
    [[ -d $_moddir ]] || return 1
    #=========(1)============
    if [[ ! -f $_moddir/module-setup.sh ]]; then
        [[ -x $_moddir/install ]] && . "$_moddir/install"
        return $?
    else
        unset check depends cmdline install installkernel
        install() { true; }
        #=========(2)============
        . $_moddir/module-setup.sh
        #=========(3)============
        moddir=$_moddir install
        _ret=$?
        unset check depends cmdline install installkernel
        return $_ret
    fi
}
```
该函数流程比较简单:
1. 查看 modir中有没有`module-setup.sh`文件, 如果没有看看有没有
`install`文件，如果有 source下(不知道为什么没有执行)
2. source下`moddir/module-setup.sh`文件，然后执行install
查看返回值

但是该问题并不是出现在这里，经调试node-1中的`mods_to_load`
多出了`05rdma` module, 而这个mod会多生成一些firmware到initramfs

我们看看`mods_to_load`变量初始化代码:

## mods_to_load变量初始化
dracut 脚本中的代码:
```
mods_to_load=""
# check all our modules to see if they should be sourced.
# This builds a list of modules that we will install next.
for_each_module_dir check_module
```
在`for_each_module_dir`中初始化了`mods_to_load`, 
```sh
for_each_module_dir() {
    local _modcheck
    local _mod
    local _moddir
    local _func
    _func=$1
    for _moddir in "$dracutbasedir/modules.d"/[0-9][0-9]*; do
        [[ -d $_moddir ]] || continue;
        [[ -e $_moddir/install || -e $_moddir/installkernel || \
            -e $_moddir/module-setup.sh ]] || continue
        _mod=${_moddir##*/}; _mod=${_mod#[0-9][0-9]}
        $_func $_mod 1
    done
    ...
}
```
该函数流程比较简单，便利每个modules,然后 调用func 也就是 `check_module`

`check_module`
```sh
if [[ " $dracutmodules $add_dracutmodules $force_add_dracutmodules" == *\ $_mod\ * ]]; then
    if [[ " $dracutmodules $force_add_dracutmodules " == *\ $_mod\ * ]]; then
        module_check $_mod 1; ret=$?
    else
        module_check $_mod 0; ret=$?
    fi
    # explicit module, so also accept ret=255
    [[ $ret = 0 || $ret = 255 ]] || return 1
else #走这个分支
    # module not in our list
    if [[ $dracutmodules = all ]]; then
        # check, if we can and should install this module
        module_check $_mod; ret=$?
        # echo module_check ret = $ret
        if [[ $ret != 0 ]]; then
            [[ $2 ]] && return 1
            [[ $ret != 255 ]] && return 1
        fi
    else
        # skip this module
        return 1
    fi
fi
```
在这个分支中，会调用`module_check`
我们来看下这个函数:


`module_check`:
```sh
module_check() {
    local _moddir=$(echo ${dracutbasedir}/modules.d/??${1} | { read a b; echo "$a"; })
    local _ret
    local _forced=0
    local _hostonly=$hostonly
    [ $# -eq 2 ] && _forced=$2
    [[ -d $_moddir ]] || return 1
    if [[ ! -f $_moddir/module-setup.sh ]]; then
        # if we do not have a check script, we are unconditionally included
        [[ -x $_moddir/check ]] || return 0
        [ $_forced -ne 0 ] && unset hostonly
        $_moddir/check $hostonly
        _ret=$?
    else
        unset check depends cmdline install installkernel
        check() { true; }
        . $_moddir/module-setup.sh
        is_func check || return 0
        #===========(1)============
        [ $_forced -ne 0 ] && unset hostonly
        moddir=$_moddir check $hostonly
        _ret=$?
        unset check depends cmdline install installkernel
    fi
    hostonly=$_hostonly
    return $_ret
}
```
该函数可能有一个或者两个参数，第二个参数代表 force, 表示
强制的意思。我们看后面(1)处的部分，如果force != 0, 则会`unset hostonly`,
而和之前module_install的逻辑一样，执行 `moddir/module-setup.sh`中的`check()`
函数，上面我们调试到是`rdma`module执行有差异，我们来看下，该文件中的`check()`
函数:
```sh
check() {
        [ -n "$hostonly" -a -c /sys/class/infiniband_verbs/uverbs0 ] && return 0
        [ -n "$hostonly" ] && return 255
        return 0
}
```
第一条语句是and操作(-a)，如果第一个为真，则会查看下`/sys/class/infiniband_verbs/uverbs0`是否为
字符设备，如果都满足，返回0, 但是该文件不是字符设备

第二条语句判断`"$hostonly"`是否是空， 不是空为真，返回 255, 而node-2 中返回255.
那么这里有个问题，`$hostonly`在何时初始化的。
我们回过头来再看`dracut`中的`$hostonly`相关代码:

## hostonly
```sh
for f in $(dropindirs_sort ".conf" "$confdir" "$dracutbasedir/dracut.conf.d"); do
    check_conf_file "$f"
    [[ -e $f ]] && . "$f"
done
```
这里会去检测查看`/usr/lib/dracut/dracut.conf.d`的个个文件，然后调用che  `check_conf_file $f`
(这个不是关键），主要是下一句，如果是有该文件，去source下该文件。我们在node-1和node-2
上加了调试语句:
```sh
for f in $(dropindirs_sort ".conf" "$confdir" "$dracutbasedir/dracut.conf.d"); do
        echo 1. $hostonly hostonly
        echo \$f "$f"
    check_conf_file "$f"
    [[ -e $f ]] && . "$f"
        echo 2.$hostonly hostonly
done
        echo E.$hostonly hostonly
```

我们来看下调试结果:

node-1:
```
1.hostonly
$f /usr/lib/dracut/dracut.conf.d/01-dist.conf
2.hostonly yes
1.hostonly yes
$f /usr/lib/dracut/dracut.conf.d/02-generic-image.conf
2.hostonly no
1.hostonly no
$f /usr/lib/dracut/dracut.conf.d/02-rescue.conf
2.hostonly no
1.hostonly no
$f /usr/lib/dracut/dracut.conf.d/50-nss-softokn.conf
2.hostonly no
E:hostonly no
```
node-2:
```
1. hostonly
$f /usr/lib/dracut/dracut.conf.d/01-dist.conf
2.yes hostonly
1. yes hostonly
$f /usr/lib/dracut/dracut.conf.d/02-rescue.conf
2.yes hostonly
1. yes hostonly
$f /usr/lib/dracut/dracut.conf.d/50-nss-softokn.conf
2.yes hostonly
E.yes hostonly
```
可以看到关于`hostonly`的值，是取决与最后一个配置文件，是否
要修改`hostonly`也就是优先级最高，而在node-1中
`/usr/lib/dracut/dracut.conf.d/02-generic-image.conf`我们来看下:
```
hostonly="no"
```

这里面设置了`hostonly=no`, 而之后的配置文件没有更改，所以最后的`hostonly=no`

而后面会根据`hostonly`的值
```
[[ $hostonly = yes ]] && hostonly="-h"
[[ $hostonly != "-h" ]] && unset hostonly
```

下面是关于`hostonly`, chatgpt的解释:

https://poe.com/s/cBfDx4jK7aDK5NcUgWn0

大概的意思是，hostonly表示只在dracut中生成当前主机需要需要的模块，
而rdma module会去看这个，如果没有hostonly,他就在dracut中生成，所以
rdma module不是仅当前主机需要的。

那么我们来看下, `02-generic-image.conf`文件在哪个包中:
```
dracut-config-generic-049-191.git20210920.es8.aarch64
```

> NOTE
>
> 关于`dracut-049-191.git20210920`版本来自于`centos-8.5`, 连接:
> https://mirrors.tuna.tsinghua.edu.cn/centos-vault/8.5.2111/BaseOS/Source/SPackages/dracut-049-191.git20210920.el8.src.rpm
>
> 而`dracut-049-201.git20220131`则来自centos8.6, 
> https://download.rockylinux.org/vault/rocky/8.6/BaseOS/source/tree/Packages/d/

而看起来新版dracut (049-201.git20220131.es8.aarch64) 已经没有这个文件了。
但是查看环境, 发现该包并没有安装:
```
[root@node-3 ~]# yum list |grep dracut-config
dracut-config-rescue.x86_64                            049-201.git20220131.es8                                @anaconda
dracut-config-generic.x86_64                           049-201.git20220131.es8                                roller
[root@node-3 ~]# yum list --installed |grep dracut-config
dracut-config-rescue.x86_64           049-201.git20220131.es8            @anaconda
```
# 做下测试
在node-2中编写`/usr/lib/dracut/dracut.conf.d/02-generic-image.conf`, 如下:
```
[root@node-2 ~]# cat /usr/lib/dracut/dracut.conf.d/02-generic-image.conf
hostonly="no"
```

执行dracut相关命令（这里做了一些中途exit,不再展示), 查看firmware目录:
```
1.9M    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/cxgb4
852K    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/qed
912K    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/mellanox
18M     /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/amdgpu
1.1M    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/nvidia
5.3M    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/radeon
36K     /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/cavium
24K     /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/ene-ub6250
136K    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/emi26
176K    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/emi62
16K     /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/kaweth
660K    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/bnx2x
220K    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/bnx2
16K     /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/tigon
4.9M    /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/liquidio
29M     /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/netronome
88K     /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/rtl_nic
65M     /var/tmp/dracut.q3nPs7/initramfs/usr/lib/firmware/
```
可以看到大小为`65M`, 已经增长了不少（可能还有驱动层面的差异，没有再去看)

# 结论
所以最终的问题结论是:

**node-1中安装了dracut-config-generic包，而node-2中没有安装该包，导致node-1中的ramdisk比node-2大**
