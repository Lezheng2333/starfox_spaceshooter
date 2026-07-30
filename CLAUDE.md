# STAR FOX SPACE SHOOTER — Claude Code 备忘录

## 项目信息

- **语言**：C++11, SDL2
- **源码**：`v2.0.0/multiple code files/`（28 个头文件 + 1 个 main.cpp = 29 个文件）\
  旧单文件存档：`v2.0.0/space_shooting ver1.2.20 single-file archive.cpp`
- **编译命令**：
  ```bash
  cd "v2.0.0/multiple code files" && make
  ```
  或手动：
  ```bash
  cd "v2.0.0/multiple code files" && clang++ -std=c++11 -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
    -D_THREAD_SAFE main.cpp -o shooter -L/opt/homebrew/lib -lSDL2
  ```
- **SDL2 安装**：`brew install sdl2`
- **GitHub 仓库**：`Lezheng2333/starfox_spaceshooter`

## 上下文恢复

- 每次对话开始或 compact 之后，必须阅读 `DEVELOPMENT_LOG.md` 了解最新开发进展和最近版本变更
- 阅读本文件中的"当前开发状态"了解正在进行的任务和下一步计划

## 当前开发状态

### 当前焦点
- **pulse技能已完成** — Ch2PulseSystem（30格能量条+单按Shift释放冲击波）+ Ch2SkillOrb（18边形护罩技能球→吸收→解锁）
- **NightElf升级** — 下一步待实现。NightElf 飞机+NightElfEnergy 白色能量条已搁置，需在剧情节点激活
- Ch2 当前使用 Ch2Trainer（横版训练机，单发，无技能），脉冲系统为全机通用

### 开发路线
```
✅ 球体Boss ✅ 自动出敌 ✅ pulse技能 → NightElf升级(暗夜精灵+能量条激活) → 门禁序列(扫描+解锁) → 章节Boss
```

### 已知脆弱点（修改前必须理解上下文）
- **Ch1 子弹横飞 Bug**：`addBullet()` 中 `Ch1Bullet.sideScroll` 必须显式设 `false`，否则垃圾值会随机触发 Ch2 弹道
- **对话级联触发**：每个分数触发的 dialogue 区块内必须同步设置 `lastScore = score`，帧级 `lastScore` 更新仅在 `!dialogueSys.isActive()` 时执行
- **Ch1 移动速度缩放**：`TrainingPlane::handleInput` 中的 y 钳制顺序必须在 x 钳制之后（先夹 y 再夹 x），否则影响 `getT()` 返回的子弹方向

## 代码规范

- 多文件架构：27 个头文件（全部 `#pragma once`）+ 1 个 `main.cpp`
- 每个文件对应一个系统/类，文件即文档
- 全部实现内联在头文件中（单翻译单元编译，无 .cpp 对应文件除了 main.cpp）
- OOP 设计：组合优于继承，静态方法替代虚函数
- 命名：Chapter 1 专用类加 `Ch1` 前缀，Chapter 2 加 `Ch2` 前缀，共享类无前缀
- **休眠代码标注**：所有未激活的类/函数用 `[DORMANT — 激活条件]` 注释标注
- **文件映射**：
  ```
  types.h = 全部 struct          constants.h = WIN_WIDTH + persp函数
  font.h   = Font                renderer.h = Renderer
  audio.h  = AudioEngine         floating_text.h = FloatingTextManager
  dialogue.h = DialogueHistory + DialogueSystem + drawTextLine
  narration.h = NarrationSystem  particles.h = ParticleManager
  aim_assist.h = AimAssist       player.h = Player + 全部4架子机
  bullets.h = BulletManager      chapter_manager.h = ChapterManager
  ui.h = UIRenderer + MenuStateMachine + MenuKeys
  ch1/ = Ch1Shockwave/Alien/Boss/Background
  ch2/ = Ch2Background + ShooterBase + Danmaku + Aliens + SphereBoss + SkillOrb + Pulse + HUD
  game.h = Game类（含全部状态机+update/draw)    main.cpp = main()
  ```

## 完整文件结构（v1.2.21 多文件架构）

### 编译入口

```
main.cpp            (36 lines)  #include 所有头文件 + main()
```

### 根目录: 共享基础 (17 files)

| 文件 | 行数 | 内容 |
|------|------|------|
| `types.h` | 143 | 全部 18 个 struct + 2 个 enum |
| `constants.h` | 19 | WIN_WIDTH/WIN_HEIGHT/CENTER_X/HORIZON_Y + perspLeft/Right/Width |
| `font.h` | 153 | Font — 5x7 位图字体渲染 |
| `renderer.h` | 37 | Renderer — SDL 渲染器封装 |
| `audio.h` | 342 | AudioEngine — 实时音频合成 + BGM + 全部 snd* 函数 |
| `floating_text.h` | 38 | FloatingTextManager |
| `dialogue.h` | 235 | DialogueHistory + DialogueSystem + drawTextLine |
| `narration.h` | 135 | NarrationSystem — 中心开幕旁白 |
| `particles.h` | 140 | ParticleManager — 粒子特效管理 |
| `aim_assist.h` | 41 | AimAssist — 瞄准辅助组件 |
| `player.h` | 280 | Player + TrainingPlane + Ch2Trainer + NightElf[DORMANT] + Druid[DORMANT] |
| `bullets.h` | 186 | BulletManager — addBullet/addBulletSideScrollAt/update/draw |
| `chapter_manager.h` | 70 | ChapterManager — 章节配置/解锁/切换 |
| `ui.h` | 185 | UIRenderer + MenuStateMachine + MenuKeys |
| `game.h` | 2144 | Game 类 — 全部状态机 + 全部 update/draw 方法 |

