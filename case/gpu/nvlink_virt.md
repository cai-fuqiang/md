## 需求概述
物理机有8个GPU, 目前需求是创建虚拟机规格可能是有2,4卡规格虚拟机，
每个虚拟机中的GPU支持通过NVLINK P2P, 需要对做nvlink share虚拟化
工作.

## 操作步骤
NVIDIA 官方提供了相关操作步骤<sup>1</sup>. 其主要的思路是, 在GPU虚拟机外
使用`nvidia-fabricmanager`接管`NVLINK`, 然后对NVLINK进行分区，将分区后某 **一个**
分区的GPU透传给 虚拟机。

其提供了两种方式:
* 使用虚拟机接管nvlink(将所有的nvlink透传到虚拟机中, 然后在虚拟机中启动`fabricmanager`)
* host 接管

由于在客户环境操作，我们采用第一种方式做测试

具体步骤如下
### host 操作

> [!NOTE]
> 
> 本文只提供目前计算节点还缺少的操作，像开启VMX，IOMMU等操作不包括在内

+ bridge 开启ACS [脚本](./enable_acs.sh)
+ 开启ATS
  + 执行下面命令查看 ats是否使能
    + `mst start`
    + `mlxconfig -d <mst device> query | grep -i ATS`
      
      输出为:`ATS_ENABLED False(0)`, 表示未开启ATS

  + 执行`mlxconfig -d /dev/mst/mt4129_pciconf0 set ATS_ENABLED=1` 将每一张IB卡都开启ATS
  + reboot

### service vm
#### 准备工作
+ 安装fmpm
  ```
  sudo apt update
  sudo apt install libjsoncpp-dev pkg-config
  sudo apt install nvidia-fabricmanager-dev-$(dpkg -l | grep '^ii' | grep
  nvidia-driver- | awk '{print $3}' | head -n 1 | awk -F"." '{print $1}')
  sudo git clone https://github.com/NVIDIA/Fabric-Manager-Client.git
  cd Fabric-Manager-Client
  sudo make
  sudo cp ./fmpm /usr/bin/
  sudo chmod +x /usr/bin/fmpm
  ```
+ 安装NVDIA driver, nvidia-fabricmanager, cuda(略)
* xml拼写, 将所有`nvswitch`透传到虚拟机中，由虚拟机接管, 例如:
  ```xml
    <hostdev mode='subsystem' type='pci' managed='yes'>
      <driver name='vfio'/>
      <source>
        <address domain='0x0000' bus='0x86' slot='0x00' function='0x0'/> <!--一个nvswitch BDF -->
      </source>
      <alias name='hostdev3'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x0a' function='0x0'/>
    </hostdev>
  ```
* 配置`service vm` `fabric manager`
  * 修改 `FABRIC_MODE`
    + `sed -i 's/FABRIC_MODE=./FABRIC_MODE=1/g' /usr/share/nvidia/nvswitch/fabricmanager.cfg`
  * restart Fabric manager service
    + `systemctl restart nvidia-fabricmanager.service`

* 启动虚拟机

#### 配置分区
执行下面命令可以查看当前环境支持的分区配置:
```
fmpm -l|grep "isActive\|numGpus\|partitionId\|physicalId" | tac
```

输出如下:

<details>
<summary>展开</summary>

```
         "partitionId" : 14
         "numGpus" : 1,
         "isActive" : 0,
               "physicalId" : 8,
         "partitionId" : 13
         "numGpus" : 1,
         "isActive" : 0,
               "physicalId" : 7,
         "partitionId" : 12
         "numGpus" : 1,
         "isActive" : 0,
               "physicalId" : 6,
         "partitionId" : 11
         "numGpus" : 1,
         "isActive" : 0,
               "physicalId" : 5,
         "partitionId" : 10
         "numGpus" : 1,
         "isActive" : 0,
               "physicalId" : 4,
         "partitionId" : 9
         "numGpus" : 1,
         "isActive" : 0,
               "physicalId" : 3,
         "partitionId" : 8
         "numGpus" : 1,
         "isActive" : 0,
               "physicalId" : 2,
         "partitionId" : 7
         "numGpus" : 1,
         "isActive" : 0,
               "physicalId" : 1,
         "partitionId" : 6
         "numGpus" : 2,
         "isActive" : 0,
               "physicalId" : 8,
               "physicalId" : 6,
         "partitionId" : 5
         "numGpus" : 2,
         "isActive" : 0,
               "physicalId" : 7,
               "physicalId" : 5,
         "partitionId" : 4
         "numGpus" : 2,
         "isActive" : 0,
               "physicalId" : 4,
               "physicalId" : 2,
         "partitionId" : 3
         "numGpus" : 2,
         "isActive" : 0,
               "physicalId" : 3,
               "physicalId" : 1,
         "partitionId" : 2
         "numGpus" : 4,
         "isActive" : 0,
               "physicalId" : 8,
               "physicalId" : 7,
               "physicalId" : 6,
               "physicalId" : 5,
         "partitionId" : 1
         "numGpus" : 4,
         "isActive" : 0,
               "physicalId" : 4,
               "physicalId" : 3,
               "physicalId" : 2,
               "physicalId" : 1,
         "partitionId" : 0
         "numGpus" : 8,
         "isActive" : 1,
               "physicalId" : 8,
               "physicalId" : 7,
               "physicalId" : 6,
               "physicalId" : 5,
               "physicalId" : 4,
               "physicalId" : 3,
               "physicalId" : 2,
               "physicalId" : 1,
```

