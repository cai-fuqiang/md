# install
-bash-4.4#  ./mlnxofedinstall --with-nvmf --add-kernel-support --force-fw-update                                                   
Detected rhel8u1 aarch64. Disabling installing 32bit rpms...
Note: This program will create MLNX_OFED_LINUX TGZ for rhel8.1 under /tmp/MLNX_OFED_LINUX-5.5-1.0.3.2-4.18.0-147.5.1.el8_1.5es.14.aarch64 directory.
See log file /tmp/MLNX_OFED_LINUX-5.5-1.0.3.2-4.18.0-147.5.1.el8_1.5es.14.aarch64/mlnx_iso.31478_logs/mlnx_ofed_iso.31478.log

Checking if all needed packages are installed...
Building MLNX_OFED_LINUX RPMS . Please wait...



Creating metadata-rpms for 4.18.0-147.5.1.el8_1.5es.14.aarch64 ...




WARNING: If you are going to configure this package as a repository, then please note
WARNING: that it contains unsigned rpms, therefore, you need to disable the gpgcheck
WARNING: by setting 'gpgcheck=0' in the repository conf file.
Created /tmp/MLNX_OFED_LINUX-5.5-1.0.3.2-4.18.0-147.5.1.el8_1.5es.14.aarch64/MLNX_OFED_LINUX-5.5-1.0.3.2-rhel8.1-ext.tgz

rpm --nosignature -e --allmatches --nodeps mlnx-ofa_kernel
rpm -e --allmatches --nodeps  mlnx-ofa_kernel-modules mlnx-ofa_kernel-devel
Installing /tmp/MLNX_OFED_LINUX-5.5-1.0.3.2-4.18.0-147.5.1.el8_1.5es.14.aarch64/MLNX_OFED_LINUX-5.5-1.0.3.2-rhel8.1-ext
/tmp/MLNX_OFED_LINUX-5.5-1.0.3.2-4.18.0-147.5.1.el8_1.5es.14.aarch64/MLNX_OFED_LINUX-5.5-1.0.3.2-rhel8.1-ext/mlnxofedinstall --force --with-nvmf --force-fw-update
Detected rhel8u1 aarch64. Disabling installing 32bit rpms...
Logs dir: /tmp/MLNX_OFED_LINUX.23755.logs
General log file: /tmp/MLNX_OFED_LINUX.23755.logs/general.log
[WANG_DEBUG] : is not kmpThis program will install the MLNX_OFED_LINUX package on your machine.
Note that all other Mellanox, OEM, OFED, RDMA or Distribution IB packages will be removed.
Those packages are removed due to conflicts with MLNX_OFED_LINUX, do not reinstall them.


rpm --nosignature -e --allmatches --nodeps libibverbs libibverbs. libibverbs libibverbs. libibverbs libibverbs. libibverbs libibverbs. mlnx-tools mlnx-tools. libibverbs libibverbs. mlnx-tools mlnx-tools. mlnx-tools mlnx-tools.

Starting MLNX_OFED_LINUX-5.5-1.0.3.2 installation ...

