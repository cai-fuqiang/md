## CPUID_VMWARE_FEATURES_LEAF 问题

在不同的机器上, 使用相同的model 和feature, 但是下面的cpuid可能不同
```
-40000010 00000000: 0027ac40 000f4240 00000000 00000000
+40000010 00000000: 0027ac3d 000f4240 00000000 00000000
```
该features为`CPUID_VMWARE_FEATURES_LEAF`

## vmware cpuid freq 代码走读

该cpuid在如下代码初始化:
```sh
kvm_arch_init_vcpu
...
## 如果用户指定了tsc freq, 则将tsc_khz 传递给kvm设置
|-> kvm_arch_set_tsc_khz()
    ## 说明用户没有配置
    |-> if !env->tsc_khz
        \-> return 0
    ## 向kvm获取当前机器的 tsc freq
    |-> cur_freq = kvm_check_extension(cs->kvm_state, KVM_CAP_GET_TSC_KHZ) ?
            kvm_vcpu_ioctl(cs, KVM_GET_TSC_KHZ) : -ENOTSUP;
    ## 查看kvm支不支持设置 tsc
    |-> if kvm_check_extension(cs->kvm_state, KVM_CAP_TSC_CONTROL)
        \-> set_ioctl = true
    ## 暂不关注
    |-> if cur_freq != -ENOTSUP && freq_within_bounds(cur_freq, env->tsc_khz
        \-> set_ioctl = true;
    ## 如果kvm 报告可以设置
    |-> r = set_ioctl ? kvm_vcpu_ioctl(cs, KVM_SET_TSC_KHZ, env->tsc_khz) :
         -ENOTUSP
    ## 如果设置失败
    |-> if (r < 0)
        ## 获取 kvm 报告的freq(这里为什么要在获取一次!!!)
        \-> cur_freq = kvm_check_extension(cs->kvm_state, KVM_CAP_GET_TSC_KHZ) ?
                   kvm_vcpu_ioctl(cs, KVM_GET_TSC_KHZ) :
                   -ENOTSUP;
        ## 如果 cur_freq < 0 或者说, 当前用户设置的tsc_khz和当前获取的不一致
        |-> if cur_freq <= 0 || cur_freq != env->tsc_khz
            |-> warn_report(...)
            ## 返回失败
            |-> return r
    |-> return 0
## 如果用户设置tsc失败了, 则返回false
|-> if r < 0:
    return r
## 如果用户没有指定tsc, 则从kvm中获取
|-> if (!env->tsc_khz):
    |-> r = kvm_check_extension(cs->kvm_state, KVM_CAP_GET_TSC_KHZ) ?
            kvm_vcpu_ioctl(cs, KVM_GET_TSC_KHZ) : -ENOTSUP
    |-> if r > 0
        ## 如果获取到了 从kvm 中获取到了tsc freq, 则赋值
        \-> env->tsc_khz = r

## bus frequency 为固定值
|-> env->apic_bus_freq = KVM_APIC_BUS_FREQUENCY;
# Guests depend on 0x40000000 to detect this feature, so only expose
# it if KVM exposes leaf 0x40000000. (Conflicts with Hyper-V)
#
# 这里有几个条件 
#   + cpu->vmware_cpuid_freq 需要用户配置(默认打开), 用于像vmware
#     一样给用户显示tsc_khz
#   + cpu->expose_kvm: 作用未知，默认打开
#   + kvm_base == KVM_CPUID_SIGNATURE: kvm param features, leaf header
#   + tsc_is_stable_and_known: tsc 是stable并且频率已知
|-> if cpu->vmware_cpuid_freq
        && cpu->expose_kvm
        && kvm_base == KVM_CPUID_SIGNATURE
        ## TSC clock must be stable and known for this feature.
        && tsc_is_stable_and_known(env)
           ## 用户指定了tsc, 或者
           ## 从系统中获取tsc，但是tsc的频率是稳定的.
           ## 该feature 通过 cpuid报告, 通过 qemu cpu prop invtsc=on 指定
           |-> return return (env->features[FEAT_8000_0007_EDX] & CPUID_APM_INVTSC)
                 || env->user_tsc_khz;
    \-> {
          c = &cpuid_data.entries[cpuid_i++];
          c->function = KVM_CPUID_SIGNATURE | 0x10;
          ## 设置eax 为 tsc_khz
          c->eax = env->tsc_khz;
          ## ebx 为 env->apic_bus_freq, 是个常量
          c->ebx = env->apic_bus_freq / 1000; /* Hz to KHz */
          c->ecx = c->edx = 0;

          c = cpuid_find_entry(&cpuid_data.cpuid, kvm_base, 0);
          ## 使能 cpuid[40000000].EAX[bit 4]
          c->eax = MAX(c->eax, KVM_CPUID_SIGNATURE | 0x10);
        }
```

