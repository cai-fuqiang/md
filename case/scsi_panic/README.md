# 从bt -l -c 36来看

```
crash> struct request ffff882e75f4c600
  q = 0xffff885f1930a340,
  mq_ctx = 0x0,
  cmd_flags = 269418517,
  cmd_type = REQ_TYPE_FS,
  atomic_flags = 1,
  cpu = 11,
 
```

# 从bt -l -c 0来看

仍然是这个request 

```
struct request ffff882e75f4c600
```
