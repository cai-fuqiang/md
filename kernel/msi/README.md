# irq domain 早期初始化代码
```
arch_early_irq_init
```

# some irq domain

## x86_vector_domain
### x86_vector_domain->ops

```
static const struct irq_domain_ops x86_vector_domain_ops = {
    .alloc      = x86_vector_alloc_irqs,
    .free       = x86_vector_free_irqs,
    .activate   = x86_vector_activate,
    .deactivate = x86_vector_deactivate,
#ifdef CONFIG_GENERIC_IRQ_DEBUGFS
    .debug_show = x86_vector_debug_show,
#endif
};
```

## msi_default_domain

### domain_info
```
static struct msi_domain_info pci_msi_domain_info = {
    .flags      = MSI_FLAG_USE_DEF_DOM_OPS | MSI_FLAG_USE_DEF_CHIP_OPS |
              MSI_FLAG_PCI_MSIX,
    .ops        = &pci_msi_domain_ops,
    .chip       = &pci_msi_controller,
    .handler    = handle_edge_irq,
    .handler_name   = "edge",
};
```

### ops
```
static struct msi_domain_ops pci_msi_domain_ops = {
    .get_hwirq  = pci_msi_get_hwirq,
    .msi_prepare    = pci_msi_prepare,
    .set_desc   = pci_msi_set_desc,
};
```

default ops:
```
static struct msi_domain_ops msi_domain_ops_default = {
    .get_hwirq  = msi_domain_ops_get_hwirq,
    .msi_init   = msi_domain_ops_init,
    .msi_check  = msi_domain_ops_check,
    .msi_prepare    = msi_domain_ops_prepare,
    .set_desc   = msi_domain_ops_set_desc,
};
```

### irq_domain->ops
```
static const struct irq_domain_ops msi_domain_ops = {
    .alloc      = msi_domain_alloc,
    .free       = msi_domain_free,
    .activate   = msi_domain_activate,
    .deactivate = msi_domain_deactivate,
};
```

msi_default_domain->parent 		==> 		x86_vector_domain


### irq_desc->irq_data 初始化
```
alloc_desc
   desc_set_defaults 
```

# DEBUGFS 编译选项

CONFIG_GENERIC_IRQ_DEBUGFS=y


# 其他
## debug stack
```
nvme_setup_irqs
msix_capability_init
msi_domain_alloc_irqs
arch_setup_msi_irqs
native_setup_msi_irqs
msi_domain_alloc_irqs  <=== enable
__irq_domain_alloc_irqs
irq_domain_alloc_irq_data           //每个父domain中都申请一个irq_data
```

```
msi_domain_alloc
```

```
assign_irq_vector_any_locked
```

```
cmd : x/10xg 0xffffc90000275000
```

根据hardware irq id查找virq
```

```


## msi irq data
```
(gdb) p irq_data->domain->ops
$1 = (const struct irq_domain_ops *) 0xffffffff82611660 <msi_domain_ops>
(gdb) p *irq_data->domain->ops
$2 = {match = 0x0 <irq_stack_union>, select = 0x0 <irq_stack_union>, map = 0x0 <irq_stack_union>, 
  unmap = 0x0 <irq_stack_union>, xlate = 0x0 <irq_stack_union>,
  alloc = 0xffffffff811d27d8 <msi_domain_alloc>, 
  free = 0xffffffff811d2959 <msi_domain_free>, 
  activate = 0xffffffff811d26bd <msi_domain_activate>,
  deactivate = 0xffffffff811d2771 <msi_domain_deactivate>, 
  translate = 0x0 <irq_stack_union>}
```

## x86 vector_domain_ops
```
(gdb) p irqd->domain->ops
$3 = (const struct irq_domain_ops *) 0xffffffff8260aa80 <x86_vector_domain_ops>

(gdb) p *irqd->domain->ops
$4 = {match = 0x0 <irq_stack_union>, select = 0x0 <irq_stack_union>, 
  map = 0x0 <irq_stack_union>, unmap = 0x0 <irq_stack_union>, xlate = 0x0 <irq_stack_union>,
  alloc = 0xffffffff8109f3fb <x86_vector_alloc_irqs>, 
  free = 0xffffffff8109f207 <x86_vector_free_irqs>, 
  activate = 0xffffffff8109efff <x86_vector_activate>,
  deactivate = 0xffffffff8109ed8b <x86_vector_deactivate>, 
  translate = 0x0 <irq_stack_union>}
```

## ACCESS_PRIVATE
### admin_queue
```
int irq_activate(struct irq_desc *desc)
{
──────> struct irq_data *d = irq_desc_get_irq_data(desc);

        if (!irqd_affinity_is_managed(d))
                return irq_domain_activate_irq(d, false);
        return 0;
}
```

```
$8 = {state_use_accessors = 84082688, node = 0, handler_data = 0x0 <irq_stack_union>, msi_desc = 0xffff88803bd81480, affinity = {{
      bits = {18446744073709551615}}}, effective_affinity = {{bits = {1}}}}
(gdb) p d->common->state_use_accessors
$9 = 84082688

(16)IRQD_IRQ_DISABLED
(17)IRQD_IRQ_MASKED  

(24)IRQD_SINGLE_TARGET
(26)IRQD_CAN_RESERVE
```

## 获取最优的分配vector的cpu
```
/* Find the best CPU which has the lowest vector allocation count */
static unsigned int matrix_find_best_cpu(struct irq_matrix *m,
                    const struct cpumask *msk)
{
    unsigned int cpu, best_cpu, maxavl = 0;
    struct cpumap *cm;

    best_cpu = UINT_MAX;

    for_each_cpu(cpu, msk) {
        cm = per_cpu_ptr(m->maps, cpu);
        //选择必须online，而且cm->available最多的cpu。
        if (!cm->online || cm->available <= maxavl)
            continue;
        //将best_cpu返回
        best_cpu = cpu;
        maxavl = cm->available;
    }
    return best_cpu;
}
```
其中, cm的值为 : 
```
(gdb) p *cm
$1 = {available = 199, allocated = 0, managed = 2, managed_allocated = 0, initialized = true, online = true, alloc_map = {0, 0, 0, 0},
  managed_map = {25769803776, 0, 0, 0}}
```
m 的值为:
```
(gdb) p *m
$3 = {matrix_bits = 256, alloc_start = 32, alloc_end = 236, alloc_size = 204, global_available = 3974, global_reserved = 14,
  systembits_inalloc = 3, total_allocated = 7, online_maps = 20, maps = 0x24e80, scratch_map = {1125912792793087, 0, 1,
    18446726481523507200}, system_map = {1125904202858495, 0, 1, 18446726481523507200}}
```


## ？？？
bitmap_find_next_zero_area_off -> find_next_bit
