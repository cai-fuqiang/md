# 未调整前
```
crash> struct kernfs_node.name 0xffff204004b88e60
  name = 0xffff004042ae6680 "kubepods"
```

> NOTE
>
> LIST中 倒数第三个

```
crash> struct mem_cgroup.nodeinfo ffff204044428000
  nodeinfo = 0xffff204044429018

crash> x/4xg 0xffff204044429018
0xffff204044429018:     0xffff0020fb92ec00      0xffff0040ca912000
0xffff204044429028:     0xffff2020fe4c7400      0xffff20401b123400
```

*  shrinker_nr_max
```
crash> p shrinker_nr_max
shrinker_nr_max = $9 = 2829
crash> p 2829/64
$10 = 44

```
* 0xffff0020fb92ec00
 ```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff0020fb92ec00
  shrinker_info = 0xffff0020fe478000
crash> shrinker_info 0xffff0020fe478000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff0020fe478020,
  map = 0xffff0020fe47da20
}
crash> x/44xg 0xffff0020fe47da20
0xffff0020fe47da20:     0xe07fffffffffffff      0xc0ffffdfffffffff
0xffff0020fe47da30:     0xfbffffffffffffff      0xfffe01f8080eec5f
0xffff0020fe47da40:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47da50:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47da60:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47da70:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47da80:     0xbdffffffffffffdf      0xffffffbb23fcdbdf
0xffff0020fe47da90:     0xffffffffffffffff      0xfffffffffff00fff
0xffff0020fe47daa0:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47dab0:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47dac0:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47dad0:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47dae0:     0xffffffffffffffff      0xffdcbff9ebffffff
0xffff0020fe47daf0:     0xff7fffffffffffff      0xfffffffffffffe95
0xffff0020fe47db00:     0x86ffffffffffffff      0xfffffffffffff7fe
0xffff0020fe47db10:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47db20:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47db30:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47db40:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47db50:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47db60:     0xffffffffffffffff      0xffffffffffffffff
0xffff0020fe47db70:     0x000000003fffffff      0x0000000000000000
```
* 0xffff0040ca912000
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff0020fb92ec00
  shrinker_info = 0xffff0020fe478000
crash> shrinker_info 0xffff0020fe478000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff0020fe478020,
  map = 0xffff0020fe47da20
}
crash> struct mem_cgroup_per_node.shrinker_info 0xffff0040ca912000
  shrinker_info = 0xffff0040b3ef8000
crash> shrinker_info 0xffff0040b3ef8000
struct shrinker_info {
  rcu = {
    next = 0x203a4814e49,
    func = 0x0
  },
  nr_deferred = 0xffff0040b3ef8020,
  map = 0xffff0040b3efda20
}
crash> x/44xg 0xffff0040b3efda20
0xffff0040b3efda20:     0xe07fffffffffffff      0xc0bfe7dfffffffff
0xffff0040b3efda30:     0xfbffffffffffffff      0xfffe01f8080eec5f
0xffff0040b3efda40:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efda50:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efda60:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efda70:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efda80:     0xbdffffffffffffdf      0xffffffbb23fcdbdf
0xffff0040b3efda90:     0xffffffffffffffff      0xfffffffffff00fff
0xffff0040b3efdaa0:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdab0:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdac0:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdad0:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdae0:     0xffffffffffffffff      0xffdcbff9ebffffff
0xffff0040b3efdaf0:     0xff7fffffffffffff      0xfffffffffffffe95
0xffff0040b3efdb00:     0x86ffffffffffffff      0xfffffffffffff7fe
0xffff0040b3efdb10:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdb20:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdb30:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdb40:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdb50:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdb60:     0xffffffffffffffff      0xffffffffffffffff
0xffff0040b3efdb70:     0x000000003fffffff      0x0000000000000000
```
* 0xffff2020fe4c7400
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff2020fe4c7400
  shrinker_info = 0xffff20246da28000
crash> struct shrinker_info 0xffff20246da28000
struct shrinker_info {
  rcu = {
    next = 0x203ed414e49,
    func = 0x0
  },
  nr_deferred = 0xffff20246da28020,
  map = 0xffff20246da2da20
}
crash> x/44xg 0xffff20246da2da20
0xffff20246da2da20:     0xe0ffffffffffffff      0xc0bfe5dfffffffff
0xffff20246da2da30:     0xfbffffffffffffff      0xfffe01f8080ee01f
0xffff20246da2da40:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2da50:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2da60:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2da70:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2da80:     0xbdffffffffffffdf      0xffffffbb23fcdbdf
0xffff20246da2da90:     0xffffffffffffffff      0xfffffffffff00fff
0xffff20246da2daa0:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2dab0:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2dac0:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2dad0:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2dae0:     0xffffffffffffffff      0xffddffffffffffff
0xffff20246da2daf0:     0xff7fffffffffffff      0xfffffffffffffe95
0xffff20246da2db00:     0x86ffffffffffffff      0xfffffffffffff7fe
0xffff20246da2db10:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2db20:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2db30:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2db40:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2db50:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2db60:     0xffffffffffffffff      0xffffffffffffffff
0xffff20246da2db70:     0x000000003fffffff      0x0000000000000000
```
* 0xffff20401b123400
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff20401b123400
  shrinker_info = 0xffff2043fb4e0000
