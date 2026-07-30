# 多文件重构评估报告 — Star Fox Space Shooter

> 评估日期：2026-06-13 | 源码版本：v1.2.20 | 当前单文件 6633 行

---

## 1. 量化指标

| 指标 | 数值 |
|---|---|
| 总行数 | **6633** |
| class 定义 | **21**（含继承 6 个） |
| struct 定义 | **18** |
| enum 定义 | **4** |
| 全局函数 (perspLeft/perspWidth) | **2** |
| #include 指令 | **7** |
| Game 类独占行数 | **2147**（32%） |
| 前向声明 | 21 |

## 2. 代码按系统分类

| 系统 | 行数 | 预估头文件 |
|---|---|---|
| **共享数据结构** (Star, FloatingText, BossConfig, ChapterConfig, FontChar, ActiveSound, MenuItem) | 137 | `types.h` |
| **Font** — 5x7 位图字体 | 145 | `font.h` / `font.cpp` |
| **Renderer** — SDL 渲染器封装 | 32 | `renderer.h` |
| **AudioEngine** — 实时音频合成 + BGM | 320 | `audio.h` / `audio.cpp` |
| **FloatingTextManager** | 47 | `floating_text.h` |
| **DialogueHistory** + **DialogueSystem** | 213 | `dialogue.h` / `dialogue.cpp` |
| **NarrationSystem** | 98 | `narration.h` / `narration.cpp` |
| **ParticleManager** | 139 | `particles.h` |
| **AimAssist** | 35 | `aim_assist.h` |
| **Player** → TrainingPlane / Ch2Trainer / NightElf / Druid | 236 | `player.h` / `planes.h` |
| **BulletManager** (Ch1+Ch2 子弹) | 178 | `bullets.h` |
| **Ch1 战斗系统** (ShockwaveMgr + AlienMgr + Boss + Background) | 930 | `ch1/` 目录下 4-5 个文件 |
| **Ch2 战斗系统** (SphereBoss + Background + ShooterBase + AlienMgr + DanmakuMgr) | 1154 | `ch2/` 目录下 5-6 个文件 |
| **Ch2 脉冲系统** (SkillOrb + PulseSystem) | 377 | `ch2/pulse.h` |
| **ChapterManager** | 67 | `chapter_manager.h` |
| **UIRenderer** | 52 | `ui_renderer.h` |
| **MenuStateMachine** | 109 | `menu.h` |
| **Game** | 2130 | `game.h` / `game.cpp` |
| **main()** | ~20 | `main.cpp` |

---

## 3. 搁置/休眠资产清单

这些是代码中**已定义但未激活**的内容，目前混在 6633 行中，容易被误删或忽略：

### 3.1 休眠类（代码完整，但从不实例化或调用）

| 资产 | 行号 | 状态 | 风险 |
|---|---|---|---|
| **Druid** (Ch3 飞机) | 1521-1559 | 未实例化，Game 类中无成员 | 高 — 后续章节需要，容易被当死代码删掉 |
| **NightElf** (Ch2 飞机) | 1464-1520 | 成员在 Game 中存在但从不设为 player 指针 | 高 — 当前搁置，门禁序列激活后使用 |
| **NightElfEnergy** (白能量条+三炮) | 3421-3484 | resetGame() 中 reset，update/draw 全移除 | 高 — NightElf 专属，届时联动激活 |
| **MenuStateMachine** | 4381-4490 | 成员 `menuSM` 存在于 Game 类，各 screen handler 未采用 | 中 — 设计为统一菜单系统，尚未全面接入 |

### 3.2 休眠视觉素材

| 资产 | 描述 |
|---|---|
| NightElf 飞机造型 | 30° 锐角机头 + 120° 燕尾 + 上半机身扫描线填充 |
| Druid 飞机造型 | 双尾翼 boomerang + 中心导航线 |
| NightElf 三炮布局 | Shell 未碎时 1 发，triple-fire 解封后 3 发（gunCount 返回 1 但设计是 3） |
| 白色能量条 HUD | 曾绘制在绿能量条下方（v1.2.18），已整体移除 |
| triple-on / triple-countdown 音效触发 | 逻辑已移除，音效函数仍保留 |
| Ch2 弹幕敌人螺旋弹幕 | `Ch2DanmakuManager` 完整可用，但 auto-spawn 只 spawn 了普敌 |
| Ch2 弹幕敌人入场位置 | 额外有右上/右下随机入口代码 |

### 3.3 休眠音频

