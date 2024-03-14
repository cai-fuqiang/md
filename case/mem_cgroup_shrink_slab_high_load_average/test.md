# 不加载任何模块
```
[root@node-2 wangfuqiang]# sh read.sh
LIMIT
85900328960
USAGE
63061295104

total_inactive_anon 50918850560
total_active_anon 231407616
total_inactive_file 9290055680
total_active_file 2593914880


sleep .... 10s
LIMIT
85900328960
USAGE
63003033600

total_inactive_anon 50897420288
total_active_anon 231866368
total_inactive_file 9246539776
total_active_file 2593718272
```

变化很小,(不到100M)

# online return

查看内存变化
```
[root@node-2 wangfuqiang]# sh read.sh
LIMIT
85900328960
USAGE
60627943424

total_inactive_anon 50769625088
total_active_anon 237305856
total_inactive_file 7310868480
total_active_file 2283798528


sleep .... 10s
LIMIT
85900328960
USAGE
57455673344

total_inactive_anon 50812616704
total_active_anon 237174784
total_inactive_file 4085317632
total_active_file 2284650496
```

可以发现变化很明显. 变化了2.95G左右.

# trylock return

查看内存变化
```
[root@node-2 wangfuqiang]# sh read.sh
LIMIT
85900328960
USAGE
57107939328

total_inactive_anon 50858950656
total_active_anon 236781568
total_inactive_file 3703898112
total_active_file 2284978176


sleep .... 10s
LIMIT
85900328960
USAGE
54725836800

total_inactive_anon 50862358528
total_active_anon 237961216
total_inactive_file 1302069248
total_active_file 2292187136
```

变化同样很明显.

# do_shrink_slab return
```
LIMIT
85900328960
USAGE
29053353984

total_inactive_anon 23811260416
total_active_anon 97320960
total_inactive_file 2902786048
total_active_file 2220097536


sleep .... 10s
LIMIT
85900328960
USAGE
29014228992
```

无明显变化

# idr find 
```
LIMIT
85900328960
USAGE
59063795712

total_inactive_anon 51278118912
total_active_anon 277151744
total_inactive_file 5858000896
total_active_file 1622605824


sleep .... 10s
LIMIT
85900328960
USAGE
57493225472

total_inactive_anon 51200000000
total_active_anon 280428544
total_inactive_file 4477550592
total_active_file 1509425152
```

变化很明显.
