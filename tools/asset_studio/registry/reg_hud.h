#pragma once
// ============================================================
// HUD / UI 元素
// 这些方法的参数正好就是函数签名，所以预览可以 1:1 映射成滑块。
// ============================================================

#include "reg_common.h"
#include "font.h"
#include "aim_assist.h"
#include "dialogue.h"
#include "ch2/ch2_hud.h"
#include "ui.h"
#include "floating_text.h"
#include "dialogue.h"
#include "narration.h"

// ---------------- 分数 -
static void drawHudScore(DrawCtx& c) {
    Font f;
    HUDBase::drawScore(c.r, f, (int)c.P("score", 1234),
                       (int)c.P("rightEdge", 760), (int)c.P("y", 30));
}
inline void registerHudScore() {
    reg("shared.hud.score", "HUD · 分数", "shared", "hud")
        .describe("右上角分数（5x7 位图字体，scale=2）。")
        .from("v2.0.0/multiple code files/ch2/ch2_hud.h:10 (HUDBase::drawScore)")
        .tag("HUD").tag("共享").tag("字体")
        .param("score",     "分数值",   0, 999999, 1234, 1)
        .param("rightEdge", "右边界",   0,    800,  760, 1)
        .param("y",         "垂直位置", 0,    600,   30, 1)
        .drawFn(drawHudScore);
}

// ---------------- 爱心血量 -
static void drawHudHearts(DrawCtx& c) {
    Font f;
    int maxHp = (int)c.P("maxHp", 3);
    int hp = (int)c.P("hp", 3);
    if (hp > maxHp) hp = maxHp;
    HUDBase::drawHPHearts(c.r, f, hp, maxHp,
                          (int)c.P("rightEdge", 760), (int)c.P("y", 30));
}
inline void registerHudHearts() {
    reg("shared.hud.hearts", "HUD · 爱心血量", "shared", "hud")
        .describe("爱心血量条。拖动「当前血量」可以从满血看到空血。")
        .from("v2.0.0/multiple code files/ch2/ch2_hud.h:15 (HUDBase::drawHPHearts)")
        .tag("HUD").tag("共享").tag("血量")
        .param("hp",        "当前血量", 0, 8, 3, 1)
        .param("maxHp",     "血量上限", 1, 8, 3, 1)
        .param("rightEdge", "右边界",   0, 800, 760, 1)
        .param("y",         "垂直位置", 0, 600, 30, 1)
        .drawFn(drawHudHearts);
}

// ---------------- 绿色能量条 -
static void drawHudEnergy(DrawCtx& c) {
    HUDBase::drawEnergyBar(c.r, (int)c.P("rightEdge", 690), (int)c.P("y", 50),
                           (int)c.P("w", 110), (int)c.P("h", 12),
                           (float)(c.P("fill", 0.6) / 100.0),
                           c.P("breathing", 0) > 0.5);
}
inline void registerHudEnergy() {
    reg("shared.hud.energy", "HUD · 绿色能量条", "shared", "hud")
        .describe("脉冲技能能量条。「呼吸」开关对应游戏里能量满时的脉动效果。")
        .from("v2.0.0/multiple code files/ch2/ch2_hud.h:22 (HUDBase::drawEnergyBar)")
        .tag("HUD").tag("共享").tag("技能")
        .param("fill",      "充能百分比", 0, 100, 60, 1)
        .param("breathing", "呼吸效果",   0,   1,  0, 1)
        .param("rightEdge", "右边界",     0, 800, 690, 1)
        .param("y",         "垂直位置",   0, 600,  50, 1)
        .param("w",         "宽度",      20, 300, 110, 1)
        .param("h",         "高度",       4,  40,  12, 1)
        .drawFn(drawHudEnergy);
}