| 函数 | 状态 |
|---|---|
| `sndTripleOn()` | 已定义，从未调用（触发代码已移除） |
| `sndTripleCountdown()` | 同上 |
| `sndCrystalCrush()` | 已定义，从未调用（计划为水晶子弹碎裂音效） |
| `sndBossHeal()` / `sndBossAbsorb()` | 已定义，仅 Ch1 Boss 调用，Ch2 未使用 |
| `sndBaseDamage()` | 已定义，从未调用 |
| `sndPlayerHit()` | 已定义，Ch2 无受击调用路径 |
| `setStartBgm()` | 空函数体（placeholder 注释） |
| BGM 音阶数据 (`bgmScale`, `bgmRhythm` 等) | ~70 行 BGM 数据 |

### 3.4 占位配置

| 位置 | 描述 |
|---|---|
| Chapters 3-5 配置 (行 4308-4313) | 全部复用 `bc1`（TELAMONDO），参数值占位 |
| Chapter 2 Boss 配置 (行 4303) | 同样复用 `bc1`，等待专门的章节 Boss 设计 |
| `setStartBgm(bool)` | 接口预留，函数体为空 |
| Ch2 绿色能量条占位符 | 现已激活（pulse 系统），非占位 |

### 3.5 未使用常量/字段

| 常量/字段 | 位置 |
|---|---|
| `Ch2SphereBoss::entryStartX` | 私有字段从未读取 |
| `Ch2SphereBoss::entryBgStopped` | 同上 |
| `ParticleManager::textScale` 局部变量 | 未使用（warning） |
| `NightElfEnergy::tripleTimer` / `tripleJustEntered` | 初始化顺序 warning |

---

## 4. 依赖分析

### 4.1 枢纽类

```
Game ───┬── Renderer, Font
        ├── AudioEngine
        ├── Background* (Ch1Background | Ch2Background*)
        ├── Player* (TrainingPlane | Ch2Trainer | NightElf)
        ├── BulletManager
        ├── ParticleManager (共享)
        ├── FloatingTextManager (共享)
        ├── ChapterManager
        ├── Ch1: Ch1AlienManager, Ch1ShockwaveManager, Ch1Boss
        ├── Ch2: Ch2AlienManager, Ch2DanmakuManager, Ch2SphereBoss
        ├── Ch2: Ch2PulseSystem, Ch2SkillOrb
        ├── Dialogue: NarrationSystem, DialogueSystem (→ DialogueHistory)
        ├── UI: UIRenderer, MenuStateMachine
        └── HUDBase (静态方法)
```

Game 类依赖所有子系统，但**子系统之间的交叉依赖有限**：

| 子系统 | 直接依赖 |
|---|---|
| `Ch2AlienManager` / `Ch2DanmakuManager` | `Ch2ShooterBase` → `BulletManager`, `ParticleManager`, `AudioEngine`, `Player` |
| `Ch1AlienManager` | `Ch1Alien`, `Ch1Boss` (引用), `ParticleManager`, `AudioEngine` |
| `Ch1Boss` | `Ch1AlienManager`, `ParticleManager`, `AudioEngine`, `FloatingTextManager` |
| `Ch2SphereBoss` | `ParticleManager`, `Ch2Background` (指针) |
| `Ch2PulseSystem` | `Ch2AlienManager`, `Ch2DanmakuManager`, `ParticleManager`, `AudioEngine` |
| `DialogueSystem` | `DialogueHistory` (组合), `Font` |
| `ChapterManager` | `BossConfig`, `ChapterConfig` |

### 4.2 全局依赖

- `perspLeft(y)` / `perspWidth(y)` — 两个全局函数，Ch1 透视计算，Ch1Alien、Ch1Boss、Ch1Background、TrainingPlane 共同使用
- `CENTER_X`, `WIN_WIDTH`, `WIN_HEIGHT`, `HORIZON_Y`, `WALL_Y` — 在 Ch1Background、Ch2Background 中分别定义，未统一
- `M_PI` — 通过 `<cmath>` 全局可用

### 4.3 Game 类中两个巨大函数

- `updateGameplay()` — 约 700 行，Ch1 + Ch2 分支在一个函数中
- `drawGameplayFrame()` — 约 500 行，同上

这两个函数是**拆分的核心难点**，因为它们混合了 Ch1/Ch2 的更新和绘制逻辑。

---

## 5. 构建系统