</details>

可以发现为每个分区 gpu数量 `8，4，2，1` 都给了分组的方案， 其中:
* `partitionId`: 表示分区方案的ID， 可以通过该ID active/inactive 该分区, active后，
  其分区内的gpu可以透传给一个虚拟机

* `physicalId`: 表示GPU的physical ID, 可以通过`nvidia-smi -q | grep -i "Module ID\|GPU 00000000"`
  命令查看具体的GPU BDF 和 `physicalId` 映射关系, 如下

  ```
  [root@10-200-128-53 13:30:29 vm_1]# nvidia-smi -q | grep -i "Module ID\|GPU 00000000"
  GPU 00000000:19:00.0
          Module Id                         : 2
  GPU 00000000:3B:00.0
          Module Id                         : 4
  GPU 00000000:4C:00.0
          Module Id                         : 1
  GPU 00000000:5D:00.0
          Module Id                         : 3
  GPU 00000000:9B:00.0
          Module Id                         : 7
  GPU 00000000:BB:00.0
          Module Id                         : 5
  GPU 00000000:CB:00.0
          Module Id                         : 6
  GPU 00000000:DB:00.0
          Module Id                         : 8
  ```

## GPU VM

### 配置步骤
* 虚拟机中要安装 `NVIDIA driver, CUDA`, 以及`nvbandwidth`(用来测试P2P带宽)

* 给虚拟机分配GPU nvlink 分区

  + 假设我们要为GPU vm 分配2个GPU， 可以选择分区3
    ```
    "partitionId" : 3
    "numGpus" : 2,
    "isActive" : 0,
          "physicalId" : 3,
          "physicalId" : 1,
    
    ------------------------
    
    GPU 00000000:4C:00.0
            Module Id                         : 1
    GPU 00000000:5D:00.0
            Module Id                         : 3
    
    ```
  + 在`Service VM`中执行下面命令, active分区
    ```
    fmpl -a 3
    ```
  + 在`Sevice VM`中执行命令，查看分区是否active
    ```sh
    "partitionId" : 3
      "numGpus" : 2,
      "isActive" : 1,  ## 成功 active
            "physicalId" : 3,
            "physicalId" : 1,
    ```

* 将`4C:00.0`, `5D:00.0` GPU 透传到虚拟机中
* 启动虚拟机

### verify execute is success
可以在`Service VM` 或者host执行`nvswitch-audit -s <source gpu> -d <dest gpu>` 来判断两个
GPU之间是否有`nvlink`链接, 我们来看下现象:

* 未active分区，或者GPUs在不同的分区
  ```
  root@ubuntu240403:~#  nvswitch-audit -s 1 -d 3
  0
  
  Note: Number of NVLinks displayed corresponds to the maximum number of GPU NVLinks
        that NVSwitches are programmed to handle. Number of GPU NVLinks might be different
        than displayed in the above matrix
  ```
* 在同一个分区, 并active:
  ```
  root@ubuntu240403:~# nvswitch-audit -s 1 -d 3
  18

  Note: Number of NVLinks displayed corresponds to the maximum number of GPU NVLinks
        that NVSwitches are programmed to handle. Number of GPU NVLinks might be different
        than displayed in the above matrix
  ```

### 其他测试
可以执行`nvbandwidth` 测试工具测试两个卡的 P2P带宽

### 注意事项
* 无论是`Service VM` 还是`Host`接管 `NVswitch`, 都需要保证fabric manager 启动，并且透传给
  虚拟机的`GPU` 一定分好组, 否则调用cuda 就会遇到下面错误:
  ```
  root@ubuntu240403:~/cuda-samples-master/Samples/1_Utilities/deviceQuery# ./deviceQuery 
  ./deviceQuery Starting...
  
   CUDA Device Query (Runtime API) version (CUDART static linking)
  
  cudaGetDeviceCount returned 802
  -> system not yet initialized
  Result = FAIL
  ```
