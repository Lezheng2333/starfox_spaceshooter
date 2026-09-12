#pragma once

// ============================================================
// 草稿：暗夜精灵号 · 参数化变体
//
// 这是「克隆为草稿」产出的东西长什么样的实例示范。
// 它和主游戏完全隔离：这里怎么改都不会影响游戏。
//
// 相比主游戏里写死常量的 NightElf::draw()，这里把每个尺寸都变成了
// 参数结构体的字段 —— 画廊会自动为每个字段生成一个滑块。
//
// 调好之后告诉我，我把数值代回主游戏的代码。
// ============================================================

#include "preview_harness.h"

struct NightElfWideParams {
    double posX     = 400;   // 画布位置
    double posY     = 300;
    double noseLen  = 30;    // 机头长度（主游戏写死 30）
    double wingSpan = 8;     // 半翼展（主游戏写死 8）
    double tailLen  = 5;     // 尾部后掠（主游戏写死 5）
    double wingBack = 2;     // 翼根相对机身中心的后移（主游戏写死 2）
    double fillStep = 2;     // 机身填充的竖线间距（主游戏写死 2）
};

inline void draw_nightelf_wide(DrawCtx& c) {
    NightElfWideParams p;
    p.posX     = c.P("posX",     400);
    p.posY     = c.P("posY",     300);
    p.noseLen  = c.P("noseLen",   30);
    p.wingSpan = c.P("wingSpan",   8);
    p.tailLen  = c.P("tailLen",    5);
    p.wingBack = c.P("wingBack",   2);
    p.fillStep = c.P("fillStep",   2);

    // 填充模式由状态切换器控制：half = 只填上半（主游戏现状），full = 上下对称
    bool fillBottom = c.S("full");

    SDL_Renderer* r = c.r;
    int px = (int)p.posX, py = (int)p.posY;

    int wingX = px + (int)p.wingBack;
    int noseX = wingX + (int)p.noseLen;
    int tailX = wingX + (int)p.tailLen;
    int ws    = (int)p.wingSpan;

    // 交给调色板层处理：写纯白，工具可以把它映射成任意阵营色
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);

    // 机身轮廓：机头 → 上翼 → 尾 → 下翼 → 回机头
    SDL_Point body[5] = {
        {noseX, py},
        {wingX, py - ws},
        {tailX, py},
        {wingX, py + ws},
        {noseX, py}
    };
    SDL_RenderDrawLines(r, body, 5);

    // 翼尖向前延伸的平行线（比机头短 3 像素）
    SDL_RenderDrawLine(r, wingX, py - ws, noseX - 3, py - ws);
    SDL_RenderDrawLine(r, wingX, py + ws, noseX - 3, py + ws);

    // 机身填充
    if (noseX != wingX) {
        int step = (int)p.fillStep;
        if (step < 1) step = 1;

        for (int sx = wingX; sx <= noseX; sx += step) {
            double tn = (double)(sx - wingX) / (double)(noseX - wingX);
            int uy = (py - ws) + (int)(ws * tn);

            int ly;
            if (sx <= tailX && tailX != wingX) {
                double tt = (double)(sx - wingX) / (double)(tailX - wingX);
                ly = (py - ws) + (int)(ws * tt);
            } else {
                ly = py;
            }
            SDL_RenderDrawLine(r, sx, uy, sx, ly);

            // 上下对称填充：主游戏目前只填上半，切到 full 就能看到差别
            if (fillBottom) {
                SDL_RenderDrawLine(r, sx, 2 * py - ly, sx, 2 * py - uy);
            }
        }
    }
}

inline void register_nightelf_wide() {
    reg("draft.nightelf_wide", "暗夜精灵号 · 参数化变体", "draft", "player")
        .describe("NightElf 的可调版本，用来看「只填上半机身」和「上下对称填充」的差别，"
                  "以及机头长度/翼展/后掠对造型的影响。"
                  "这是草稿区的示范：改这里不会影响主游戏。")
        .from("drafts/nightelf_wide/draw.h")
        .tag("草稿").tag("参数化").tag("战机")
        .state_("half",  "上半填充（游戏现状）")
        .state_("full",  "上下对称填充")
        .param("posX",     "水平位置",   0,   800, 400, 1)
        .param("posY",     "垂直位置",   0,   600, 300, 1)
        .param("noseLen",  "机头长度",  10,    60,  30, 1)
        .param("wingSpan", "半翼展",     2,    20,   8, 1)
        .param("tailLen",  "尾部后掠",   0,    20,   5, 1)
        .param("wingBack", "翼根后移",   0,    15,   2, 1)
        .param("fillStep", "填充间距",   1,     6,   2, 1)
        .drawFn(draw_nightelf_wide);
}
