# 合入patch
```
 900 void drop_slab_node(int nid)
 901 {
 902         unsigned long freed;
 903         int shift = 0;
 904
 905         do {
 906                 struct mem_cgroup *memcg = NULL;
 907
 908                 if (fatal_signal_pending(current))
 909                         return;
 910
 911                 freed = 0;
 912                 memcg = mem_cgroup_iter(NULL, NULL, NULL);
 913                 do {
 914                         freed += shrink_slab(GFP_KERNEL, nid, memcg, 0);
 915                 } while ((memcg = mem_cgroup_iter(NULL, memcg, NULL)) != NULL);
 916         } while ((freed >> shift ++) > 1);
 917 }
```

# 编写stap脚本如下
```
global freed=0;
global loop_time=0

probe kernel.statement("drop_slab_node@mm/vmscan.c:911")
{
        loop_time=0;
        printf("ppfunc(%s):911 pid(%d) tid(%d) shift(%d) freed(%d)\n", ppfunc(), pid(), tid(), $shift, freed);
}

probe kernel.function("drop_slab_node@mm/vmscan.c").return
{
        printf("ppfunc(%s) return \n", ppfunc());
}
probe kernel.function("drop_slab_node@mm/vmscan.c")
{
        printf("ppfunc(%s) enter \n", ppfunc());
}

probe kernel.function("shrink_slab").return
{
        if ($return > 0) {
                printf("ppfunc(%s) return(%d) loop_time(%d) memcg(%lx)\n", ppfunc(), $return, loop_time, $memcg);
                freed += $return
        }
        loop_time++;
}

probe kernel.statement("drop_slab_node@mm/vmscan.c:916")
{
        printf("ppfunc(%s):916 pid(%d) tid(%d) shift(%d) freed(%d) loop_time(%d)\n", ppfunc(), pid(), tid(), $shift, freed, loop_time);
}
```

# 执行stap脚本, 获取结果

> NOTE
>
> 截取一次nid
```
ppfunc(drop_slab_node) enter
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(0) freed(0)
ppfunc(shrink_slab) return(131362) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(0) freed(131362) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(1) freed(131362)
ppfunc(shrink_slab) return(143385) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(1) freed(274747) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(2) freed(274747)
ppfunc(shrink_slab) return(61387) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(2) freed(336134) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(3) freed(336134)
ppfunc(shrink_slab) return(39525) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(3) freed(375659) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(4) freed(375659)
ppfunc(shrink_slab) return(94781) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(4) freed(470440) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(5) freed(470440)
ppfunc(shrink_slab) return(25718) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(5) freed(496158) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(6) freed(496158)
ppfunc(shrink_slab) return(48213) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(6) freed(544371) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(7) freed(544371)
ppfunc(shrink_slab) return(20339) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(7) freed(564710) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(8) freed(564710)
ppfunc(shrink_slab) return(47387) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(8) freed(612097) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(9) freed(612097)
ppfunc(shrink_slab) return(47099) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(9) freed(659196) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(10) freed(659196)
ppfunc(shrink_slab) return(21707) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(10) freed(680903) loop_time(2083)
ppfunc(drop_slab_node):911 pid(48367) tid(48367) shift(11) freed(680903)
ppfunc(shrink_slab) return(3378) loop_time(0) memcg(ffff00ff802fe000)
ppfunc(drop_slab_node):916 pid(48367) tid(48367) shift(11) freed(684281) loop_time(2083)
ppfunc(drop_slab_node) return
```

可以发现,每次都是`ffff00ff802fe000` mem_cgroup 能回收到大量的slab object(每次都上千),
所以原来的代码容易一直循环,而现在shift 增长很快(指数级), 所以该patch可以优化这个流程.

怀疑是root memcg, 验证:
```
crash> p root_mem_cgroup
root_mem_cgroup = $1 = (struct mem_cgroup *) 0xffff00ff802fe000
```

确实是这样
