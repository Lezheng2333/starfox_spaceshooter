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

[Download v1.2.16](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.16) (latest)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.16)

- **角色对话音效！** Ally、Bryssa、Tower 三位战友现在各有专属打字音效——Ally (ai copilot) 是清脆的高频正弦 blip，像机载AI在耳机里滴滴答答；Tower (ai) 是低沉的中低频 noise sweep，像塔台无线电的沙沙声；Bryssa from Tower 是中高频方波 sweep，人类通讯官的声音温暖而有力量。开幕旁白的经典电报音效保持原样。暂停翻对话历史的时候不会再听到音效——只有实时的游戏内对话才会出声。
- **对话历史即时写入！** 以前要等对话完全上浮消失才能翻看历史，现在对话一开始弹出就立刻写进暂停页历史日志——暂停随时翻，一句都不会漏。
- **Boss 震爆音量修复！** 二阶段震动音效和 Boss 爆炸音效之前过于震耳且不受音量设置约束，现已全面调低——震动音量从 0.22 降到 0.12、时长缩短、触发间隔放宽；三层爆炸音效同步降低，整体音量和谐可控。

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
└── v2.0.0/                                     # 当前开发主线 (v1.2.16)
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
        └── Shooter ver1.2.16.app/ + .zip                  # v1.2.16 (最新)
```

## License

MIT
