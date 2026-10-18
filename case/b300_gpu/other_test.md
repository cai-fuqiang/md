# 测试
## 测试单GPU, 无nvlink透传测试

同样报告下面的错误 -- `NVRM`, 但是执行测试程序正常运行

```
[   23.753543] nvidia: loading out-of-tree module taints kernel.
[   23.784860] nvidia: module verification failed: signature and/or required key missing - tainting kernel
[   23.832475] nvidia-nvlink: Nvlink Core is being initialized, major device number 234
[   27.836351] NVRM: Failed to read PCI extended capability header at offset 0x100 while searching for NVIDIA DVSEC3, rc 0xffffffea
[   27.908511] NVRM: GPU0 kbifCacheVFInfo_GB100: Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS
[   29.679618] NVRM: GPU0 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   29.683905] NVRM: GPU0 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   29.949072] NVRM: loading NVIDIA UNIX Open Kernel Module for x86_64  615.71.09  Release Build  (dvs-builder@U22-I3-F05-2-4)  Fri Sep  4 22:33:33 UTC 2026
```

程序执行:
```
root@ceshi20261013:~/B300# python3 a.py
PID: 4266
加载 libcuda.so.1 ...
已加载
调用 cuInit ...
cuInit = 0
调用 cuDeviceGetCount ...
cuDeviceGetCount = 0, count = 1
脚本主体执行完毕，准备退出
```

## 测试两个GPU, 无 nvlink 透传测试:

程序执行仍然成功.

## 测试8个GPU，无nvlink透传测试:

cuda程序卡住:
```
root@ceshi20261013:~/B300# python3 cuda.py



```

## 测试6个GPU，无nvlink透传测试
```
root@ceshi20261013:~/B300# python3 a.py
PID: 39231
加载 libcuda.so.1 ...
已加载
调用 cuInit ...
cuInit = 0
调用 cuDeviceGetCount ...
cuDeviceGetCount = 0, count = 6
脚本主体执行完毕，准备退出
root@ceshi20261013:~/B300# python3 cuda.py
cuInit = 0
cuDeviceGetCount = 0
```

## 测试7个 GPU, 无nvlink 透传测试卡住
```
root@ceshi20261013:~/B300# python3 a.py
PID: 4299
加载 libcuda.so.1 ...
已加载
调用 cuInit ...


python3 cuda.py

```

## 测试6个 GPU，无nvlink透传测试(6卡包含最后两个卡)

透传后，卡住.

## 透传下面两个卡，无NVLINK透传测试
透传后卡住，透传的两个卡为:
```
40:00.0
62:00.0
```
## 透传下面一个卡，无nvlink透传
```
40:00,0
```

程序正常执行:
```
root@ceshi20261013:~/B300# python cuda.py
('cuInit =', 0)
('cuDeviceGetCount =', 0)
```

## 透传下面一个卡, 无nvlink透传
```
62:00.0
```

执行测试程序卡住

## 透传除`62:00.0`卡之外的其他卡,无nvlink透传

执行程序不卡住

```
root@ceshi20261013:~/B300# python3 cuda.py

cuInit = 0
cuDeviceGetCount = 0
```

# TODO

在物理机安装驱动包，测试
