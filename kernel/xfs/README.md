# xfs中Re-logging 介绍
XFS logging 结合了logical和physical logging。某些对象，例如inodes和dquots, 
被记录为logical的格式，这些记录的细节由in-core structures的改变，而不是
on-disk structure.某些对象，典型的为buffers 他们以physical change 记录。
这两者不同的原因是为了减少经常需要被logged log 空间的大小。inode的一些
部分比起其他来说更高频率的被log，并且inodes一般来说会修改的更频繁相对于
其他的对象（superblock buffer要例外），所以保持metadata logged low是首要的
重要性。

一个重要的担心点为XFS允许对于一个对象进行多个单独的modificaition在任何时间都要
被记录下来。这样需要记录日志对这个object记录一个新的change时避免去flush 任何的
对磁盘的change。XFS通过一个方法做到这样，被称为`re-logging`。 概念上来说，它十分
简单--他要求任何对这个object 的new change 需要带着一个new copy 对于所有已经存在
于将要被写入log的new transaction的所有的change. 

举例来说，如果我们有一个场景时change A -> F，并且object 在change到D的时候会写入
磁盘，我们讲看到日志中记录的一系列的transaction, 他们的contents和LSN 如下:

```
Transaction     Contents        LSN
A               A               X
B               A+B             X+n
C               A+B+C           X+n+m
D               A+B+C+D         X+n+m+o
<object written to disk>
E               E               Y (> X+n+m+o)
F               E+F             Y+p
```
换句话说，当该object每一次被relogged，这个新的transaction包含之前所有的最近held only 
in the log 的change 


