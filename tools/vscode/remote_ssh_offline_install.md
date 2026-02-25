
## 离线安装remote ssh

1. 下载最新的vscode:

   ![vscode_exe_download](pic/vscode_exe_download.png)

   下载链接中包含commit id， 记住该commit id

   > [!NOTE]
   >
   > e.g., 下载链接为:
   >
   > ```
   > https://vscode.download.prss.microsoft.com/dbazure/download/stable/072586267e68ece9a47aa43f8c108e0dcbf44622/VSCode-darwin-universal.dmg
   > ```
   >
   > commit id为:
   >
   > ```
   > 072586267e68ece9a47aa43f8c108e0dcbf44622
   > ```

2. 下载cli文件

   ![vscode_cli](pic/vscode_cli.png)

   > [!NOTE]
   >
   > 由于我的远程机器为 x86 所以这里下载x86架构的cli文件

   下载后的文件名为: `vscode_cli_alpine_x64_cli.tar.gz`

3. 下载 stable 文件

   ```
   export COMMIT_ID=072586267e68ece9a47aa43f8c108e0dcbf44622
   wget https://update.code.visualstudio.com/commit:$COMMIT_ID/server-linux-x64/stable
   ```

   commit id 为 vscode app 的commit id

4. 文件下载完成，将上面文件copy到远端服务器

5. 执行下面脚本
   * cli:

     ```sh
     export COMMIT_ID=072586267e68ece9a47aa43f8c108e0dcbf44622
     mkdir -p ~/.vscode-server/cli/servers/Stable-${COMMIT_ID}/server
     tar xf vscode_cli_alpine_x64_cli.tar.gz
     chmod +x code
     mv code ~/.vscode-server/code-${COMMIT_ID_ID_ID_ID}
     ```

   * stable:

     ```sh
     mkdir /tmp/tmp_dir
     cp stable /tmp/tmp_dir
     cd /tmp/tmp_dir
     mv stable stable.tar.gz
     tar xf stable_tar.gz
     cd vscode-server-linux-x64/
     cp -r vscode-server-linux-x64/*  ~/.vscode-server/cli/servers/Stable-${COMMIT}/server
     cd ~
     rm -rf /tmp/tmp_dir
     ```

执行完上述步骤后, 目录如下:

```sh
.vscode-server/
[root@A06-R08-I134-73-919XB72 ~]# tree .vscode-server/ -L 5
├── cli
│   └── servers
│       └── Stable-072586267e68ece9a47aa43f8c108e0dcbf44622
│           └── server
│               ├── bin
│               ├── extensions
│               ├── LICENSE
│               ├── node
│               ├── node_modules
│               ├── out
│               ├── package.json
│               └── product.json
└── code-072586267e68ece9a47aa43f8c108e0dcbf44622
```

## 参考链接

[离线安装vscode remote ssh](https://zhuanlan.zhihu.com/p/7782586456)
