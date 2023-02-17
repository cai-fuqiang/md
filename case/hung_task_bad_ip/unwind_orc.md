# ORC unwind
orc 和frame pointer 一样都用于堆栈回溯。我们先来看下frame pointer
的实现。

## frame pointer
我们编写一个小程序来看下:
```cpp
#include <stdio.h>
void func2()
{
        return;
}
void func1()
{
        func2();
        return;
}
int main()
{
        func1();
        return 0;
}
```
使用 -O0 编译选项编译，然后反编译看下:
```
0000000000001119 <func2>:
    1119:       55                      push   %rbp
    111a:       48 89 e5                mov    %rsp,%rbp
    111d:       90                      nop
    111e:       5d                      pop    %rbp
    111f:       c3                      ret

0000000000001120 <func1>:
    1120:       55                      push   %rbp
    1121:       48 89 e5                mov    %rsp,%rbp
    1124:       b8 00 00 00 00          mov    $0x0,%eax
    1129:       e8 eb ff ff ff          call   1119 <func2>
    112e:       90                      nop
    112f:       5d                      pop    %rbp
    1130:       c3                      ret

0000000000001131 <main>:
    1131:       55                      push   %rbp
    1132:       48 89 e5                mov    %rsp,%rbp
    1135:       b8 00 00 00 00          mov    $0x0,%eax
    113a:       e8 e1 ff ff ff          call   1120 <func1>
    113f:       b8 00 00 00 00          mov    $0x0,%eax
    1144:       5d                      pop    %rbp
    1145:       c3                      ret
```
可以看到，在函数开始都会去执行下面两条指令
```
push %rbp ===1
mov %rsp, %rbp ====2
```
堆栈回溯，主要的两个寄存器，sp, ip, 我们分别说下:
* 关于sp，这里push的 rbp, 就是上一级函数执行到push %rbp 之后的sp值, 
 这个比较好理解，可以看底下的图
* 关于ip，其实这个开头是固定的，也就是说，`mov %rsp, %rbp`这个操作一定发生在
`push %rbp`之后，不能有其他的堆栈操作。这样就固定了ip和sp之间的关系

> NOTE
>
> 这里举个例子:
>
> ```
> 0xffffffffb1c605f0 <__schedule>:        push   %rbp
> 0xffffffffb1c605f1 <__schedule+1>:      mov    %rsp,%rbp
> 0xffffffffb1c605f4 <__schedule+4>:      push   %r15
> 0xffffffffb1c605f6 <__schedule+6>:      push   %r14
> ```
> 可以看到, 在`__schedule`函数中，还需要保存其他的寄存器，例如
> r15, r14 。但是 mov 操作，还是紧跟着 push %rbp之后，这样固定下来
> 之后，就方便之后的堆栈回溯分析，因为固定了保存堆栈的位置，这个位置
> - 16 正好是ra 的位置（return address) 也就是调用者调用call指令
> 存放ip的位置

我们先来结合反汇编看下:
* func2: 假设执行到111a 偏移(func2+0x0)，这时执行完了`push %rbp`，此时sp指向的内存
 存放的是rbp, rbp中的值，是为执行到func1函数执行到1121的sp的值，NOTE 中也提到，
 这个位置是固定的，那这个位置 - 16, 正好可以获取到 ip 的位置。这样就可以找到func1调用
call指令的ip, sp
* func1: 通过func1 传过来的 rbp ip, 我们主要再利用rbp, 找到上一级的sp, ip。
 rbp指向的内存正好是上一级的sp, 而ip的位置和sp的位置
 又是固定的，所以就可以一直往上找。

但是这里有一个问题，第一级函数func2，我们必须知道存放rbp时的位置，而这时%rbp可能指向的不是存放上一级sp的位置
例如:
```
0xffffffffb1c605f0 <__schedule>:        push   %rbp      <===在这个时候需要堆栈回溯
0xffffffffb1c605f1 <__schedule+1>:      mov    %rsp,%rbp
```
这个时候很可能在堆栈回溯时，就把上一级函数越过了。

我们再来看个图, 更清晰些:

