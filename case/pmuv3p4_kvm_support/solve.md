## 规避方法

1. 编写脚本`disable_pmu.stp`

   ```
   probe process("/usr/libexec/qemu-kvm").statement("kvm_arm_set_cpu_features_from_host@/usr/src/debug/qemu-4.1.0.33-7b50881a.ky10.aarch64/target/arm/kvm.c:165")
   {
       printf("env->features: 0x%x\n", $cpu->env->features)
       $cpu->env->features = $cpu->env->features & (~(1 << 38));
   
       printf("env->features after: 0x%x\n", $cpu->env->features)
   }
   probe process("/usr/bin/qemu-kvm").statement("kvm_arm_set_cpu_features_from_host@/usr/src/debug/qemu-4.1.0.33-7b50881a.ky10.aarch64/target/arm/kvm.c:165")
   {
       printf("env->features: 0x%x\n", $cpu->env->features)
       $cpu->env->features = $cpu->env->features & (~(1 << 38));
   
       printf("env->features after: 0x%x\n", $cpu->env->features)
   }
   ```

2. 在环境中安装systemtap
   ```
   yum install systemtap -y
   ```
3. 执行该脚本(后台执行):
   ```
   nohup stap -v disable_pmu.stp -g > stap.log 2>&1 &
   ```

4. 等待stap.log中打印:
   ```
   Pass 5: starting run.
   ```
   再执行第五个步骤。


5. 启动虚拟机, `stap.log`会打印:
   ```
   env->features: 0x40c0300400
   env->features after: 0xc0300400
   ```

## 验证方法

启动虚拟机后，观察虚拟机console.log, 如果没有panic打印，表示生效

## 回滚方法

杀死`stap`进程即可(或者重启物理机)
```
kill `pidof stap`
```

## 注意事项
该脚本会在重启后失效，如果物理机重启，但是有需要该脚本所提供的功能，需要重复
`规避方法`中的步骤
