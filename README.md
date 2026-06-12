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

[Download v1.2.20](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.20) (latest)

    [Download v1.2.19](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.19)
	
	[Download v1.2.18](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.18)

	[Download v1.2.17](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.17)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.20)

- **脉冲技能上线！** 击败弹幕敌人后会掉落一个包裹着白色几何护罩的发光技能球——打掉护罩，按住 Shift 在它旁边吸收能量，等绿色能量条涨满，松开 Shift 再按一下，全屏白色冲击波清空弹幕、灼伤敌人。从拿到技能球到按下第一次冲击波，每一步都有音效和粒子反馈。
- **技能球获取流程：** 漂浮 → 射击18次打碎护罩 → 按住 Shift 吸收金色核心（5秒，能量条同步从空到满）→ 松手再按释放。中途松手能量会快速流失，安全了再回来吸就行。
- **测试模式新入口 PULSE ORB TEST：** 直接进入弹幕敌人+25分场景，立刻测试技能球全流程。

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
└── v2.0.0/                                     # 当前开发主线 (v1.2.20)
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
        ├── Shooter ver1.2.14.app/ + .zip                  # v1.2.14
        ├── Shooter ver1.2.15.app/ + .zip                  # v1.2.15
        ├── Shooter ver1.2.16.app/ + .zip                  # v1.2.16
        ├── Shooter ver1.2.17.app/ + .zip                  # v1.2.17
        ├── Shooter ver1.2.18.app/ + .zip                  # v1.2.18
        ├── Shooter ver1.2.19.app/ + .zip                  # v1.2.19
        └── Shooter ver1.2.20.app/ + .zip                  # v1.2.20 (最新)
```

## License

MIT