* 如果分错了组，例如将两个GPU透传给一个虚拟机，但是这两个GPU出于两个分区， 如下:
  ```sh
  "partitionId" : 9
  "numGpus" : 1,
  "isActive" : 1,
        "physicalId" : 3,
  ----------------
  "partitionId" : 7
  "numGpus" : 1,
  "isActive" : 1,
        "physicalId" : 1,
  ```
  cuda 层面不再会报错，但是`nvbandwidth`会报错
  ```
  Running device_to_device_memcpy_read_ce.
   Invalid value when checking the pattern at 0x735f80000000
   Current offset : 200064 
  ASSERT in expression h_errorFlag == 0 in void memcmpPatternHelper
    (CUstream, CUdeviceptr, long long unsigned int, unsigned int,
     CompareKernelFunc, std::shared_ptr<NodeHelper>)() :
     /root/nvbandwidth-main/memcpy.cpp:165
  ```

  Sevice vm 也会有报错:
  ```
  [ 8123.617543] nvidia-nvswitch1: SXid (PCI:0000:00:08.0): 11004, Non-fatal, Link 36 ingress invalid ACL (First)
  [ 8123.617548] nvidia-nvswitch2: SXid (PCI:0000:00:09.0): 11004, Non-fatal, Link 42 ingress invalid ACL (First)
  [ 8123.617552] nvidia-nvswitch0: SXid (PCI:0000:00:07.0): 11004, Non-fatal, Link 40 ingress invalid ACL (First)
  [ 8123.617554] nvidia-nvswitch3: SXid (PCI:0000:00:0a.0): 11004, Non-fatal, Link 58 ingress invalid ACL (First)
  [ 8123.617798] nvidia-nvswitch2: SXid (PCI:0000:00:09.0): 11004, Severity 0 Engine instance 42 Sub-engine instance 00
  [ 8123.618226] nvidia-nvswitch1: SXid (PCI:0000:00:08.0): 11004, Severity 0 Engine instance 36 Sub-engine instance 00
  [ 8123.618249] nvidia-nvswitch0: SXid (PCI:0000:00:07.0): 11004, Severity 0 Engine instance 40 Sub-engine instance 00
  [ 8123.619146] nvidia-nvswitch3: SXid (PCI:0000:00:0a.0): 11004, Severity 0 Engine instance 58 Sub-engine instance 00
  [ 8123.619785] nvidia-nvswitch1: SXid (PCI:0000:00:08.0): 11004, Data {0x00000008, 0xff9fc39c, 0x00000008, 0xff9fc39c, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}
  [ 8123.619817] nvidia-nvswitch2: SXid (PCI:0000:00:09.0): 11004, Data {0x00000008, 0xff9fc39c, 0x00000008, 0xff9fc39c, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}
  [ 8123.619827] nvidia-nvswitch0: SXid (PCI:0000:00:07.0): 11004, Data {0x00000008, 0xff9fc39c, 0x00000008, 0xff9fc39c, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}
  [ 8123.621161] nvidia-nvswitch3: SXid (PCI:0000:00:0a.0): 11004, Data {0x00000008, 0xff9fc39c, 0x00000008, 0xff9fc39c, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}
  [ 8123.621420] nvidia-nvswitch2: SXid (PCI:0000:00:09.0): 11004, Severity 0 Engine instance 42 Sub-engine instance 00
  [ 8123.621429] nvidia-nvswitch0: SXid (PCI:0000:00:07.0): 11004, Severity 0 Engine instance 40 Sub-engine instance 00
  [ 8123.621861] nvidia-nvswitch1: SXid (PCI:0000:00:08.0): 11004, Severity 0 Engine instance 36 Sub-engine instance 00
  [ 8123.623225] nvidia-nvswitch3: SXid (PCI:0000:00:0a.0): 11004, Severity 0 Engine instance 58 Sub-engine instance 00
  [ 8123.623284] nvidia-nvswitch2: SXid (PCI:0000:00:09.0): 11004, Data {0x00000038, 0x008bb02f, 0x0000002a, 0x30008000, 0x8008c065, 0x0000005b, 0x28201022, 0x00000001, 0x00000000}
  [ 8123.623355] nvidia-nvswitch0: SXid (PCI:0000:00:07.0): 11004, Data {0x00000038, 0x0055e609, 0x00000028, 0x30008000, 0x0008c065, 0x000007db, 0x28201022, 0x00000000, 0x00000000}
  [ 8123.623929] nvidia-nvswitch1: SXid (PCI:0000:00:08.0): 11004, Data {0x00000038, 0x00559b5b, 0x00000024, 0x30008000, 0x0008c065, 0x000001bc, 0x28201022, 0x00000000, 0x00000000}
  
  ```

所以, 应该保持以下原则:
* host/Service VM  fabric manager 必须开启
* 分配给虚拟机的GPU，其分区一定active，并且一定在同一个分区

## 其他
### cuda simples 编译不通过

例如，我们只编译`deviceQuer`(`Samples/1_Utilities/deviceQuer`), 执行cmake && make  后，会遇到下面错误:

```
nvcc fatal error: Unsupported gpu architecture 'compute_110' (but compute_120 is supported
```
根据链接<sup>3</sup>, workround方案是将`CMakeList.txt`中的 `110` 手动删除...

## 参考链接
1. [NVIDIA HGX Shared NVSwitch GPU Passthrough Virtualization Integration Guide](https://docs.nvidia.com/hgx-platforms/shared-nvswitch-gpu-passthrough-virtualization-integration-guide.pdf)
2. [GITHUB: nvbandwidth](https://github.com/NVIDIA/nvbandwidth)
3. [nvcc fatal error: Unsupported gpu architecture 'compute_110' (but compute_120 is supported) ](https://github.com/NVIDIA/cuda-samples/issues/388)
