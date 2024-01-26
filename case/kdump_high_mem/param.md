1. 如果要将crashkernel加载到高内存(> 4G), 需要如下配置kernel cmdline
```
crashkernel=xxx,low crashkernel=xxx,high
```

`crashkernel=xxx,low`表示在低内存中预留的内存大小, `crashkernel=xxx,high`表示
在高内存中预留内存的大小.
需要注意的是: 
* 如果配置了`crashkernel=xxx,high`, `crashkernel=xxx,low`一定要配置,否则第二个
  kernel会因为分配不到low memory, 导致kernel panic
* low memory  最好配置 >= 256M (否则在kunpeng 环境中会导致
  crashkernel low memory 不足, 导致 `hibmc-drm`驱动oops

2. crashkernel 还可以配置为下面的格式
  * `crashkernel=size[KMG][@offset[KMG]]`

    其中size为 crashkernel 预留内存大小, offset则为 crashkernel的base, offset
    可以不指定, 不指定base 则在 (0, 4G)范围内
  * `crashkernel=range1:size1[,range2:size2,...][@offset]`

    和上面类似, 但是 `size`  可以根据环境内存大小来选择, 例如在 [0, range1]选择 size1, 
    [range1, range2] 选择 size2, offset 和上面一样
  * `crashkernel=auto`

    和上面类似 (`crashkernel=range1:size1[,range2:size2]...`) 但是这些range和size的关系
    则是使用的kernel 代码中默认的, 不同架构 默认值不一样
    ```cpp
    static int __init __parse_crashkernel(char *cmdline,
                                 unsigned long long system_ram,
                                 unsigned long long *crash_size,
                                 unsigned long long *crash_base,
                                 const char *name,
                                 const char *suffix)
    {
            ...
            if (strncmp(ck_cmdline, "auto", 4) == 0) {
    #ifdef CONFIG_X86_64
                    ck_cmdline = "1G-64G:160M,64G-1T:256M,1T-:512M";
    #elif defined(CONFIG_S390)
                    ck_cmdline = "4G-64G:160M,64G-1T:256M,1T-:512M";
    #elif defined(CONFIG_ARM64)
                    ck_cmdline = "2G-:448M";
    #elif defined(CONFIG_PPC64)
                    ck_cmdline = "2G-4G:384M,4G-16G:512M,16G-64G:1G,64G-128G:2G,128G-:4G";
    #endif
                    pr_info("Using crashkernel=auto, the size chosen is a best effort estimation.\n");
            }
            ...
    }
    ```
