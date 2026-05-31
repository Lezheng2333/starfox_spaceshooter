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

[Download v1.2.19](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.19) (latest)
	
	[Download v1.2.18](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.18)

	[Download v1.2.17](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.17)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.19)

- **自动出敌系统！** 球体Boss动画结束后，普敌自动刷新——Wave 1三只→Wave 2五只→Wave 3初始五只+四轮增援（每消灭3只补3只）→最终弹幕敌人。敌人飞出画面立刻补充，波次用分数门槛判定，节奏紧凑不中断。
- **第二章测试模式双入口！** TEST→CHAPTER 2新增两个入口：SPHERE BOSS FULL观看完整Boss动画+后续战斗，COMBAT ONLY跳过Boss直接进入出敌波次，和第一章测试入口一样方便。
- **Ch2训练机上线！** 第二章当前使用横版训练机（与第一章同款三角机身旋转侧视角），单发基础射击，NightElf和能量条系统暂搁置后续激活。

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
└── v2.0.0/                                     # 当前开发主线 (v1.2.19)
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
        └── Shooter ver1.2.19.app/ + .zip                  # v1.2.19 (最新)
```

## License

MIT
