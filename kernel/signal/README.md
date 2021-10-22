# 简述
signal是linux进程间通信方式中唯一的一种异步通信方式，可以打乱进程的
代码执行顺序，当进程收到一个信号时，并且该信号是可捕捉又设置了相关
的信号处理函数, 进程会中断当前的代码执行，去执行信号处理函数。实际上，
这很像中断，而kernel在实现signal机制的时候，也是仿照的中断去设计的。
在介绍相关数据结构时, 会结合中断机制介绍他们的异同。

signal有一大部分的需求是给用户态使用，让其实现异步的通信。另外一部分
需求是让内核作为信号的发送者，去控制进程的行为，例如当进程发生段错误
时，kernel 会给当前进程发送一个SIGSEGV信号。那先来简单看下主要的几个
系统调用，了解下signal机制的需求。

# 系统调用简介
## kill - 信号发送
用户态程序可以调用kill系统效用来向其他进程发送信号。

```C/C++
SYSCALL_DEFINE2(kill, pid_t, pid, int, sig)
```
|参数类型|参数|解释|
|--|--|--|
|pid_t|pid|接收信号的进程|
|int|sig|发送的信号类型|

## signal - 设置信号处理函数
用于设置信号处理函数。
```
SYSCALL_DEFINE2(signal, int, sig, __sighandler_t, handler)
```
|参数类型|参数|解释|
|--|--|--|
|int|sig|设置的信号类型|
|__sighandler_t|handler|信号处理函数指针|
其中`__sighandler_t`该类型的声明为:

```C/C++
typedef void __signalfn_t(int);
typedef __signalfn_t __user *__sighandler_t;
```

## 
