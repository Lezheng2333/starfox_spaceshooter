#pragma once
// ============================================================
// 背景 / 场景
// 零件树：整张背景可以整体看，也能单独看星空 / 地面 / 基地 / 廊桥各层。
// ============================================================

#include "reg_common.h"
#include "chapter_manager.h"
#include "ch1/ch1_background.h"
#include "ch2/ch2_background.h"

// Ch1Background 持有 ChapterConfig 的引用，所以配置必须比它活得久
// （previewChapterMgr 在 reg_common.h 里，是 static 的）。
inline Ch1Background makeCh1Bg(int chapterIdx) {
    return Ch1Background(previewChapterMgr().getChapterConfig(chapterIdx));
}

// ---------------- 第一章 · 星空 -
static void drawCh1Stars(DrawCtx& c) {
    Ch1Background bg = makeCh1Bg((int)c.P("chapter", 0));
    bg.drawStars(c.r);
}
inline void registerCh1Stars() {
    reg("ch1.bg.stars", "第一章 · 星空", "ch1", "bg")
        .describe("地平线以上的闪烁星点。星星数量/亮度取自游戏真实章节配置。"
                  "注意：只有星点，天空底色用的是工坊的全局背景色（画廊右上角可调）。")
        .from("v2.0.0/multiple code files/ch1/ch1_background.h:30 (Ch1Background::drawStars)")
        .tag("背景").tag("第一章").tag("零件")
        .noCrop()
        .param("chapter", "章节配置", 0, 4, 0, 1)
        .drawFn(drawCh1Stars);
}

// ---------------- 第一章 · 全屏星空 -
static void drawCh1StarsFull(DrawCtx& c) {
    Ch1Background bg = makeCh1Bg((int)c.P("chapter", 0));
    bg.drawStarsFullscreen(c.r);
}
inline void registerCh1StarsFull() {
    reg("ch1.bg.stars_full", "第一章 · 全屏星空", "ch1", "bg")
        .describe("铺满整屏的星空版本（过场/结局用）。")
        .from("v2.0.0/multiple code files/ch1/ch1_background.h:41 (Ch1Background::drawStarsFullscreen)")
        .tag("背景").tag("第一章").tag("零件")
        .noCrop()
        .param("chapter", "章节配置", 0, 4, 0, 1)
        .drawFn(drawCh1StarsFull);
}

// ---------------- 第一章 · 地面透视 -
static void drawCh1Terrain(DrawCtx& c) {
    Ch1Background bg = makeCh1Bg((int)c.P("chapter", 0));
    bg.drawBackground(c.r);
}
inline void registerCh1Terrain() {
    reg("ch1.bg.terrain", "第一章 · 地面透视", "ch1", "bg")
        .describe("地平线 + 星空 + 17 条放射线 + 16 条纵深线，构成向后延伸的透视地面。"
                  "切到第二章配置可以看到地面颜色的变化。")
        .from("v2.0.0/multiple code files/ch1/ch1_background.h:65 (Ch1Background::drawBackground)")
        .tag("背景").tag("第一章").tag("透视")
        .noCrop()
        .param("chapter", "章节配置", 0, 4, 0, 1)
        .drawFn(drawCh1Terrain);
}

// ---------------- 第一章 · 基地 -
static void drawCh1Base(DrawCtx& c) {
    Ch1Background bg = makeCh1Bg((int)c.P("chapter", 0));
    bg.drawBase(c.r);
}
inline void registerCh1Base() {
    reg("ch1.bg.base", "第一章 · 基地", "ch1", "bg")
        .describe("地平线上的基地建筑群（会随章节配置移动/变化）。")
        .from("v2.0.0/multiple code files/ch1/ch1_background.h:95 (Ch1Background::drawBase)")
        .tag("背景").tag("第一章").tag("零件")
        .param("chapter", "章节配置", 0, 4, 0, 1)
        .drawFn(drawCh1Base);
}

// ---------------- 第一章 · 完整背景 -
static void drawCh1Full(DrawCtx& c) {
    Ch1Background bg = makeCh1Bg((int)c.P("chapter", 0));
    SDL_SetRenderDrawColor(c.r, 0, 0, 0, 255);   // 天空底色（游戏里由外部清屏负责）
    SDL_Rect full = {0, 0, c.W, c.H};
    SDL_RenderFillRect(c.r, &full);
    bg.drawBackground(c.r);
    bg.drawBase(c.r);
}
inline void registerCh1Full() {
    reg("ch1.bg.full", "第一章 · 完整背景", "ch1", "bg")
        .describe("星空 + 透视地面 + 基地叠起来的完整第一章背景，与游戏里看到的画面一致。")
        .from("v2.0.0/multiple code files/ch1/ch1_background.h:65,95")
        .tag("背景").tag("第一章").tag("整体")
        .noCrop()
        .framesN(16)
        .param("chapter", "章节配置", 0, 4, 0, 1)
        .drawFn(drawCh1Full);
}

