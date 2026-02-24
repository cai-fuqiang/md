## ranger 工具使用

### keymap

* 当前目录移动和 vim 相同, <kbd>j</kbd>,
  <kbd>e</kbd>,
  <kbd>k</kbd>,
  <kbd>l</kbd>
* 在父目录移动
  * <kbd>[</kbd> : 父目录向上移动
  * <kbd>]</kbd> : 父目录向下移动
* <kbd>g</kbd>: goto
  * <kbd>h</kbd>: cd /home
  * <kbd>gn</kbd>: 创建一个新的tab
  * <kbd>gt</kbd>: 切换到后一个tab
  * <kbd>gT</kbd>: 切换到前一个tab
  * <kbd>gc</kbd>: tab close
* <kbd>o</kbd>: soft(略)
* <kbd>shift</kbd>: 历史记录跳转（没用明白)
  * <kbd>shift + h</kbd>: 历史记录中向上跳转
  * <kbd>shift + l</kbd>: 历史记录中向下跳转
* 显示隐藏文件<kbd>zh</kbd>, <kbd>ctrl+h</kbd>
* 当ranger切换到一个目录想进入这个目录
  * <kbd>shift + s</kbd>: 进入选中目录，这相当于新启动了一个shell
  * <kbd>ctrl + d</kbd>:  杀死shell, 退回ranger
* <kbd>y</kbd>: copy
  * <kbd>yp</kbd>: ranger终端复制选中文件绝对路径
* rename
  * <kbd>a</kbd>: 在文件名后缀前加入光标，例如: `a.vim` -- `a` + `光标` + `.vim`
  * <kbd>m</kbd>: 在文件名最后加入光标, 例如 `a.vim` -- `a.vim` + `光标`
* 选中
  * <kbd>space</kbd>: 有个场景是选中后，使用bulkrename 可以使用`vim`来重命名
    批量文件
* 复制粘贴 剪切删除(和vim类似)
  * <kbd>yy</kbd>: 复制
  * <kbd>dd</kbd>: 剪切
  * <kbd>dD</kbd>: 删除
  * <kbd>pp</kbd>: 粘贴
  * <kbd>po</kbd>: 粘贴，如果目标目录有同名文件，则覆盖
* 查看目录中文件大小
  * <kbd>du</kbd>: 不过和mac不匹配，因为其默认执行`du -h --max-depth xxx`, mac没有
    这个参数，应该改为`du -h -d xxx`  `TOOD`
* 任务管理器: 例如copy一个大文件，这个任务会后台执行，可以通过任务管理器管理这个任务的
  优先级，以及启停
  * <kbd>w</kbd>: 进入任务管理器
  * <kbd>w/q</kbd>: 在任务管理器中执行这些快捷键，则退出任务管理器
  * <kbd>dd</kbd>: 在任务管理器停止选中任务

* 解压，压缩 `TODO`:  视频<sup>1</sup>:`22:54` 时间左右.
* bookmarks(有了`fzm` plugin 拜拜了您)
  * <kbd>`</kbd>, <kbd>'</kbd>: 进入书签页面
  * <kbd>m</kbd>: 增加书签
  * <kbd>um</kbd>: 删除书签

## 配置

* 配置文件位置: `~/.config/ranger`
* 新建一个自定义的配置文件: `ranger --copy-config=all`
* 禁用默认的配置文件, 使用自己的配置文件
  * 配置环境变量: `RANGER_LOAD_DEFAULT_RC` -- `FALSE`

* `~/.config/ranger` 目录文件作用
  * `rc.conf`: 主配置文件
  * `commands.py`: 自定义命令
  * `rfile.conf` : 不同文件格式默认打开软件
  * `scope.sh`: 和预览有关

`rc.conf`非常有用的配置:

* `vcs_aware`: 显示git状态，推荐打开
* 预览图片

## 插件

* 插件安装方法
  * 一般是git clone to `~/.config/ranger/plugins`

### 具体插件

* [ranger-devicons2](https://github.com/cdump/ranger-devicons2): 显示文件图标
  * 评价: 好看，但是个人感觉比较鸡肋
* [ranger-fzf-marks](https://github.com/laggardkernel/ranger-fzf-marks): fzf 管理bookmark
  * 评价:  绝对的生产力神器

    原因是`ranger` 下的, 书签功能只支持单个字符不是很好用, 而fzf支持给目录打标签，并用
    fzf 过滤，扩展强的同时效率还高.

  * 命令
    * fmark <bookmark name>: 给当前路径增加一个标签
    * dmark <bookmark name>: 删除一个标签
    * fzm: 查找标签, 这个比较常用，设置快捷键为<kbd>ctrl + g</kbd>
* [ranger_tmux](https://github.com/joouha/ranger_tmux)
  * 搭配tmux 生产力!

## 官方提供的可选的命令

参考链接<sup>4</sup>, 添加命令的方式为在`commands.py`中增加`python`函数，
问了方便使用，一般会在`rc.conf`配置文件中添加快捷键

比较实用的功能

* [fzf-select](https://github.com/ranger/ranger/wiki/Custom-Commands#fzf-integration): 实用fzf搜索

## 参考链接

1. [bilibili:把你的终端变成逆天高效神器：Ranger终极配置方案](https://www.bilibili.com/video/BV1b4411R7ck/?spm_id_from=333.337.search-card.all.click&vd_source=7b6e9d67dce90e6019e7489e4a65411d)
2. [ranger github wiki](https://github.com/ranger/ranger/wiki)
3. [ranger github wiki -- plugins](https://github.com/ranger/ranger/wiki/Plugins)
4. [ranger github wiki -- custom commands](https://github.com/ranger/ranger/wiki/Custom-Commands)
5. [ranger github wiki -- image preview](https://github.com/ranger/ranger/wiki/Image-Previews)
