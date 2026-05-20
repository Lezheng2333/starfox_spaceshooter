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

[Download v1.2.13](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.13) (latest)

[Download v1.2.12](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.12)

[Download v1.2.11](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.11)

[Download v1.2.10](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.10)

[Download v1.2.9](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.9)

[Download v1.2.8](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.8)

[Download v1.2.7](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.7)

[Download v1.2.5](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.5)

[Download v1.2.4](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.4)

[Download v1.2.2](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.2)

[Download v1.1.0](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.1.0)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.13)

- **暂停页对话历史** — 暂停时右半显示已触发对话日志，可上下滚动回顾，焦点移动+边界滚动。
- **对话系统完善** — Ally/Bryssa/Tower 三角色，8个分数触发点，开场对话结束后敌人才刷新。

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
        ├── Shooter ver1.2.12.app/ + .zip                  # v1.2.12
        └── Shooter ver1.2.13.app/ + .zip                  # v1.2.13 (最新)
```

## License

MIT
