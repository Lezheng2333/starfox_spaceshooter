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

[Download v1.2.15](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.15) (latest)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.15)

- **对话系统大爆发！** 第一章新增 13 个分数触发点（55/70/80/90/105/120/160/180/195/210），从 Ally 的系统自检、塔台重新失联，到 Bryssa 侦测到 Telamondo 级主力舰逼近——完整叙事弧线一气呵成，打到 210 分还在推进剧情！Boss 半血进二阶段时更有专属对话弹出："Telamondo can absorb energy!"，战斗信息无缝融入对话系统。
- **角色身份升级！** Ally、Bryssa、Tower 三人现在带有身份标签——Ally (ai copilot) 是机载AI副驾驶，Tower (ai) 是基地塔台AI，Bryssa from Tower 是塔台的人类通讯官。三人的声音层次分明，不再只是三个名字。
- **中文剧本同步！** 所有新增对话的中英文版本完整写入旁白脚本（narration_scripts.txt），策划文案一步到位，后续版本直接取用。
- **测试模式历史预填充！** 从任意分数进入测试模式，低于该分数的所有对话自动写入暂停页历史日志。选 180 分进入？前面 0~160 分的全部对话一字排开随便翻——开发调试和剧情回顾的神器。
- **章节守卫机制！** 第一章对话只在第一章触发，第二章从 0 分重新开始不会串戏。Boss 二阶段对话也归入统一触发区块，所有对话代码集中管理，不再散落各处。
- **细节打磨：** 开幕旁白 Flight code 更正为 21395；小写 g 字型重新设计，碗部饱满降部利落；triggeredScores 数组扩容到 256 迎接更高分。

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
└── v2.0.0/                                     # 当前开发主线 (v1.2.15)
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
        └── Shooter ver1.2.15.app/ + .zip                  # v1.2.15 (最新)
```

## License

MIT
