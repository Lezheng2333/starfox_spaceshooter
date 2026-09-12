#pragma once
// ============================================================
// 普通敌人（3 种）
//
// 敌人的入场是逐帧动画（在屏幕外生成 → 飞入 → 到位），
// 所以预览不能只调一次 draw()，必须先推进 update() 若干帧。
// 「入场进度」滑块就是这个推进帧数。
// ============================================================

#include "reg_common.h"
#include "audio.h"
#include "floating_text.h"
#include "particles.h"
#include "player.h"
#include "bullets.h"
#include "ch1/ch1_aliens.h"
#include "ch2/ch2_aliens.h"
#include "ch2/ch2_danmaku.h"

// ============================================================
// 第一章外星飞船
// ============================================================
// Ch1AlienManager::update 需要一串宿主对象，逐帧推进时要用到
struct Ch1EnemyHost {
    Ch1AlienManager aliens;
    ParticleManager particles;
    AudioEngine     audio;
    bool gameOver;
    int  baseHP;
    Ch1EnemyHost() : gameOver(false), baseHP(3) {}
};

static void drawCh1Alien(DrawCtx& c) {
    Ch1EnemyHost* st = (Ch1EnemyHost*)c.priv;
    if (!st) {
        st = new Ch1EnemyHost();
        c.priv = st;
        st->aliens.spawnAlien((int)c.P("score", 0));
        st->aliens.setAllVulnerable();
        int frames = (int)c.P("enter", 55);
        for (int i = 0; i < frames; ++i)
            st->aliens.update(false, 1.0, st->gameOver, st->baseHP, st->particles, &st->audio);
    } else {
        st->aliens.update(false, 1.0, st->gameOver, st->baseHP, st->particles, &st->audio);
    }
    st->aliens.draw(c.r);
}

inline void registerCh1Alien() {
    reg("ch1.enemy.alien", "第一章外星飞船", "ch1", "enemy")
        .describe("第一章的透视外星飞船。它的尺寸完全由透视决定，而地平线以上透视缩放被钳死在 0.17，"
                  "所以存活的前 60% 时间里它只有 4~5 像素。实测尺寸时间线："
                  "推进 55 帧→5x9（y=60）、1000 帧→仍是 5x9（y=192）、1300 帧→11x17（y=262）、"
                  "1600 帧→31x47（y=494）、约 1850 帧后越过屏幕底部销毁。"
                  "滑块默认停在 1500 帧，也就是它真正看得清的时候。")
        .from("v2.0.0/multiple code files/ch1/ch1_aliens.h:162 (Ch1AlienManager::draw)")
        .tag("敌人").tag("第一章").tag("透视投影").tag("入场动画")
        .param("enter", "推进帧数", 0, 2000, 1500, 25)
        .param("score", "玩家分数", 0, 200, 0, 1)
        .drawFn(drawCh1Alien);
}

// ============================================================
// 第二章菱形敌 / 弹幕敌
// ============================================================
// 这两个管理器构造时需要血量和 gameOver 的引用，且 update 需要完整宿主
struct Ch2EnemyHost {
    int  hp;
    bool go;
    Ch2AlienManager   aliens;
    Ch2DanmakuManager danmaku;
    BulletManager     bullets;
    ParticleManager   particles;
    AudioEngine       audio;
    FloatingTextManager ft;
    Player            pl;
    int score, hitCount;

    Ch2EnemyHost()
        : hp(3), go(false),
          aliens(hp, go), danmaku(hp, go),
          score(0), hitCount(0) {
        pl.setX(100); pl.setY(300);
    }
};

static void drawCh2Alien(DrawCtx& c) {
    Ch2EnemyHost* st = (Ch2EnemyHost*)c.priv;
    if (!st) {
        st = new Ch2EnemyHost();
        c.priv = st;
        st->aliens.forceSpawn();
        int frames = (int)c.P("enter", 40);
        for (int i = 0; i < frames; ++i)
            st->aliens.update(st->bullets, st->particles, st->audio,
                              st->score, st->pl, st->ft, st->hitCount);
    } else {
        st->aliens.update(st->bullets, st->particles, st->audio,
                          st->score, st->pl, st->ft, st->hitCount);
    }
    st->aliens.drawEnemy(c.r);
    if (c.P("bullets", 1) > 0.5) st->aliens.drawBullets(c.r);
}

inline void registerCh2Alien() {
    reg("ch2.enemy.alien", "第二章菱形敌", "ch2", "enemy")
        .describe("第二章普敌：菱形机身 + 中心横线。入场时是青色（100,180,255）并带拖尾点，"
                  "入场完成后按剩余血量从绿渐变到红。它会朝玩家连射 5 发。"
                  "「入场进度」0 帧时在屏幕外，40 帧左右入场完成。")
        .from("v2.0.0/multiple code files/ch2/ch2_aliens.h:139 (Ch2AlienManager::drawEnemy)")
        .tag("敌人").tag("第二章").tag("入场动画").tag("会射击")
        .param("enter",   "入场进度(帧)", 0, 120, 40, 1)
        .param("bullets", "显示敌弹",     0,   1,  1, 1)
        .withScene("ch2.bg.corridor")
        .drawFn(drawCh2Alien);
}

static void drawCh2Danmaku(DrawCtx& c) {
    Ch2EnemyHost* st = (Ch2EnemyHost*)c.priv;
    if (!st) {
        st = new Ch2EnemyHost();
        c.priv = st;
        st->danmaku.spawnEnemy();
        int frames = (int)c.P("enter", 40);
        for (int i = 0; i < frames; ++i)
            st->danmaku.update(st->bullets, st->particles, st->audio,
                               st->score, st->pl, st->ft, st->hitCount);
    } else {
        st->danmaku.update(st->bullets, st->particles, st->audio,
                           st->score, st->pl, st->ft, st->hitCount);
    }
    st->danmaku.drawEnemy(c.r);
    if (c.P("bullets", 1) > 0.5) st->danmaku.drawBullets(c.r);
}

inline void registerCh2Danmaku() {
    reg("ch2.enemy.danmaku", "第二章弹幕敌", "ch2", "enemy")
        .describe("螺旋弹幕敌人。会围绕自身持续发射旋转弹幕，是第二章主要的弹幕压力来源。"
                  "拖动「入场进度」可以看到它入场并开始撒弹幕的过程。")
        .from("v2.0.0/multiple code files/ch2/ch2_danmaku.h:174 (Ch2DanmakuManager::drawEnemy)")
        .tag("敌人").tag("第二章").tag("弹幕").tag("入场动画")
        .param("enter",   "入场进度(帧)", 0, 160, 90, 1)
        .param("bullets", "显示弹幕",     0,   1,  1, 1)
        .withScene("ch2.bg.corridor")
        .drawFn(drawCh2Danmaku);
}

inline void registerAllEnemies() {
    registerCh1Alien();
    registerCh2Alien();
    registerCh2Danmaku();
}
