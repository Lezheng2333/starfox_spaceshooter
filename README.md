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

[Download v1.2.10](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.10) (latest)

[Download v1.2.9](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.9)

[Download v1.2.8](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.8)

[Download v1.2.7](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.7)

[Download v1.2.5](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.5)

[Download v1.2.4](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.4)

[Download v1.2.2](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.2)

[Download v1.1.0](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.1.0)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.10)

- **敌人互斥系统** — 弹幕敌人不再挤在一起，垂直分散出场，入场位置自动检测间距，振荡节奏各不相同。
- **碰撞逻辑优化** — 玩家同一帧只扣一滴血，NightElf 机头不再穿透隐形墙，受击反馈更公平。
- **一堆 Bug 修复** — 心心不减少、瞄准辅助遗漏弹幕敌人、倒计时粒子错位、测试模式飞机错乱、暂停菜单闪烁，全修了。
- **代码质量** — 消除全部编译器警告，重构重复代码，Game Over 界面优化。

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
        └── Shooter ver1.2.10.app/ + .zip                  # v1.2.10 (最新)
```

## License

MIT