[WANG_DEBUG] : sub install  ver 5.5
[WANG_DEBUG] : sub install  package ofed-scripts
[WANG_DEBUG] : sub install  ver 5.2.0
[WANG_DEBUG] : sub install  package mlnx-tools
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
mlnx-tools-5.2.0-0.55103              ########################################
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package rdma-core
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package libibverbs
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
libibverbs-55mlnx37-1.55103           ########################################
[WANG_DEBUG] : sub install  ver 5.5
[WANG_DEBUG] : sub install  package mlnx-ofa_kernel
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.rhel8u1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing mlnx-ofa_kernel 5.5 RPM
Preparing...                          ########################################
Updating / installing...
mlnx-ofa_kernel-5.5-OFED.5.5.1.0.3.1.r########################################
[WANG_DEBUG] : sub install  ver 5.5
[WANG_DEBUG] : sub install  package mlnx-ofa_kernel-modules
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.rhel8u1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing mlnx-ofa_kernel-modules 5.5 RPM
Preparing...                          ########################################
Updating / installing...
mlnx-ofa_kernel-modules-5.5-OFED.5.5.1########################################
depmod: ERROR: fstatat(5, drbd_transport_tcp.ko): No such file or directory
depmod: ERROR: fstatat(5, drbd.ko): No such file or directory
depmod: ERROR: fstatat(5, hinic.ko): No such file or directory
[WANG_DEBUG] : sub install  ver 5.5
[WANG_DEBUG] : sub install  package mlnx-ofa_kernel-devel
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.rhel8u1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing mlnx-ofa_kernel-devel 5.5 RPM
Preparing...                          ########################################
Updating / installing...
mlnx-ofa_kernel-devel-5.5-OFED.5.5.1.0########################################
[WANG_DEBUG] : sub install  ver 4.18.0
[WANG_DEBUG] : sub install  package kernel-mft
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is 106
[WANG_DEBUG]: the release is 106
[WANG_DEBUG]: the release is 106.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing kernel-mft 4.18.0 RPM
Preparing...                          ########################################
Updating / installing...
kernel-mft-4.18.0-106.kver.4.18.0_147.########################################
depmod: ERROR: fstatat(5, drbd_transport_tcp.ko): No such file or directory
depmod: ERROR: fstatat(5, drbd.ko): No such file or directory
depmod: ERROR: fstatat(5, hinic.ko): No such file or directory
[WANG_DEBUG] : sub install  ver 1.1.4.90mlnx1
[WANG_DEBUG] : sub install  package knem
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is OFED.5.5.0.5.2.1.rhel8u1
[WANG_DEBUG]: the release is OFED.5.5.0.5.2.1
[WANG_DEBUG]: the release is OFED.5.5.0.5.2.1.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing knem 1.1.4.90mlnx1 RPM
Preparing...                          ########################################
Updating / installing...
knem-1.1.4.90mlnx1-OFED.5.5.0.5.2.1.rh########################################
[WANG_DEBUG] : sub install  ver 1.1.4.90mlnx1
[WANG_DEBUG] : sub install  package knem-modules
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is OFED.5.5.0.5.2.1.rhel8u1
[WANG_DEBUG]: the release is OFED.5.5.0.5.2.1
[WANG_DEBUG]: the release is OFED.5.5.0.5.2.1.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing knem-modules 1.1.4.90mlnx1 RPM
Preparing...                          ########################################
Updating / installing...
knem-modules-1.1.4.90mlnx1-OFED.5.5.0.########################################
depmod: ERROR: fstatat(5, drbd_transport_tcp.ko): No such file or directory
depmod: ERROR: fstatat(5, drbd.ko): No such file or directory
depmod: ERROR: fstatat(5, hinic.ko): No such file or directory
[WANG_DEBUG] : sub install  ver 2.6.3
[WANG_DEBUG] : sub install  package xpmem
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is 1.55103.rhel8u1
[WANG_DEBUG]: the release is 1.55103
[WANG_DEBUG]: the release is 1.55103.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing xpmem 2.6.3 RPM
Preparing...                          ########################################
Updating / installing...
xpmem-2.6.3-1.55103.rhel8u1           ########################################
[WANG_DEBUG] : sub install  ver 2.6.3
[WANG_DEBUG] : sub install  package xpmem-modules
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is 1.55103.rhel8u1
[WANG_DEBUG]: the release is 1.55103
[WANG_DEBUG]: the release is 1.55103.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing xpmem-modules 2.6.3 RPM
Preparing...                          ########################################
Updating / installing...
xpmem-modules-2.6.3-1.55103.kver.4.18.########################################
[WANG_DEBUG] : sub install  ver 5.5
[WANG_DEBUG] : sub install  package iser
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing iser 5.5 RPM
Preparing...                          ########################################
Updating / installing...
iser-5.5-OFED.5.5.1.0.3.1.kver.4.18.0_########################################
depmod: ERROR: fstatat(5, drbd_transport_tcp.ko): No such file or directory
depmod: ERROR: fstatat(5, drbd.ko): No such file or directory
depmod: ERROR: fstatat(5, hinic.ko): No such file or directory
[WANG_DEBUG] : sub install  ver 5.5
[WANG_DEBUG] : sub install  package srp
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing srp 5.5 RPM
Preparing...                          ########################################
Updating / installing...
srp-5.5-OFED.5.5.1.0.3.1.kver.4.18.0_1########################################
depmod: ERROR: fstatat(5, drbd_transport_tcp.ko): No such file or directory
depmod: ERROR: fstatat(5, drbd.ko): No such file or directory
depmod: ERROR: fstatat(5, hinic.ko): No such file or directory
[WANG_DEBUG] : sub install  ver 5.5
[WANG_DEBUG] : sub install  package isert
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing isert 5.5 RPM
Preparing...                          ########################################
Updating / installing...
isert-5.5-OFED.5.5.1.0.3.1.kver.4.18.0########################################
depmod: ERROR: fstatat(5, drbd_transport_tcp.ko): No such file or directory
depmod: ERROR: fstatat(5, drbd.ko): No such file or directory
depmod: ERROR: fstatat(5, hinic.ko): No such file or directory
[WANG_DEBUG] : sub install  ver 5.5
[WANG_DEBUG] : sub install  package mlnx-nvme
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1
[WANG_DEBUG]: the release is OFED.5.5.1.0.3.1.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing mlnx-nvme 5.5 RPM
Preparing...                          ########################################
Updating / installing...
mlnx-nvme-5.5-OFED.5.5.1.0.3.1.kver.4.########################################
depmod: ERROR: fstatat(5, drbd_transport_tcp.ko): No such file or directory
depmod: ERROR: fstatat(5, drbd.ko): No such file or directory
depmod: ERROR: fstatat(5, hinic.ko): No such file or directory
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package librdmacm
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package libibumad
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package infiniband-diags
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package rdma-core-devel
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package libibverbs-utils
[WANG_DEBUG] : sub install  ver 0.10
[WANG_DEBUG] : sub install  package ibsim
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package ibacm
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package librdmacm-utils
[WANG_DEBUG] : sub install  ver 5.10.0.MLNX20211115.e645cc83
[WANG_DEBUG] : sub install  package opensm-libs
[WANG_DEBUG] : sub install  ver 5.10.0.MLNX20211115.e645cc83
[WANG_DEBUG] : sub install  package opensm
[WANG_DEBUG] : sub install  ver 5.10.0.MLNX20211115.e645cc83
[WANG_DEBUG] : sub install  package opensm-devel
[WANG_DEBUG] : sub install  ver 5.10.0.MLNX20211115.e645cc83
[WANG_DEBUG] : sub install  package opensm-static
[WANG_DEBUG] : sub install  ver 4.5
[WANG_DEBUG] : sub install  package perftest
[WANG_DEBUG] : sub install  ver 4.16.0
[WANG_DEBUG] : sub install  package mstflint
[WANG_DEBUG] : sub install  ver 4.18.0
[WANG_DEBUG] : sub install  package mft
[WANG_DEBUG] : sub install  ver 55mlnx37
[WANG_DEBUG] : sub install  package srp_daemon
[WANG_DEBUG] : sub install  ver 2.1.1
[WANG_DEBUG] : sub install  package ibutils2
[WANG_DEBUG] : sub install  ver 1.0
[WANG_DEBUG] : sub install  package dump_pr
[WANG_DEBUG] : sub install  ver 1.0
[WANG_DEBUG] : sub install  package ar_mgr
[WANG_DEBUG] : sub install  ver 6.0.0
[WANG_DEBUG] : sub install  package ibdump
[WANG_DEBUG] : sub install  ver 1.1.17
[WANG_DEBUG] : sub install  package dpcp
[WANG_DEBUG] : sub install  ver 1.12.0
[WANG_DEBUG] : sub install  package ucx
[WANG_DEBUG] : sub install  ver 1.12.0
[WANG_DEBUG] : sub install  package ucx-devel
[WANG_DEBUG] : sub install  ver 2.6.1.MLNX20211124.aac4a56
[WANG_DEBUG] : sub install  package sharp
[WANG_DEBUG] : sub install  ver 1.12.0
[WANG_DEBUG] : sub install  package ucx-cma
[WANG_DEBUG] : sub install  ver 1.12.0
[WANG_DEBUG] : sub install  package ucx-ib
[WANG_DEBUG] : sub install  ver 1.12.0
[WANG_DEBUG] : sub install  package ucx-rdmacm
[WANG_DEBUG] : sub install  ver 1.12.0
[WANG_DEBUG] : sub install  package ucx-knem
[WANG_DEBUG] : sub install  ver 2.6.3
[WANG_DEBUG] : sub install  package libxpmem
[WANG_DEBUG]: install kernel rpm BEG
[WANG_DEBUG]: the release is 1.55103.rhel8u1
[WANG_DEBUG]: the release is 1.55103
[WANG_DEBUG]: the release is 1.55103.kver.4.18.0_147.5.1.el8_1.5es.14.aarch64
Installing libxpmem 2.6.3 RPM
Preparing...                          ########################################
Updating / installing...
libxpmem-2.6.3-1.55103.rhel8u1        ########################################
[WANG_DEBUG] : sub install  ver 1.12.0
[WANG_DEBUG] : sub install  package ucx-xpmem
[WANG_DEBUG] : sub install  ver 1.0.3
[WANG_DEBUG] : sub install  package mpi-selector
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
mpi-selector-1.0.3-1.55103            ########################################
[WANG_DEBUG] : sub install  ver 4.7.3202
[WANG_DEBUG] : sub install  package hcoll
[WANG_DEBUG] : sub install  ver 4.1.2rc2
[WANG_DEBUG] : sub install  package openmpi
[WANG_DEBUG] : sub install  ver 5.13
[WANG_DEBUG] : sub install  package mlnx-ethtool
[WANG_DEBUG] : sub install  ver 5.14.0
[WANG_DEBUG] : sub install  package mlnx-iproute2
[WANG_DEBUG] : sub install  ver 2.0.6
[WANG_DEBUG] : sub install  package rshim
[WANG_DEBUG] : sub install  ver 5.5
[WANG_DEBUG] : sub install  package mlnxofed-docs
[WANG_DEBUG] : sub install  ver 3.2.20
[WANG_DEBUG] : sub install  package mpitests_openmpi
Installing user level RPMs:
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ofed-scripts-5.5-OFED.5.5.1.0.3       ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
rdma-core-55mlnx37-1.55103            ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
librdmacm-55mlnx37-1.55103            ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
libibumad-55mlnx37-1.55103            ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
infiniband-diags-55mlnx37-1.55103     ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
rdma-core-devel-55mlnx37-1.55103      ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
libibverbs-utils-55mlnx37-1.55103     ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ibsim-0.10-1.55103                    ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ibacm-55mlnx37-1.55103                ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
librdmacm-utils-55mlnx37-1.55103      ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
opensm-libs-5.10.0.MLNX20211115.e645cc########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
opensm-5.10.0.MLNX20211115.e645cc83-0.########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
opensm-devel-5.10.0.MLNX20211115.e645c########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
opensm-static-5.10.0.MLNX20211115.e645########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
perftest-4.5-0.12.ge93c538.55103      ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
mstflint-4.16.0-1.55103               ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
mft-4.18.0-106                        ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
srp_daemon-55mlnx37-1.55103           ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ibutils2-2.1.1-0.143.MLNX20211118.gdf3########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
dump_pr-1.0-5.10.0.MLNX20211107.g9b215########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ar_mgr-1.0-5.10.0.MLNX20211020.gceea2f########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ibdump-6.0.0-1.55103                  ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
dpcp-1.1.17-1.55103                   ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ucx-1.12.0-1.55103                    ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ucx-devel-1.12.0-1.55103              ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
sharp-2.6.1.MLNX20211124.aac4a56-1.551########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ucx-cma-1.12.0-1.55103                ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ucx-ib-1.12.0-1.55103                 ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ucx-rdmacm-1.12.0-1.55103             ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ucx-knem-1.12.0-1.55103               ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
ucx-xpmem-1.12.0-1.55103              ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
hcoll-4.7.3202-1.55103                ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
openmpi-4.1.2rc2-1.55103              ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
mlnx-ethtool-5.13-1.55103             ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
mlnx-iproute2-5.14.0-1.55103          ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
Failed to get D-Bus connection: Operation not permitted
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
mlnxofed-docs-5.5-1.0.3.2             ########################################
[WANG_DEBUG]: install rpm beginPreparing...                          ########################################
mpitests_openmpi-3.2.20-5d20b49.55103 ########################################
Device (01:00.0):
        01:00.0 Ethernet controller: Mellanox Technologies MT27710 Family [ConnectX-4 Lx]
        Link Width: x8
        PCI Link Speed: 8GT/s