// ---------------- 第二章 · 廊桥（兼作场景上下文）----------------
struct CorridorSceneState { Ch2Background bg; };

static void drawCorridor(DrawCtx& c) {
    CorridorSceneState* st = (CorridorSceneState*)c.priv;
    if (!st) {
        st = new CorridorSceneState();
        st->bg.reset();
        double target = c.P("scroll", 900);
        int steps = (int)(target / 3.5);   // scrollSpeed 默认 3.5
        for (int i = 0; i < steps; ++i) st->bg.update();
        c.priv = st;
    }
    if (c.totalFrames > 1) st->bg.update();   // 动画时每帧继续滚动
    st->bg.draw(c.r);
}
inline void registerCorridor() {
    reg("ch2.bg.corridor", "第二章廊桥", "ch2", "bg")
        .describe("侧滚廊桥：星空 + 透视地板缝 + 玻璃幕墙 + 立柱视差。"
                  "既是独立背景素材，也是第二章各素材的「真实场景上下文」。")
        .from("v2.0.0/multiple code files/ch2/ch2_background.h:141 (Ch2Background::draw)")
        .tag("背景").tag("场景").tag("第二章").tag("动画")
        .noCrop()
        .framesN(24)
        .param("scroll", "滚动位置", 0, 2000, 900, 10)
        .drawFn(drawCorridor);
}

// ---------------- 第二章 · 廊桥各层零件 -
// 说明：Ch2Background 的 drawSky / drawFloor / drawGlassPanels / drawPillars
// 都是 private，工具无法单独调用其中一层。要让廊桥也能拆零件看，
// 需要在游戏侧把这四个方法改成 public —— 属于主游戏改动，待你确认后再做。

// ============================================================
// 第二章 · 门禁场景（真空门 + 扫描光束 + 脉冲光环）
// ============================================================
#include "ch2/ch2_gate.h"
#include "ch2/ch2_lab.h"

struct GateHost {
    Ch2GateScene gate;
    AudioEngine  audio;
};

static void gateSetup(GateHost* st, DrawCtx& c) {
    st->gate.reset();
    if (c.S("open"))        st->gate.skipToOpen();
    else if (c.S("closed")) { /* 停在 DIALOGUE：门关闭、无光束、无光环 */ }
    // 默认自动推进流程。注意游戏里 DIALOGUE → PULSING 是由主循环在对话结束后
    // 调 startPulsing() 触发的，预览里不补这一步就会永远卡在 DIALOGUE。
    else                    st->gate.startPulsing();
}

static void drawGate(DrawCtx& c) {
    GateHost* st = (GateHost*)c.priv;
    if (!st) { st = new GateHost(); c.priv = st; gateSetup(st, c); }
    double px = c.P("posX", 300), py = c.P("posY", 300);
    for (int i = 0; i < (int)c.P("advance", 0); ++i) st->gate.update(px, py, st->audio);

    st->gate.drawDoor(c.r);
    st->gate.drawRings(c.r, px, py);
    st->gate.drawScanBeam(c.r, px, py);
}

inline void registerGate() {
    reg("ch2.scene.gate", "第二章 · 门禁场景", "ch2", "bg")
        .describe("第二章开场的门禁：真空门两片对开面板 + 扫描仪 + 脉冲解锁光环。"
                  "流程是 对话(DIALOGUE) → 脉冲(PULSING) → 扫描(SCANNING) → 开门(OPENING) → 全开(OPEN)。"
                  "「推进帧数」推动流程，「玩家位置」决定扫描光束从哪射来。")
        .from("v2.0.0/multiple code files/ch2/ch2_gate.h:120 (Ch2GateScene::drawDoor, "
              "Ch2GateScene::drawScanBeam, Ch2GateScene::drawRings)")
        .tag("场景").tag("第二章").tag("动画").tag("零件树")
        .noCrop()
        .state_("closed",  "门关闭")
        .state_("pulsing", "脉冲解锁中")
        .state_("open",    "门全开")
        .param("advance", "推进帧数",   0, 600,  0, 5)
        .param("posX",    "玩家位置 X", 0, 800, 300, 1)
        .param("posY",    "玩家位置 Y", 0, 600, 300, 1)
        .withScene("ch2.bg.corridor")
        .framesN(16)
        .drawFn(drawGate);
}

