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
