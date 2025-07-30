## INIT
```sh
early_csv_reserve_mem
|=> csv_cma_reserve_mem
    //struct csv_mem
    |=> csv_smr = memblock_alloc_node()

    |=> for_each_node()
        ## 每个node 预留多少个内存大小
        ## (present_pages_in_node(nid) * csv_mem_percentage / 100) << PAGE_SHIFT
        |=> size = csv_early_percent_memory_on_node(node);
        ## 以 PUD_SHIFT 分割per node res memory to one cma
        ## CSV_CMA_SHIFT = PUD_SHIFT
        |=> count = DIV_ROUND_UP(size, 1 << CSV_CMA_SHIFT);
        ## LAYOUT:
        ## array
        ## csv_cma
        ## csv_cma
        ## csv_cma
        ## ...
        |=> cma_array_size = count * sizeof(*csv_cma) + sizeof(*array);
        |=> //struct cma_array
        |=> array = memblock_alloc_node()
        |=> csv_contiguous_pernuma_area[node] = array
        |=> for (i = 0; i < count; i++)
            ## 为每个csv_cma分配 cma
            |=> cma_declare_contiguous_nid(0, CSV_CMA_SIZE, ..., fixed=false);
```

cma 会从memblock中预留一部分RAM，稍后将其以底层页块迁移类型为 MIGRATE_CMA 的形式交还给伙伴
系统。<sup>1</sup>


## csv3 set guest private memory
```sh
csv3_set_guest_private_memory
```

## 参考链接
1. [A deep dive into CMA](https://lwn.net/Articles/486301/)
