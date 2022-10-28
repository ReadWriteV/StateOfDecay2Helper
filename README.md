# Stata of Decay 2 Helper

游戏 **腐烂国度2(State of Decay 2)** 的小工具。

特点：

+ 自动摇人，摇到想要的特质（e.g. 不灭、血疫幸存者）
+ 单个程序，无需安装，不会写入系统信息
+ 支持 V30 版本之后 UI 和之前的 UI

## 构建

``` bash
cmake -S . -B build

cmake --build build
```

## 用法

目前我还不知道怎么编写程序在全屏程序上方，所以只能将 腐烂国度2(State of Decay 2) 启动后设置为无边框窗口全屏，如果是全屏则遮盖住本窗口，无法使用。

启动 腐烂国度2(State of Decay 2) 进入摇人界面后，点击小工具中的 START 按钮，然后把鼠标放在想要摇人的那个槽位，就可以了。暂停可以通过点击 Pause 按钮，或者按 ESC 键。要是想退出，先暂停摇人，再点击 CLOSE 退出，否则可能会卡住。

## Qt 版本
Qt 版本是早期版本，在 **Qt-version** 分支，只支持 V30 版本之前的 UI。
