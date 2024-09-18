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
     <details>
     <summary> kmodtool.env 字段详细解释 </summary>

     + KMOD_NAME: %{name}
     + KMOD_VERSION: %{version}
     + KMOD_RELEASE: %{release}
     + KMOD_SOURCES: source tar 包 名称
     + KMOD_TARVER:  为spec文件中 tarball_version 变量值, 
                     但未找到在什么地方使用到该变量
     * KMOD_DESC: 定义了`%description` 字段

     下面的字段是optional:

     + KMOD_TARBALL: 用于`%setup` macro 的 `-n`参数, 表示tar包解压出
                     来的目录的name, 详见 `kmodtool.spec.in`
     * KMOD_PATCH: 定义了`%Patchx` 字段, 以空格隔开各个patch name, 
                   详见`mlnx-nvme-23.10-1.1.9.1` kmod.
     + 和基于mlnx devel 包相关的选项
       * KMOD_WITH_OFED: 基于kernel-devel 编译intree版本, 并使用mlnx-ofa_kernel-devel,
                         mlnx-nvme-devel编译`OFED`版本
       * KMOD_DONT_NEED_DEF: 不编译intree 版本
       * KMOD_DONT_NEED_MLNX_NVME: 编译`OFED`版本时, 仅使用`mlnx-ofa_kernel-devel`而不使用
                                   `mlnx-nvme-devel`(该选项一般用于编译`kmod-mlnx-nvme`)
     </summary>

   * build: 自定义`%build`中的部分代码(主要是make部分), 详细见, `Makefile`,`kmods/common.env`,
            `kmodtool.spec.in`部分(下面会简单介绍整个原理)
   * prep: 自定义`%prep`中的部分代码. 如要了解详情, 同`build`
   * install: 自定义`%install`中的部分代码, 如果要了解详情, 同`build`
   * install_extras: 自定义`%install`中的部分代码, 和`install`文件不同的是, `install_extras`
                     不会替换`%install`中的`make -C ... modules_install`部分, 如果要了解
                     详情, 同build
   * subpkgs(dir): 用于定义其他的rpm包. 放在下面介绍

4. 定义好上述文件后, 在`kver/${KENREL_VERSION}`目录中创建软链接, 以`example` kmod, 
   `KVER:4.18.0-372`为例:
   ```
   ➜  kmod-collection git:(mlnx-ofa_kernel-23.10) cd kver/4.18.0-372
   ➜  4.18.0-372 git:(mlnx-ofa_kernel-23.10) ln -sf ../../kmods/example example
   ➜  4.18.0-372 git:(mlnx-ofa_kernel-23.10) cd -
   ```
5. 将源码包copy到 `SOURCES` 目录, name同 `kmodtool.env`文件中的`KMOD_SOURCES`.
6. 在根层级执行下面命令:
   ```
   ➜  kmod-collection git:(mlnx-ofa_kernel-23.10) export LANG=en;export KVER=4.18.0-372; export KMOD=example
   ➜  kmod-collection git:(mlnx-ofa_kernel-23.10) make srpm
   ...
   Wrote: /home/wang/workspace/kmod/kmod-collection/SRPMS/kmod-example-kmodtool-1-0.es8_4.k372.src.rpm
   ```

   最终可以在 `SRPMS` 目录中找到相应的srpm package. 并且在 `SPECS`目录可以找到相应的spec文件. 如果需要只生成
   spec 文件, 可以执行`make spec`


## 原理介绍

### 主要流程

`kmod-collection`, 本身是依靠`kmodtool.spec.in`作为基本模块, 然后使用`kmods/common.env`和
当前正在编译的kmod dir中的各个文件作为hook, 然后生成相应的spec文件, 在定义binary rpm
时, 使用的是`kernel-rpm-macros`中的`kernel_module_package`宏, 替换了原本的`kmodtool`
(在SOURCES/kmodtool)


### spec 文件生成流程

我们先来看下Makefile中的 `spec` 流程
```
spec:check
   ...
   (pushd kmods/$(KMOD) ; export TAG=$(TAG); source kmodtool.env ; 
   source ../common.env ;popd && envsubst '$(shell ./kmods/get_env_for_envsubst 
   ./kmods/common.env)' < kmodtool.spec.in > SPECS/$(KMOD).spec)
   ...
```
该行命令步骤大概是:
1. source kmodtool.env; source kmods/common.env  定义某一些环境变量的值.
2. 调用 envsubst 将 kmodtool.spec.in某些变量替换为当前环境变量的值, 并生成新的
   spec文件

   其中所需要替换的环境变量由 `kmods/get_env_for_envsubst`收集:
   ```sh
   #!/bin/sh
   COMM_ENV_PATH=$1
   printf '${%s} ' $(cat $COMM_ENV_PATH |grep export |cut -d'=' -f1|awk  '{$1=""; print $0}')
   ```

   方法也很简单, 找到 `common.env`中所有export的变量

