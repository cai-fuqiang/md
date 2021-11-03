# 查找/var/lib/node-exporter/下的相关文件
在node-8上查找相关文件
```
[root@node-8 ~]# find / -name smartmon.prom
/var/lib/kubelet/pods/ecca07ae-6072-43f0-9dbe-a29ae7877801/volumes/kubernetes.io~empty-dir/collector-data/smartmon.prom
```
发现其在一个pod中，其中有个目录是`collector-data`, 这个目录名称在`/`上是唯一的。

# 通过xfs_db工具查找其他信息
|目录名称|ino|
|----|----|
|/													|64				|
|var                                             	|12582977		|
|lib                                             	|4194382		|
|kubelet                                         	|1510180		|
|pods                                            	|5010277		|
|610c61be-02b0-4f56-b80a-06d7d094e902				|155189514		|
|volumes											|159383755		|
|kubernetes.io~empty-dir							|230686870		|
|collector-data										|234881166		|

但是这里的`collector-data(234881166)`目录实际上是`xfs_repair`/ `kernel`串口报错的inode，
该inode extents指向的内容被写入`0a 00 00 ...`， 内容读不到。
