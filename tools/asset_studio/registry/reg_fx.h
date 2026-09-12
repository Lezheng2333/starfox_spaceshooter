#pragma once
// ============================================================
// 子弹 / 粒子 / 技能球 / 脉冲 / 冲击波
//
// 这些都是"有生命周期"的特效：生成 → 逐帧演化 → 消失。
// 所以预览统一用「推进帧数」滑块来看它们在生命周期不同阶段的样子。
// ============================================================

#include "reg_common.h"
#include "audio.h"
#include "particles.h"
#include "player.h"
#include "bullets.h"
#include "ch1/ch1_shockwave.h"
#include "ch2/ch2_aliens.h"
#include "ch2/ch2_skill_orb.h"
#include "ch2/ch2_pulse.h"

// ============================================================
// 子弹宿主：BulletManager 需要玩家实例才能生成子弹
// ============================================================
struct BulletHost {
    BulletManager bm;
    TrainingPlane tp;      // 第一章透视弹
    Ch2Trainer    ct;      // 第二章侧滚弹
    std::vector<Ch1Alien> noAliens;
    BulletHost() {
        tp.reset();
        tp.setX(400); tp.setY(500);
        ct.reset();
        ct.setX(100); ct.setY(300);
    }
};

// ---------------- 第一章 · 透视子弹 -
static void drawCh1Bullet(DrawCtx& c) {
    BulletHost* st = (BulletHost*)c.priv;
    if (!st) {
        st = new BulletHost();
        c.priv = st;
        st->tp.setX((int)c.P("posX", 400));
        st->tp.setY((int)c.P("posY", 500));
        if (c.S("beam")) {
            st->bm.addBossBeam(400, 120, st->tp.getX(), st->tp.getY(), -1);
        } else {
            for (int i = 0; i < 3; ++i) st->bm.addBullet(st->tp, nullptr);
        }
    }
    int frames = (int)c.P("advance", 12);
    for (int i = 0; i < frames; ++i) st->bm.update(st->noAliens);
    st->bm.draw(c.r);
}

inline void registerCh1Bullet() {
    reg("ch1.bullet.player", "第一章 · 透视子弹", "ch1", "bullet")
        .describe("第一章玩家子弹：沿透视弹道从机头飞向地平线，越远越汇聚、越小。"
                  "弹道方向由玩家所处位置决定——拖动「机头水平位置」可以看到子弹斜着飞出去。"
                  "「Boss 光束」状态是 Boss 战里的另一种弹型。")
        .from("v2.0.0/multiple code files/bullets.h:129 (BulletManager::draw)")
        .tag("子弹").tag("第一章").tag("透视投影")
        .state_("normal", "普通子弹")
        .state_("beam",   "Boss 光束")
        .param("posX",    "机头水平位置", 100, 700, 400, 1)
        .param("posY",    "机头垂直位置", 220, 570, 500, 1)
        .param("advance", "推进帧数",     0,  90,  12, 1)
        .drawFn(drawCh1Bullet);
}

// ---------------- 第二章 · 侧滚子弹 -
static void drawCh2Bullet(DrawCtx& c) {
    BulletHost* st = (BulletHost*)c.priv;
    if (!st) {
        st = new BulletHost();
        c.priv = st;
        st->ct.setX((int)c.P("posX", 100));
        st->ct.setY((int)c.P("posY", 300));
        int n = (int)c.P("count", 1);
        if (n < 1) n = 1;
        for (int i = 0; i < n; ++i)
            st->bm.addBulletSideScrollAt(st->ct, 12, (i - (n - 1) / 2) * 8, nullptr);
    }
    int frames = (int)c.P("advance", 20);
    for (int i = 0; i < frames; ++i) st->bm.update(st->noAliens);
    st->bm.draw(c.r);
}

