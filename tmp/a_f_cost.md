```
commit 7cf111bc39f6792abedcdfbc4e6291a5603b0ef0
Author: Johannes Weiner <hannes@cmpxchg.org>
Date:   Wed Jun 3 16:03:06 2020 -0700
```

mm: vmscan: determine anon/file pressure balance at the reclaim root

We split the LRU lists into anon and file, and we rebalance the scan
pressure between them when one of them begins thrashing: if the file cache
experiences workingset refaults, we increase the pressure on anonymous
pages; if the workload is stalled on swapins, we increase the pressure on
the file cache instead.

> ```
> thrashing: 打, 抽, 剧烈抖动
> experiences: 经验, 经历
> ```
>
> 我们将 LRU list 拆分为匿名列表和文件列表，并且当其中一个列表开始thrashing，
> 我们会重新平衡它们之间的扫描压力：如果file cache experiences workingset
> refaults, 我们会增加 file cache 的压力

With cgroups and their nested LRU lists, we currently don't do this
correctly.  While recursive cgroup reclaim establishes a relative LRU
order among the pages of all involved cgroups, LRU pressure balancing is
done on an individual cgroup LRU level. As a result, when one cgroup is
thrashing on the filesystem cache while a sibling may have cold anonymous
pages, pressure doesn't get equalized between them.

> ```
> recursive: 递归
> ```

This patch moves LRU balancing decision to the root of reclaim - the same
level where the LRU order is established.

It does this by tracking LRU cost recursively, so that every level of the
cgroup tree knows the aggregate LRU cost of all memory within its domain.
When the page scanner calculates the scan balance for any given individual
cgroup's LRU list, it uses the values from the ancestor cgroup that
initiated the reclaim cycle.

If one sibling is then thrashing on the cache, it will tip the pressure
balance inside its ancestors, and the next hierarchical reclaim iteration
will go more after the anon pages in the tree.

