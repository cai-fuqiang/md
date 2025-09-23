## csv3 force use swiotlb

使用下面的函数用来判断是否强制使用 swiotlb作为bounce
```cpp
static inline bool is_swiotlb_force_bounce(struct device *dev)
{
    struct io_tlb_mem *mem = dev->dma_io_tlb_mem;

    return mem && mem->force_bounce;
}
```

设备如果不指定，`dev->dma_io_tlb_mem`赋值为`io_tlb_default_mem`
(以virtio 设备为例)
```
register_virtio_device
|-> device_initialize
    |-> swiotlb_dev_init
        |-> dev->dma_io_tlb_mem = &io_tlb_default_mem;
```

下面的路径中会赋值`io_tlb_default_mem.force_bounce`
```
pci_iommu_alloc
|-> swiotlb_init(x86_swiotlb_enable, x86_swiotlb_flags);
    |-> swiotlb_init_remap(addressing_limit, flags, NULL);
        |-> io_tlb_default_mem.force_bounce =
                swiotlb_force_bounce || (flags & SWIOTLB_FORCE)
```


下面代码会赋值`x86_swiotlb_enable`,  `x86_swiotlb_flags`
```cpp
static void __init pci_swiotlb_detect(void)
{
    /* don't initialize swiotlb if iommu=off (no_iommu=1) */
    if (!no_iommu && max_possible_pfn > MAX_DMA32_PFN)
        x86_swiotlb_enable = true;

    /*
     * Set swiotlb to 1 so that bounce buffers are allocated and used for
     * devices that can't support DMA to encrypted memory.
     */
    if (cc_platform_has(CC_ATTR_HOST_MEM_ENCRYPT))
        x86_swiotlb_enable = true;

    /*
     * Guest with guest memory encryption currently perform all DMA through
     * bounce buffers as the hypervisor can't access arbitrary VM memory
     * that is not explicitly shared with it.
     */
    if (cc_platform_has(CC_ATTR_GUEST_MEM_ENCRYPT)) {
        x86_swiotlb_enable = true;
        x86_swiotlb_flags |= SWIOTLB_FORCE;
    }
}
```
我们主要来看下`CC_ATTR_GUEST_MEM_ENCRYPT`:
```cpp
static bool noinstr amd_cc_platform_has(enum cc_attr attr)
{
#ifdef CONFIG_AMD_MEM_ENCRYPT

    if (sev_status & MSR_AMD64_SNP_VTOM)
        return amd_cc_platform_vtom(attr);

    switch (attr) {
    case CC_ATTR_MEM_ENCRYPT:
        return sme_me_mask;

    case CC_ATTR_HOST_MEM_ENCRYPT:
        return sme_me_mask && !(sev_status & MSR_AMD64_SEV_ENABLED);

    case CC_ATTR_GUEST_MEM_ENCRYPT:
        return sev_status & MSR_AMD64_SEV_ENABLED;

    case CC_ATTR_GUEST_STATE_ENCRYPT:
        return sev_status & MSR_AMD64_SEV_ES_ENABLED;

    /*
     * With SEV, the rep string I/O instructions need to be unrolled
     * but SEV-ES supports them through the #VC handler.
     */
    case CC_ATTR_GUEST_UNROLL_STRING_IO:
        return (sev_status & MSR_AMD64_SEV_ENABLED) &&
            !(sev_status & MSR_AMD64_SEV_ES_ENABLED);

    case CC_ATTR_GUEST_SEV_SNP:
        return sev_status & MSR_AMD64_SEV_SNP_ENABLED;

    default:
        return false;
    }
#else
    return false;
#endif
}
```

可以看到只要是`sev`被使能, `CC_ATTR_GUEST_MEM_ENCRYPT` 就会返回true.

## 调试hygon csv3
```
[root@wangfuqiang49-csv3-instance-own-image ~]# lspci |grep -i virtio |grep block
00:05.0 SCSI storage controller: Virtio: Virtio 1.0 block device (rev 01)
crash> dev -p |grep 00:05.0
  ffff88810148e000 0000:00:05.0  0100  1af4:1042  ENDPOINT
crash> pci_dev.dev ffff88810148e000 |grep io_tlb
    dma_io_tlb_mem = 0xffffffff9df574c0 <io_tlb_default_mem>,

crash> p io_tlb_default_mem
io_tlb_default_mem = $7 = {
  defpool = {
    start = 1441529856,
    end = 1978400768,
    vaddr = 0xffff888055ec0000,
    nslabs = 262144,
    late_alloc = false,
    nareas = 4,
    area_nslabs = 65536,
    areas = 0xffff88823ffe6f40,
    slots = 0xffff888236a00000
  },
  nslabs = 262144, <---this
  debugfs = 0xffff888101b20360,
  force_bounce = true, <---this
  for_alloc = false,
  total_used = {
    counter = 12816
  },
  used_hiwater = {
    counter = 73775
  }
}
crash> p x86_swiotlb_enable
x86_swiotlb_enable = $8 = true
crash> p x86_swiotlb_flags
x86_swiotlb_flags = $9 = 2
crash> p sev_status
sev_status = $10 = 1073741827
crash> eval 1073741827
hexadecimal: 40000003
    decimal: 1073741827
      octal: 10000000003
     binary: 0000000000000000000000000000000001000000000000000000000000000011
```

## 结论
只要是 使用了sev，就会强制开启swiotlb, 然而即便是sev-snp也会 有pvalidate 的方式，
将private page 转换public page 供 hyp 访问，那为什么还要使用swiotlb呢?
我们可以引用kernel中的注释:
```
Guest with guest memory encryption currently perform all DMA through
bounce buffers as the hypervisor can't access arbitrary VM memory
that is not explicitly shared with it.
```
原因是dma buffer 并没有显示的 share, 驱动不可能都会适配sev，在写入数据之前share
下，而是先写入加密的buffer，然后在copy到 shared（非加密）的 swiotlb中。

## 问题解决方法

参考<sup>1</sup>

## 参考链接
1. [在g7h实例中构建CSV机密计算环境](https://help.aliyun.com/zh/ecs/user-guide/build-csv-encrypted-computing-environment?spm=a2c4g.11186623.help-menu-25365.d_3_1_9_2_3.7b8474d26LTxfX&scm=20140722.H_2507879._.OR_help-T_cn~zh-V_1)
