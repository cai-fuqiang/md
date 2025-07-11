## csv3 远程证明测试
```
[root@localhost /]# ./get-attestation
get attestation report & save to ./report.cert
mmap 0x7f4557249000
data:
75736572206461746100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
mnonce:
84a8c0b2a3373fdb9c0c4d4e4aa4ccb2
hash:
74b3122cc117bf508e3553ebaecba6bcbc78d2b21826035d1b7775ab9065b070
data: user data
user_data_pa: 108d66000
hypercall fail: -5
get attestation report fail
get report fail
```

dmesg 中打印
```
[54330.571170] CSV: vm attestation ret 0xfffffffb, error 0x18
```

> NOTE
>
> 使用csv(`policy=0x1`)，以及csv2(`policy=0x5`) 并没有这个问题.

## 问题简单分析
查看相关代码
* guest hygon-devkit -- 发起者
  ```
  //目前采用vmmcall的方式和vmm通信
  //====file vmmcall_get_attestation.c ===
  main
  // 结构体 From 参考链接1 中的 "报告输出数据格式"
  // 大小为 (0x9d4 + 256 / 8) = 2548
  |=> len = sizeof(struct csv_attestation_report);
  |=> vmmcall_get_attestation_report()
      |=> get_attestation_report
          ## 申请一段内存空间作为 Attestation 指令的 PADDR输入
          ## ===(1)===
          |=> user_data = mmap(,PAGE_SIZE,...)
          ## user_data 赋值"user data"字符串
          |=> user_data.data = "user data"
          ## mnonce 128-bit 是随机数
          |=> user_data.mnonce = get_randome_bytes()
          ## 使用sm3 计算mnonce和data部分的hash
          |=> user_data.hash = sm3(user_data, sizeof(mnonce, data))
          ## 获取pa
          |=> user_data_pa = va_to_pa(user_data)

          ## 调用hypercall -- KVM_HC_VM_ATTESTATION trap to KVM
          |=> hypercall(KVM_HC_VM_ATTESTATION, user_data_pa, PAGE_SIZE);
      //验证
      |=> verify_session_mac
  ```
  1. 这个PADDR不再是一个IN param，而是IN/OUT param, IN的格式见<sup>1</sup>
  中的"报告输入格式", OUT见<sup>1</sup>报告输出格式

* KVM 流程
  ```
  kvm_emulate_hypercall
  |=> case KVM_HC_VM_ATTESTATION
      |=> csv_vm_attestation()
          ## 和sev ATTESTATION API 保持兼容 -- 见 SEV KM API spec中 ATTESTATION 
          ## "ATTESTATION Command Buffer" 部分
          ##
          ## csv_vm_mnonce 这里简单赋值: "VM_ATTESTATION"
          |=> memcpy(data->mnonce, csv_vm_mnonce, sizeof(csv_vm_mnonce));
          ## 通过gpa -> hpa, 这一过程省略
          |=> data->address = __sme_page_pa() + offset
          |=> data->len = len
          |=> ret = hygon_kvm_hooks.sev_issue_cmd(kvm, SEV_CMD_ATTESTATION_REPORT,
                          data, &error)
              ## init in  sev_install_hooks()
              |=> sev_issue_cmd()
                  |=> __sev_issue_cmd()
          |=> if ret:
              ## 我们在这里打印出错
              |=> pr_err("vm attestation ret %#x, error %#x\n", ret, error);
  ```
  再来展开 `sev_issue_cmd`:
  ```
  |=> sev_do_cmd
      |=> __sev_do_cmd_locked
          ## 这里没有什么问题，只是copy的 cmdbuffer，而不是guest memory,
          ## cmdbuffer  {mnonce; data; len}
          |=> memcpy(sev->cmd_buf, data, buf_len);
          |=> sev_wait_cmd_ioc(sev, &reg, psp_timeout);
              |=> if psp_ret
                  ## *psp_ret 这里被赋值0x18
                  |=> *psp_ret = FIELD_GET(PSP_CMDRESP_STS, reg);
              |=> if FIELD_GET(PSP_CMDRESP_STS, reg)
                  ## 0xfffffffb
                  |=> ret = EIO 
          |=> memcpy(data, sev->cmd_buf, buf_len);
  ```
