[root@node-1 13993]# cat maps
aaaae03c0000-aaaae17c0000 r-xp 00000000 fc:00 1612175158                 /usr/bin/ceph-osd
aaaae17d0000-aaaae1830000 r--p 01400000 fc:00 1612175158                 /usr/bin/ceph-osd
aaaae1830000-aaaae1840000 rw-p 01460000 fc:00 1612175158                 /usr/bin/ceph-osd
aaaae1840000-aaaae9a70000 rw-p 00000000 00:00 0
aaab1c090000-aaabf0220000 rw-p 00000000 00:00 0                          [heap]
ffff694d0000-ffff694e0000 ---p 00000000 00:00 0
ffff694e0000-ffff69ce0000 rw-p 00000000 00:00 0
ffff69ce0000-ffff69cf0000 ---p 00000000 00:00 0
ffff69cf0000-ffff6a4f0000 rw-p 00000000 00:00 0
ffff6a4f0000-ffff6a500000 ---p 00000000 00:00 0
ffff6a500000-ffff6ad00000 rw-p 00000000 00:00 0
ffff6ad00000-ffff6ad10000 ---p 00000000 00:00 0

[root@node-1 wangfuqiang]# stap -v brk.stap -g
pid (13993) brk(aaabf021c000)