### ch1/ 子目录: Chapter 1 战斗系统 (4 files)

| 文件 | 行数 | 内容 |
|------|------|------|
| `ch1_shockwave.h` | 191 | Ch1ShockwaveManager + ID 碰撞机制 |
| `ch1_aliens.h` | 231 | Ch1AlienManager — 透视外星飞船管理 |
| `ch1_boss.h` | 422 | Ch1Boss — TELAMONDO（二阶段+治疗波+吸收） |
| `ch1_background.h` | 122 | Ch1Background — 星空+地平线+基地 |

### ch2/ 子目录: Chapter 2 战斗系统 (8 files)

| 文件 | 行数 | 内容 |
|------|------|------|
| `ch2_background.h` | 293 | Ch2Background — 侧滚廊桥背景 |
| `ch2_shooter_base.h` | 149 | Ch2ShooterBase + NightElfEnergy[DORMANT] |
| `ch2_hud.h` | 39 | HUDBase — drawScore/drawHPHearts/drawEnergyBar |
| `ch2_danmaku.h` | 217 | Ch2DanmakuManager — 螺旋弹幕敌人 |
| `ch2_aliens.h` | 154 | Ch2AlienManager — 普敌管理（菱形造型，四边突袭） |
| `ch2_sphere_boss.h` | 383 | Ch2SphereBoss — 六角密铺菱形球体，7 状态机 |
| `ch2_skill_orb.h` | 201 | Ch2SkillOrb — 漂浮技能球 + 18边形护罩 + 吸收 |
| `ch2_pulse.h` | 197 | Ch2PulseSystem — 绿色能量条 + Shift 单按释放冲击波 |

### 休眠资产 (grep `[DORMANT]` 查找)

| 资产 | 文件 | 激活条件 |
|------|------|------|
| NightElf | player.h | 门禁序列完成后 player 指针切换 |
| NightElfEnergy | ch2/shooter_base.h | NightElf 激活后联动 |
| Druid | player.h | Chapter 3 开发时激活 |
| setStartBgm() | audio.h | 开始画面专属 BGM 设计完成后 |

### 关键设计决策

- **Player 切换**：`resetGame()` 中根据 `isSideScrolling` 分支选择 player 指针指向的实例
- **Ch2 移动**：在 `updateGameplay` 中直接读键移动，不走 `handleInput()`
- **Ch2 射击**：`addBulletSideScrollAt(Player&)` 支持任意枪位，dx=11 恒定速度
- **Ch1 射击**：`addBullet(TrainingPlane&)` 依赖 `getT()` 做透视弹道
- **Ch1 子弹横飞 Bug 根因**：`sideScroll` 未初始化，垃圾值随机触发 Ch2 弹道 — 修复在 `addBullet()` 中显式设 `sideScroll=false`
- **碰撞 ID 机制**：Ch1Shockwave 用 `lastHitBySW == sw.id`，Ch2Pulse 用 `lastHitByPulse == w.id`，每波唯一，每敌人只受一次伤害
- **只移动不改逻辑**：重构全部是剪切粘贴，零行逻辑修改

## OOP 重构后检查清单

每次 OOP 重构（拆分/合并类、重命名、提取方法）完成后，必须逐项检查：
1. 编译零错误零警告
2. 运行 `./shooter` 快速启动，确认不崩溃
3. 逐项核对受影响的功能点（用 `grep -n` 查找所有调用方，确认每个调用点已更新）
4. 全局搜索旧方法名/旧类名，确保无残留引用

## 开发日志格式规范

`DEVELOPMENT_LOG.md` 遵循以下格式规则：

1. **标题行**：`Ver X.X.X | 简短中文概括`（不超过一行）
2. **条目**：全部使用 `- ` 开头，4 空格缩进。每个条目之间用换行分隔（不插入空行）。续行用 6+ 空格缩进紧跟父条目。
3. **顺序**：新功能/优化/enhancement 在前，**BUGFIX 统一在最后**
4. **BUGFIX 格式**：`- BUGFIX: 问题描述 + 修复方法`，与其他条目同级缩进
5. 每个条目尽量控制在一行内，避免不必要的多行展开

## 发布流程 (/release 技能)

每次发布新版本时，按以下步骤操作：

1. **开发日志**：在 `DEVELOPMENT_LOG.md` 末尾写入新版本条目（格式参照已有条目）
2. **源码版本号**：修改 `font.drawString(r, "Ver X.X.X", ...)` 中的版本号
3. **README 更新**：
   - 下载链接：新增最新版本，保留历史版本
   - What's New：用人话简述最近更新（下次发版直接替换内容）
   - 项目结构：更新版本号和 Release Version 目录
4. **运行发布脚本**：
   ```bash
   .claude/skills/release.sh <version> <title> <notes>
   ```
   脚本自动完成：编译 → .app 封装 → 签名 → zip → git commit/push → gh release

### 发布脚本示例
```bash
.claude/skills/release.sh 1.2.9 \
  "Ver 1.2.9: 新功能描述" \
  "## 更新内容\n- 功能A\n- 功能B"
```