crash> shrinker_info 0xffff2043fb4e0000
struct shrinker_info {
  rcu = {
    next = 0x203a4814e49,
    func = 0x0
  },
  nr_deferred = 0xffff2043fb4e0020,
  map = 0xffff2043fb4e5a20
}
crash> x/44xg0xffff2043fb4e5a20
0xffff2043fb4e5a20:     0xffffffffffffffff      0xc0ffffffffffffff
0xffff2043fb4e5a30:     0xfbffffffffffffff      0xfffe01f8080ee01f
0xffff2043fb4e5a40:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5a50:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5a60:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5a70:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5a80:     0xbdffffffffffffdf      0xffffffbb23fcdbdf
0xffff2043fb4e5a90:     0xffffffffffffffff      0xfffffffffff00fff
0xffff2043fb4e5aa0:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5ab0:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5ac0:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5ad0:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5ae0:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5af0:     0xff7fffffffffffff      0xfffffffffffffe95
0xffff2043fb4e5b00:     0x86ffffffffffffff      0xfffffffffffff7fe
0xffff2043fb4e5b10:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5b20:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5b30:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5b40:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5b50:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5b60:     0xffffffffffffffff      0xffffffffffffffff
0xffff2043fb4e5b70:     0x000000003fffffff      0x0000000000000000
```

# 调整后 -- 03-14 21:00 左右调试
```
crash> p shrinker_nr_max
shrinker_nr_max = $1 = 703
crash> p 703/64
$2 = 10
```

## cgroup
```
crash> list -o 72 -H 0xffff00208027a058 -s cgroup_subsys_state.cgroup
ffff00208f3ac000
  cgroup = 0xffff00208f251000
ffff00208f38a000
  cgroup = 0xffff00208f248000
ffff0020e7786000
  cgroup = 0xffff00208ee57000
ffff0020f474a000
  cgroup = 0xffff00208ebc4000
ffff0040f8d40000
  cgroup = 0xffff00400a476000
ffff0020f477e000
  cgroup = 0xffff00208ebd5000

crash> struct cgroup.kn 0xffff00400a476000
  kn = 0xffff004009777f80
crash> struct kernfs_node.name 0xffff004009777f80
  name = 0xffff00401d762000 "kubepods"
```

## nodinfo
```
crash> struct mem_cgroup.nodeinfo ffff0040f8d40000
  nodeinfo = 0xffff0040f8d41018

crash> x/4xg 0xffff0040f8d41018
0xffff0040f8d41018:     0xffff0021e514a400      0xffff0040ef0e4800
0xffff0040f8d41028:     0xffff202055c11800      0xffff20401fa3ec00

crash> struct mem_cgroup_per_node.shrinker_info 0xffff0021e514a400
  shrinker_info = 0xffff002302f7a000
