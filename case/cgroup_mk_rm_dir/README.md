# cgroup删除目录抓取调试
启用调试:
```
cd /sys/kernel/debug/tracing/events/cgroup/cgroup_rmdir
echo 'stacktrace if (path ~ "*cinder-volume-copy*")' > trigger
echo 'path ~ "*cinder-volume-copy*"' > filter 
echo 1 >  enable
```

禁用调试:
```
echo 0 > enable
cd /sys/kernel/debug/tracing/events/cgroup/cgroup_rmdir
echo '!stacktrace if (path ~ "*cinder-volume-copy*")' > trigger    
echo 0 > filter    
# 如需清空 trace buffer
cd /sys/kernel/debug/tracing
echo '' > trace
```

复现后查看调试信息:
```
cd /sys/kernel/debug/tracing
cat trace
```
# cgroup 创建目录抓取调试
启用调试:
```
cd /sys/kernel/debug/tracing/events/cgroup/cgroup_mkdir
echo 'stacktrace if (path ~ "*cinder-volume-copy*")' > trigger
echo 'path ~ "*cinder-volume-copy*"' > filter 
echo 1 >  enable
```

禁用调试:
```
echo 0 > enable
cd /sys/kernel/debug/tracing/events/cgroup/cgroup_mkdir
echo '!stacktrace if (path ~ "*cinder-volume-copy*")' > trigger    
echo 0 > filter    
# 如需清空 trace buffer
cd /sys/kernel/debug/tracing
echo '' > trace
```

复现后查看调试信息:
```
cd /sys/kernel/debug/tracing
cat trace
```
