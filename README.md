# STAR FOX SPACE SHOOTER

> 星际火狐 极简版 — A 2D space shooting game built with C++ and SDL2.

## Gameplay / 玩法

Control your starfighter in a massive space, shoot down alien ships, survive waves of enemies, and defeat the boss.

在无垠空间中操控星际战机，击落外星飞船，在敌潮中生存，击败关底 Boss。

| Key / 按键 | Action / 操作 |
|---|---|
| W/A/S/D | Move / 移动 |
| Space / 空格 | Shoot / 射击 |
| Shift | Pulse / 脉冲冲击波（获得技能球后） |
| ESC | Pause / 暂停 |

暂停菜单：`RESUME` / `SAVE GAME` / `LOAD GAME` / `RESTART` / `OPTIONS` / `BACK TO MAIN MENU` / `EXIT`

任意时刻 ESC 暂停即可存档；下次启动在主菜单选 `LOAD GAME` 读档，画面会停在你存档那一刻（连飞行中的子弹与弹幕都原样恢复），按 `RESUME` 倒计时 3-2-1 后继续战斗。

Save at any moment from the pause menu, then pick `LOAD GAME` from the main menu next time — the game resumes on the exact frame you saved (bullets in flight included).

## Download / 下载

> macOS 11.0+

[Download v1.2.23](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.23) (latest)

[Download v1.2.21](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.21)

[Download v1.2.20](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.20)

[Download v1.2.19](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.19)

[Download v1.2.18](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.18)

[Download v1.2.17](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.17)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.23)

- **存档功能上线！** 以前想接着玩只能用测试模式跳关，现在随时 ESC 暂停 → `SAVE GAME` 存档，关掉游戏下次从主菜单 `LOAD GAME` 读档，回到你存档的那一瞬间——画面就是你按下暂停时的那一帧，连满天飞的弹幕、敌人位置、粒子特效、正在播放的对话都原样恢复，按 `RESUME` 倒计时 3-2-1 后继续战斗。
- 存档槽位：`AUTO` 自动槽 + `SLOT 1-6`（列出章节/流程段落/分数/血量/时间）。每章开始与第二章每次段落切换会自动存档，忘了手动存也不会丢进度。
- 还能用内置文件浏览器**自由读档**（浏览电脑上任意 `.sav`），或把存档**另存到任意文件夹**，方便备份某个精彩瞬间。
- 原 `TEST` 测试模式从主菜单退役（保留为开发参数 `./shooter --test`），位置让给 `LOAD GAME`。
- `OPTIONS` 里的瞄准辅助、语音语言、音量与 EQ 现在会被记住。
- 本版同时包含上一版未发布的**第二章完整流程**（门禁真空门序列、中央研究室、章节 Boss「MOONWELL WARDEN」、结局旁白）与**中英双语配音功能**（`OPTIONS → VOICE LANG` 切换；本发布包暂未内置音频文件，运行时自动降级为字母弹出音效）。

## Build from Source / 从源码编译

```bash
cd "v2.0.0/multiple code files" && make
```
或手动：
```bash
cd "v2.0.0/multiple code files" && clang++ -std=c++11 -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
  -D_THREAD_SAFE main.cpp -o shooter -L/opt/homebrew/lib -lSDL2
```

Requires SDL2: `brew install sdl2`

开发用运行参数：

```bash
./shooter              # 正常游玩
./shooter --test       # 主菜单显示隐藏的 TEST 入口（跳关/预设状态调试）
./shooter --selftest   # 存档系统自检（无窗口，0=PASS）
```

## Project Structure / 项目结构

```
├── README.md
├── DEVELOPMENT_LOG.md
├── CLAUDE.md
├── v1.0.0/                                            # v1.0.0 稳定归档
│   ├── space_shooting ver1.0.0.cpp
│   ├── Shooter ver1.0.0.app/
│   └── Shooter ver1.0.0.zip
└── v2.0.0/                                            # 当前开发主线 (v1.2.23)
    ├── multiple code files/                            # ★ 当前开发源码（多文件架构）
    │   ├── main.cpp                                    # 入口 + #include 所有头文件
    │   ├── game.h                                      # Game 类 (3607 行)
    │   ├── save_system.h  save_menu.h                  # 存档格式/IO + 存读档界面
    │   ├── types.h  constants.h                        # 数据结构和常量
    │   ├── font.h  renderer.h  audio.h                 # 共享基础设施
    │   ├── player.h  bullets.h  particles.h            # 玩家/子弹/粒子
    │   ├── dialogue.h  narration.h  aim_assist.h       # 对话/旁白/瞄准
    │   ├── floating_text.h  chapter_manager.h  ui.h    # 浮动文字/章节/UI
    │   ├── ch1/                                        # Ch1 系统 (4 个文件)
    │   │   ├── ch1_shockwave.h  ch1_aliens.h
    │   │   ├── ch1_boss.h  ch1_background.h
    │   └── ch2/                                        # Ch2 系统 (11 个文件)
    │       ├── ch2_background.h  ch2_shooter_base.h
    │       ├── ch2_danmaku.h  ch2_aliens.h
    │       ├── ch2_sphere_boss.h  ch2_hud.h
    │       ├── ch2_skill_orb.h  ch2_pulse.h
    │       ├── ch2_gate.h  ch2_lab.h  ch2_boss.h
    │       └── Makefile
    ├── space_shooting ver1.2.20 single-file archive.cpp # v1.2.20 单文件存档
    ├── space_shooting ver2.0.0 deprecated.cpp           # 原始代码 (已废弃)
    └── Release Version/                                 # 发布版本
        ├── Shooter ver1.1.0.app/ + .zip                 # v1.1.0
        ├── Shooter ver1.2.2.app/ + .zip                 # v1.2.2
        ├── Shooter ver1.2.4.app/ + .zip                 # v1.2.4
        ├── Shooter ver1.2.5.app/ + .zip                 # v1.2.5
        ├── Shooter ver1.2.7.app/ + .zip                 # v1.2.7
        ├── Shooter ver1.2.8.app/ + .zip                 # v1.2.8
        ├── Shooter ver1.2.9.app/ + .zip                 # v1.2.9
        ├── Shooter ver1.2.10.app/ + .zip                # v1.2.10
        ├── Shooter ver1.2.11.app/ + .zip                # v1.2.11
        ├── Shooter ver1.2.14.app/ + .zip                # v1.2.14
        ├── Shooter ver1.2.15.app/ + .zip                # v1.2.15
        ├── Shooter ver1.2.16.app/ + .zip                # v1.2.16
        ├── Shooter ver1.2.17.app/ + .zip                # v1.2.17
        ├── Shooter ver1.2.18.app/ + .zip                # v1.2.18
        ├── Shooter ver1.2.19.app/ + .zip                # v1.2.19
        ├── Shooter ver1.2.20.app/ + .zip                # v1.2.20
        ├── Shooter ver1.2.21.app/ + .zip                # v1.2.21
        └── Shooter ver1.2.23.app/ + .zip                # v1.2.23 (最新)
```

## License

MIT
