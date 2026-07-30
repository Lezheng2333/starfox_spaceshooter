# STAR FOX SPACE SHOOTER — Claude Code 备忘录

## 项目信息

- **语言**：C++11, SDL2
- **源码**：`v2.0.0/multiple code files/`（27 个头文件 + 1 个 main.cpp）\
  旧单文件版本仍保留在 `v2.0.0/space_shooting ver2.0.0 developing.cpp` 作为参考
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

## 完整类结构

### 数据结构 (struct)

```
Star                    — 背景星点 (x,y,phase,twinkleSpeed,driftSpeed)
FloatingText            — 浮动文字 (x,y,text,alpha,life,ox,oy)
BossConfig              — Boss 配置 (name, hp, triggerScore)
ChapterConfig           — 章节配置 (isSideScrolling, bgSpeed, bossConfig, waveParams)
FontChar                — 5x7 位图字形 (rows[7])
ActiveSound             — 活跃音效追踪 (freq,targetFreq,phase,volume,...)
MenuItem                — 菜单状态机条目

Ch1Particle             — 粒子 (x,y,vx,vy,color[3],life,maxLife)
BulletBase              — 子弹基类 (x,y,dx,dy,active,startX,startY,sideScroll,blueBeam,...)
Ch1Bullet : BulletBase  — Ch1 子弹 (+canDamage,beamTargetIndex)
EnemyData               — 敌人基类 (active,entering,defeated,hp,maxHp,enterFrame,...)
Ch1Alien : EnemyData    — Ch1 透视外星飞船
Ch2DanmakuEnemy : EnemyData — Ch2 弹幕敌人
Ch2Alien : EnemyData    — Ch2 普通敌人
Ch1Shockwave            — 冲击波 (y,id,active)
Ch1HealWave             — 治疗波 (radius,id,active)
Ch2EnemyBullet          — Ch2 敌人子弹

MenuKeys                — 边沿检测辅助 (up/down/left/right/enter/esc now+was)
```

### 共享基础设施

```
Font                    — 5x7 位图字体渲染 (drawChar/drawString, 浮点缩放)
Renderer                — SDL 渲染器封装 (setColor/clear/present)
AudioEngine             — 实时音频合成 (正弦/噪音/方波 + 包络/EQ/音量)
FloatingTextManager     — 浮动文字管理 (spawn/update/draw)
AimAssist               — 瞄准辅助组件 (snapProgress, update, draw)
HUDBase                 — HUD 静态绘制 (drawScore/drawHPHearts/drawEnergyBar)
```

### 对话/旁白系统

```
DialogueHistory         — 对话历史存储 (滚动/焦点, size/add/moveUp/moveDown/resetView)
DialogueSystem          — 游戏内对话 (队列+状态机+渲染+打字机效果), 组合 DialogueHistory
NarrationSystem         — 中心开幕旁白 (打字机+多行渲染)
```

### Player 继承体系

```
Player                  — 基类 (x,y,rollAngle,invFrames, AimAssist 组件)
├── TrainingPlane       — Ch1 透视角度的训练机 (三角机身+机翼+尾翼, handleInput 含透视速度缩放)
├── Ch2Trainer          — Ch2 横版训练机 (与 Ch1 同飞机侧视角度, 单发, 无翻滚动画) [当前活跃]
├── NightElf             — Ch2 暗夜精灵 (30°锐角机头+燕尾+扫描线填充, 1发) [已搁置]
└── Druid                — Ch3 备用设计 (双尾翼 boomerang 造型) [未激活]
```

### Ch1 游戏系统

```
ParticleManager         — 粒子管理器 (spawnExplosion/spawnDigitShatter...)
BulletManager           — 子弹管理器 (addBullet/addBulletSideScrollAt/update...)
Ch1ShockwaveManager     — 冲击波管理器
Ch1AlienManager         — Ch1 外星飞船管理器
Ch1Boss                 — Ch1 Boss (Telamondo, HP 条+二阶段)
Ch1Background           — Ch1 星空+地平线+基地背景
```

### Ch2 游戏系统

```
Ch2Background           — Ch2 侧滚廊桥背景 (玻璃面板+柱子+地板+星点)
Ch2ShooterBase          — Ch2 射击基类 (bullets, playerHP/go 引用共享, updateBullets/drawBullets)
├── Ch2DanmakuManager   — 弹幕敌人管理器 (螺旋弹幕图案)
└── Ch2AlienManager     — Ch2 普敌管理器 (菱形造型, 四边突袭入场, 水晶子弹)
Ch2SphereBoss           — 球体 Boss (六角密铺菱形, 7 状态机, 碎片物理)
NightElfEnergy          — 白色能量条+三炮系统 (50命中蓄满/15秒/倒计时音效) [NightElf专属,当前未激活]
```

### 游戏框架

```
ChapterManager          — 章节选择/解锁/配置管理
UIRenderer              — UI 绘制 (drawMenuCursor/drawMenuUnderline/drawSlider)
MenuStateMachine        — 通用菜单状态机
Game                    — 主游戏类 (持有所有系统, updateGameplay/drawGameplayFrame)
```

### Game 类关键成员

```
// Player (按章节切换指针)
TrainingPlane trainingPlane; Ch2Trainer ch2Trainer; NightElf nightElf; Player* player;

// Ch1 系统
BulletManager bulletMgr; Ch1AlienManager alienMgr; ParticleManager particleMgr;
Ch1ShockwaveManager shockwaveMgr; Ch1Boss boss; Ch1Background* background;

// Ch2 系统
Ch2Background* sideBg; Ch2DanmakuManager dmMgr; Ch2AlienManager ch2AlienMgr;
Ch2SphereBoss sphereBoss; bool sphereBossActive;
NightElfEnergy nightElfEnergy; int playerHitCount; int tripleBeepCounter;

// 对话
NarrationSystem narration; DialogueSystem dialogueSys; bool inNarration;

// 状态
int ch2PlayerHP; bool ch2GameOver; int dmFireCooldown;
GamePhase phase; int score; bool paused;
```

### 关键设计决策

- **Player 切换**：`resetGame()` 中根据 `isSideScrolling` 分支选择 player 指针指向的实例
- **Ch2 移动**：在 `updateGameplay` 中直接读键移动，不走 `handleInput()`
- **Ch2 射击**：`addBulletSideScrollAt(Player&)` 支持任意枪位，dx=11 恒定速度
- **Ch1 射击**：`addBullet(TrainingPlane&)` 依赖 `getT()` 做透视弹道
- **Ch1 子弹横飞 Bug 根因**：`sideScroll` 未初始化，垃圾值随机触发 Ch2 弹道 — 修复在 `addBullet()` 中显式设 `sideScroll=false`
- **NightElf 搁置**：Ch2 当前使用 Ch2Trainer，NightElf 类和 NightElfEnergy 系统保留待后续激活
- **球体 Boss 测试入口**：TEST → Chapter 2 直接触发入场动画

## OOP 重构后检查清单

每次 OOP 重构（拆分/合并类、重命名、提取方法）完成后，必须逐项检查：
1. 编译零错误零警告
2. 运行 `./shooter` 快速启动，确认不崩溃
3. 逐项核对受影响的功能点（用 `grep -n` 查找所有调用方，确认每个调用点已更新）
4. 全局搜索旧方法名/旧类名，确保无残留引用

## 开发日志格式规范

`DEVELOPMENT_LOG.md` 遵循以下格式规则：

1. **标题行**：`Ver X.X.X | 简短中文概括`（不超过一行）
2. **条目**：全部使用 `- ` 开头，2 空格缩进
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
