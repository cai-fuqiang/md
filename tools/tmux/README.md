# tmux 工具使用

## 插件

### 还未调研但是想用的插件

#### gernal

* [ ] tmux-fzf
* [X] [tmux-gentrify](https://github.com/kristopolous/tmux-gentrify):
     用来在不同windows之间剪切粘贴panes
  * <kbd>ctrl+x</kbd> 剪切(默认是<kbd>alt+x</kbd>)
  * <kbd>ctrl+v</kbd> 粘贴(默认是<kbd>alt+x</kbd>)
* [X] [tmux-pane-focus](https://github.com/graemedavidson/tmux-pane-focus)
  * 用来自动调整force pane 的大小
  * 目前配置只调整高度，调整高度为70%(先用用看)
* [X] [Tmux-Power-Zoom](https://github.com/jaclu/tmux-power-zoom)
  * 强力缩放功能，正常情况下<kbd>z</kbd>用来在当前windows中缩放,但是该插件
    允许缩放到另外一个窗口，并且可以取消缩放
  * 个人使用评价: 还可以，先用着
* [ ] [tmux-which-key](https://github.com/alexwforsythe/tmux-which-key)
  * 个人认为是强有力的生产力插件。待详细研究
  * TODO

#### copy

* [X] [extrakto](https://github.com/laktak/extrakto)
  * 用来负责粘贴目前buffer(可能是buffer哈) 中的文本
  * 我只能说, 有点无敌, 之前一直使用vim模式复制粘贴，今天多了一种选择，如果
    再有一个窗口能够显示(甚至简单点，搞个透明度降低也行否则要copy的文本还得记忆下),
    这些片段在原有text中的位置，那就是无敌于天下
  * 使用方式:
    * <kbd>Tab</kbd>  打开 `extrakto`窗口
    * extrakto窗口中
      * <kbd>ctrl+l,j</kbd> 向上，向下移动
      * <kbd>enter</kbd>: 复制
      * <kbd>tab</kbd>: 直接复制粘贴到当前的pane中(我尼玛一个字 --- 爽爽爽)
* [X] [tmux-fastcopy](https://github.com/abhinav/tmux-fastcopy)
  * 使用easymotion方式改善复制体验
  * 上面说爽说早了, 对于习惯使用vim easymotion 插件的我来说，这个是真的爽
  * 安装:
    * 除了用tmux-plugin安装插件外，还需要安装其他工具
    * mac: `brew install abhinav/tap/tmux-fastcopy`
  * 不过，mac好像有bug，暂时先用fzf吧

### 不考虑安装的插件

* [tmux-menus](https://github.com/jaclu/tmux-menus):
  * 管理 tmux 环境的弹出菜单
  * 和`which-key` 冲突了, 暂不使用

# 参考链接

1. [tmux plugins list](https://github.com/tmux-plugins/list)
