# 更激进的测试程序
原理:

目前, `__mod_memcg_state`函数, batch charge有个阈值,
该值为`MEMCG_CHANGE_BATCH` -- `32`, 如下
```cpp
/* idx can be of type enum memcg_stat_item or node_stat_item */
static inline void __mod_memcg_state(struct mem_cgroup *memcg,
                                     int idx, int val)
{
        long x;

        if (mem_cgroup_disabled())
                return;

        x = val + __this_cpu_read(memcg->stat_cpu->count[idx]);
        if (unlikely(abs(x) > MEMCG_CHARGE_BATCH)) {
                atomic_long_add(x, &memcg->stat[idx]);
                x = 0;
        }
        __this_cpu_write(memcg->stat_cpu->count[idx], x);
}
```

为了更方便的复现, 打算让每个dying cgroup 的每个per cpu counter
`memcg->stat_cpu->count[rss_index]` 都能接近一个
`MEMCG_CHANGE_BATCH * PAGE_SIZE`的负值.

那么就需要alloc page的时候,尽量在一个cpu上申请, free page的时候,要
在多个cpu上分别释放一部分. 程序如下:
```cpp
~/.../alloc_mem_cpu# cat main.c

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define CPU_NUM         64
//#define CPU_NUM               8

//#define DEBUG
#ifdef DEBUG
#define pr_deb(...)             printf( __VA_ARGS__)
#else
#define pr_deb(...)             {}
#endif

unsigned long page_size;

#define BATCH_PAGE_NUM  30
unsigned int alloc_cpu = 0;

char *g_buf;

int modify_exec_cpu(int target_cpu)
{
        cpu_set_t cpu_set;
        int new_cpu;

        CPU_ZERO(&cpu_set);
        CPU_SET(target_cpu, &cpu_set);

        pid_t pid = getpid();

        if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set) != 0) {
                perror("sched_setaffinity");
                return -1;
        }

        return 0;
}

int malloc_g_buf(size_t size)
{
        void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (!addr) {
                perror("mmap  error\n");
                return -1;
        }
        g_buf = addr;
        alloc_cpu = sched_getcpu();
        pr_deb("alloc cpu (%d) g_buf(%lx) g_buf_end(%lx)\n", alloc_cpu, (unsigned long)g_buf,
                        (unsigned long)(g_buf + size));
        return 0;
}
void get_page_size()
{
        page_size = getpagesize();
}
void free_mem(char *addr, size_t size)
{
        int ret = madvise(addr, size, MADV_DONTNEED);
        if (ret < 0) {
                perror("madvise error");
        }
        return;
}
void let_other_cpu_free()
{
        int i;
        char *cur_ptr = g_buf;
        size_t free_size = BATCH_PAGE_NUM * page_size;

        for (i = 0; i < CPU_NUM; i++) {
                modify_exec_cpu(i);
                pr_deb("free cpu (%d) free_addr(%lx)\n", i, (unsigned long)cur_ptr);
                free_mem(cur_ptr, free_size);
                cur_ptr += free_size;
        }
        pr_deb("free end ptr(%lx)\n", (unsigned long)cur_ptr);
}
void access_page(size_t alloc_size)
{
        char *end_ptr = g_buf + alloc_size;
        char *cur_ptr = g_buf;
        while (cur_ptr < end_ptr) {
                *cur_ptr = ' ';
                cur_ptr += page_size;
        }
}
int main()
{
        size_t alloc_size;
        int ret;

        get_page_size();
        alloc_size = CPU_NUM * BATCH_PAGE_NUM * page_size;

        pr_deb("cpu num(%d) alloc_size(%d) per_cpu_size(%d) page_size(%d)\n",
                        CPU_NUM, alloc_size, BATCH_PAGE_NUM * page_size, page_size);

        modify_exec_cpu(90);
        ret = malloc_g_buf(alloc_size);

        if (ret < 0) {
                printf("alloc g buf error\n");
                goto err;
        }

        access_page(alloc_size);
        let_other_cpu_free();

        return 0;
err:
        return -1;
}
```

该程序让 申请内存(mmap, access) 在cpu 90 上做, 申请大小为:
```
CPU_NUM(64) * BATCH_PAGE_NUM(30) * page_size
```

而释放内存(madvise - MADV_DONTNEED) 在 cpu `0-63`上做, 每个cpu
释放
```
BATCH_PAGE_NUM(30) * page_size
```

通过这样的方法, 很容易达到内存上限, 我们在外围用脚本包装下:

* memcg2.sh
```sh
#!/bin/bash
. ./common.sh
set -e
if [ $# -lt 2 ];then
        #echo "Usage: ./$0 start_idx count"
        #exit
        start=0
        count=1000
else
        start=$1
        count=$2
fi

let count=start+count

for((i=$start;i<$count;i++))
do
        cgcreate -g memory:${CGROUP_DIR}/test-$i
        mkdir -p ${FILEDIR}/$start/

        cgexec -g memory:${CGROUP_DIR}/test-$i cp ${WORKDIR}/file4k ${FILEDIR}/$start/$i.file

        cgexec -g memory:${CGROUP_DIR}/test-$i cat ${FILEDIR}/$start/$i.file > /dev/null

        cgexec -g memory:${CGROUP_DIR}/test-$i ${WORKDIR}/main
        cgdelete -g memory:${CGROUP_DIR}/test-$i
done
```
执行该脚本, 我们看下 `root memcg`的`usage_in_bytes`变化

```
# A=$(cat /sys/fs/cgroup/memory/memory.usage_in_bytes); echo $((A/1024/1024/1024))
301

# time sh memcg2.sh

real    0m15.098s
user    0m6.307s
sys     0m8.866s
# A=$(cat /sys/fs/cgroup/memory/memory.usage_in_bytes); echo $((A/1024/1024/1024))
416
```

可以发现15s的时间, 大概可以产生100G, 这个还只是单线程,并且只在64个cpu上残留. 
总之用该程序,可以方便我们做复现测试