上面可知，如果使用kvm, 则需要在额外配置`invtsc` feature即可使能该cpuid，我们来测
试下:
```
## 开启invtsc
                   ## cpuid[40000000].EAX[bit 4] = 1
                   ## 报告该feature enable
40000000 00000000: 40000010 4b4d564b 564b4d56 0000004d
...
4000000f 00000000: 00000000 00000000 00000000 00000000
### 有该leaf
40000010 00000000: 0027ac3d 000f4240 00000000 00000000
80000000 00000000: 80000008 756e6547 6c65746e 49656e69

## 关闭invtsc
                   ## cpuid[40000000].EAX[bit 4] = 0
                   ## 报告该feature disable
40000000 00000000: 40000001 4b4d564b 564b4d56 0000004d
40000001 00000000: 0100007b 00000000 00000000 00000000
### 没有该leaf
80000000 00000000: 80000008 756e6547 6c65746e 49656e69
```
可以发现，关闭invtsc后, 40000000 中不再报告`40000010 leaf`

> 相关patch见 [附录 - vmware-cpuid-freq patch](#vmware-cpuid-freq-patch)

接下来我们关注两个问题:
1. qemu 有哪些途径可以设置tsc
2. kvm vcpu ioctl 流程
   + KVM_GET_TSC_KHZ
   + KVM_SET_TSC_KHZ

## qemu set user tsc
1. `tsc-frequency` cpu prop
   ```
   x86_cpu_common_class_init
   |-> object_class_property_add(oc, "tsc-frequency", "int",
                          x86_cpuid_get_tsc_freq,
                          x86_cpuid_set_tsc_freq, NULL, NULL);
        |-> x86_cpuid_set_tsc_freq
            ## 设置 env.user_tsc_khz 和 env.tsc_khz
            |-> cpu->env.tsc_khz = cpu->env.user_tsc_khz = value / 1000;
   ```
2. live migration(直接流程不会设置)

   热迁移时候, 会迁移env.tsc_kzh
   ```cpp
   static const VMStateDescription vmstate_tsc_khz = {
       .name = "cpu/tsc_khz",
       .version_id = 1,
       .minimum_version_id = 1,
       .needed = tsc_khz_needed,
       .fields = (const VMStateField[]) {
           VMSTATE_INT64(env.tsc_khz, X86CPU),
           VMSTATE_END_OF_LIST()
       }
   };
   ```
   <!--
   下面路径load
   ```
   qmp_migrate_incoming
     qemu_start_incoming_migration
     ...
       qemu_loadvm_state_main
         qemu_loadvm_section_start_full
           vmstate_load
             vmstate_load_state
   ```
   -->
所以，频率是可以迁移到目的端的.但是迁移只会迁移`env.tsc_khz`，不会迁移
`env.user_tsc_khz`, 可以理解，因为`env.user_tsc_khz`本身就是qemu参数控制.

如果迁移之后，发现源端配置的`env.tsc_khz`和目的端`env.user_tsc_khz`
不同，则报错
```cpp
static int cpu_post_load(void *opaque, int version_id)
{
    ...
    if (env->tsc_khz && env->user_tsc_khz &&
        env->tsc_khz != env->user_tsc_khz) {
        error_report("Mismatch between user-specified TSC frequency and "
                     "migrated TSC frequency");
        return -EINVAL;
    }
    ...
}
```

但是这个流程不会涉及set tsc khz 到 kvm。而是在下面流程触发:
```
process_incoming_migration_co
  qemu_loadvm_state
    cpu_synchronize_all_post_init
      cpu_synchronize_post_init
        kvm_cpu_synchronize_post_init
          # run vcpu thread
          run_on_cpu(cpu, do_kvm_cpu_synchronize_post_init, RUN_ON_CPU_NULL);
            kvm_arch_put_registers(cpu, KVM_PUT_FULL_STATE);
              if (level == KVM_PUT_FULL_STATE) {
                 /* We don't check for kvm_arch_set_tsc_khz() errors here,
                  * because TSC frequency mismatch shouldn't abort migration,
                  * unless the user explicitly asked for a more strict TSC
                  * setting (e.g. using an explicit "tsc-freq" option).
                  */
                 ## 其不关心返回值，因为 tsc 设置不成功不会 abort migration，
                 ## 除非用户设置了 tsc-freq，但是这个也是在cpu_post_load 时检查
                 ## 的
                 kvm_arch_set_tsc_khz(cpu);
              }
```
这里会让所有cpu重新加载一次`tsc_khz`, 但是其不管成功还是失败
## kvm vcpu ioctl - TSC KHZ
### SET TSC KHZ
代码路径:
```sh
kvm_arch_vcpu_ioctl
|-> case KVM_SET_TSC_KHZ:
    \-> kvm_set_tsc_khz(vcpu, user_tsc_khz)
        ## 根据user_tsc_khz 设置 virtual_tsc_shift, virtual_tsc_mult
        |-> kvm_get_time_scale(user_tsc_khz * 1000LL, NSEC_PER_SEC,
               &vcpu->arch.virtual_tsc_shift,
               &vcpu->arch.virtual_tsc_mult);
        ## 设置 virtual_tsc_khz
        |-> vcpu->arch.virtual_tsc_khz = user_tsc_khz;
        ## 下面还有一些流程先不关注...
```
### GET TSC KHZ
```sh
kvm_arch_vcpu_ioctl
|-> case KVM_GET_TSC_KHZ:
    \-> r = vcpu->arch.virtual_tsc_khz;
```

可以看到直接从`arch.virtual_tsc_khz`获取, 该变量除了在
`kvm_arch_vcpu_ioctl(KVM_SET_TSC_KHZ)` 中赋值, 还可以在
`kvm_arch_vcpu_create`:
```
kvm_arch_vcpu_create:
|->kvm_set_tsc_khz(vcpu, vcpu->kvm->arch.default_tsc_khz);
```
而该函数
```sh
kvm_arch_init_vm
## 赋值default_tsc_khz 为当前系统的
|-> kvm->arch.default_tsc_khz = max_tsc_khz ? : 
                        tsc_khz
```
***

所以最终`CPUID[40000010].EAX` 最终报告的为系统的TSC.

## INVTSC  block migration
INVTSC 会block migrate, 迁移时会报错:
```
(qemu) migrate tcp:localhost:54555
Error: State blocked by non-migratable CPU device (invtsc flag)
```
具体代码, 
```cpp
int kvm_arch_init_vcpu(CPUState *cs)
{
    ...
    if (!env->user_tsc_khz) {
        if ((env->features[FEAT_8000_0007_EDX] & CPUID_APM_INVTSC) &&
            invtsc_mig_blocker == NULL) {
            error_setg(&invtsc_mig_blocker,
                       "State blocked by non-migratable CPU device"
                       " (invtsc flag)");
            r = migrate_add_blocker(&invtsc_mig_blocker, &local_err);
            if (r < 0) {
                error_report_err(local_err);
                return r;
            }
        }
    }
    ...
}
```
如果要使用`invtsc` feature 进行热迁移, 需要配置`tsc-frequency` prop.

为什么要这样做呢? 我们来看下引入的patch
```
commit 68bfd0ad4a1dcc4c328d5db85dc746b49c1ec07e
Author: Marcelo Tosatti <mtosatti@redhat.com>
Date:   Wed May 14 16:30:09 2014 -0300

    target-i386: block migration and savevm if invariant tsc is exposed

    Invariant TSC documentation mentions that "invariant TSC will run at a
    constant rate in all ACPI P-, C-. and T-states".

    This is not the case if migration to a host with different TSC frequency
    is allowed, or if savevm is performed. So block migration/savevm.
```
Patch的意思说, invariant TSC features 在sdm 中的说明是在很多电源状态下都不变。
但是热迁移允许迁移到tsc不同的目的端上(在tsc 设置失败的情况下，可能保留原有值). 
所以这里加了这个限制。

而初版patch 只要发现 虚拟机使能了`invtsc` 就会设置`blocking migrate`

```diff
 int kvm_arch_init_vcpu(CPUState *cs)
@@ -705,6 +709,17 @@ int kvm_arch_init_vcpu(CPUState *cs)
                                   !!(c->ecx & CPUID_EXT_SMX);
     }

+    c = cpuid_find_entry(&cpuid_data.cpuid, 0x80000007, 0);
+    if (c && (c->edx & 1<<8) && invtsc_mig_blocker == NULL) {
+        /* for migration */
+        error_setg(&invtsc_mig_blocker,
+                   "State blocked by non-migratable CPU device"
+                   " (invtsc flag)");
+        migrate_add_blocker(invtsc_mig_blocker);
+        /* for savevm */
+        vmstate_x86_cpu.unmigratable = 1;
+    }
```
后续再:
```
commit d99569d9d8562c480e0befab601756b0b7b5d0e0
Author: Eduardo Habkost <ehabkost@redhat.com>
Date:   Sun Jan 8 15:32:34 2017 -0200

    kvm: Allow invtsc migration if tsc-khz is set explicitly
```
放宽了条件，允许用户指定`tsc_freq` 情况下, 允许`invtsc` 虚拟机迁移:
```diff
+    if (!env->user_tsc_khz) {
+        if ((env->features[FEAT_8000_0007_EDX] & CPUID_APM_INVTSC) &&
+            invtsc_mig_blocker == NULL) {
+            /* for migration */
+            error_setg(&invtsc_mig_blocker,
+                       "State blocked by non-migratable CPU device"
+                       " (invtsc flag)");
+            migrate_add_blocker(invtsc_mig_blocker);
+            /* for savevm */
+            vmstate_x86_cpu.unmigratable = 1;
+        }
```
为什么呢? commit message 中有说明
```
We can safely allow a VM to be migrated with invtsc enabled if
tsc-khz is set explicitly, because:
  * QEMU already refuses to start if it can't set the TSC frequency
    to the configured value.
  * Management software is already required to keep device
    configuration (including CPU configuration) the same on
    migration source and destination.
```
因为如果用户指定了`invtsc`启动，并且设置了既定的`tsc freq`, 如果设置失败，
早在`kvm_arch_init_vcpu`时就失败了. 所以，只要虚拟机启动，一定会设置成功
`user_tsc_khz`

那怎么迁移呢:

**在目的端，使用`tsc-frequency`设置 tsc freq 为源端**

另外, `-cpu host` 会使能 `invtsc` 么

## cpu host invtsc

`-cpu host` model 有点特殊，其没有具体型号的model features, 其model type
是继承的`-cpu max`

具体代码

```cpp
static const TypeInfo host_cpu_type_info = {
    .name = X86_CPU_TYPE_NAME("host"),
    .parent = X86_CPU_TYPE_NAME("max"),
    .class_init = host_cpu_class_init,
};
```
其没有`instance_init`， 所以继承父类:
```sh
static const TypeInfo max_x86_cpu_type_info = {
    .name = X86_CPU_TYPE_NAME("max"),
    .parent = TYPE_X86_CPU,
    .instance_init = max_x86_cpu_initfn,
    .class_init = max_x86_cpu_class_init,
};
```
`instance_init` 时会 赋值`max_features`, 但是在这里不会赋值features,
因此此时还不知道 `migratable` 是true 还是false

```cpp
static void max_x86_cpu_initfn(Object *obj)
{
    X86CPU *cpu = X86_CPU(obj);

    /* We can't fill the features array here because we don't know yet if
     * "migratable" is true or false.
     */
    cpu->max_features = true;
    ...
}
```

什么时候赋值的 features呢?
```cpp
void x86_cpu_expand_features(X86CPU *cpu, Error **errp)
{
    ...
    if (cpu->max_features) {
        for (w = 0; w < FEATURE_WORDS; w++) {
            /* Override only features that weren't set explicitly
             * by the user.
             */
            env->features[w] |=
                x86_cpu_get_supported_feature_word(w, cpu->migratable) &
                ~env->user_features[w] &
                ~feature_word_info[w].no_autoenable_flags;
        }
    }
    ...
}
//如果指定了 migratable_only, 则会再次判断, 该features是不是 可以热迁移的features
uint64_t x86_cpu_get_supported_feature_word(FeatureWord w,
                                            bool migratable_only)
{
    if (kvm_enabled()) {
        switch (wi->type) {
        case CPUID_FEATURE_WORD:
            r = kvm_arch_get_supported_cpuid(kvm_state, wi->cpuid.eax,
                                                        wi->cpuid.ecx,
                                                        wi->cpuid.reg);
        ...
        if (migratable_only) {
        r &= x86_cpu_get_migratable_flags(w);
    }
    return r;
}

//x86_cpu_get_migratable_flags 代码不展开，我们直接看在哪里设置
FeatureWordInfo feature_word_info[FEATURE_WORDS] = {
    [FEAT_8000_0007_EDX] = {
        .type = CPUID_FEATURE_WORD,
        .feat_names = {
            ...
            "invtsc", NULL, NULL, NULL,
            ...
        },
        .cpuid = { .eax = 0x80000007, .reg = R_EDX, },
        .tcg_features = TCG_APM_FEATURES,
        .unmigratable_flags = CPUID_APM_INVTSC,
    },
}
```
可以发现 invtsc 是无法热迁移的feature.

而 用户可以指定`-cpu host,migratable=false`来标记该机器是不需要热迁移的.
默认是true
```cpp
static Property max_x86_cpu_properties[] = {
    DEFINE_PROP_BOOL("migratable", X86CPU, migratable, true),
    DEFINE_PROP_BOOL("host-cache-info", X86CPU, cache_info_passthrough, false),
    DEFINE_PROP_END_OF_LIST()
};
```


## 附录
### vmware-cpuid-freq patch
```sh
## add vmware-cpuid-freq
commit 9954a1582e18b03ddb66f6c892dccf2c3508f4b2
Author: Phil Dennis-Jordan <phil@philjordan.eu>
Date:   Fri Jan 20 15:11:34 2017 +0100

    x86-KVM: Supply TSC and APIC clock rates to guest like VMWare

## make default vmware-cpuid-freq prop to true
commit 0b564e6f53b42cf036d09dafa5037e6be1395bda
Author: Phil Dennis-Jordan <phil@philjordan.eu>
Date:   Fri Jan 20 15:11:36 2017 +0100

    pc: Enable vmware-cpuid-freq CPU option for 2.9+ machine types
```