crash> shrinker_info 0xffff002302f7a000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff002302f7a020,
  map = 0xffff002302f7b820
}
crash> x/10xg 0xffff002302f7b820
0xffff002302f7b820:     0x0000000000000000      0x0000000000000000
0xffff002302f7b830:     0x0000000000000000      0x0000000000000000
0xffff002302f7b840:     0x0000000000000000      0x0000000000000000
0xffff002302f7b850:     0x0000000000000000      0x0000000000000000
0xffff002302f7b860:     0x0000000000000000      0x0000000000000000
```

# 03-15 10:00 左右调试
```
crash> p shrinker_nr_max
shrinker_nr_max = $2 = 730
crash> p 730/64
$3 = 11

crash> x/4xg 0xffff0040f8d41018
0xffff0040f8d41018:     0xffff0021e514a400      0xffff0040ef0e4800
0xffff0040f8d41028:     0xffff202055c11800      0xffff20401fa3ec00
```

* 0xffff0021e514a400
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff0021e514a400
  shrinker_info = 0xffff002302f7a000
crash> struct shrinker_info 0xffff002302f7a000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff002302f7a020,
  map = 0xffff002302f7b820
}
crash> x/11xg 0xffff002302f7b820
0xffff002302f7b820:     0x0000000000000000      0x0000000000000000
0xffff002302f7b830:     0x0000000000000000      0x0000000000000000
0xffff002302f7b840:     0x0000000000000000      0x0000000000000000
0xffff002302f7b850:     0x0000000000000000      0x0000000000000000
0xffff002302f7b860:     0x0000000000000000      0x0000000000000000
0xffff002302f7b870:     0x0000000000000000
```
* 0xffff0040ef0e4800
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff0040ef0e4800
  shrinker_info = 0xffff004123624000
crash> struct shrinker_info 0xffff004123624000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff004123624020,
  map = 0xffff004123625820
}
crash> x/11xg 0xffff004123625820
0xffff004123625820:     0x0000000000000000      0x0000000000000000
0xffff004123625830:     0x0000000000000000      0x0000000000000000
0xffff004123625840:     0x0000000000000000      0x0000000000000000
0xffff004123625850:     0x0000000000000000      0x0000000000000000
0xffff004123625860:     0x0000000000000000      0x0000000000000000
0xffff004123625870:     0x0000000000000000
```
* 0xffff202055c11800
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff202055c11800
  shrinker_info = 0xffff20212e5ea000
crash> struct shrinker_info  0xffff20212e5ea000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff20212e5ea020,
  map = 0xffff20212e5eb820
}
crash> x/11xg 0xffff20212e5eb820
0xffff20212e5eb820:     0x0000000000000000      0x0000000000000000
0xffff20212e5eb830:     0x0000000000000000      0x0000000000000000
0xffff20212e5eb840:     0x0000000000000000      0x0000000000000000
0xffff20212e5eb850:     0x0000000000000000      0x0000000000000000
0xffff20212e5eb860:     0x0000000000000000      0x0000000000000000
0xffff20212e5eb870:     0x0000000000000000
```
* 0xffff20401fa3ec00
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff20401fa3ec00
  shrinker_info = 0xffff20404a54c000
crash> struct shrinker_info 0xffff20404a54c000
struct shrinker_info {
  rcu = {
    next = 0xd84944e439e7773d,
    func = 0x5a91fc9972ddddea
  },
  nr_deferred = 0xffff20404a54c020,
  map = 0xffff20404a54d820
}
crash> x/11xg 0xffff20404a54d820
0xffff20404a54d820:     0x0000000000000000      0x0000000000000000
0xffff20404a54d830:     0x0000000000000000      0x0000000000000000
0xffff20404a54d840:     0x0000000000000000      0x0000000000000000
0xffff20404a54d850:     0x0000000000000000      0x0000000000000000
0xffff20404a54d860:     0x0000000000000000      0x0000000000000000
0xffff20404a54d870:     0x0000000000000000
```

## patch合入后
> Patch:
>
> https://github.com/easystack/escore_kernel_source/commit/f2ff9dcf456b07e8f08a2751d20d5a0a78e980a0

### shrinker_nr_max && k8s memcg
```
crash> p shrinker_nr_max
shrinker_nr_max = $4 = 3358
crash> p shrinker_nr_max/64
$5 = 52
crash> list -o 72 -H 0xffff00208027a058 -s cgroup_subsys_state.cgroup
ffff002080258000
  cgroup = 0xffff002080086000