### sub package

sub package 功能可以让我们生成多个rpm, 例如source包或者devel包.
其使用方法也很简单. 以 example kmod为例:

在`example`kmod路径下创建如下文件:
```
DEFINE  ENV  description_  files_  post_
```

其文件所有显而易见, 不再过多描述. 

创建完成之后, 其他的步骤和上面`使用方法`章节提到的步骤一致

我们来看下具体的原理:

我们首先看makefile
```
spec: check
        ...
        @ > BUILD/$(KMOD)_subpkgs.def
        @ > BUILD/$(KMOD)_subpkgs.files
#ifneq (,$(wildcard kmods/$(KMOD)/subpkgs))
        $(foreach pn,$(wildcard kmods/$(KMOD)/subpkgs/*), \
./kmods/gen_subpkg_def $(pn) >>  BUILD/$(KMOD)_subpkgs.def ;\
./kmods/gen_subpkg_files $(pn) >>  BUILD/$(KMOD)_subpkgs.files;)
#endif
        ...
#ifneq (,$(wildcard kmods/$(KMOD)/subpkgs))
        sed -i '/^###KMOD_SUBPACKAGES_DEFINES###$$/r BUILD/$(KMOD)_subpkgs.def' SPECS/$(KMOD).spec
        sed -i '/^###KMOD_SUBPACKAGES_FILES###$$/r BUILD/$(KMOD)_subpkgs.files' SPECS/$(KMOD).spec
        ...
```
可以看到步骤大概有两个:
1. 通过 `gen_subpkg_files` `gen_subpkg_def` 得到一些输出,并重定向到相应文件.
2. 通过`sed` 命令替换 spec 文件中的某些注释行

通过上面的流程就定义出了`%package`和`%files` 字段

所以, 我们主要来看下上面脚本输出, 以`gen_subpkg_def`为例:
```sh
➜  kmod-collection git:(mlnx-ofa_kernel-23.10) cat kmods/gen_subpkg_def
#!/bin/sh

PN=$1
DIR=$(dirname ${PN})
KMOD=$(basename $(dirname ${DIR}))
SUBPKG=$(basename ${PN})

. ./kmods/common_subpkg.env && . $PN/ENV || true

echo "# $PN $DIR $KMOD $SUBPKG"
echo "# ${IS_FULLNAME}"

echo "############################"
[ -z ${SUBPKGARCH} ] || echo "%ifarch ${SUBPKGARCH}"
echo %package $(test "true" == "${IS_FULLNAME}" && echo ' -n' || true ) ${SUBPKG}
echo ""
cat $PN/DEFINE
[ -z ${SUBPKGARCH} ] || echo "%endif"
echo ""
echo ""
```
原理也比较简单, 大概就是定义了 `%package` 字段, `-n` 参数为`subpkgs`目录名.
而`%package` 字段内容由 文件 `DEFINE`展开.

### KMOD PATCH
kmod patch的功能主要用于spec文件中 `Patchxxx` 字段定义, 我们以
`mlnx-nvme-23.10-1.1.9.1`为例, 来看下增加一个patch需要添加那些内容:

1. 在`kmodtool.env` 中, 增加 `KMOD_PATCH`字段
```
KMOD_PATCH="0001-nvmet-fix-nvme-status-code-when-namespace-is-disable.patch"
```
> NOTE
>
> 该字段主要用于在生成 srpm 之前, 将patch文件copy到 SOURCES 目录
> ```
>
> srpm: spec
>         $(foreach patch, $(subst ",, $(KMOD_PATCH)),    \
>                 cp kmods/$(KMOD)/$(patch) SOURCES;)
>         rm -f SRPMS/kmod-$(KMOD_NAME)-kmodtool*.src.rpm
>         rpmbuild --define "_topdir `pwd`"   --define "dist .$(TAG)"  -bs SPECS/$(KMOD).spec
> 
> clean:
>         $(foreach patch, $(subst ",, $(KMOD_PATCH)),    \
>                 rm -f SOURCES/$(patch))
> ```

2. 在`required`文件中, 增加
```
Patch0: 0001-nvmet-fix-nvme-status-code-when-namespace-is-disable.patch
```
3. 在`prep`中, 增加
```
%patch0 -p1
```

### memory load order

当我们kmod中含有多个module, 并且各个kmod中module有部分重合时, 我们需要
定义好module加载的优先级. 以`kmod-mlnx-nvme`和`kmod-mlnx-ofa_kernel`为例:

