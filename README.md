# STAR FOX SPACE SHOOTER

> 星际火狐 极简版 — A 2D perspective space shooting game built with C++ and SDL2.

## Gameplay / 玩法

Control your starfighter in perspective space, shoot down alien ships, survive waves of enemies, and defeat the boss.

在透视空间中操控星际战机，击落外星飞船，在敌潮中生存，击败关底 Boss。

| Key / 按键 | Action / 操作 |
|---|---|
| W/A/S/D | Move / 移动 |
| Space / 空格 | Shoot / 射击 |
| ESC | Pause / 暂停 |

## Download / 下载

> macOS 11.0+

[Download v1.2.7](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.7) (latest)

[Download v1.2.5](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.5)

[Download v1.2.4](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.4)

[Download v1.2.2](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.2)

[Download v1.1.0](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.1.0)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.7)

- **第二章：新增普通外星飞船** — 从画面边缘突袭入场，躲在隐形墙后方朝你发射水晶子弹，打爆后第一章同款爆炸特效。
- **玩家受伤无敌** — 被子弹打中后有 1 秒无敌时间，飞机会闪烁，不会再被连续命中。
- **画面右上角信息栏重做** — 分数靠右对齐，血条和能量条整齐排列，不再乱糟糟。
- **代码大整理** — 给第一章和第二章的代码分别加了 Ch1 / Ch2 前缀，理清了继承关系，为后面章节铺路。

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
└── v2.0.0/                                     # 当前开发主线 (v1.2.7)
    ├── space_shooting ver2.0.0 developing.cpp              # 当前开发源码
    ├── space_shooting ver2.0.0 deprecated.cpp              # 原始代码 (已废弃)
    └── Release Version/                          # 发布版本
        ├── Shooter ver1.1.0.app/ + .zip                   # v1.1.0
        ├── Shooter ver1.2.2.app/ + .zip                   # v1.2.2
        ├── Shooter ver1.2.4.app/ + .zip                   # v1.2.4
        ├── Shooter ver1.2.5.app/ + .zip                   # v1.2.5
        └── Shooter ver1.2.7.app/ + .zip                   # v1.2.7 (最新)
```

## License

MIT
