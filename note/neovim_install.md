# install vim-plug
https://github.com/junegunn/vim-plug

## install cmd
```
sh -c 'curl -fLo "${XDG_DATA_HOME:-$HOME/.local/share}"/nvim/site/autoload/plug.vim --create-dirs \
       https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
```
# install nodejs
yay -S nodejs

# install cscope
yay -S cscope

# install ctags
yay -S ctags

# dict
```
Plug 'iamcco/dict.vim'

" 有道翻译2
let g:api_key = "1932136763"
let g:keyfrom = "aioiyuuko"

"--普通模式下，<Leader>d 即可翻译光标下的文本，并在命令行回显
nmap <silent> <Leader>d <Plug>DictSearch
"--可视化模式下，<Leader>d 即可翻译选中的文本，并在命令行回显
vmap <silent> <Leader>d <Plug>DictVSearch
"--普通模式下，<Leader>w 即可翻译光标下的文本，并且在Dict新窗口显示
nmap <silent> <Plug>DictWSearch
"--可视化模式下，<Leader>w 即可翻译选中的文本，并且在Dict新窗口显示
vmap <silent> <Plug>DictWVSearch
""--普通模式下，<Leader><Leader>r 即可翻译光标下的单词，并替换为翻译的结果
"nmap <silent> <Leader><Leader>r <Plug>DictRSearch
""--可视化模式下，<Leader><Leader>r 即可翻译光标下的单词，并替换为翻译的结果
"vmap <silent> <Leader><Leader>r <Plug>DictRVSearch
```
