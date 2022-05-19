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

# timeoutlist
```
crash> p ((struct request *)0xffff882e75f4c600)->timeout_list
$3 = {
  next = 0xffff882e75f4c750,
  prev = 0xffff882e75f4c750
}
crash> p &((struct request *)0xffff882e75f4c600)->timeout_list)
$4 = (struct list_head *) 0xffff882e75f4c750
```

# re queue 
```
struct request 0xffff885f1930a340
```

