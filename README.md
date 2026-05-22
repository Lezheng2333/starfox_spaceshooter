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

[Download v1.2.17](https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v1.2.17) (latest)

Download the `.zip`, unzip, and run `Shooter.app`.

下载 `.zip` 后解压，直接运行 `Shooter.app`。

## What's New / 最近更新 (v1.2.17)

- **结束页面全新设计！** Boss 击败后的 MISSION COMPLETE 现在用旁白风格墨绿文本框优雅呈现，两个选项居中排列——NEXT CHAPTER 一键直达下一章开幕旁白，BACK TO MAIN MENU 返回主菜单。光标是醒目的黄色下划线，再也不会找不到焦点在哪。
- **第一章透视飞行感！** 战机移动速度现在随高度变化——贴地平线飞的时候速度自然放慢（~0.7x），俯冲到画面下端时速度加快（~1.3x），中间高度保持默认速度。越靠近地平线越像在远处巡航，越靠近下端越有高速掠过地面的感觉。
- **第二章开幕旁白完整重写！** 12 页氛围感旁白，从惨胜后的基地废墟、老旧教练机疲惫的引擎震颤、太空的死寂，到 Moonwell 研发中心门前那扇沉默的真空门——还没开打就已经沉浸在故事里了。
- **开始/暂停页面静音！** 开始画面和暂停画面不再播放 BGM，耳根清净。同时预留了 setStartBgm() 接口，等你想给它配上专属音乐。
- **细节打磨：** 暂停页右半区新增 HISTORY DIALOGUE 标题；对话单行收窄到 20 字符，信息更紧凑；字体库补齐小写 q（p 的完美镜像）；第二章背景玻璃不再在画面内闪现刷新。

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
└── v2.0.0/                                     # 当前开发主线 (v1.2.17)
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
        └── Shooter ver1.2.17.app/ + .zip                  # v1.2.17 (最新)
```

## License

MIT
