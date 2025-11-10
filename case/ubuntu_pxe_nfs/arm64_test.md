## 测试步骤

### nfs prepare
1. 在链接 [1] 中下载rootfs
2. 解压tar包到既定路径
3. 启动nfs-server

### dhcp prepare

1. 配置dhcpd
   ```
   subnet 172.17.224.0 netmask 255.255.255.0 {
       default-lease-time 600;
       max-lease-time 7200;
       range 172.17.224.150 172.17.224.200;
   
       option routers 172.17.224.1;
   
       option domain-name-servers 8.8.8.8, 8.8.4.4;
   
       option broadcast-address 172.17.224.255;
   
       option subnet-mask 255.255.255.0;
   }
   ```
2. 执行下面脚本配置br1
   ```sh
   ip link add name br1 type bridge
   ip link set br1 up
   ip link set eth1 master br1
   ip addr flush dev eth1
   ifconfig br1 172.17.224.1 netmask 255.255.255.0
   ```
3. 启动dhcpd服务: `systemctl restart dhcpd`

### nfs initrd

下载 kernel initrd from [2]

### 启动虚拟机测试
使用下面命令行参数
```sh
KERNEL_CMDLINE="netboot=nfs nfsroot=172.17.224.1:/srv/nfs/rootfs ip=dhcp rw  boot=nfs console=ttyAMA0,115200 hwaddr=50:54:11:00:10:47 rd.break=cmdline rd.udev.log_level=debug"

/usr/libexec/qemu-kvm -name guest=centos8,debug-threads=on \
        -m 8192 \
        -smp 8          \
        -nodefaults -boot strict=on     \
        -name wang_rocky,debug-threads=on \
        -machine virt,accel=kvm,usb=off,dump-guest-core=off,gic-version=3 \
        -boot strict=on \
        -kernel  ./jammy-server-cloudimg-arm64-vmlinuz-generic \
        -initrd  ./jammy-server-cloudimg-arm64-initrd-generic  \
        -append "$KERNEL_CMDLINE" \
        -cpu host \
        -qmp tcp:localhost:9999,server,nowait \
        -drive file=./QEMU_EFI-pflash.raw,if=pflash,format=raw,unit=0,readonly=on \
        -drive file=./fd_file,if=pflash,format=raw,unit=1 \
        -msg timestamp=on \
        -nographic   \
        -device pci-bridge,id=bridge0,chassis_nr=3,addr=0x14 \
        -usb \
        -serial telnet:localhost:66668,server,nowait -gdb tcp::1237 \
        -net nic,model=virtio -net tap,ifname=tap2,script=no,downscript=no   \
        -device virtio-net-pci,mac=50:54:11:00:10:46,netdev=internet,id=internet-dev    \
        -netdev user,id=internet,hostfwd=tcp::55557-:22    \
```

可以引导挂载成功.
## 参考链接 
1. [rootfs 下载地址](https://cloud-images.ubuntu.com/jammy/current/)
2. [kernel initrd](https://cloud-images.ubuntu.com/jammy/current/unpacked/)
