# 关于__warn打印
实际上该__warn打印是由于intel硬件变动导致，之前intel
NUMA 架构，每个NUMA 共享一个LLC(last level cache)，
但是在sub numa cluster（SNC）架构上不再这样设计，不同的NUMA
CPU core也可以共享LLC（当然并不是所有的cpu共享）。而
客户的服务器类型为Intel's Skylake Server CPU就是该架构。

我们先来分析下本机上的kernel代码流程

## 当前版本kernel代码分析

### 代码流程
```
start_secondary
   set_cpu_sibling_map
        match_llc 
```

### set_cpu_sibling_map
```C/C++
void set_cpu_sibling_map(int cpu)                             
{
	...
	struct cpuinfo_x86 *c = &cpu_data(cpu);
	...

	cpumask_set_cpu(cpu, cpu_sibling_setup_mask);    //在这里会设置，表示cpu已经初始化
	...                                                         
    for_each_cpu(i, cpu_sibling_setup_mask) {
        o = &cpu_data(i); 
                                                              
        if ((i == cpu) || (has_smt && match_smt(c, o))) 
            link_mask(sibling, cpu, i);                       
                                                              
		//相当于会拿当前cpu和已经setup的cpu作为参数传入match_llc
        if ((i == cpu) || (has_mp && match_llc(c, o))) 
            link_mask(llc_shared, cpu, i);                    
                                                              
    } 
	...

```

实际上上面的流程就是所有的cpu都会进行`match_llc`的两两比较。

### match_llc

```C/C++
static bool match_llc(struct cpuinfo_x86 *c, struct cpuinfo_x86 *o)
{
    int cpu1 = c->cpu_index, cpu2 = o->cpu_index;

    if (per_cpu(cpu_llc_id, cpu1) != BAD_APICID &&
        per_cpu(cpu_llc_id, cpu1) == per_cpu(cpu_llc_id, cpu2))
        return topology_sane(c, o, "llc");

    return false;
}

```
这个函数的逻辑是，如果两个CPU的`cpu_llc_id`相同，则会调用`topology_sane`
进行下一步处理
### topology_sane
```C/C++
static bool
topology_sane(struct cpuinfo_x86 *c, struct cpuinfo_x86 *o, const char *name)
{
    int cpu1 = c->cpu_index, cpu2 = o->cpu_index;

    return !WARN_ONCE(!topology_same_node(c, o),		//如果不在一个numa上，则会触发_WARN_ONCE
        "sched: CPU #%d's %s-sibling CPU #%d is not on the same node! "
        "[node: %d != %d]. Ignoring dependency.\n",
        cpu1, name, cpu2, cpu_to_node(cpu1), cpu_to_node(cpu2));
}

```

综合上个函数来看，结论是：
如果两个cpu `cpu_llc_id`相同，但是numa node 不同，则会触发这个__warn

## 上游内核改动
* commit 信息
```
commit 1340ccfa9a9afefdbab90d7935d4ed19817e37c2
Author: Alison Schofield <alison.schofield@intel.com>
Date:   Fri Apr 6 17:21:30 2018 -0700
```

* code

```C/C++
+static const struct x86_cpu_id snc_cpu[] = {
+       { X86_VENDOR_INTEL, 6, INTEL_FAM6_SKYLAKE_X },
+       {}
+};
+
 static bool match_llc(struct cpuinfo_x86 *c, struct cpuinfo_x86 *o)
 {
        int cpu1 = c->cpu_index, cpu2 = o->cpu_index;

-       if (per_cpu(cpu_llc_id, cpu1) != BAD_APICID &&
-           per_cpu(cpu_llc_id, cpu1) == per_cpu(cpu_llc_id, cpu2))
-               return topology_sane(c, o, "llc");
+       /* Do not match if we do not have a valid APICID for cpu: */
+       if (per_cpu(cpu_llc_id, cpu1) == BAD_APICID)
+               return false;

-       return false;
+       /* Do not match if LLC id does not match: */
+       if (per_cpu(cpu_llc_id, cpu1) != per_cpu(cpu_llc_id, cpu2))
+               return false;
+
+       /*
+        * Allow the SNC topology without warning. Return of false
+        * means 'c' does not share the LLC of 'o'. This will be
+        * reflected to userspace.
+        */
		/*
		能走到这说明是一个cpu_llc_id， 如果是不同的NUMA 并且
		是snc 架构的cpu的话，则不在进行检查比较
		 */
+       if (!topology_same_node(c, o) && x86_match_cpu(snc_cpu))
+               return false;
+
+       return topology_sane(c, o, "llc");
 }
```

从上述改动来看，上游社区进行了特殊处理，该改动已经合入了`4.18.147`版本内核
# 相关链接
https://lwn.net/Articles/750539/
https://www.starduster.me/2018/05/18/talk-about-evolution-from-broadwell-to-skylake/<br />