Device (01:00.1):
        01:00.1 Ethernet controller: Mellanox Technologies MT27710 Family [ConnectX-4 Lx]
        Link Width: x8
        PCI Link Speed: 8GT/s

Device (81:00.0):
        81:00.0 Ethernet controller: Mellanox Technologies MT27710 Family [ConnectX-4 Lx]
        Link Width: x8
        PCI Link Speed: 8GT/s

Device (81:00.1):
        81:00.1 Ethernet controller: Mellanox Technologies MT27710 Family [ConnectX-4 Lx]
        Link Width: x8
        PCI Link Speed: 8GT/s


Installation finished successfully.


Preparing...                          ################################# [100%]
Updating / installing...
   1:mlnx-fw-updater-5.5-1.0.3.2      ################################# [100%]

Added 'RUN_FW_UPDATER_ONBOOT=no to /etc/infiniband/openib.conf

Initializing...
Attempting to perform Firmware update...

The firmware for this device is not distributed inside Mellanox driver: 01:00.0 (PSID: HUA0020040036)
To obtain firmware for this device, please contact your HW vendor.


The firmware for this device is not distributed inside Mellanox driver: 81:00.0 (PSID: HUA0020040036)
To obtain firmware for this device, please contact your HW vendor.

Failed to update Firmware.
See /tmp/MLNX_OFED_LINUX.23755.logs/fw_update.log
You may need to update your initramfs before next boot. To do that, run:

   dracut -f
To load the new driver, run:
/etc/init.d/openibd restart
Note: In order to load the new nvme-rdma and nvmet-rdma modules, the nvme module must be reloaded.