将该函数中的dgb打开，再次测试:
(打印cmdbuffer 里面内容)
```
echo 'file drivers/crypto/ccp/sev-dev.c +p' > /sys/kernel/debug/dynamic_debug/control
echo 'file drivers/crypto/ccp/sev-dev.c +p' > /sys/kernel/debug/dynamic_debug/control
```
dmesg 新增:
```
[57216.840550] ccp 0000:07:00.4: sev command id 0x36 buffer 0x0000001104ce9000 timeout 100s
[57216.840566] (in):  00000000: 0001 0000 0000 0000 4000 93f5 00b0 0000
[57216.840574] (in):  00000010: 4d56 415f 5454 5345 4154 4954 4e4f 0000
[57216.840578] (in):  00000020: 1000 0000
[57216.841000] ccp 0000:07:00.4: sev command 0x36 failed (0x00000018)
[57216.841010] (out): 00000000: 0001 0000 0000 0000 4000 93f5 00b0 0000
[57216.841015] (out): 00000010: 4d56 415f 5454 5345 4154 4954 4e4f 0000
[57216.841020] (out): 00000020: 1000 0000
[57216.841030] CSV: vm attestation ret 0xfffffffb, error 0x18
```

## 错误打印
在SEV KM API spec中, `4.5 Status Codes` 中列举了所有的STATUS CODE, 其中,
```
SECURE_DATA_INVALID 

0x0018

The part-specific SEV data failed integrity checks.
```
表示SEV data integrity check 失败

> NOTE
>
> 由于csv 和 sev 的 ATTESTATION 实现不同，sev ks api spec中 ATTESTATION 章节中的
> STATUS CODE 无参考意义

简单思考下，这里提到了 integrity error。在输入部分中，对user_data 做了sm3()算法
的hash, 是不是 csv fw 对这部分的验证出现了问题?

<font color=red size=5><strong>但是为什么csv2测试则没有问题</strong></font>

## 附1: 环境参数
```
## hardware
CPU: HYGON 4G

## software: 均来自openeuler-24.03-sp1
KERNEL: 6.6.0-72.0.0.76.oe2403sp1.x86_64
QEMU:   qemu-8.2.0-27.oe2403sp1.x86_64
EDK2:   edk2-ovmf-202308-17.oe2403sp1.noarch
```

## 附2: CSV2测试打印
工具打印:
```
[root@localhost /]# ./get-attestation
get attestation report & save to ./report.cert
mmap 0x7f9e2432d000
data:
75736572206461746100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
mnonce:
baa34d749cc198e17783136bd3a471f5
hash:
c36b802a2a6a7d28de42a93145ed8ee6d3daacd59108abc8bdb9aca312af2ca4
data: user data
user_data_pa: 11ca31000
attestation report MAC verify success
check PEK cert and ChipId successfully
done
```

dmesg:
```
[57872.515317] ccp 0000:07:00.4: sev command id 0x36 buffer 0x0000001104ce9000 timeout 100s
[57872.515327] (in):  00000000: 0001 0000 0000 0000 7000 3cf7 0014 0000
[57872.515332] (in):  00000010: 4d56 415f 5454 5345 4154 4954 4e4f 0000
[57872.515335] (in):  00000020: 1000 0000
[57872.519554] (out): 00000000: 0001 0000 0000 0000 7000 3cf7 0014 0000
[57872.519563] (out): 00000010: 4d56 415f 5454 5345 4154 4954 4e4f 0000
[57872.519566] (out): 00000020: 1000 0000
```

## 附3: 其他测试
我们在csv2中，强行把`sm3()`生成的hash改一部分试试，看看能复现同样的错误;

发现dmesg 中并未打印:
```
[58572.578496] kvm: enter  KVM_HC_VM_ATTESTATION
[58572.644301] ccp 0000:07:00.4: sev command id 0x36 buffer 0x0000001104ce9000 timeout 100s
[58572.644312] (in):  00000000: 0001 0000 0000 0000 1000 610b 00b1 0000
[58572.644317] (in):  00000010: 4d56 415f 5454 5345 4154 4954 4e4f 0000
[58572.644320] (in):  00000020: 1000 0000
[58572.648594] (out): 00000000: 0001 0000 0000 0000 1000 610b 00b1 0000
[58572.648603] (out): 00000010: 4d56 415f 5454 5345 4154 4954 4e4f 0000
[58572.648607] (out): 00000020: 1000 0000
[58572.648638] mnonce hex: 00000000: 56 4d 5f 41 54 54 45 53 54 41 54 49 4f 4e 00     VM_ATTESTATION.
```
但是`get-attestation`工具的check的部分出现了错误:
```
get attestation report & save to ./report.cert
mmap 0x7f2e61c2f000
data:
75736572206461746100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
mnonce:
01020329105db55459a5e99ac87716fc
hash:
857a50c696af5099621f5bb26de4b983cb0854a09611bd385d46ed1e82dcde91
data: user data
user_data_pa: 1126b1000
attestation report MAC verify failed
PEK cert and ChipId have been tampered with
get report fail
```
## 参考链接
1. [2-CSV远程认证技术介绍](https://openanolis.cn/sig/Hygon-Arch/doc/1256840217191415834)
