## 我们来看和request相关的数据结构
```
struct tracepoint __tracepoint_##name;
static inline void trace_##name(proto) {}
static inline int                       \                            
register_trace_##name(void (*probe)(data_proto), void *data)    \    
{                               \                                    
    return tracepoint_probe_register(&__tracepoint_##name,  \        
                    (void *)probe, data);   \                        
}                               \                                    

```


