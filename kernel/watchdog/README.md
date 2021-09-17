# 现象
在每次重启系统后，查看IPMI WEB SEL(system events log),都会出现
下面的告警打印:
* 华为IPMI界面打印<br/>
![ipmi_web告警打印.img](pic/ipmi_web告警打印.img.png)
* DELL IPMI 界面打印<br/>
![dell_ipmi_web告警打印](pic/dell_ipmi_web告警打印.png)

# 初步调查(询问厂商售后)
询问DELL和huawei售后，在查询到BIOS中ipmi watchdog已经被disable,
给出的一致性的结论是: 该日志是由上层软件产生的，并非有ipmi watchdog
触发生成．所以当时得出的假设是，是不是软件可以通过ipmi cmd，去使
BMC去记录日志．而并未是IPMI WATCHDOG超时触发记录．

但是经过后面调研，发现并非是这样．下面主要说下调查过程．

# 进一步调查
## 初步测试
一开始的测试是使用排除法，看看将ipmi相关驱动删除后，该日志还是
否会被记录，经过测试发现，ipmi_watchdog模块被移除后，日志就无法
记录，可以暂时得出的结论是，ipmi_watchdog会触发日志的记录．

## 初步查看代码
1. 查看ipmi_watchdog模块有没有直接记录日志的命令
经过查看代码并没有发现相关命令，只有ipmi watchdog cmd相关命令，
其中包括SET, GET和RESET, 下面会详细介绍．

因为该日志记录发生在系统初始化期间，所以查看ipmi_watchdog初始化
期间，有没有对ipmi_watchdog进行set操作

2. 查看ipmi_watchdog模块初始化操作
在ipmi_watchdog初始化期间，确实有对ipmi_watchdog进行set操作，操作
如下:
```
                ipmi_set_timeout
[   23.424482]  ipmi_new_smi+0x28/0x3c [ipmi_watchdog]
[   23.469231]  ipmi_register_smi+0x400/0x558 [ipmi_msghandler]
[   23.469245]  try_smi_init+0x500/0x7d0 [ipmi_si]
[   23.489798]  init_ipmi_si+0x98/0x17c [ipmi_si]
[   23.489800]  do_one_initcall+0x54/0x1d8
[   23.489804]  do_init_module+0x60/0x1f0
[   23.489805]  load_module+0x139c/0x16b8
[   23.489807]  __se_sys_init_module+0x168/0x280
[   23.489808]  __arm64_sys_init_module+0x24/0x30
```
如上面的堆栈所示，上面的堆栈表示的流程为：
系统初始化System Management Interface,　可以理解为BMC的消
息通道，初始化末尾，需要调用`ipmi_register_smi`对SMI进行注册，
注册后，需要去调用下每个watcher，让每个watcher进行简单的初
始化并进行功能验证，`ipmi_register_smi`代码如下:

```C/C++
int ipmi_register_smi(const struct ipmi_smi_handlers *handlers,
    void                     *send_info,
    struct device            *si_dev,
    unsigned char            slave_addr)
{
    ....
    /* After this point the interface is legal to use. */
    call_smi_watchers(i, intf->si_dev);
    ...
}
```
从注释中也可以看出, `call_smi_watchers`的作用之一，是为了
验证该wather是否正确使用该SMI．

查看`ipmi_new_smi`相关代码

```C/C++
static void ipmi_new_smi(int if_num, struct device *device)
{
   ipmi_register_watchdog(if_num);
}

static void ipmi_register_watchdog(int ipmi_intf)
{
    ...
     out:
   if ((start_now) && (rv == 0)) {
       /* Run from startup, so start the timer now. */
       start_now = 0; /* Disable this function after first startup. */
       ipmi_watchdog_state = action_val;
       ipmi_set_timeout(IPMI_SET_TIMEOUT_FORCE_HB);
       pr_info("Starting now!\n");
   } else {
       /* Stop the timer now. */
       ipmi_watchdog_state = WDOG_TIMEOUT_NONE;
       ipmi_set_timeout(IPMI_SET_TIMEOUT_NO_HB);
   }
}
```
在`ipmi_register_watchdog`函数末尾，会有两个分支，
分别进行了`ipmi_set_watchdog`的操作

首先查看第一个分支:
第一个分支表示，当`start_now`使能是，则使能watchdog.
第二个分支则表示，关闭watchdog
而start_now参数定义如下:
```C/C++
module_param(start_now, int, 0444);
MODULE_PARM_DESC(start_now, "Set to 1 to start the watchdog as"
        "soon as the driver is loaded.");
```
根据注释可以看出，该参数如果为1，则表示在该module load时，将
watchdog使能.

经过调试发现kernel走了下面的分支，也就是说，是需要关闭watchdog
这个那么关闭watchdog会触发ipmi sel么，我们需要结合ipmi 手册去
分析下`ipmi_set_timeout`这个接口.