ffff00208d97e000
  cgroup = 0xffff00208af6b000
ffff202007dea000
  cgroup = 0xffff202009515000
ffff20205de60000
  cgroup = 0xffff202052490000
ffff2020649dc000
  cgroup = 0xffff2020096a6000
ffff202063610000
  cgroup = 0xffff20205d415000
ffff2040d1b04000
  cgroup = 0xffff002bdbaf1000
crash> struct cgroup.kn 0xffff2020096a6000
  kn = 0xffff202008dffd68
crash> struct kernfs_node.name 0xffff202008dffd68
  name = 0xffff2020b64dd980 "kubepods"
```
### nodeinfo
```
crash> struct mem_cgroup.nodeinfo ffff20205de60000
  nodeinfo = 0xffff20205de61018
crash> x/4xg 0xffff20205de61018
0xffff20205de61018:     0xffff00208a26c000      0xffff004043e33400
0xffff20205de61028:     0xffff20205a6ac000      0xffff2040c7b1fc00
```

* 0xffff00208a26c000
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff00208a26c000
  shrinker_info = 0xffff0024e5658000
crash> struct shrinker_info 0xffff0024e5658000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff0024e5658020,
  map = 0xffff0024e565ea20
}
crash> x/52xg 0xffff0024e565ea20
0xffff0024e565ea20:     0x0000000000000200      0x0000000000000000
0xffff0024e565ea30:     0x0000000000000000      0x0000000000000000
0xffff0024e565ea40:     0x0000000000000000      0x0000000000000000
0xffff0024e565ea50:     0x0000000000000000      0x0000000000000000
0xffff0024e565ea60:     0x0000000000000000      0x0000000000000000
0xffff0024e565ea70:     0x0000000000000000      0x0000000000000000
0xffff0024e565ea80:     0x0000000000000000      0x0000000000000000
0xffff0024e565ea90:     0x0000000000000000      0x0000000000000000
0xffff0024e565eaa0:     0x0000000000000000      0x0000000000000000
0xffff0024e565eab0:     0x0000000000000000      0x0000000000000000
0xffff0024e565eac0:     0x0000000000000000      0x0000000000000000
0xffff0024e565ead0:     0x0000000000000000      0x0000000000000000
0xffff0024e565eae0:     0x0000000000000000      0x0000000000000000
0xffff0024e565eaf0:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb00:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb10:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb20:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb30:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb40:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb50:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb60:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb70:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb80:     0x0000000000000000      0x0000000000000000
0xffff0024e565eb90:     0x0000000000000000      0x0000000000000000
0xffff0024e565eba0:     0x0000000000000000      0x0000000000000000
0xffff0024e565ebb0:     0x0000000000000000      0x0000000000000000
```

<!--
* 0xffff004043e33400
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff004043e33400
  shrinker_info = 0xffff004074d78000