// ---------------- 白色能量条（暗夜精灵三炮模式）----------------
static void drawHudEnergyWhite(DrawCtx& c) {
    HUDBase::drawEnergyBarWhite(c.r, (int)c.P("rightEdge", 690), (int)c.P("y", 70),
                                (int)c.P("w", 110), (int)c.P("h", 10),
                                (float)(c.P("fill", 0.5) / 100.0),
                                (int)c.P("mode", 0));
}
inline void registerHudEnergyWhite() {
    reg("shared.hud.energy_white", "HUD · 白色能量条", "shared", "hud")
        .describe("暗夜精灵号三炮模式的充能条。「模式」对应三炮激活的不同阶段配色。")
        .from("v2.0.0/multiple code files/ch2/ch2_hud.h:42 (HUDBase::drawEnergyBarWhite)")
        .tag("HUD").tag("共享").tag("三炮模式")
        .param("fill",      "充能百分比", 0, 100, 50, 1)
        .param("mode",      "模式",       0,   3,  0, 1)
        .param("rightEdge", "右边界",     0, 800, 690, 1)
        .param("y",         "垂直位置",   0, 600,  70, 1)
        .param("w",         "宽度",      20, 300, 110, 1)
        .param("h",         "高度",       4,  40,  10, 1)
        .drawFn(drawHudEnergyWhite);
}

// ---------------- Boss 血条 -
static void drawHudBossBar(DrawCtx& c) {
    Font f;
    HUDBase::drawBossBar(c.r, f, (c.P("showName", 1) > 0.5 ? "TELAMONDO" : ""),
                         (int)c.P("hp", 700), (int)c.P("maxHp", 1000),
                         (int)c.P("x", 300), (int)c.P("y", 90),
                         (int)c.P("w", 400), (int)c.P("h", 14));
}
inline void registerHudBossBar() {
    reg("shared.hud.bossbar", "HUD · Boss 血条", "shared", "hud")
        .describe("Boss 血条（红条 + 灰底 + 描边 + 左侧 Boss 名）。"
                  "拖动「当前血量」可以看到血条收缩。")
        .from("v2.0.0/multiple code files/ch2/ch2_hud.h:63 (HUDBase::drawBossBar)")
        .tag("HUD").tag("共享").tag("Boss")
        .param("hp",       "当前血量", 0, 1000, 700, 10)
        .param("maxHp",    "血量上限", 1, 2000, 1000, 10)
        .param("x",        "左边界",   0,  800, 300, 1)
        .param("y",        "垂直位置", 0,  600,  90, 1)
        .param("w",        "宽度",    40,  780, 400, 5)
        .param("h",        "高度",     4,   40,  14, 1)
        .param("showName", "显示名称", 0,    1,   1, 1)
        .drawFn(drawHudBossBar);
}

// ---------------- 位图字体字库 -
static void drawFontAtlas(DrawCtx& c) {
    Font f;
    int scale = (int)c.P("scale", 2);
    int x = (int)c.P("posX", 40), y = (int)c.P("posY", 60);
    const char* rows[] = {
        "0123456789",
        "ABCDEFGHIJ",
        "KLMNOPQRST",
        "UVWXYZ",
        "-+=:.,!?/()%*#@",
        "VER 1.2.23  SCORE 9999"
    };
    SDL_SetRenderDrawColor(c.r, 255, 255, 255, 255);
    int lineH = 10 * scale;
    for (int i = 0; i < 6; ++i)
        f.drawString(c.r, rows[i], x, y + i * lineH, scale);
}
inline void registerFontAtlas() {
    reg("shared.font.atlas", "位图字体字库", "shared", "hud")
        .describe("游戏的 5x7 位图字体全字库。这是所有文字（分数、对话、旁白、Boss 名）的"
                  "字形来源，改这里会影响全游戏的文字观感。")
        .from("v2.0.0/multiple code files/font.h:128 (Font::drawString, Font::drawChar, Font::drawCharFloat)")
        .tag("字体").tag("共享").tag("字库")
        .param("scale", "字号",      1, 6, 2, 1)
        .param("posX",  "水平位置",  0, 800, 40, 1)
        .param("posY",  "垂直位置",  0, 600, 60, 1)
        .drawFn(drawFontAtlas);
}

