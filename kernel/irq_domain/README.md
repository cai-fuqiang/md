# debug stack
```
nvme_setup_irqs
msix_capability_init
msi_domain_alloc_irqs
arch_setup_msi_irqs
native_setup_msi_irqs
msi_domain_alloc_irqs  <=== enable
__irq_domain_alloc_irqs
irq_domain_alloc_irq_data
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

# msi irq data
```
(gdb) p irq_data->domain->ops
$1 = (const struct irq_domain_ops *) 0xffffffff82611660 <msi_domain_ops>
(gdb) p *irq_data->domain->ops
$2 = {match = 0x0 <irq_stack_union>, select = 0x0 <irq_stack_union>, map = 0x0 <irq_stack_union>, unmap = 0x0 <irq_stack_union>, xlate = 0x0 <irq_stack_union>,
  alloc = 0xffffffff811d27d8 <msi_domain_alloc>, free = 0xffffffff811d2959 <msi_domain_free>, activate = 0xffffffff811d26bd <msi_domain_activate>,
  deactivate = 0xffffffff811d2771 <msi_domain_deactivate>, translate = 0x0 <irq_stack_union>}
```

# x86 vector_domain_ops
```
(gdb) p irqd->domain->ops
$3 = (const struct irq_domain_ops *) 0xffffffff8260aa80 <x86_vector_domain_ops>

(gdb) p *irqd->domain->ops
$4 = {match = 0x0 <irq_stack_union>, select = 0x0 <irq_stack_union>, map = 0x0 <irq_stack_union>, unmap = 0x0 <irq_stack_union>, xlate = 0x0 <irq_stack_union>,
  alloc = 0xffffffff8109f3fb <x86_vector_alloc_irqs>, free = 0xffffffff8109f207 <x86_vector_free_irqs>, activate = 0xffffffff8109efff <x86_vector_activate>,
  deactivate = 0xffffffff8109ed8b <x86_vector_deactivate>, translate = 0x0 <irq_stack_union>}
```
