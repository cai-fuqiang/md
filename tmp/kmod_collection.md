## 简介

`kmod-collection` 用来更方便的编译kmod, 其框架是基于`kernel-rpm-macros`
做的. 通过使用 `kmod-collection` 我们一方面可以忽略掉很多细节,另
一方面, 也可以让我们编译出来的kmod的很多行为保持一致.


## 使用方法

1. 下载`kmod-collection`代码
   ```
   git clone git@sz.koji.easystack.io:distro/kmod-collection.git 
   ```
2. 创建kmod目录
   ```
   cd ${kmod_root}/kmods

   mkdir ${KMOD_DIR}
   ```
3. 在该目录中创建如下文件
   * AUTHORS: 维护人邮箱
   * changelog: 该kmod的changelog
   * kmodtool.env: 定义该kmod的各个属性:
     + KMOD_NAME: %{name}
     + KMOD_VERSION: %{version}
     + KMOD_RELEASE: %{release}
     + KMOD_SOURCES: source.tar 路径
     + KMOD_TARVER:  为spec文件中 tarball_version 变量值, 
                     但未找到在什么地方使用到该变量
     + KMOD_TARBALL: 

## 主要文件介绍
* Makefile
  
* kmodtool.spec.in
* kver
