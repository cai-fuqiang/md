## seah

### 安装

1. mac : brew install sesh

### Shell Completion

* fish

```sh
sesh completion fish > ~/.config/fish/completions/sesh.fish
source ~/.config/fish/config.fish
```

### tmux + sesh

配置

```
bind-key "s" run-shell "sesh connect \"$(
  sesh list --icons | fzf-tmux -p 80%,70% \
    --no-sort --ansi --border-label ' sesh ' --prompt '⚡  ' \
    --header '  ^a all ^t tmux ^g configs ^x zoxide ^d tmux kill ^f find' \
    --bind 'tab:down,btab:up' \
    --bind 'ctrl-a:change-prompt(⚡  )+reload(sesh list --icons)' \
    --bind 'ctrl-t:change-prompt(🪟  )+reload(sesh list -t --icons)' \
    --bind 'ctrl-g:change-prompt(⚙️  )+reload(sesh list -c --icons)' \
    --bind 'ctrl-x:change-prompt(📁  )+reload(sesh list -z --icons)' \
    --bind 'ctrl-f:change-prompt(🔎  )+reload(fd -H -d 2 -t d -E .Trash . ~)' \
    --bind 'ctrl-d:execute(tmux kill-session -t {2..})+change-prompt(⚡  )+reload(sesh list --icons)' \
    --preview-window 'right:55%' \
    --preview 'sesh preview {}'
)\""
```

### configuration

创建默认的配置文件

```
mkdir -p ~/.config/sesh && touch ~/.config/sesh/sesh.toml
```

> [!NOTE]
>
> 执行命令时, 可以通过 `-C`, 或者 `--config`指定自定义配置文件路径
>
> ```
> sesh -C /path/to/custom/sesh.toml list
> sesh --config /path/to/custom/sesh.toml connect my-session
> ```

`sesh.toml` 中可以指定section 的显示顺序:

```sh
sort_order = [
    "tmuxinator", # show first
    "config",
    "tmux",
    "zoxide", # show last
]
```

也可以只关心特定会话的类型顺序:

```

```
