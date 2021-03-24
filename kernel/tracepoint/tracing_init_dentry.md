# 作用
实际上该函数的作用就是创建顶层目录`tracing`, 并且定义好automount函数.

**NOTE**: automount实际上指的是，访问该路径是自动调用，产生挂载点

# 代码
```C/C++
struct dentry *tracing_init_dentry(void)
{
    struct trace_array *tr = &global_trace;

    /* The top level trace array uses  NULL as parent */
    if (tr->dir)
        return NULL;

    if (WARN_ON(!tracefs_initialized()) ||
        (IS_ENABLED(CONFIG_DEBUG_FS) &&
         WARN_ON(!debugfs_initialized())))
        return ERR_PTR(-ENODEV);

    /*
     * As there may still be users that expect the tracing
     * files to exist in debugfs/tracing, we must automount
     * the tracefs file system there, so older tools still
     * work with the newer kerenl.
     */
    tr->dir = debugfs_create_automount("tracing", NULL,
                       trace_automount, NULL);
    if (!tr->dir) {
        pr_warn_once("Could not create debugfs directory 'tracing'\n");
        return ERR_PTR(-ENOMEM);
    }

    return NULL;
}
```

实际上该函数除了一些错检查外，只是调用了`debugfs_create_automount`，
函数，我们来看下这个函数的实现

## debugfs_create_automount
```C/C++
struct dentry *debugfs_create_automount(const char *name,
                    struct dentry *parent,
                    debugfs_automount_t f,
                    void *data)
{
    struct dentry *dentry = start_creating(name, parent);
    struct inode *inode;

    if (IS_ERR(dentry))
        return NULL;

    inode = debugfs_get_inode(dentry->d_sb);
    if (unlikely(!inode))
        return failed_creating(dentry);

    make_empty_dir_inode(inode);
    inode->i_flags |= S_AUTOMOUNT;
    inode->i_private = data;
    dentry->d_fsdata = (void *)f;
    /* directory inodes start off with i_nlink == 2 (for "." entry) */
    inc_nlink(inode);
    d_instantiate(dentry, inode);                       //将inode连接到dentry
    inc_nlink(d_inode(dentry->d_parent));
    fsnotify_mkdir(d_inode(dentry->d_parent), dentry);
    return end_creating(dentry);
}
```

首先会调用`start_creating`

