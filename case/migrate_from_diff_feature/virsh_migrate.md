## 测试环境
||node1|node2|
|----|----|----|
|cpu|Intel(R) Xeon(R) CPU E5-2640 v3 @ 2.60GHz|INTEL(R) XEON(R) PLATINUM 8538C Unknown CPU @ 2.6GHz|

## combind virsh capabilities and virsh cpu-baseline

参考链接<sup>1</sup>, 在源节点, 目的节点分别执行`virsh capabilities` 获取
host cap, 然后在通过`virsh cpu-baseline`获取cap交集

具体命令如下:
node1 node2均执行
```
virsh capabilities |xmllint --xpath '//capabilities/host/cpu' - > node1.txt
ssh root@dest "virsh capabilities |xmllint --xpath '//capabilities/host/cpu" - > node1.txt
cat node1.txt node2.txt |virsh cpu-baseline --migratable  /dev/stdin
```

输出见 [cpu_baseline.xml](cap_cpu_baseline/cpu_baseline.txt)

将该xml copy 到 虚拟机xml中，启动虚拟机(在e5机器).
启动时，会报下面的错误（片段):
```
2025-07-31T11:38:38.347138Z qemu-system-x86_64: warning: host doesn't support requested feature: CPUID.01H:EDX.ds [bit 21]
2025-07-31T11:38:38.347145Z qemu-system-x86_64: warning: host doesn't support requested feature: CPUID.01H:EDX.acpi [bit 22]
2025-07-31T11:38:38.347148Z qemu-system-x86_64: warning: host doesn't support requested feature: CPUID.01H:EDX.ht [bit 28]
2025-07-31T11:38:38.347151Z qemu-system-x86_64: warning: host doesn't support requested feature: CPUID.01H:EDX.tm [bit 29]
```
在虚拟机中执行`cpuid`命令, 观察`leaf-01-edx.ds[bit 21]`
```
00000001 00000000: 000306c1 0f000800 fffa3223 0f8bfbff
```
可以看到bit 21 是0,在host中执行:
```
00000001 00000000: 000306f2 08100800 7ffefbff bfebfbff
```
bit 21 是1.

由此可见, 使用 `virsh capabilities` 采集cpu cap, 似乎在虚拟机中无法使能该
feature.

qemu在获取host features时(其实是kvm features), 也就是kvm可以报告给虚拟机的
features. 例如host上虽然支持vmx, 但是kvm不支持nested virt, 那vmx就不能报告给
guest, 所以有一些features 需要在kvm-intel模块刚加载时，就可以确定该features
究竟能不能让guest使用。代码大致流程如下(只展开DS 的代码)

kvm 代码为:
```
vmx_set_cpu_caps
  kvm_set_cpu_caps
    ## 在这里不使能该feature，只是标记下
    kvm_cpu_cap_init(CPUID_1_EDX,
       ...
       VENDOR_F(DS),
       ...
    ## 查看
        //return boot_cpu_has(X86_FEATURE_PEBS) && kvm_pmu_cap.pebs_ept;
    if (vmx_pebs_supported()) {
        kvm_cpu_cap_check_and_set(X86_FEATURE_DS);
        kvm_cpu_cap_check_and_set(X86_FEATURE_DTES64);
    }

/*
 * Vendor Features - For features that KVM supports, but are added in later
 * because they require additional vendor enabling.
 *
 * 该feature 需要kvm支持，但是需要后续根据特定的vendor来使能
 */
#define VENDOR_F(name)                                          \
({                                                              \
        KVM_VALIDATE_CPU_CAP_USAGE(name);                       \
})

#define X86_FEATURE_DS                  ( 0*32+21) /* "dts" Debug Store */

#define KVM_VALIDATE_CPU_CAP_USAGE(name)                                \
do {                                                                    \
        u32 __leaf = __feature_leaf(X86_FEATURE_##name);                \
                                                                        \
        BUILD_BUG_ON(__leaf != kvm_cpu_cap_init_in_progress);           \
} while (0)
```

`vmx_pebs_supported`展开:
```cpp
static inline bool vmx_pebs_supported(void)
{
        return boot_cpu_has(X86_FEATURE_PEBS) && kvm_pmu_cap.pebs_ept;
}
//而 e5 cpu 有pebs, 所以要看pebs_ept在哪里赋值
__init int intel_pmu_init(void)
{
    ...
    switch (boot_cpu_data.x86_vfm) { 
    case INTEL_SAPPHIRERAPIDS_X:
    case INTEL_EMERALDRAPIDS_X:
        ...
        goto glc_common;
    case INTEL_GRANITERAPIDS_X:
    case INTEL_GRANITERAPIDS_D:
        ...
    glc_common:
        x86_pmu.pebs_ept = 1;
    }
    ...
}
```
可以看到某些型号的cpu可以直接赋值 `pebs_ept`.

