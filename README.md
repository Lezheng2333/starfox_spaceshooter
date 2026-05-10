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

[Download v1.1.0](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.1.0)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

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
└── v2.0.0/                                     # 当前开发主线 (v1.1.0+)
    ├── space_shooting ver2.0.0 developing.cpp              # 当前开发源码
    ├── space_shooting ver2.0.0 developing (deprecated).cpp  # 原始代码 (已废弃)
    ├── Shooter ver1.1.0.app/                               # 最新发布版
    └── Shooter ver1.1.0.zip
```

## License

MIT