inline void registerCh2Bullet() {
    reg("ch2.bullet.player", "第二章 · 侧滚子弹", "ch2", "bullet")
        .describe("第二章玩家子弹：恒速 11 px/帧 向右直飞，每发带 ±8/50 的随机散布。"
                  "「弹数」可以模拟三炮模式一次打出多发。")
        .from("v2.0.0/multiple code files/bullets.h:129 (BulletManager::draw)")
        .tag("子弹").tag("第二章").tag("侧滚")
        .param("posX",    "出膛水平位置", 0, 700, 100, 1)
        .param("posY",    "出膛垂直位置", 0, 600, 300, 1)
        .param("count",   "弹数",         1,   5,   1, 1)
        .param("advance", "推进帧数",     0,  60,  20, 1)
        .withScene("ch2.bg.corridor")
        .drawFn(drawCh2Bullet);
}

// ---------------- 第二章 · 敌弹 -
struct EnemyBulletHost {
    int hp; bool go;
    Ch2AlienManager aliens;
    BulletManager   bm;
    ParticleManager pm;
    AudioEngine     audio;
    FloatingTextManager ft;
    Player pl;
    int score, hitCount;
    EnemyBulletHost() : hp(3), go(false), aliens(hp, go), score(0), hitCount(0) {
        pl.setX(60); pl.setY(300);
    }
};

static void drawCh2EnemyBullet(DrawCtx& c) {
    EnemyBulletHost* st = (EnemyBulletHost*)c.priv;
    if (!st) {
        st = new EnemyBulletHost();
        c.priv = st;
        st->aliens.forceSpawn();
    }
    int frames = (int)c.P("advance", 110);
    for (int i = 0; i < frames; ++i)
        st->aliens.update(st->bm, st->pm, st->audio, st->score, st->pl, st->ft, st->hitCount);
    st->aliens.drawBullets(c.r);
}

inline void registerCh2EnemyBullet() {
    reg("ch2.bullet.enemy", "第二章 · 敌弹", "ch2", "bullet")
        .describe("敌方子弹：菱形敌朝玩家方向连射 5 发（速度 1.5 px/帧，血量 3）。"
                  "这里只画弹幕不画敌人本体。「推进帧数」要拉到 100 帧以上、敌人入场完毕才会开火。")
        .from("v2.0.0/multiple code files/ch2/ch2_shooter_base.h:73 (Ch2ShooterBase::drawBullets)")
        .tag("子弹").tag("第二章").tag("敌弹")
        .param("advance", "推进帧数", 0, 300, 110, 5)
        .withScene("ch2.bg.corridor")
        .drawFn(drawCh2EnemyBullet);
}

// ============================================================
// 粒子特效
// ============================================================
struct ParticleHost {
    ParticleManager pm;
    Font font;
};

static void drawExplosion(DrawCtx& c) {
    ParticleHost* st = (ParticleHost*)c.priv;
    if (!st) {
        st = new ParticleHost();
        c.priv = st;
        st->pm.spawnExplosion(c.P("posX", 400), c.P("posY", 300), (int)c.P("count", 35));
    }
    int frames = (int)c.P("advance", 6);
    for (int i = 0; i < frames; ++i) { st->pm.update(); st->pm.removeInactive(); }
    st->pm.draw(c.r);
}

inline void registerExplosion() {
    reg("shared.fx.explosion", "爆炸粒子", "shared", "fx")
        .describe("爆炸粒子（白 + 橙混合）。敌人被击毁、基地受击、Boss 命中都用它。"
                  "粒子有生命期，拖动「推进帧数」可以看到爆炸从绽放到消散。")
        .from("v2.0.0/multiple code files/particles.h:111 (ParticleManager::draw)")
        .tag("特效").tag("共享").tag("粒子").tag("动画")
        .param("posX",    "水平位置", 0, 800, 400, 1)
        .param("posY",    "垂直位置", 0, 600, 300, 1)
        .param("count",   "粒子数量", 1, 120, 35, 1)
        .param("advance", "推进帧数", 0,  60,  6, 1)
        .framesN(12)
        .drawFn(drawExplosion);
}

static void drawFireworks(DrawCtx& c) {
    ParticleHost* st = (ParticleHost*)c.priv;
    if (!st) {
        st = new ParticleHost();
        c.priv = st;
        st->pm.spawnFireworks(c.P("posX", 400), c.P("posY", 300));
    }
    int frames = (int)c.P("advance", 10);
    for (int i = 0; i < frames; ++i) { st->pm.update(); st->pm.removeInactive(); }
    st->pm.draw(c.r);
}

