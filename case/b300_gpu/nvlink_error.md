# B300  nvlink 透传后, 执行测试程序卡住


```
[   19.602972] hid: raw HID events driver (C) Jiri Kosina
[   20.062167] NVRM: Failed to read PCI extended capability header at offset 0x100 while searching for NVIDIA DVSEC3, rc 0xffffffea
[   20.074280] NVRM: Failed to read PCI extended capability header at offset 0x100 while searching for NVIDIA DVSEC3, rc 0xffffffea
[   20.086698] NVRM: Failed to read PCI extended capability header at offset 0x100 while searching for NVIDIA DVSEC3, rc 0xffffffea
[   20.111695] NVRM: Failed to read PCI extended capability header at offset 0x100 while searching for NVIDIA DVSEC3, rc 0xffffffea
[   20.210929] NVRM: GPU0 kbifCacheVFInfo_GB100: Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS
[   20.236566] ACPI Warning: \_SB.PCI0.S40._DSM: Argument #4 type mismatch - Found [Buffer], ACPI requires [Package] (20210730/nsarguments-61)
[   20.310569] NVRM: Failed to read PCI extended capability header at offset 0x100 while searching for NVIDIA DVSEC3, rc 0xffffffea
[   20.321347] NVRM: Failed to read PCI extended capability header at offset 0x100 while searching for NVIDIA DVSEC3, rc 0xffffffea
[   20.328569] NVRM: Failed to read PCI extended capability header at offset 0x100 while searching for NVIDIA DVSEC3, rc 0xffffffea
[   20.405946] NVRM: GPU1 kbifCacheVFInfo_GB100: Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS
[   20.418713] ACPI Warning: \_SB.PCI0.S48._DSM: Argument #4 type mismatch - Found [Buffer], ACPI requires [Package] (20210730/nsarguments-61)
[   20.492356] NVRM: Failed to read PCI extended capability header at offset 0x100 while searching for NVIDIA DVSEC3, rc 0xffffffea
[   20.507296] NVRM: GPU2 kbifCacheVFInfo_GB100: Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS
[   20.519764] ACPI Warning: \_SB.PCI0.S50._DSM: Argument #4 type mismatch - Found [Buffer], ACPI requires [Package] (20210730/nsarguments-61)
[   20.559030] NVRM: GPU3 kbifCacheVFInfo_GB100: Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS
[   20.570467] ACPI Warning: \_SB.PCI0.S38._DSM: Argument #4 type mismatch - Found [Buffer], ACPI requires [Package] (20210730/nsarguments-61)
[   20.605002] NVRM: GPU4 kbifCacheVFInfo_GB100: Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS
[   20.618625] ACPI Warning: \_SB.PCI0.S70._DSM: Argument #4 type mismatch - Found [Buffer], ACPI requires [Package] (20210730/nsarguments-61)
[   20.683866] NVRM: GPU5 kbifCacheVFInfo_GB100: Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS
[   20.695727] ACPI Warning: \_SB.PCI0.S58._DSM: Argument #4 type mismatch - Found [Buffer], ACPI requires [Package] (20210730/nsarguments-61)
[   20.760966] NVRM: GPU6 kbifCacheVFInfo_GB100: Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS
[   20.773191] ACPI Warning: \_SB.PCI0.S60._DSM: Argument #4 type mismatch - Found [Buffer], ACPI requires [Package] (20210730/nsarguments-61)
[   20.861536] NVRM: GPU7 kbifCacheVFInfo_GB100: Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS
[   20.876974] ACPI Warning: \_SB.PCI0.S68._DSM: Argument #4 type mismatch - Found [Buffer], ACPI requires [Package] (20210730/nsarguments-61)
[   23.825804] NVRM: GPU0 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   23.831013] NVRM: GPU0 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   24.116575] NVRM: --- last print repeated 4 times ---
[   24.119714] NVRM: GPU3 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   24.124387] NVRM: GPU3 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   24.406130] NVRM: --- last print repeated 4 times ---
[   24.408866] NVRM: GPU5 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   24.413573] NVRM: GPU5 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   24.700426] NVRM: --- last print repeated 4 times ---
[   24.703510] NVRM: GPU1 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   24.708266] NVRM: GPU1 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   24.995632] NVRM: --- last print repeated 4 times ---
[   24.998515] NVRM: GPU2 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   25.003445] NVRM: GPU2 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   25.250158] NVRM: --- last print repeated 4 times ---
[   25.252449] NVRM: GPU2 nvAssertFailedNoLog: Assertion failed: GPPut < WATCHDOG_GPFIFO_ENTRIES @ kernel_rc_watchdog.c:1608
[   25.323449] NVRM: GPU6 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   25.328075] NVRM: GPU6 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   25.611329] NVRM: --- last print repeated 4 times ---
[   25.613995] NVRM: GPU7 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   25.618278] NVRM: GPU7 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   25.905419] NVRM: --- last print repeated 4 times ---
[   25.908797] NVRM: GPU4 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   25.914466] NVRM: GPU4 ceEncodeLceTypeMetadataForPcie: ceEncodeLceTypeMetadataForPcie unknown
[   26.149825] NVRM: loading NVIDIA UNIX Open Kernel Module for x86_64  615.71.09  Release Build  (dvs-builder@U22-I3-F05-2-4)  Fri Sep  4 22:33:33 UTC 2026
[   26.224961] nvidia-modeset: Loading NVIDIA UNIX Open Kernel Mode Setting Driver for x86_64  615.71.09  Release Build  (dvs-builder@U22-I3-F05-2-4)  Fri Sep  4 22:15:27 UTC 2026
```