// ---------------- 门禁零件：真空门 / 扫描光束 / 脉冲光环 ----------------
static void drawGateDoorPart(DrawCtx& c) {
    GateHost* st = (GateHost*)c.priv;
    if (!st) { st = new GateHost(); c.priv = st; gateSetup(st, c); }
    for (int i = 0; i < (int)c.P("advance", 0); ++i)
        st->gate.update(c.P("posX", 300), c.P("posY", 300), st->audio);
    st->gate.drawDoor(c.r);
}
static void drawGateBeamPart(DrawCtx& c) {
    GateHost* st = (GateHost*)c.priv;
    if (!st) { st = new GateHost(); c.priv = st; gateSetup(st, c); }
    double px = c.P("posX", 300), py = c.P("posY", 300);
    // 光束只在 SCANNING 阶段出现，所以要推进到那个阶段
    for (int i = 0; i < 3000 && st->gate.getStage() != Ch2GateScene::SCANNING; ++i)
        st->gate.update(px, py, st->audio);
    for (int i = 0; i < (int)c.P("advance", 20); ++i) st->gate.update(px, py, st->audio);
    st->gate.drawScanBeam(c.r, px, py);
}
static void drawGateRingsPart(DrawCtx& c) {
    GateHost* st = (GateHost*)c.priv;
    if (!st) { st = new GateHost(); c.priv = st; gateSetup(st, c); }
    double px = c.P("posX", 300), py = c.P("posY", 300);
    // PULSING 阶段每 120 帧才生成一组光环，所以要推进到 120 的倍数才看得到
    for (int i = 0; i < (int)c.P("advance", 130); ++i) st->gate.update(px, py, st->audio);
    st->gate.drawRings(c.r, px, py);
}

inline void registerGateParts() {
    reg("ch2.scene.gate.door", "门禁零件 · 真空门", "ch2", "bg")
        .describe("门禁的真空门本体：左右两片对开面板 + 中缝 + 指示灯。"
                  "切到「门全开」可以看到两片面板完全分开的样子。")
        .from("v2.0.0/multiple code files/ch2/ch2_gate.h:120 (Ch2GateScene::drawDoor)")
        .tag("场景").tag("第二章").tag("零件")
        .noCrop()
        .state_("closed", "门关闭")
        .state_("open",   "门全开")
        .param("advance", "推进帧数", 0, 300, 0, 5)
        .drawFn(drawGateDoorPart);

    reg("ch2.scene.gate.beam", "门禁零件 · 扫描光束", "ch2", "bg")
        .describe("扫描仪射向玩家的光束 + 扫描点。")
        .from("v2.0.0/multiple code files/ch2/ch2_gate.h:247 (Ch2GateScene::drawScanBeam)")
        .tag("场景").tag("第二章").tag("零件")
        .param("advance", "推进帧数",   0, 300, 20, 5)
        .param("posX",    "玩家位置 X", 0, 800, 300, 1)
        .param("posY",    "玩家位置 Y", 0, 600, 300, 1)
        .drawFn(drawGateBeamPart);

    reg("ch2.scene.gate.rings", "门禁零件 · 脉冲光环", "ch2", "bg")
        .describe("脉冲解锁时从玩家身上一圈圈扩散出去的光环。")
        .from("v2.0.0/multiple code files/ch2/ch2_gate.h:301 (Ch2GateScene::drawRings)")
        .tag("场景").tag("第二章").tag("零件").tag("动画")
        .param("advance", "推进帧数",   0, 600, 130, 5)
        .param("posX",    "玩家位置 X", 0, 800, 300, 1)
        .param("posY",    "玩家位置 Y", 0, 600, 300, 1)
        .framesN(16)
        .drawFn(drawGateRingsPart);
}

// ============================================================
// 第二章 · 中央研究室
// ============================================================
struct LabHost { Ch2LabScene lab; };

static void drawLab(DrawCtx& c) {
    LabHost* st = (LabHost*)c.priv;
    if (!st) { st = new LabHost(); c.priv = st; }
    for (int i = 0; i < (int)c.P("advance", 0); ++i) st->lab.update();
    st->lab.draw(c.r);
}

inline void registerLab() {
    reg("ch2.scene.lab", "第二章 · 中央研究室", "ch2", "bg")
        .describe("第二章的圆形研究室：圆形房间轮廓 + 中央基座（暗夜精灵号的停放点）。"
                  "触碰基座上的暗夜精灵号后切换飞机。")
        .from("v2.0.0/multiple code files/ch2/ch2_lab.h:28 (Ch2LabScene::draw)")
        .tag("场景").tag("第二章").tag("动画")
        .noCrop()
        .param("advance", "推进帧数", 0, 600, 0, 5)
        .framesN(16)
        .drawFn(drawLab);
}

inline void registerAllBackgrounds() {
    registerCh1Stars();
    registerCh1StarsFull();
    registerCh1Terrain();
    registerCh1Base();
    registerCh1Full();
    registerCorridor();
    registerGate();
    registerGateParts();
    registerLab();
}
