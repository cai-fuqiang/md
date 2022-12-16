# clock source
## 用户态操作
可以通过参看
```
/sys/devices/system/clocksource/clocksource0/available_clocksource
```
可获得的clocksource,通过
```
/sys/devices/system/clocksource/clocksource0/current_clocksources
```
查看当前的clocksource

> NOTE
>
> 当前在物理机中 ，查看上面两个文件输出如下:<br/>
> available_clocksource :`tsc hpet acpi_pm`<br/>
> current_clocksources  : `tsc`
>
> 而在虚拟机中 :<br/>
> available_clocksource: `kvm-clock tsc hpet acpi_pm` <br/>
> current_clocksources  : `kvm-clock`
>
> tsc: Time Stamp Counter
> hpet: High Precision Event Timer
> acpi_pm : ACPI Power Management Timer


# PS
## 参考代码
linux code tag v6.0-rc1

## 参考文档
[Linux内核深入理解定时器和时间管理（6）：x86_64 相关的时钟源（kvm-clock，tsc，acpi_pm，hpet)](https://blog.csdn.net/Rong_Toa/article/details/115350561)<br/>
[ 时间子系统 二、计时原理－timekeeper与clocksource](https://rootw.github.io/2018/01/%E8%AE%A1%E6%97%B6/)
https://0xax.gitbooks.io/linux-insides/content/Timers/linux-timers-2.html