crash> struct shrinker_info 0xffff004074d78000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff004074d78020,
  map = 0xffff004074d7ea20
}
crash> x/52xg 0xffff004074d7ea20
0xffff004074d7ea20:     0x0000000000000200      0x0000000000000000
0xffff004074d7ea30:     0x0000000000000000      0x0000000000000000
0xffff004074d7ea40:     0x0000000000000000      0x0000000000000000
0xffff004074d7ea50:     0x0000000000000000      0x0000000000000000
0xffff004074d7ea60:     0x0000000000000000      0x0000000000000000
0xffff004074d7ea70:     0x0000000000000000      0x0000000000000000
0xffff004074d7ea80:     0x0000000000000000      0x0000000000000000
0xffff004074d7ea90:     0x0000000000000000      0x0000000000000000
0xffff004074d7eaa0:     0x0000000000000000      0x0000000000000000
0xffff004074d7eab0:     0x0000000000000000      0x0000000000000000
0xffff004074d7eac0:     0x0000000000000000      0x0000000000000000
0xffff004074d7ead0:     0x0000000000000000      0x0000000000000000
0xffff004074d7eae0:     0x0000000000000000      0x0000000000000000
0xffff004074d7eaf0:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb00:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb10:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb20:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb30:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb40:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb50:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb60:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb70:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb80:     0x0000000000000000      0x0000000000000000
0xffff004074d7eb90:     0x0000000000000000      0x0000000000000000
0xffff004074d7eba0:     0x0000000000000000      0x0000000000000000
0xffff004074d7ebb0:     0x0000000000000000      0x0000000000000000
```
* 0xffff20205a6ac000
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff20205a6ac000
  shrinker_info = 0xffff202161b78000
crash> struct shrinker_info 0xffff202161b78000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff202161b78020,
  map = 0xffff202161b7ea20
}
crash> x/52xg 0xffff202161b7ea20
0xffff202161b7ea20:     0x0000000000000200      0x0000000000000000
0xffff202161b7ea30:     0x0000000000000000      0x0000000000000000
0xffff202161b7ea40:     0x0000000000000000      0x0000000000000000
0xffff202161b7ea50:     0x0000000000000000      0x0000000000000000
0xffff202161b7ea60:     0x0000000000000000      0x0000000000000000
0xffff202161b7ea70:     0x0000000000000000      0x0000000000000000
0xffff202161b7ea80:     0x0000000000000000      0x0000000000000000
0xffff202161b7ea90:     0x0000000000000000      0x0000000000000000
0xffff202161b7eaa0:     0x0000000000000000      0x0000000000000000
0xffff202161b7eab0:     0x0000000000000000      0x0000000000000000
0xffff202161b7eac0:     0x0000000000000000      0x0000000000000000
0xffff202161b7ead0:     0x0000000000000000      0x0000000000000000
0xffff202161b7eae0:     0x0000000000000000      0x0000000000000000
0xffff202161b7eaf0:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb00:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb10:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb20:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb30:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb40:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb50:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb60:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb70:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb80:     0x0000000000000000      0x0000000000000000
0xffff202161b7eb90:     0x0000000000000000      0x0000000000000000
0xffff202161b7eba0:     0x0000000000000000      0x0000000000000000
0xffff202161b7ebb0:     0x0000000000000000      0x0000000000000000
```
* 0xffff2040c7b1fc00
```
crash> struct mem_cgroup_per_node.shrinker_info 0xffff2040c7b1fc00
  shrinker_info = 0xffff204103b28000
crash> struct shrinker_info 0xffff204103b28000
struct shrinker_info {
  rcu = {
    next = 0x3a3336312e313532,
    func = 0x4e222c2235303139
  },
  nr_deferred = 0xffff204103b28020,
  map = 0xffff204103b2ea20
}
crash> x/52xg 0xffff204103b2ea20
0xffff204103b2ea20:     0x0000000000000200      0x0000000000000000
0xffff204103b2ea30:     0x0000000000000000      0x0000000000000000
0xffff204103b2ea40:     0x0000000000000000      0x0000000000000000
0xffff204103b2ea50:     0x0000000000000000      0x0000000000000000
0xffff204103b2ea60:     0x0000000000000000      0x0000000000000000
0xffff204103b2ea70:     0x0000000000000000      0x0000000000000000
0xffff204103b2ea80:     0x0000000000000000      0x0000000000000000
0xffff204103b2ea90:     0x0000000000000000      0x0000000000000000
0xffff204103b2eaa0:     0x0000000000000000      0x0000000000000000
0xffff204103b2eab0:     0x0000000000000000      0x0000000000000000
0xffff204103b2eac0:     0x0000000000000000      0x0000000000000000
0xffff204103b2ead0:     0x0000000000000000      0x0000000000000000
0xffff204103b2eae0:     0x0000000000000000      0x0000000000000000
0xffff204103b2eaf0:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb00:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb10:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb20:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb30:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb40:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb50:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb60:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb70:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb80:     0x0000000000000000      0x0000000000000000
0xffff204103b2eb90:     0x0000000000000000      0x0000000000000000
0xffff204103b2eba0:     0x0000000000000000      0x0000000000000000
0xffff204103b2ebb0:     0x0000000000000000      0x0000000000000000
```
-->

## 编写测试程序进一步测试
```
mkdir /sys/fs/cgroup/memory/ct
echo 4000M > /sys/fs/cgroup/memory/ct/memory.kmem.limit_in_bytes
#for i in `seq 0 64`

for i in `seq 0 192`
do
        mkdir /sys/fs/cgroup/memory/ct/$i
        echo $$ > /sys/fs/cgroup/memory/ct/$i/cgroup.procs
        mkdir -p s/$i
        mount -t tmpfs $i s/$i
        touch s/$i/file
done
```

> NOTE
>
> 以上脚本来自:
>
> https://lore.kernel.org/all/152399129187.3456.5685999465635300270.stgit@localhost.localdomain/

执行脚本后, 读取map
```
crash> struct mem_cgroup.nodeinfo ffff20205de60000
  nodeinfo = 0xffff20205de61018
crash> x/4xg 0xffff20205de61018
0xffff20205de61018:     0xffff00208a26c000      0xffff004043e33400
0xffff20205de61028:     0xffff20205a6ac000      0xffff2040c7b1fc00
crash> struct mem_cgroup_per_node.shrinker_info 0xffff00208a26c000
  shrinker_info = 0xffff002dbbf90000
crash> struct shrinker_info 0xffff002dbbf90000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff002dbbf90020,
  map = 0xffff002dbbf97020
}
crash> x/52xg 0xffff002dbbf97020
0xffff002dbbf97020:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97030:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97040:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97050:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97060:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97070:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97080:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97090:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf970a0:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf970b0:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf970c0:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf970d0:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf970e0:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf970f0:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97100:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97110:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97120:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97130:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97140:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97150:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97160:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97170:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97180:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf97190:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf971a0:     0xffffffffffffffff      0xffffffffffffffff
0xffff002dbbf971b0:     0xffffffffffffffff      0xffffffffffffffff
```

执行`echo 2 > /proc/sys/vm/drop_caches`
然后再次查看
```
crash> struct mem_cgroup.nodeinfo ffff20205de60000
  nodeinfo = 0xffff20205de61018
crash> x/4xg 0xffff20205de61018
0xffff20205de61018:     0xffff00208a26c000      0xffff004043e33400
0xffff20205de61028:     0xffff20205a6ac000      0xffff2040c7b1fc00
crash> struct mem_cgroup_per_node.shrinker_info 0xffff00208a26c000
  shrinker_info = 0xffff002dbbf90000
crash> shrinker_info 0xffff002dbbf90000
struct shrinker_info {
  rcu = {
    next = 0x0,
    func = 0x0
  },
  nr_deferred = 0xffff002dbbf90020,
  map = 0xffff002dbbf97020
}
crash> x/52xg 0xffff002dbbf97020
0xffff002dbbf97020:     0x0000000000000200      0x0000000000000000
0xffff002dbbf97030:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97040:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97050:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97060:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97070:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97080:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97090:     0x0000000000000000      0x0000000000000000
0xffff002dbbf970a0:     0x0000000000000000      0x0000000000000000
0xffff002dbbf970b0:     0x0000000000000000      0x0000000000000000
0xffff002dbbf970c0:     0x0000000000000000      0x0000000000000000
0xffff002dbbf970d0:     0x0000000000000000      0x0000000000000000
0xffff002dbbf970e0:     0x0000000000000000      0x0000000000000000
0xffff002dbbf970f0:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97100:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97110:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97120:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97130:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97140:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97150:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97160:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97170:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97180:     0x0000000000000000      0x0000000000000000
0xffff002dbbf97190:     0x0000000000000000      0x0000000000000000
0xffff002dbbf971a0:     0x0000000000000000      0x0000000000000000
0xffff002dbbf971b0:     0x0000000000000000      0x0000000000000000
```

没有问题, 符合预期.
