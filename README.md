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

[Download v1.2.18](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.18) (latest)
	
	[Download v1.2.17](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.17)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.18)

- **球体Boss战来了！** 第二章新增巨型蓝色球体Boss——由600多颗六角密铺菱形组成，从右侧滚动入场，从下到上蓝变橙逐个激活，受击后菱形碎片飞溅散落，HP归零后碎裂→震动→突袭飞散全流程华丽动画。测试模式直接选CHAPTER 2即可体验。
- **夜精灵能量条系统！** 命中敌人积蓄白色能量条，50次命中蓄满后自动激活15秒三炮火力全开模式，屏幕右上角两道能量条一绿一白清晰显示状态，最后3秒加速蜂鸣倒计时。
- **对话系统防级联修复！** 修复了对话触发后连续弹出一串的问题，现在每个触发点严格只触发一次，节奏清爽。
- **细节修复：** 暂停页对话历史方向键首次按就生效；开始/章节选择/设置/任务完成等所有菜单页统一静音；NEXT CHAPTER 直接进入下一章再也不跳回主菜单。

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
└── v2.0.0/                                     # 当前开发主线 (v1.2.18)
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
        └── Shooter ver1.2.18.app/ + .zip                  # v1.2.18 (最新)
```

## License

MIT
