# kunpeng
```
[root@node-3 perf]# perf stat -e cycles -e instructions -e l1d_cache -e mem_access -e l1d_cache_refill -C 16
 Performance counter stats for 'CPU(s) 16':

    28,494,880,905      cycles
    47,933,630,473      instructions              #    1.68  insn per cycle
    10,874,345,112      l1d_cache
    10,874,739,690      mem_access
       461,757,378      l1d_cache_refill

      11.685643798 seconds time elapsed
```

# s5000
```
[root@node-1 perf]# perf stat -e cycles -e instructions -e l1d_cache -e mem_access -e l1d_cache_refill -C 16
 Performance counter stats for 'CPU(s) 16':

    30,121,953,144      cycles
    10,730,968,230      instructions              #    0.36  insn per cycle
     3,210,940,535      l1d_cache
     3,211,059,572      mem_access
       169,391,794      l1d_cache_refill

      14.353699342 seconds time elapsed
```