* kmod-mlnx-ofa_kernel
```sh
➜  mlnx-ofa rpm -ql kmod-mlnx-ofa_kernel-23.10-1.1.9.1.es8_5.k372.aarch64.rpm |grep -E 'nvmet|depmod'
/etc/depmod.d/zz01-mlnx-ofa_kernel.conf
/lib/modules/4.18.0-372.19.1.es8_10.aarch64/extra/mlnx-ofa_kernel/drivers/nvme/target/nvmet-rdma.ko
```
* kmod-mlnx-nvme
```
➜  mlnx-ofa rpm -ql kmod-mlnx-nvme-OFED-23.10-1.1.9.1.es8_6.k372.aarch64.rpm |grep -E 'nvmet.ko|depmod'
/etc/depmod.d/zz02-mlnx-nvme-OFED.conf
/lib/modules/4.18.0-372.19.1.es8_10.aarch64/extra/mlnx-nvme-OFED/target/nvmet.ko
```

可以看到两个包中都有`nvmet.ko`驱动,但是 `kmod-mlnx-ofa_kernel`包中的驱动是`dummy`的, 需要
优先加载`kmod-mlnx-nvme` rpm包中的该驱动

目前的做法是, 定义`depmod.conf`文件名称前缀, 例如`zz01`, `zz02`, 其中`zz02`的优先级比`zz01`高.

而具体的配置, 则需要在`kmodtool.env`中增加下面字段
```
KMOD_DEPMOD_ORDER="zz02"
```
> NOTE
>
> 该字段的默认值为zz01

### build for mlnx devel

该功能主要用于基于一份source, 编译出多个版本的binary rpm, 和 numa64 功能独立. 举个
例子, 目前需要基于intree, 和mlnx devel 编译出两个版本, 在arm64 平台上会编译出下面几个包:

* kmod-xxx
* kmod-xxx-OFED
* kmod-xxx-numa64
* kmod-xxx-OFED-numa64

其中OFED为基于mlnx devel 编译出来的.

配置方法以`drbd`为例, 需要在`kmodtool.env`中增加:
```
KMOD_WITH_OFED=1
```
具体原理为:

1. 在`kmodtool.spec.in`中定义 bflavors:
```
%if %{with_default}
%global bflavors default
%endif

%if %{with_MLNX_OFED}
%global bflavors %{?bflavors} OFED
%endif
```
目前仅定义了`default` 和 `OFED`两个`bflavors`

2. 增加`BuildRequires`
```
%if %{with_MLNX_OFED}
BuildRequires: mlnx-ofa_kernel-devel
%endif

%if %{with_MLNX_NVME}
BuildRequires: mlnx-nvme-devel
%endif
```

3. 最重要的, 增加 `kernel_module_package`宏的调用, 以增加binary rpm的生成
```
%if %{with_MLNX_OFED}
%kernel_module_package -s %{SOURCE1} -f /tmp/file.list.default-OFED -n %{kmod_name}-OFED default
%endif

%if %{with_MLNX_OFED} && %{with_numa64}
%kernel_module_package -s %{SOURCE1} -f /tmp/file.list.numa64-OFED -n %{kmod_name}-OFED numa64
%endif
```
4. 在 `%build`阶段时, 如果`bflavor`为`OFED`, 则需要增加对 头文件引用和Module.symvers的链接.如下:
```
for flavor in %{?kflavors}; do
for bflavor in %{?bflavors}; do

MLNX_OFA=/usr/src/ofa_kernel/%{_arch}/$(get_kernel_pluspath $flavor)

if [[ "$bflavor" == "OFED" ]];then
        _PRE_CFLAGS="${_PRE_CFLAGS} -include $MLNX_OFA/include/linux/compat-2.6.h \
        -I$MLNX_OFA/include \
        -I$MLNX_OFA/include/uapi"

        _KBUILD_EXTRA_SYMBOLS="$_KBUILD_EXTRA_SYMBOLS $MLNX_OFA/Module.symvers"
%if %{with_MLNX_NVME}
        MLNX_NVME=/usr/src/mlnx-nvme/%{_arch}/$(get_kernel_pluspath $flavor)
        _KBUILD_EXTRA_SYMBOLS="$_KBUILD_EXTRA_SYMBOLS $MLNX_NVME/Module.symvers"
%endif
else
        _PRE_CFLAGS=""
        _KBUILD_EXTRA_SYMBOLS=""
fi
```
5. install阶段则只需要额外关心 depmod 的名称
```
install -m 644 -D obj/%{flavor_dir $flavor $bflavor}/depmod.conf $RPM_BUILD_ROOT/etc/depmod.d/\
    %{kmod_depmod_order}-%{kmod_name}%{bflavor2str $bflavor}.conf
```

