## Linux backup tool overflow
Linux中有很多开源的备份工具，其中较为知名的有基于`rsync`的`rsnapshot`, 被很多
个人电脑所使用。`rsnapshot` 虽然可以使用硬连接的方式，用于实现文件增量,减少备份
占用空间, 但是假如文件出现了增量改动, `rsnapshot`还是会将整个文件备份.

而 `rdiff-backup`在`rsnapshot` 上做了进一步优化，可以实现文件的增量备份。

`borg`不仅继承了`rdiff-backuup`的所有优点, 还增加了加密功能 。这比较适合企业用户
使用。 

> NOTE
>
> 参考<sup>1</sup> 

## borg overflow

BorgBackup（简称：Borg）是一款去重备份程序。它还可选地支持压缩和认证加密.

Borg 的主要目标是提供高效且安全的数据备份方式。其采用的数据去重技术使其适用于日
常备份，因为仅存储变更数据。而经过认证的加密技术则使其适用于备份到非完全可信的目
标位置。

## 简单测试

### 安装

从下面下载链接中安装x86<sup>4</sup>, 下载下来后是一个可执行程序:
```sh
[root@20250729-instance-wangfuqiang49u1 ~]# file borg-linux-glibc231-x86_64
borg-linux-glibc231-x86_64: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=979c889cc9ca56a33ed67aeb4d2d05cb8db2df1b, for GNU/Linux 2.6.32, stripped
```

将其copy到`/usr/bin/`下:
```sh
[root@20250729-instance-wangfuqiang49u1 ~]# cp borg-linux-glibc231-x86_64 /usr/bin/borg
```

参考[easy to use](https://borgbackup.readthedocs.io/en/stable/#easy-to-use) 章节简单测试下:

### test ref "easy to use"

1. 初始化一个新的备份存储库(-e 可以指定加密 key 类型)
   ```sh
   ~: borg init -e repokey /backup
   [root@20250729-instance-wangfuqiang49u1 ~]# borg init -e repokey /backup
   Enter new passphrase:
   Enter same passphrase again:
   Do you want your passphrase to be displayed for verification? [yN]: y
   Your passphrase (between double-quotes): "1234"
   Make sure the passphrase displayed above is exactly what you wanted.
   
   IMPORTANT: you will need both KEY AND PASSPHRASE to access this repo!
   
   Key storage location depends on the mode:
   - repokey modes: key is stored in the repository directory.
   - keyfile modes: key is stored in the home directory of this user.
   
   For any mode, you should:
   1. Export the borg key and store the result at a safe place:
      borg key export           REPOSITORY encrypted-key-backup
      borg key export --paper   REPOSITORY encrypted-key-backup.txt
      borg key export --qr-html REPOSITORY encrypted-key-backup.html
   2. Write down the borg key passphrase and store it at safe place.
   ```
2. 给一个目录进行备份:
   ```
   [root@20250729-instance-wangfuqiang49u1 datadir]# tree
   .
   ├── a.txt
   ├── b.txt
   └── c.txt
   [root@20250729-instance-wangfuqiang49u1 datadir]# cat c.txt
   aaa
   ```
   执行备份命令:
   ```sh
   [root@20250729-instance-wangfuqiang49u1 datadir]# borg create /backup::my_test01 ~/datadir
   Enter passphrase for key /backup:
   ```

   使用`-v` 参数可以详细打印
   ```sh
   [root@20250729-instance-wangfuqiang49u1 datadir]# borg create -v --stats /backup::my_test02 ~/datadir
   Enter passphrase for key /backup:
   Enter passphrase for key /backup:
   Creating archive at "/backup::my_test02"
   ------------------------------------------------------------------------------
   Repository: /backup
   Archive name: my_test02
   Archive fingerprint: b00a366d29ad84d2258a72b8ec5bc3412d889793ce06391a6c05f9d9a8741567
   Time (start): Thu, 2026-01-08 15:42:22
   Time (end):   Thu, 2026-01-08 15:42:22
   Duration: 0.00 seconds
   Number of files: 3
   Utilization of max. archive size: 0%
   ------------------------------------------------------------------------------
                          Original size      Compressed size    Deduplicated size
   This archive:                    4 B                 47 B                  0 B
   All archives:                    8 B                 94 B              1.39 kB

                          Unique chunks         Total chunks
   Chunk index:                       4                    6
   ------------------------------------------------------------------------------
   ```
3. 查看备份信息
   ```sh
   [root@20250729-instance-wangfuqiang49u1 datadir]# borg list /backup
   Enter passphrase for key /backup:
   my_test01                            Thu, 2026-01-08 15:40:45 [523901bbe1f70e5718f7ca874dfc8947d05b0e4b0db5fd0d2e51596ef21b0930]
   my_test02                            Thu, 2026-01-08 15:42:22 [b00a366d29ad84d2258a72b8ec5bc3412d889793ce06391a6c05f9d9a8741567]
   ```
4. 测试回滚功能
   + 查看之前的归档:
     ```sh
     [root@20250729-instance-wangfuqiang49u1 datadir]# borg extract --list /backup::my_test01
     Enter passphrase for key /backup:
     root/datadir
     root/datadir/a.txt
     root/datadir/b.txt
     root/datadir/c.txt
     ```
   + 修改文件内容
     ```sh
     [root@20250729-instance-wangfuqiang49u1 datadir]# cat c.txt
     aaa
     [root@20250729-instance-wangfuqiang49u1 datadir]# echo ccc >> c.txt
     [root@20250729-instance-wangfuqiang49u1 datadir]# cat c.txt
     aaa
     ccc
     ```
   + 恢复归档, 会将归档文件解压到当前目录
     ```sh
     [root@20250729-instance-wangfuqiang49u1 datadir]# borg extract  /backup::my_test01
     Enter passphrase for key /backup:
     [root@20250729-instance-wangfuqiang49u1 datadir]# tree root
     root
     └── datadir
         ├── a.txt
         ├── b.txt
         └── c.txt
     [root@20250729-instance-wangfuqiang49u1 datadir]# cat root/datadir/c.txt
     aaa
     ```

   > NOTE
   >
   > Currently, extract always writes into the current working directory (“.”), so
   > make sure you cd to the right place before calling borg extract.
   > 
   > When parent directories are not extracted (because of using file/directory
   > selection or any other reason), Borg cannot restore parent directories’
   > metadata, e.g., owner, group, permissions, etc.

   + 只能解压到当前目录，如果要覆盖原来目录，需要cd /
     ```sh
     [root@20250729-instance-wangfuqiang49u1 datadir]# cd /
     [root@20250729-instance-wangfuqiang49u1 /]# borg extract  /backup::my_test01
     Enter passphrase for key /backup:
     [root@20250729-instance-wangfuqiang49u1 /]# cat /root/datadir/c.txt
     aaa
     ```
## 参考链接
1. [Incremental Backup Solution (rsync vs rsnapshot vs rdiff-backup)](https://www.reddit.com/r/linuxquestions/comments/o5443j/incremental_backup_solution_rsync_vs_rsnapshot_vs/)
2. [Borg Documentation](https://borgbackup.readthedocs.io/en/stable/)
3. [Borg Documentation : installation](https://borgbackup.readthedocs.io/en/stable/installation.html)
4. [borgbackup release](https://github.com/borgbackup/borg/releases)