## 执行下面测试程序
```
import ctypes
cuda = ctypes.CDLL("libcuda.so.1")
r = cuda.cuInit(0)
print("cuInit =", r)

n = ctypes.c_int()
if r == 0:
    r2 = cuda.cuDeviceGetCount(ctypes.byref(n))
    print("cuDeviceGetCount =", r2)
```
执行程序会卡住，进程处于 `R` 状态, 无法杀死
```
root@ceshi20261013:~# ps aux |grep cuda
root        5322 99.9  0.0 8661476 104684 pts/0  Rl+  10:09   5:50 python3 cuda.py
root        5508  0.0  0.0   6480  2444 pts/2    S+   10:15   0:00 grep --color=auto cuda
root@ceshi20261013:~# cat /proc/5322/stack 
root@ceshi20261013:~# kill -9 5322
root@ceshi20261013:~# ps aux |grep cuda
root        5322 99.9  0.0 8661476 104756 pts/0  R+   10:09   6:37 python3 cuda.py
root        5519  0.0  0.0   6480  2416 pts/2    S+   10:16   0:00 grep --color=auto cuda
```

再次执行`nvidia-smi`, `nvidia-smi` 会陷入`D`状态
```
root@ceshi20261013:~# ps aux |grep nvidia-smi
root        5526  2.6  0.0 211032 26564 pts/1    Dl+  10:16   0:00 nvidia-smi -q
root        5529  0.0  0.0   6612  2420 pts/2    S+   10:16   0:00 grep --color=auto nvidia-smi
root@ceshi20261013:~# cat /proc/5526/stack 
[<0>] uvm_va_space_destroy+0x408/0x630 [nvidia_uvm]
[<0>] uvm_release.constprop.0+0x7f/0x180 [nvidia_uvm]
[<0>] uvm_release_entry.part.0.isra.0+0x80/0xb0 [nvidia_uvm]
[<0>] uvm_release_entry+0x2a/0x30 [nvidia_uvm]
[<0>] __fput+0x9c/0x280
[<0>] ____fput+0xe/0x20
[<0>] task_work_run+0x6a/0xb0
[<0>] exit_to_user_mode_loop+0x157/0x160
[<0>] exit_to_user_mode_prepare+0xa0/0xb0
[<0>] syscall_exit_to_user_mode+0x27/0x50
[<0>] do_syscall_64+0x63/0xb0
[<0>] entry_SYSCALL_64_after_hwframe+0x6c/0xd6

```

# 目前已做的测试

* 增加kernel cmdline `nokaslr pci=realloc`
* 增加下面内核参数
  ```
  root@ceshi20261013:~# cat /etc/modprobe.d/nvidia-gsp.conf 
  options nvidia NVreg_EnableGpuFirmware=0
  root@ceshi20261013:~# cat /etc/modprobe.d/nvidia-uvm.conf 
  options nvidia_uvm uvm_disable_hmm=1
  ```
* 修改xml，增加
  ```
    <qemu:commandline>
      <qemu:arg value='-fw_cfg'/>
      <qemu:arg value='opt/ovmf/X-PciMmio64Mb,string=262144'/>
    </qemu:commandline>
  ```

以上方法均不能让问题彻底解决，（有的时候会解决，但是不稳定，重启后，可能问题又出
现）
