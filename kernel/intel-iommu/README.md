# cmdline
```c
__setup("intel_iommu=", intel_iommu_setup);

static int __init intel_iommu_setup(char *str)
{
    if (!str)
        return -EINVAL;
    while (*str) {
        if (!strncmp(str, "on", 2)) {
            dmar_disabled = 0;
            pr_info("IOMMU enabled\n");
		}else if
		...
	}
	return 0;
}
```

# intel-iommu setup
```c
int __init intel_iommu_init(void)
{


}
```


