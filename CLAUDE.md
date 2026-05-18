# STAR FOX SPACE SHOOTER — Claude Code 备忘录

## 项目信息

- **语言**：C++11, SDL2
- **源码**：`v2.0.0/space_shooting ver2.0.0 developing.cpp`
- **编译命令**：
  ```bash
  cd v2.0.0 && clang++ -std=c++11 -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
    -D_THREAD_SAFE "space_shooting ver2.0.0 developing.cpp" -o shooter \
    -L/opt/homebrew/lib -lSDL2
  ```
- **SDL2 安装**：`brew install sdl2`
- **GitHub 仓库**：`Lezheng2333/starfox_spaceshooter`

## 代码规范

- 单文件架构，类定义在 `main()` 之前
- OOP 设计：组合优于继承，静态方法替代虚函数
- 命名：Chapter 1 专用类加 `Ch1` 前缀，Chapter 2 加 `Ch2` 前缀，共享类无前缀
- 继承层次：
  ```
  BulletBase → Ch1Bullet
  EnemyData → Ch1Alien / Ch2DanmakuEnemy / Ch2Alien
  Ch2ShooterBase → Ch2DanmakuManager / Ch2AlienManager
  PlayerBase (含 AimAssist 组件) → Ch1Player
  HUDBase (静态绘制方法)
  ```

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
