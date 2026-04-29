#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init read_dfr0_init(void)
{
    u64 val;

    asm volatile("mrs %0, id_aa64dfr0_el1" : "=r" (val));

    printk(KERN_INFO "SYS_ID_AA64DFR0_EL1 = 0x%016llx\n", val);
    return 0;
}

static void __exit read_dfr0_exit(void)
{
    printk(KERN_INFO "read_dfr0 module unloaded\n");
}

module_init(read_dfr0_init);
module_exit(read_dfr0_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Read ARMv8 ID_AA64DFR0_EL1 system register");
MODULE_AUTHOR("Your Name");
