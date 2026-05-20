# STAR FOX SPACE SHOOTER

> 星际火狐 极简版 — A 2D space shooting game built with C++ and SDL2.

## Gameplay / 玩法

Control your starfighter in a massive space, shoot down alien ships, survive waves of enemies, and defeat the boss.

在无垠空间中操控星际战机，击落外星飞船，在敌潮中生存，击败关底 Boss。

| Key / 按键 | Action / 操作 |
|---|---|
| W/A/S/D | Move / 移动 |
| Space / 空格 | Shoot / 射击 |
| ESC | Pause / 暂停 |

## Download / 下载

> macOS 11.0+

[Download v1.2.14](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.14) (latest)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.14)

- **全新开幕旁白！** 第一章现在有一段完整的开场故事——Martha通过最终考核成为Huntress，基地突然遇袭，主力战机全毁，她开着一架老教练机孤身升空。打字机特效逐字展开，Mario式弹性弹出动画，暗绿色夜精灵风格文本框，滴滴答答的电报音效——还没开打就已经燃起来了！按ENTER推进剧情，按ESC直接跳过开打，你说了算。
- **实时队友对话系统！** Ally、Bryssa、Tower 三位战友全程在线，8个关键分数点精准触发——刚起飞有人喊加油，3分有人教你打能量体，15分塔台恢复通讯，20分防御炮就位……打到61分还有新对话！名字标签+暗绿文字在画面左侧浮现，打完自动上浮消失，完全不挡视线。重点是——**暂停菜单里能翻对话历史了！** 按暂停切到右半区，所有已经触发的对话一字排开，上下滚动回看，焦点移动顺滑得像翻聊天记录，长句子自动折行整整齐齐。漏了什么话？回来翻就是了！
- **代码重构 + Bug 修复！** 臃肿的旁白系统已拆成三个精悍小队（NarrationSystem / DialogueSystem / DialogueHistory），各司其职，后续加对话更容易。Boss 蓝色追踪弹不会再抽风变成白色横弹了。所有菜单光标都不会再鬼畜回绕——到顶就停，原路返回。

## Build from Source / 从源码编译

```bash
clang++ -std=c++11 -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
  -D_THREAD_SAFE "space_shooting ver2.0.0 developing.cpp" -o shooter \
  -L/opt/homebrew/lib -lSDL2
```

Requires SDL2: `brew install sdl2`

## Project Structure / 项目结构

```
├── README.md
├── DEVELOPMENT_LOG.md
├── v1.0.0/                                     # v1.0.0 稳定归档
│   ├── space_shooting ver1.0.0.cpp
│   ├── Shooter ver1.0.0.app/
│   └── Shooter ver1.0.0.zip
└── v2.0.0/                                     # 当前开发主线 (v1.2.10)
    ├── space_shooting ver2.0.0 developing.cpp              # 当前开发源码
    ├── space_shooting ver2.0.0 deprecated.cpp              # 原始代码 (已废弃)
    └── Release Version/                          # 发布版本
        ├── Shooter ver1.1.0.app/ + .zip                   # v1.1.0
        ├── Shooter ver1.2.2.app/ + .zip                   # v1.2.2
        ├── Shooter ver1.2.4.app/ + .zip                   # v1.2.4
        ├── Shooter ver1.2.5.app/ + .zip                   # v1.2.5
        ├── Shooter ver1.2.7.app/ + .zip                   # v1.2.7
        ├── Shooter ver1.2.8.app/ + .zip                   # v1.2.8
        ├── Shooter ver1.2.9.app/ + .zip                   # v1.2.9
        ├── Shooter ver1.2.10.app/ + .zip                  # v1.2.10
        ├── Shooter ver1.2.11.app/ + .zip                  # v1.2.11
        └── Shooter ver1.2.14.app/ + .zip                  # v1.2.14 (最新)
```

## License

MIT