// ---------------- 对话文本行 -
static void drawDialogueLine(DrawCtx& c) {
    Font f;
    std::string txt = "STATUS REPORT: SHIELD INTEGRITY 87%";
    drawTextLine(c.r, f, txt, (int)c.P("posX", 60), (int)c.P("posY", 300),
                 (int)c.P("scale", 2), (int)c.P("mul", 1), (int)c.P("charW", 12));
}
inline void registerDialogueLine() {
    reg("shared.text.dialogueline", "对话文本行", "shared", "hud")
        .describe("对话框里的文本绘制函数（drawTextLine）。逐字符渲染，"
                  "是对话界面排版的基础。")
        .from("v2.0.0/multiple code files/dialogue.h:264 (drawTextLine)")
        .tag("字体").tag("共享").tag("对话")
        .param("scale", "字号",     1, 4, 2, 1)
        .param("mul",   "字宽倍率", 1, 4, 1, 1)
        .param("charW", "字符间距", 4, 40, 12, 1)
        .param("posX",  "水平位置", 0, 800, 60, 1)
        .param("posY",  "垂直位置", 0, 600, 300, 1)
        .drawFn(drawDialogueLine);
}

// ---------------- 瞄准辅助准星 -
static void drawAimAssist(DrawCtx& c) {
    AimAssist aa;
    int snaps = (int)(c.P("snap", 0) / 5.0);   // update(true) 每次 +0.05
    for (int i = 0; i < snaps; ++i) aa.update(true);
    aa.draw(c.r, c.P("posX", 400), c.P("posY", 300),
            (int)c.P("dotBig", 6), (int)c.P("dotSmall", 3));
}
inline void registerAimAssist() {
    reg("shared.fx.aimassist", "瞄准辅助准星", "shared", "fx")
        .describe("四角括号式准星，锁定时向内收缩并中心点变大。"
                  "拖动「锁定进度」可以看到从搜索到锁定的完整过程。"
                  "锁定进度 50% 是中心点变大的分界。")
        .from("v2.0.0/multiple code files/aim_assist.h:19 (AimAssist::draw)")
        .tag("特效").tag("共享").tag("准星")
        .param("snap",     "锁定进度%", 0, 100, 0, 5)
        .param("posX",     "水平位置",  0, 800, 400, 1)
        .param("posY",     "垂直位置",  0, 600, 300, 1)
        .param("dotBig",   "锁定点大小", 2, 14, 6, 1)
        .param("dotSmall", "搜索点大小", 1, 10, 3, 1)
        .drawFn(drawAimAssist);
}

// ---------------- UI 元件 ----------------
static void drawUiCursor(DrawCtx& c) {
    UIRenderer::drawMenuCursor(c.r, (int)c.P("x", 200), (int)c.P("y", 300), (int)c.P("size", 10));
}
inline void registerUiCursor() {
    reg("shared.ui.cursor", "UI · 菜单光标", "shared", "hud")
        .describe("菜单里指向当前选项的黄色三角箭头。")
        .from("v2.0.0/multiple code files/ui.h:8 (UIRenderer::drawMenuCursor)")
        .tag("UI").tag("共享").tag("菜单")
        .param("x",    "水平位置", 0, 800, 200, 1)
        .param("y",    "垂直位置", 0, 600, 300, 1)
        .param("size", "尺寸",     4,  40,  10, 1)
        .drawFn(drawUiCursor);
}

static void drawUiUnderline(DrawCtx& c) {
    UIRenderer::drawMenuUnderline(c.r, (int)c.P("x", 150), (int)c.P("y", 300), (int)c.P("w", 200));
}
inline void registerUiUnderline() {
    reg("shared.ui.underline", "UI · 菜单下划线", "shared", "hud")
        .describe("选中项下面的黄色横线。")
        .from("v2.0.0/multiple code files/ui.h:16 (UIRenderer::drawMenuUnderline)")
        .tag("UI").tag("共享").tag("菜单")
        .param("x", "水平位置", 0, 800, 150, 1)
        .param("y", "垂直位置", 0, 600, 300, 1)
        .param("w", "宽度",    10, 780, 200, 5)
        .drawFn(drawUiUnderline);
}

