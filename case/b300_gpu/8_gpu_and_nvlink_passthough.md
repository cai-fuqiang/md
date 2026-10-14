# 软件包安装
## doca

```
wget https://www.mellanox.com/downloads/DOCA/DOCA_v3.5.0/host/doca-host_3.5.0-082000-26.07-ubuntu2204_amd64.deb
sudo dpkg -i doca-host_3.5.0-082000-26.07-ubuntu2204_amd64.deb
sudo apt-get update
sudo apt-get -y install doca-all
```

> 参考链接 1

## gpu driver 安装
```
wget https://developer.download.nvidia.com/compute/nvidia-driver/615.71.09/local_installers/nvidia-driver-local-repo-ubuntu2204-615.71.09_1.0-1_amd64.deb
sudo dpkg -i nvidia-driver-local-repo-ubuntu2204-615.71.09_1.0-1_amd64.deb
sudo cp /var/nvidia-driver-local-repo-ubuntu2204-615.71.09/nvidia-driver-*-keyring.gpg /usr/share/keyrings/
sudo apt-get update
```

> 参考链接 2

## cuda 安装

```
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-ubuntu2204.pin
sudo mv cuda-ubuntu2204.pin /etc/apt/preferences.d/cuda-repository-pin-600
wget https://developer.download.nvidia.com/compute/cuda/13.4.3/local_installers/cuda-repo-ubuntu2204-13-4-local_13.4.3-1_amd64.deb
sudo dpkg -i cuda-repo-ubuntu2204-13-4-local_13.4.3-1_amd64.deb
sudo cp /var/cuda-repo-ubuntu2204-13-4-local/cuda-*-keyring.gpg /usr/share/keyrings/
sudo apt-get update
sudo apt-get -y install cuda-toolkit-13-4
```

> 参考链接 3

## nvlsm

```sh
wget https://developer.download.nvidia.cn/compute/cuda/repos/ubuntu2204/x86_64/nvlsm_2025.12.211-1_amd64.deb
dpkg -i nvlsm_2025.12.211-1_amd64.deb
```

## nvidia-fabrcmanager

```sh
wget https://developer.download.nvidia.cn/compute/cuda/repos/ubuntu2204/x86_64/nvidia-fabricmanager_615.71.09-1ubuntu1_amd64.deb
wget https://developer.download.nvidia.cn/compute/cuda/repos/ubuntu2204/x86_64/nvidia-fabricmanager-dev_615.71.09-1ubuntu1_amd64.deb

dpkg -i nvidia-fabricmanager_*.deb
```

# 参考链接
1. [doca download](https://developer.nvidia.com/doca-downloads?deployment_platform=Host-Server&deployment_package=DOCA-Host&target_os=Linux&Architecture=x86_64&Profile=doca-all&Distribution=Ubuntu&version=22.04&installer_type=deb_local)
2. [gpu driver download](https://developer.nvidia.com/datacenter-driver-downloads?target_os=Linux&target_arch=x86_64&Distribution=Ubuntu&target_version=22.04&target_type=deb_local)
3. [cuda toolkit download](https://developer.nvidia.com/cuda-downloads?target_os=Linux&target_arch=x86_64&Distribution=Ubuntu&target_version=22.04&target_type=deb_local)
# 验证
1. `nvidia-smi nvlink -s` 命令
```
...
GPU 7: NVIDIA B300 SXM6 AC (UUID: GPU-5fcd1169-d47b-519f-1c5c-da71a893f9be)
	 Link 0: 53.125 GB/s
	 Link 1: 53.125 GB/s
	 Link 2: 53.125 GB/s
	 Link 3: 53.125 GB/s
	 Link 4: 53.125 GB/s
	 Link 5: 53.125 GB/s
	 Link 6: 53.125 GB/s
	 Link 7: 53.125 GB/s
	 Link 8: 53.125 GB/s
	 Link 9: 53.125 GB/s
	 Link 10: 53.125 GB/s
	 Link 11: 53.125 GB/s
	 Link 12: 53.125 GB/s
	 Link 13: 53.125 GB/s
	 Link 14: 53.125 GB/s
	 Link 15: 53.125 GB/s
	 Link 16: 53.125 GB/s
	 Link 17: 53.125 GB/s
```

> 8张GPU卡均如上打印
2. `nvlink-smi topo -m`

```
root@ceshi20261013:~# nvidia-smi topo -m
	GPU0	GPU1	GPU2	GPU3	GPU4	GPU5	GPU6	GPU7	CPU Affinity	NUMA Affinity	GPU NUMA ID
GPU0	 X 	NV18	NV18	NV18	NV18	NV18	NV18	NV18	0-251	0		N/A
GPU1	NV18	 X 	NV18	NV18	NV18	NV18	NV18	NV18	0-251	0		N/A
GPU2	NV18	NV18	 X 	NV18	NV18	NV18	NV18	NV18	0-251	0		N/A
GPU3	NV18	NV18	NV18	 X 	NV18	NV18	NV18	NV18	0-251	0		N/A
GPU4	NV18	NV18	NV18	NV18	 X 	NV18	NV18	NV18	0-251	0		N/A
GPU5	NV18	NV18	NV18	NV18	NV18	 X 	NV18	NV18	0-251	0		N/A
GPU6	NV18	NV18	NV18	NV18	NV18	NV18	 X 	NV18	0-251	0		N/A
GPU7	NV18	NV18	NV18	NV18	NV18	NV18	NV18	 X 	0-251	0		N/A
```
3. 查看 `nvidia-fabricmanager`服务状态
```
root@ceshi20261013:~# systemctl status nvidia-fabricmanager.service 
● nvidia-fabricmanager.service - NVIDIA fabric manager service
     Loaded: loaded (/lib/systemd/system/nvidia-fabricmanager.service; enabled; vendor preset: enabled)
     Active: active (running) since Wed 2026-10-14 15:33:11 CST; 1h 54min ago
```
4. `nvidia-smi -q`打印
```
    Fabric
        State                                          : Completed
        Status                                         : Success
```
5. 执行下面程序
```
import ctypes, sys, os
def log(msg):
    print(msg, flush=True)

log("PID: %d" % os.getpid())
log("加载 libcuda.so.1 ...")
cuda = ctypes.CDLL("libcuda.so.1")
log("已加载")

log("调用 cuInit ...")
r = cuda.cuInit(0)
log("cuInit = %s" % r)

if r == 0:
    n = ctypes.c_int()
    log("调用 cuDeviceGetCount ...")
    r2 = cuda.cuDeviceGetCount(ctypes.byref(n))
    log("cuDeviceGetCount = %s, count = %d" % (r2, n.value))

log("脚本主体执行完毕，准备退出")
```

正常打印, 不卡：
```
root@ceshi20261013:~# time python3 a.py
PID: 9158
加载 libcuda.so.1 ...
已加载
调用 cuInit ...
cuInit = 999
脚本主体执行完毕，准备退出

real	0m0.025s
user	0m0.019s
```
