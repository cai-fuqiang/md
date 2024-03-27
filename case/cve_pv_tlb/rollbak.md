# 回退方法:
1. yum install kernel-headers-4.18.0-147.5.1.es8_24
2. 执行
```
ls /boot/loader/entries/ |grep 4.18.0-147
```
查看两个版本kernel 前缀是否一样, 如果一样, 
只需要将 `/boot//efi/EFI/escore/grubenv` 中 `es8_25` 修改为 `es8_24`

e.g.
```
[root@node-25 147_25_all]# ls /boot/loader/entries/ |grep 4.18.0-147
-rw-r--r-- 1 root 372 Mar 20 21:35 7e44c499bed946339f5cf1e5aac6c75a-4.18.0-147.5.1.es8_24.x86_64.conf
-rw-r--r-- 1 root 372 Mar 22 13:54 7e44c499bed946339f5cf1e5aac6c75a-4.18.0-147.5.1.es8_25.x86_64.conf
                                ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ (一样)
[root@node-25 147_25_all]# cat /boot/efi/EFI/escore/grubenv
# GRUB Environment Block
saved_entry=7e44c499bed946339f5cf1e5aac6c75a-4.18.0-147.5.1.es8_25.x86_64
                                                            ^^^^^^
                                                            (修改为 es8_24)
```

如果不一样, 则需要修改:
```
[root@node-25 147_25_all]# cat /boot/efi/EFI/escore/grubenv
# GRUB Environment Block
saved_entry=7e44c499bed946339f5cf1e5aac6c75a-4.18.0-147.5.1.es8_25.x86_64
            ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
```
> NOTE
>
> 这里需要注意 grubenv 文件中的 `save_entry` 属性 和 /boot/load/entries 中的文件名不同的是:
> 其没有`.conf`后缀.