static void drawUiSlider(DrawCtx& c) {
    UIRenderer::drawSlider(c.r, (int)c.P("x", 200), (int)c.P("y", 300), (int)c.P("w", 300),
                           (int)c.P("val", 7), (int)c.P("lo", 0), (int)c.P("hi", 10),
                           c.P("sym", 0) > 0.5);
}
inline void registerUiSlider() {
    reg("shared.ui.slider", "UI · 设置滑块", "shared", "hud")
        .describe("设置菜单里的音量/画质滑块。「对称」开关对应需要以中点为零的选项。")
        .from("v2.0.0/multiple code files/ui.h:21 (UIRenderer::drawSlider)")
        .tag("UI").tag("共享").tag("设置")
        .param("x",   "水平位置", 0, 800, 200, 1)
        .param("y",   "垂直位置", 0, 600, 300, 1)
        .param("w",   "宽度",    20, 700, 300, 5)
        .param("val", "当前值",   0,  20,   7, 1)
        .param("lo",  "最小值",   0,  20,   0, 1)
        .param("hi",  "最大值",   1,  20,  10, 1)
        .param("sym", "对称模式", 0,   1,   0, 1)
        .drawFn(drawUiSlider);
}

static void drawUiOverlay(DrawCtx& c) {
    UIRenderer::drawHalfTransparentOverlay(c.r, (int)c.P("alpha", 128));
}
inline void registerUiOverlay() {
    reg("shared.ui.overlay", "UI · 半透明遮罩", "shared", "hud")
        .describe("暂停 / 设置界面背后的横条纹半透明遮罩（每 3 像素画 2 像素黑条）。")
        .from("v2.0.0/multiple code files/ui.h:49 (UIRenderer::drawHalfTransparentOverlay)")
        .tag("UI").tag("共享").tag("菜单")
        .noCrop()
        .param("alpha", "遮罩不透明度", 0, 255, 128, 8)
        .drawFn(drawUiOverlay);
}

// ---------------- 浮动文字 ----------------
// 注意：FloatingTextManager::draw() 其实是空实现 —— 它只设置绘制颜色，
// 一个像素都不画，注释写着 "Use external font reference — handled in Game::draw"。
// 真正画字的是 Game::drawFloatingTexts()。这里忠实复刻它的画法（用公有的 all() 读数据）。
struct FloatTextHost {
    FloatingTextManager ft;
    Font font;
};

static void drawFloatingText(DrawCtx& c) {
    FloatTextHost* st = (FloatTextHost*)c.priv;
    if (!st) {
        st = new FloatTextHost();
        c.priv = st;
        st->ft.spawn((float)c.P("posX", 400), (float)c.P("posY", 300), "100",
                     (int)c.P("cr", 50), (int)c.P("cg", 255), (int)c.P("cb", 80));
    }
    for (int i = 0; i < (int)c.P("advance", 10); ++i) st->ft.update();

    int scale = (int)c.P("scale", 3);
    for (size_t i = 0; i < st->ft.all().size(); ++i) {
        const FloatingText& ft = st->ft.all()[i];
        float t = (float)ft.life / ft.totalLife;
        if (t < 0.05f) continue;
        SDL_SetRenderDrawColor(c.r, (Uint8)(ft.r * t), (Uint8)(ft.g * t), (Uint8)(ft.b * t), 255);
        int cx = (int)(ft.x - strlen(ft.text) * 6 * scale / 2);
        for (const char* p = ft.text; *p; ++p) {
            if (*p != ' ') st->font.drawChar(c.r, *p, cx, (int)ft.y, scale);
            cx += 6 * scale;
        }
    }
}

inline void registerFloatingText() {
    reg("shared.text.floating", "浮动分数文字", "shared", "hud")
        .describe("击杀敌人时从敌人位置飘起来并淡出的分数文字。"
                  "拖动「推进帧数」可以看到它上升 + 淡出的全过程。"
                  "（FloatingTextManager::draw() 本体是空实现，这里复刻的是 Game::drawFloatingTexts 的真实画法。）")
        .from("v2.0.0/multiple code files/game.h:3651 (Game::drawFloatingTexts)")
        .tag("字体").tag("共享").tag("动画")
        .param("posX",    "水平位置", 0, 800, 400, 1)
        .param("posY",    "垂直位置", 0, 600, 300, 1)
        .param("cr",      "红",       0, 255,  50, 5)
        .param("cg",      "绿",       0, 255, 255, 5)
        .param("cb",      "蓝",       0, 255,  80, 5)
        .param("scale",   "字号",     1, 6, 3, 1)
        .param("advance", "推进帧数", 0, 120, 10, 1)
        .framesN(12)
        .drawFn(drawFloatingText);
}