而8538C 是什么型号呢?
```
[root@fpga10 wangfuqiang49]# lscpu |grep  '型号：'
型号：                                207

恰好是:
#define INTEL_EMERALDRAPIDS_X           IFM(6, 0xCF)
```

那么由此可以推断，在8538C cpu上，必有这个features 使能, 我们测试验证下.
启动后，使用cpuid获取信息.
```
00000001 00000000: 000306c1 00000800 fffa3227 0fabfbff
```
可以发现CPUID_1_EDX[bit 21] = 1. 

**所以, 使用该方式获取host上cpu features交集是不靠谱的, 需要采集guest features**

## combind virsh domcapabilities and virsh cpu-baseline
使用virsh domcapabilities 获取的features更精简，过滤掉 host支持，但是hyp 无法
模拟的features, 我们在e5机器执行
```
[root@A06-R08-I134-73-919XB72 wangfuqiang49]# virsh domcapabilities  |grep name=ds
[root@A06-R08-I134-73-919XB72 wangfuqiang49]#
```
可以发现并没有`ds` features

但是`virsh domcapabilities` 并不能很好的和`virsh cpu-baseline`结合使用(格式不认)
需要修改下.
```
virsh domcapabilities |xmllint --xpath '//domainCapabilities/cpu/mode[@name="host-model"]' - \
|sed '1c <cpu>' |sed '$c </cpu>'|sed '2i\\t<arch>x86_64</arch>' | \
sed 's/ fallback="forbid"//'
```

还是老方法，在两个节点执行 上面命令 保存到 `cpuid1.txt` `cpuid2.txt`

得出的子集如下:

[cpubaseline](./docap_cpu_baseline/cpu-baseline.xml)

将其copy到node1,node2两个节点的 qemu xml中。启动虚拟机，并执行cpuid 做对比, 结果
如下:
```
 4000000d 00000000: 00000000 00000000 00000000 00000000
 4000000e 00000000: 00000000 00000000 00000000 00000000
 4000000f 00000000: 00000000 00000000 00000000 00000000
-40000010 00000000: 0027ac40 000f4240 00000000 00000000
+40000010 00000000: 0027ac3d 000f4240 00000000 00000000
 80000000 00000000: 80000008 756e6547 6c65746e 49656e69
 80000001 00000000: 000306c1 00000000 00000021 2c100800
 80000002 00000000: 65746e49 6f43206c 50206572 65636f72
```
可以看到`40000010` leaf 有不同. 在kernel代码中的定义是:
```
#define CPUID_VMWARE_FEATURES_LEAF           0x40000010
```
这是一个vmware 半虚拟化features，按照道理不应该在 kvm guest中报告。
**个人认为是kvm BUG**. 不过这里我们先不关注， 认为两侧features相同

> !!!!!!!!!!!!!
> !!!遗留问题!!
> !!!!!!!!!!!!!

## cpu model HOST guest virsh capabilities and cpu-baseline
我们创建一个`host-passthrough`的虚拟机, 然后在虚拟机中安装libvirt，
执行`virsh capabilities` 获取的结果会怎样的, 会不会获取和在
host中执行`virsh domcapabilities`相同的结果. 我们来测试

具体命令:
host中执行:
```
virsh domcapabilities |xmllint --xpath '//domainCapabilities/cpu/mode[@name="host-model"]' - | \
sed '1c <cpu>' |sed '$c </cpu>'|sed '2i\\t<arch>x86_64</arch>' | sed 's/ fallback="forbid"//' | \
virsh cpu-baseline /dev/stdin
```
guest中执行:
```
virsh capabilities |virsh cpu-baseline /dev/stdin
```
结果不过多展开, 在guest中可能会报告出更多的features， 例如在8538c上, 可以看到使
能 `avx512-fp16`, 但是`host-passthrough`会绕过libvirt, qemu目前支持`avx512-fp16`,
但是libvirt不支持.

在xml中添加
```
  <feature policy='require' name='avx512-fp16'/>
```
启动虚拟机会遇到下面报错
```
[root@fpga10 wangfuqiang49]# virsh start wang_disk_attach
错误：Failed to start domain 'wang_disk_attach'
错误：unsupported configuration: unknown CPU feature: avx512-fp16
```

个人认为，该方法不确定性太大，还是采用libvirt比较标准的方式...

> 测试结果在:
>
> [链接](./guest_host_passthough_docap)

## 结论
目前来看，使用`combind virsh domcapabilities and virsh cpu-baseline` 比较靠谱

## 参考链接

1. [REDHAT 2024 How to use virsh cpu-compare and cpu-baseline to determine the cpu compatibility ?](https://access.redhat.com/solutions/2891431)
2. [Guest CPU model configuration in libvirt with QEMU/KVM](https://www.berrange.com/posts/2010/02/15/guest-cpu-model-configuration-in-libvirt-with-qemukvm/)
3. [virsh capabilities vs. domcapabilities](https://lists.libvirt.org/archives/list/devel@lists.libvirt.org/thread/TMK6POAK7267SU6F3J2FK6ZOBDLJWF3Q/)
