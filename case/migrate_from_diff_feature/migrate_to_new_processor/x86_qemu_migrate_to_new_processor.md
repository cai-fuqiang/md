## 需求

用户机器更新换代，往往需要将老机器上的虚拟机迁移到新机器上，但是
现在的虚拟机配置+热迁移的方式不满足这一需求。


## 设计思路

热迁移需要保证的是, 热迁移源端和目的端的cpu(vcpu) features 是一致的.
(需要讨论下，是否目的端vcpu 可以配置为是源端 vcpu features 的超级)

如果是这样，就不能使用现在的`host-passthough`的方式. 需要自定义`cpu model`
以及features.

## 设计方法

### 虚拟机创建

创建虚拟机时, 采用 host-model的方式创建虚拟机

xml:
```xml
<cpu mode='host-model' match='exact'/>
```
启动之后，qemu cmdline关于features打印:

```
-cpu Icelake-Server,ds=on,ss=on,dtes64=on,vmx=on,pdcm=on,hypervisor=on,
tsc-adjust=on,avx512ifma=on,sha-ni=on,waitpkg=on,rdpid=on,cldemote=on,
movdiri=on,movdir64b=on,fsrm=on,md-clear=on,stibp=on,arch-capabilities=on,
avx-vnni=on,avx512-bf16=on,xsaves=on,ibpb=on,ibrs=on,amd-stibp=on,amd-ssbd=on,
rdctl-no=on,ibrs-all=on,skip-l1dfl-vmentry=on,mds-no=on,pschange-mc-no=on,tsx-ctrl=on,
hle=off,rtm=off,mpx=off,intel-pt=off
```

其xml是copy的`virsh domcapabilities`, 但是`virsh domcapabilities`多增加了`invtsc features`
```sh
[root@fpga10 wangfuqiang49]# virsh domcapabilities|grep name |grep feature |wc -l
36
[root@fpga10 wangfuqiang49]# virsh dumpxml wang_disk_attach  |grep feature |grep name |wc -l
35
[root@fpga10 wangfuqiang49]# virsh domcapabilities|grep name |grep feature |grep invtsc
      <feature policy='require' name='invtsc'/>
[root@fpga10 wangfuqiang49]# virsh dumpxml wang_disk_attach  |grep feature |grep name |grep invtsc
```
经过之前调研可知`invtsc`不是migratable features， 所以我们需要disable 掉该
features.

所以我们在创建虚拟机时需要指定如下xml(为保险起见)
```xml
<cpu mode='host-model' match='exact'>
   <feature policy='disable' name='invtsc'/>
</cpu>
```

### 迁移之前
获取源端该虚拟机xml，然后在源端, 目的端, 使用相同的features启动cpuid, 然后对比打
印结果，如果结果相同则可以迁移，结果不同则不能迁移

> TODO
>
> 1. 确认这样的方式是否可以
> 2. 修改cpuid代码

## 迁移时, 定义目的端xml
将源端xml 中的 `<cpu>` 一栏全部, copy到目的端
> NOTE
>
> 注意: 要使用 virsh dumpxml 获取

xml 片段示例:
```xml
  <cpu mode='custom' match='exact' check='full'>
    <model fallback='forbid'>Icelake-Server</model>
    <vendor>Intel</vendor>
    <feature policy='require' name='ds'/>
    ...
    <feature policy='disable' name='mpx'/>
    <feature policy='disable' name='intel-pt'/>
  </cpu>
```

## 总结
1. 由于要完成不同model cpu 之间的热迁移，不能使用`host-passthough`的方式
2. 为了尽可能的使用 host cpu features, 使用`host-model`的方式定义cpu model 和
   feature。
3. 在热迁移之前, 使用cpuid命令, 判断目的端启动的虚拟机和源端虚拟机获取到的值
   是否一致
4. 目的端启动的虚拟机的xml `<cpu>`部分 ，要和源端保持一致