// ---------------- 对话框 ----------------
// 文本用的是游戏里的真实台词（取自 game.h 的 queueDialogue 调用），
// 这样预览看到的排版宽度就是实际游戏里的效果。
struct DialogueHost {
    DialogueSystem dlg;
    Font font;
};

static void drawDialogueBox(DrawCtx& c) {
    DialogueHost* st = (DialogueHost*)c.priv;
    if (!st) {
        st = new DialogueHost();
        c.priv = st;
        st->dlg.reset();
        if (c.S("tower")) {
            st->dlg.queueDialogue("Tower (ai)", "Shockwave cannon ready.");
        } else if (c.S("long")) {
            st->dlg.queueDialogue("Ally (ai copilot)",
                                  "Hold on as long as you can. The base shockwave cannon is charging.");
        } else {
            st->dlg.queueDialogue("Ally (ai copilot)", "Martha, you're the only one in the air.");
        }
        st->dlg.start();
    }
    // 逐字打字机效果：拖动「推进帧数」可以看到文字一个个冒出来
    for (int i = 0; i < (int)c.P("advance", 90); ++i) st->dlg.update(false);
    st->dlg.draw(c.r, st->font);
}

inline void registerDialogueBox() {
    reg("shared.ui.dialogue", "对话框", "shared", "hud")
        .describe("游戏左下角的对话框：说话人 + 逐字打出正文 + 底部继续提示。"
                  "文本用的是游戏真实台词。「推进帧数」控制打字机进度——"
                  "拖到 0 是刚开口，拉到 90 以上一句话打完。")
        .from("v2.0.0/multiple code files/dialogue.h:220 (DialogueSystem::draw)")
        .tag("UI").tag("共享").tag("对话").tag("打字机")
        .state_("short", "短句")
        .state_("long",  "长句（看换行排版）")
        .state_("tower", "塔台通讯（无头像）")
        .param("advance", "推进帧数(打字进度)", 0, 240, 90, 2)
        .drawFn(drawDialogueBox);
}

// ---------------- 旁白框 ----------------
struct NarrationHost {
    NarrationSystem nar;
    Font font;
};

static void drawNarrationBox(DrawCtx& c) {
    NarrationHost* st = (NarrationHost*)c.priv;
    if (!st) {
        st = new NarrationHost();
        c.priv = st;
        st->nar.reset();
        if (c.S("moonwell"))
            st->nar.queue("Martha takes one last look at Moonwell.\nThe engines roar to life.");
        else
            st->nar.queue("The Warden falls.\nThe lab falls silent.");
        st->nar.start();
    }
    for (int i = 0; i < (int)c.P("advance", 60); ++i) st->nar.update(false);
    st->nar.draw(c.r, st->font);
}

inline void registerNarrationBox() {
    reg("shared.ui.narration", "旁白框", "shared", "hud")
        .describe("章节开场/结尾的居中旁白：整段文字逐字打出，可多行。"
                  "「推进帧数」控制打字机进度。多行文本的换行位置由文本里的 \\n 决定。")
        .from("v2.0.0/multiple code files/narration.h:121 (NarrationSystem::draw)")
        .tag("UI").tag("共享").tag("旁白").tag("打字机")
        .state_("warden",   "两行旁白")
        .state_("moonwell", "另一个章节的两行旁白")
        .param("advance", "推进帧数(打字进度)", 0, 300, 60, 2)
        .framesN(20)
        .drawFn(drawNarrationBox);
}

inline void registerAllHud() {
    registerHudScore();
    registerHudHearts();
    registerHudEnergy();
    registerHudEnergyWhite();
    registerHudBossBar();
    registerFontAtlas();
    registerDialogueLine();
    registerAimAssist();
    registerUiCursor();
    registerUiUnderline();
    registerUiSlider();
    registerUiOverlay();
    registerFloatingText();
    registerDialogueBox();
    registerNarrationBox();
}
