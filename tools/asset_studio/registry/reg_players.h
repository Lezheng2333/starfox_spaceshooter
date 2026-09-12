#pragma once
// ============================================================
// 玩家战机（4 架）
// 只读接入：直接调用游戏真实的 draw()，主游戏零改动。
// ============================================================

#include "reg_common.h"
#include "player.h"

// ---------------- 训练机（第一章 · 透视视角）----------------
static void drawTrainingPlane(DrawCtx& c) {
    TrainingPlane p;
    p.reset();
    p.setX((int)c.P("posX", 400));
    p.setY((int)c.P("posY", 480));
    p.setRollAngle(c.P("roll", 0) * M_PI / 180.0);
    p.draw(c.r);
}

inline void registerPlayerTrainingPlane() {
    reg("ch1.player.trainingplane", "训练机", "ch1", "player")
        .describe("第一章透视视角战机。位置越靠上越接近地平线，机身随透视缩小、弹道也随之汇聚。"
                  "拖动「垂直位置」滑块可以从 y=220 一路看到 y=570，直接观察透视缩放效果。"
                  "「横滚角度」控制机翼的滚转姿态。")
        .from("v2.0.0/multiple code files/player.h:55 (TrainingPlane::draw)")
        .tag("战机").tag("可驾驶").tag("第一章").tag("透视投影")
        .param("posX", "水平位置", 100, 700, 400, 1)
        .param("posY", "垂直位置", 220, 570, 480, 1)
        .param("roll", "横滚角度", 0, 360, 0, 5)
        .drawFn(drawTrainingPlane);
}

// ---------------- 训练机（第二章 · 侧滚视角）----------------
static void drawCh2Trainer(DrawCtx& c) {
    Ch2Trainer p;
    p.reset();
    p.setX((int)c.P("posX", 400));
    p.setY((int)c.P("posY", 300));
    p.draw(c.r);
}

inline void registerPlayerCh2Trainer() {
    reg("ch2.player.trainer", "训练机（侧滚）", "ch2", "player")
        .describe("第一章那架训练机旋转 90° 后的侧滚版本：机头朝右，"
                  "机身是同一个三角造型，1:1 同比例。")
        .from("v2.0.0/multiple code files/player.h:158 (Ch2Trainer::draw)")
        .tag("战机").tag("可驾驶").tag("第二章").tag("侧滚视角")
        .param("posX", "水平位置", 0, 800, 400, 1)
        .param("posY", "垂直位置", 0, 600, 300, 1)
        .withScene("ch2.bg.corridor")
        .drawFn(drawCh2Trainer);
}

// ---------------- 暗夜精灵号（第二章主力机）----------------
static void drawNightElf(DrawCtx& c) {
    NightElf elf;
    elf.reset();
    elf.setX((int)c.P("posX", 400));
    elf.setY((int)c.P("posY", 300));
    elf.setTripleFire(c.S("triple"));
    elf.draw(c.r);
}

// 炮口标记放在叠加层：在内容分析之后绘制，
// 这样它既不会撑大包围盒，也不会把对称度拉低（炮口只在一侧）。
static void overlayNightElfMuzzle(DrawCtx& c) {
    if (c.P("muzzle", 1) <= 0.5) return;
    NightElf elf;
    elf.reset();
    elf.setX((int)c.P("posX", 400));
    elf.setY((int)c.P("posY", 300));
    elf.setTripleFire(c.S("triple"));
    int n = elf.getGunCount();
    for (int i = 0; i < n; ++i) {
        int ox = 0, oy = 0;
        elf.getGunOffset(i, ox, oy);
        drawMuzzleMarker(c.r, elf.getX() + ox, elf.getY() + oy);
    }
}

inline void registerPlayerNightElf() {
    reg("ch2.player.nightelf", "暗夜精灵号", "ch2", "player")
        .describe("第二章门禁序列后解锁的主力战机。机头 30° 尖锐造型，尾部 120° 反折。"
                  "三炮模式下上下翼尖各多一处炮口（黄色十字为炮口位置标记，可关）。")
        .from("v2.0.0/multiple code files/player.h:215 (NightElf::draw)")
        .tag("战机").tag("可驾驶").tag("第二章").tag("线稿复杂")
        .state_("single", "单炮模式")
        .state_("triple", "三炮模式")
        .param("posX",   "水平位置", 0, 800, 400, 1)
        .param("posY",   "垂直位置", 0, 600, 300, 1)
        .param("muzzle", "显示炮口标记", 0, 1, 1, 1)
        .withScene("ch2.bg.corridor")
        .drawFn(drawNightElf)
        .overlayFn(overlayNightElfMuzzle);
}

// ---------------- 德鲁伊（第三章 · 休眠中）----------------
static void drawDruid(DrawCtx& c) {
    Druid p;
    p.reset();
    p.setX((int)c.P("posX", 400));
    p.setY((int)c.P("posY", 300));
    p.draw(c.r);
}

inline void registerPlayerDruid() {
    reg("ch3.player.druid", "德鲁伊号", "ch3", "player")
        .describe("第三章飞机，游戏内标注为 [DORMANT — Chapter 3 激活]，尚未在主线出现。"
                  "燕尾造型：机头 60°、尾部 90° 凹口，正中有导航箭头的引导线。")
        .from("v2.0.0/multiple code files/player.h:272 (Druid::draw)")
        .tag("战机").tag("可驾驶").tag("第三章").tag("休眠资产")
        .param("posX", "水平位置", 0, 800, 400, 1)
        .param("posY", "垂直位置", 0, 600, 300, 1)
        .withScene("ch2.bg.corridor")
        .drawFn(drawDruid);
}

inline void registerAllPlayers() {
    registerPlayerTrainingPlane();
    registerPlayerCh2Trainer();
    registerPlayerNightElf();
    registerPlayerDruid();
}
