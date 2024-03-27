# 分析 51 环境 node-19 03-15 20点左右message
```

[root@node-1 EAS-123302-boot-not-mount]# cat messages-20240317 |egrep "Found |Breaking" |grep systemd
Mar 15 20:21:05 node-19 systemd[1]: Found device /dev/mapper/os-root.
Mar 15 20:21:08 node-19 systemd[1]: local-fs.target: Found ordering cycle on boot.mount/start
Mar 15 20:21:08 node-19 systemd[1]: local-fs.target: Found dependency on local-fs-pre.target/start
Mar 15 20:21:08 node-19 systemd[1]: local-fs.target: Found dependency on multipathd-ensure.service/start
Mar 15 20:21:08 node-19 systemd[1]: local-fs.target: Found dependency on basic.target/start
Mar 15 20:21:08 node-19 systemd[1]: local-fs.target: Found dependency on sockets.target/start
Mar 15 20:21:08 node-19 systemd[1]: local-fs.target: Found dependency on dbus.socket/start
Mar 15 20:21:08 node-19 systemd[1]: local-fs.target: Found dependency on sysinit.target/start
Mar 15 20:21:09 node-19 systemd[1]: Found device /dev/docker/containerd_root.
```

可以发现有 `ordering cycle`, 我们列出来
```

+->local-fs.target-->boot.mount-->local-fs-pre.target-->multipathd-ensure.service-+
|                                                                                 |
+------------------sysinit.target<--dbus.socket<--sockets.target<--basic.target<--+
                                                            
```

# 查看代码

我们这里以命令
```
systemd-analyze verify $UNIT
```
为例, 代码路径为:
```
main  -- FILE src/analyze/analyze.c
  do_verify
    verify_units
      verify_unit
        manager_add_job
```

我们从 `manager_add_job`, 开始看.

##  manager_add_job
```cpp
int manager_add_job(
                Manager *m,
                JobType type,
                Unit *unit,
                JobMode mode,
                Set *affected_jobs,
                sd_bus_error *error,
                Job **ret) {

        Transaction *tr;
        int r;

        ...

        tr = transaction_new(mode == JOB_REPLACE_IRREVERSIBLY);
        ...

        r = transaction_add_job_and_dependencies(tr, type, unit, NULL, true, false,
                                                 IN_SET(mode, JOB_IGNORE_DEPENDENCIES, JOB_IGNORE_REQUIREMENTS),
                                                 mode == JOB_IGNORE_DEPENDENCIES, error);
        ...

        r = transaction_activate(tr, m, mode, affected_jobs, error);
        ...
}
```


该函数主要干三件事:
1. 新创建一个 transaction -- `transaction_new()`
2. 向该 transaction 中增加 该 job 和其 job dependency(递归, i.e., job, job->dep, job->dep->dep...)
3. activate 整个事务.

在看这些代码之前, 我们先来看些这些代码涉及的 基本的数据结构

## struct

### Transaction
```cpp
struct Transaction {
        /* Jobs to be added */
        Hashmap *jobs;      /* Unit object => Job object list 1:1 */
        Job *anchor_job;      /* the job the user asked for */
        bool irreversible;
}
```
* jobs: 存放本次事务要处理的jobs, `<key, value> --> <unit, obj>`
* anchor_job: 用户本来想要处理的job
* irreversible:

### Job
```cpp
struct Job {
        Manager *manager;
        Unit *unit;

        LIST_FIELDS(Job, transaction);

        ...

        LIST_HEAD(JobDependency, subject_list);
        LIST_HEAD(JobDependency, object_list);

        /* Used for graph algs as a "I have been here" marker */
        /* algs: algorithms ?? */
        Job* marker;
        unsigned generation;

        uint32_t id;

        JobType type;
        JobState state;
        ...
};
```

* transaction: 用于链接该translation中所有相同unit 的Job
* subject_list: 链接 JobDependency->subject
* object_list: 链接 JobDependency->object
* **marker**: 注释里的意思是,用于标记, "我之前看到过" , 和 generation 一起使用,
          代码后面分析
* **generation**:
* type: `enum JobType`列举些常见的
  ```
  JOB_START,
  JOB_STOP,
  JOB_RELOAD,
  JOB_RESTART
  ```
* state: `enum state`
  ```
  JOB_WAITING,
  JOB_RUNNING
  ```
### JobDependency
```cpp
struct JobDependency {
        /* Encodes that the 'subject' job needs the 'object' job in
         * some way. This structure is used only while building a transaction. */
        Job *subject;
        Job *object;

        LIST_FIELDS(JobDependency, subject);
        LIST_FIELDS(JobDependency, object);

        bool matters:1;
        bool conflicts:1;
};
```
* subject, object: subject need object
* list: 见Job::subject_list,  Job::object_list
* matters: 表示该subject对于object很重要, (Requries , not Wants)
* conflicts: 表示该subject对于object 来说冲突.

## function
### transaction_new


```cpp
Transaction *transaction_new(bool irreversible) {
        Transaction *tr;

        tr = new0(Transaction, 1);
        if (!tr)
                return NULL;

        tr->jobs = hashmap_new(NULL);
        if (!tr->jobs)
                return mfree(tr);

        tr->irreversible = irreversible;

        return tr;
}
```

该函数比较简单, 不过多解释

###  transaction_add_job_and_dependencies

该函数的作用是, 将一个unit 以及其依赖的unit都生成
obj, 然后添加到该transaction中.

```cpp
int transaction_add_job_and_dependencies(
                Transaction *tr,
                JobType type,
                Unit *unit,
                Job *by,
                bool matters,
                bool conflicts,
                bool ignore_requirements,
                bool ignore_order,
                sd_bus_error *e)
```
我们先看下参数:
* Unit: 本次要添加的unit
* by: 首先该函数功能实现是需要递归的, by依赖 unit, 也就是
      在add by 时, 发现unit是其依赖,也需要add obj
* matter: 表示, unit 对于 by 是重要的, 在 因为 ordering cycle 
          delete job时不delete.

从`manager_add_job()->transaction_add_job_and_dependencies()`路径中
```
unit=unit,
by=NULL,
matters=true,
conflicts=false
```
那么本次调用到该函数时,会走哪些流程呢?
```
transaction_add_job_and_dependencies()
{
   transaction_add_one_job()   //首先将该job加入到 tr中
   ...
   if (by) {
   } else {
     /* If the job has no parent job, it is the anchor job. */
     //该job没有parenet, 所以该进程就是 anchor job ( 相当于root)
     assert(!tr->anchor_job);
     tr->anchor_job = ret;
   }
   //下面的流程就是处理dependency
   if (is_new && !ignore_requirements && type != JOB_NOP) {

   }
}
```

# 

#  参考连接

https://access.redhat.com/solutions/3032831

https://unix.stackexchange.com/questions/193714/generic-methodology-to-debug-ordering-cycles-in-systemd/193854#193854