inline void registerFireworks() {
    reg("shared.fx.fireworks", "胜利烟花", "shared", "fx")
        .describe("通关烟花（多色粒子 + 重力下坠）。")
        .from("v2.0.0/multiple code files/particles.h:111 (ParticleManager::draw)")
        .tag("特效").tag("共享").tag("粒子").tag("动画")
        .param("posX",    "水平位置", 0, 800, 400, 1)
        .param("posY",    "垂直位置", 0, 600, 300, 1)
        .param("advance", "推进帧数", 0,  90,  10, 1)
        .framesN(16)
        .drawFn(drawFireworks);
}

static void drawDigitShatter(DrawCtx& c) {
    ParticleHost* st = (ParticleHost*)c.priv;
    if (!st) {
        st = new ParticleHost();
        c.priv = st;
        char d = (char)('0' + (int)c.P("digit", 7));
        st->pm.spawnDigitShatter(st->font, d, (int)c.P("digitScale", 4),
                                 (int)c.P("posX", 400), (int)c.P("posY", 300));
    }
    int frames = (int)c.P("advance", 4);
    for (int i = 0; i < frames; ++i) { st->pm.update(); st->pm.removeInactive(); }
    st->pm.draw(c.r);
}

inline void registerDigitShatter() {
    reg("shared.fx.digitshatter", "分数数字碎裂", "shared", "fx")
        .describe("分数数字被打碎成粒子的特效——逐位把 5x7 字形的每个亮点变成一粒碎屑。"
                  "拖动「数字」可以看 0-9 每个字形碎裂的样子。")
        .from("v2.0.0/multiple code files/particles.h:111 (ParticleManager::draw)")
        .tag("特效").tag("共享").tag("粒子").tag("字体")
        .param("digit",      "数字",     0,   9,   7, 1)
        .param("digitScale", "字号",     1,   8,   4, 1)
        .param("posX",       "水平位置", 0, 800, 400, 1)
        .param("posY",       "垂直位置", 0, 600, 300, 1)
        .param("advance",    "推进帧数", 0,  60,   4, 1)
        .framesN(12)
        .drawFn(drawDigitShatter);
}

// ============================================================
// 第一章 · 冲击波
// ============================================================
struct ShockwaveHost {
    Ch1ShockwaveManager sw;
    ParticleManager pm;
};

static void drawCh1Shockwave(DrawCtx& c) {
    ShockwaveHost* st = (ShockwaveHost*)c.priv;
    if (!st) {
        st = new ShockwaveHost();
        c.priv = st;
        st->sw.spawn(nullptr, c.P("particles", 1) > 0.5 ? &st->pm : nullptr);
    }
    int frames = (int)c.P("advance", 40);
    for (int i = 0; i < frames; ++i) { st->sw.update(); st->pm.update(); st->pm.removeInactive(); }
    st->sw.draw(c.r);
    if (c.P("particles", 1) > 0.5) st->pm.draw(c.r);
}

inline void registerCh1Shockwave() {
    reg("ch1.fx.shockwave", "第一章 · 冲击波", "ch1", "fx")
        .describe("第一章的环形冲击波：从屏幕底部向上推进，命中敌人造成伤害。"
                  "生成时会沿底部圆弧炸出 40 粒绿色粒子。拖动「推进帧数」看它向上飞。")
        .from("v2.0.0/multiple code files/ch1/ch1_shockwave.h:100 (Ch1ShockwaveManager::draw)")
        .tag("特效").tag("第一章").tag("技能").tag("动画")
        .param("advance",   "推进帧数", 0, 400, 40, 5)
        .param("particles", "显示粒子", 0,   1,  1, 1)
        .drawFn(drawCh1Shockwave);
}

// ============================================================
// 第二章 · 技能球
// ============================================================
struct OrbHost {
    Ch2SkillOrb    orb;
    ParticleManager pm;
};