- 当前：单条 `clang++` 命令编译
- 无 Makefile / CMakeLists.txt
- 无条件编译 (#ifdef / #ifndef)
- 7 个系统头文件

拆分后需要一个极简的构建文件（Makefile 或 CMakeLists.txt），但只是 `clang++ *.cpp` 级别的复杂度。

---

## 6. 收益分析

| 维度 | 当前（单文件） | 拆分后（多文件） |
|---|---|---|
| **编译时间** | 必须全量编译，6633 行 | 增量编译，改一类只重编该文件 |
| **IDE 体验** | 严重卡顿、搜索/跳转慢 | 流畅，每个文件 < 500 行 |
| **资产可见性** | 休眠代码混在 6633 行中，容易误删 | 每个休眠类/素材独立文件，标题注释标注 `[DORMANT]` |
| **协作风险** | 改动一处可能破坏千里之外的代码 | 头文件接口清晰，改动影响面可控 |
| **新章节开发** | 必须继续往 Game::updateGameplay 里塞分支 | 新 Chapter 系统只需 include 头文件 |
| **git 冲突** | 两类功能在同一个文件→极易冲突 | 按系统分文件→独立并行修改 |

---

## 7. 风险分析

| 风险 | 等级 | 缓解措施 |
|---|---|---|
| **循环依赖** | 中 | 前向声明 + `#include` 只放在 .cpp 中 |
| **Ch1/Ch2 交叉污染** | 低 | 使用 `namespace ch1` / `namespace ch2` 或前缀 |
| **全局函数/常量冲突** | 低 | 放入命名空间或共用的 `constants.h` |
| **构建系统复杂度** | 低 | 7 个系统头文件 + Makefile 即可 |
| **Game 类仍会很胖** | 中 | `updateGameplay` 可拆为 Ch1/Ch2 两个独立方法 |
| **头文件顺序依赖** | 中 | 单一 `game.h` 做桥梁 include，保证顺序 |

---

## 8. 建议

### 8.1 拆分时机：**现在应该拆分**

理由：
- 6633 行已经严重影响开发效率（IDE 卡顿 + 搜索困难）
- 项目按章节推进，第二/三章及以后的开发会持续膨胀 Game 类
- 休眠资产混在长文件中，风险已经在 v1.2.19~1.2.20 的开发中出现（`key1Was`/`key2Was` 功能删了但变量忘了删，charge 机制改为单按 Shift 但死代码残留）
- 每次新增一个系统（脉冲、技能球）都要在同一个文件中找插入位置，容易出错
- 根据代码行业习惯，单文件超过 2000-3000 行通常建议拆分

### 8.2 建议的拆分方式：18 个头文件 + 1 个 Makefile

```
v2.0.0/
├── main.cpp                          # main() 入口 (~20行)
├── Makefile                          # 构建规则
├── game.h / game.cpp                 # Game 类 (当前 2147行→目标 800行)
│
├── types.h                           # 所有 struct 定义 (Star, FloatingText, BossConfig, ChapterConfig, FontChar, ActiveSound, MenuItem, BulletBase, Ch1Bullet, EnemyData, Ch1Alien, Ch1Shockwave, Ch1HealWave, Ch2EnemyBullet, Ch2PulseWave, Ch2DanmakuEnemy, Ch2Alien, Ch2AlienData, MenuKeys)
│
├── constants.h                       # 全局常量 (CENTER_X, WIN_WIDTH, WIN_HEIGHT, HORIZON_Y, WALL_Y, M_PI等)
│   └── persp_utils.h/persp_utils.cpp # perspLeft/perspWidth 透视函数
│
├── font.h / font.cpp                 # Font 类
├── renderer.h / renderer.cpp         # Renderer 类
├── audio.h / audio.cpp               # AudioEngine 类 + BGM 数据
├── floating_text.h                   # FloatingTextManager 类
├── particles.h / particles.cpp       # ParticleManager 类
├── aim_assist.h                      # AimAssist 组件
│
├── player.h / player.cpp             # Player 基类
├── planes.h / planes.cpp             # TrainingPlane, Ch2Trainer, NightElf [DORMANT], Druid [DORMANT]
│
├── dialogue.h / dialogue.cpp         # DialogueHistory + DialogueSystem
├── narration.h / narration.cpp       # NarrationSystem
│
├── bullets.h / bullets.cpp           # BulletManager
│
├── ch1/                              # Chapter 1 系统
│   ├── ch1_shockwave.h / .cpp        # Ch1ShockwaveManager
│   ├── ch1_aliens.h / .cpp           # Ch1AlienManager
│   ├── ch1_boss.h / .cpp             # Ch1Boss
│   └── ch1_background.h / .cpp       # Ch1Background
│
├── ch2/                              # Chapter 2 系统
│   ├── ch2_background.h / .cpp       # Ch2Background
│   ├── ch2_shooter_base.h            # Ch2ShooterBase 基类
│   ├── ch2_aliens.h / .cpp           # Ch2AlienManager
│   ├── ch2_danmaku.h / .cpp          # Ch2DanmakuManager
│   ├── ch2_sphere_boss.h / .cpp      # Ch2SphereBoss
│   ├── ch2_pulse.h / .cpp            # Ch2PulseSystem + Ch2PulseWave
│   └── ch2_skill_orb.h / .cpp        # Ch2SkillOrb + ShieldDebris
│
├── chapter_manager.h / .cpp          # ChapterManager
├── ui_renderer.h                     # UIRenderer
├── menu.h / menu.cpp                 # MenuStateMachine + ScreenType + Action enum
├── hud_base.h                        # HUDBase 静态绘制
│
└── nightelf_energy.h / .cpp          # NightElfEnergy [DORMANT — 等待NightElf激活]
```

### 8.3 拆分后的 Game 类结构

```cpp
// game.h
class Game {
    // 渲染 & 字体
    Renderer renderer;
    Font font;
    AudioEngine& audio;

    // 共享系统
    ParticleManager particleMgr;
    FloatingTextManager floatingTextMgr;
    BulletManager bulletMgr;
    ChapterManager chapterMgr;

    // 玩家
    TrainingPlane trainingPlane;
    Ch2Trainer ch2Trainer;
    NightElf nightElf;       // [DORMANT]
    Player* player;

    // Ch1
    Ch1AlienManager alienMgr;
    Ch1ShockwaveManager shockwaveMgr;
    Ch1Boss boss;
    Ch1Background* background;

    // Ch2
    Ch2Background* sideBg;
    Ch2AlienManager ch2AlienMgr;
    Ch2DanmakuManager dmMgr;
    Ch2SphereBoss sphereBoss; bool sphereBossActive;
    Ch2PulseSystem pulseSystem;
    Ch2SkillOrb skillOrb;
    NightElfEnergy nightElfEnergy; // [DORMANT]

    // 对话
    NarrationSystem narration;
    DialogueSystem dialogueSys;

    // 状态
    int score, ch2PlayerHP, playerHitCount;
    bool paused, gameOver;
    // ... (auto-spawn vars, test-mode vars, etc.)

    void updateGameplay(const Uint8* keys);
    void drawGameplayFrame();
    void updateCh1(const Uint8* keys);  // 原 updateGameplay 的 Ch1 分支
    void updateCh2(const Uint8* keys);  // 原 updateGameplay 的 Ch2 分支
    void drawCh1();                     // 原 drawGameplayFrame 的 Ch1 分支
    void drawCh2();                     // 原 drawGameplayFrame 的 Ch2 分支
};
```

### 8.4 休眠资产标注规范

休眠文件/类使用统一标识：

```cpp
// [DORMANT] — 等待 NightElf 章节节点激活，保留完整代码
// 激活条件：门禁序列完成后 player 指针切换为 &nightElf
class NightElf : public Player { ... };
```

这样 grep `[DORMANT]` 就能找到所有未激活资产，不会被误删。

### 8.5 实施建议

1. **不要一次全拆**。分 3 次迭代：
   - **第一步**：提取 `types.h`（全部 struct 定义）+ `constants.h` + `persp_utils.h`。改动最小，立即减少 200 行。
   - **第二步**：提取共享基础设施（Font、Renderer、AudioEngine、ParticleManager、FloatingTextManager、BulletManager、ChapterManager、HUDBase）。编译验证。
   - **第三步**：提取 Ch1 和 Ch2 系统到 `ch1/` 和 `ch2/` 子目录。最后拆分 Game 类。
2. 每次拆分后**编译零错误零警告**再继续。
3. 用 `git mv` 而非新建文件，保留 git history（但本单文件场景可能需要手动 annotate 来源行号）。
4. 拆分完成后更新 CLAUDE.md 的类结构描述。

---

## 9. 结论

**应该立即拆分，且时机恰好。** 6633 行的单文件已经越过了收益阈值，Ch2 脉冲系统刚完成是拆分的最佳时机——此时代码结构清晰，没有正在开发的半成品。拆分为 18 个头文件后，后续 NightElf 激活、门禁序列、章节 Boss 的开发效率将大幅提升，休眠资产也不会再被误删。

建议的拆分顺序：types.h → 共享基础设施 → Ch1/Ch2 系统 → Game 类拆分。