static void drawSkillOrb(DrawCtx& c) {
    OrbHost* st = (OrbHost*)c.priv;
    if (!st) {
        st = new OrbHost();
        c.priv = st;
        st->orb.spawn(c.P("posX", 400), c.P("posY", 300));
        if (c.S("damaged"))   st->orb.shieldHp = (int)c.P("shieldHp", 6);
        if (c.S("core"))    { st->orb.shieldHp = 0; st->orb.shieldBroken = true;
                              st->orb.state = Ch2SkillOrb::CORE; }
        if (c.S("absorb"))  { st->orb.shieldHp = 0; st->orb.shieldBroken = true;
                              st->orb.startAbsorb(); }
    }
    int frames = (int)c.P("advance", 0);
    for (int i = 0; i < frames; ++i) {
        st->orb.update(); st->pm.update(); st->pm.removeInactive();
    }
    st->orb.draw(c.r);
    st->pm.draw(c.r);
}

inline void registerSkillOrb() {
    reg("ch2.fx.skillorb", "第二章 · 技能球", "ch2", "fx")
        .describe("漂浮的技能球：外圈 18 边形护罩 + 内核。护罩需要 18 次命中才会破碎，"
                  "破碎后露出核心，吸收后解锁脉冲技能。"
                  "「护罩血量」滑块可以直接看护罩从完好到濒碎的变化。")
        .from("v2.0.0/multiple code files/ch2/ch2_skill_orb.h:129 (Ch2SkillOrb::draw)")
        .tag("特效").tag("第二章").tag("技能").tag("动画")
        .state_("floating", "护罩完好")
        .state_("damaged",  "护罩受损")
        .state_("core",     "核心暴露")
        .state_("absorb",   "吸收中")
        .param("posX",      "水平位置", 0,  800, 400, 1)
        .param("posY",      "垂直位置", 0,  600, 300, 1)
        .param("shieldHp",  "护罩血量", 0,   18,  6, 1)
        .param("advance",   "推进帧数", 0,  120,  0, 5)
        .withScene("ch2.bg.corridor")
        .drawFn(drawSkillOrb);
}

// ============================================================
// 第二章 · 脉冲冲击波
// ============================================================
struct PulseHost {
    Ch2PulseSystem ps;
    ParticleManager pm;
    AudioEngine audio;
};

static void drawPulse(DrawCtx& c) {
    PulseHost* st = (PulseHost*)c.priv;
    if (!st) {
        st = new PulseHost();
        c.priv = st;
        st->ps.reset();
        st->ps.release(c.P("posX", 120), c.P("posY", 300), st->pm, st->audio);
    }
    int frames = (int)c.P("advance", 10);
    for (int i = 0; i < frames; ++i) { st->ps.update(); st->pm.update(); st->pm.removeInactive(); }
    st->ps.draw(c.r);
    st->pm.draw(c.r);
}

inline void registerPulse() {
    reg("ch2.fx.pulse", "第二章 · 脉冲冲击波", "ch2", "fx")
        .describe("Shift 单按释放的脉冲波：以玩家为中心扩散的白色圆环 + 50 粒白色粒子爆发。"
                  "这是第二章的核心技能，能一次性清掉范围内的敌人和弹幕。")
        .from("v2.0.0/multiple code files/ch2/ch2_pulse.h:102 (Ch2PulseSystem::draw)")
        .tag("特效").tag("第二章").tag("技能").tag("动画")
        .param("posX",    "释放位置 X", 0, 800, 120, 1)
        .param("posY",    "释放位置 Y", 0, 600, 300, 1)
        .param("advance", "推进帧数",   0, 120,  10, 1)
        .withScene("ch2.bg.corridor")
        .framesN(14)
        .drawFn(drawPulse);
}

inline void registerAllFx() {
    registerCh1Bullet();
    registerCh2Bullet();
    registerCh2EnemyBullet();
    registerExplosion();
    registerFireworks();
    registerDigitShatter();
    registerCh1Shockwave();
    registerSkillOrb();
    registerPulse();
}
