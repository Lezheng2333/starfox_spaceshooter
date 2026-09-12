#pragma once

// ============================================================
// 调色板 —— 阵营配色 / 颜色实验
//
// 游戏素材几乎全是纯白线框（SDL_SetRenderDrawColor(r,255,255,255,255)）。
// 通过 color_intercept.h 接管该调用，把颜色过一遍调色板再交给真正的 SDL，
// 于是「改配色」不需要动主游戏一个字符。
// ============================================================

#include <SDL.h>
#include <string>

struct Palette {
    std::string name = "none";
    // 0 = none   原样放行
    // 1 = replace 仅把纯白映射为目标色（最安全，只碰主线条）
    // 2 = tint    保留输入亮度、套用目标色相（灰色网格变成暗色系，整体统一）
    int    mode = 0;
    Uint8  r = 255, g = 255, b = 255;
};

inline bool parsePalette(const std::string& spec, Palette& out) {
    if (spec == "none")      { out = Palette(); out.name = "none"; return true; }
    if (spec == "friendly")  { out = Palette(); out.name = "friendly"; return true; }              // 己方白
    if (spec == "enemy")     { out.mode = 1; out.r = 255; out.g =  60; out.b =  60; out.name = "enemy";    return true; }
    if (spec == "skill")     { out.mode = 1; out.r = 120; out.g = 255; out.b = 140; out.name = "skill";    return true; }
    if (spec == "neutral")   { out.mode = 1; out.r = 120; out.g = 220; out.b = 255; out.name = "neutral";  return true; }
    if (spec == "gold")      { out.mode = 1; out.r = 255; out.g = 200; out.b =  80; out.name = "gold";     return true; }
    if (spec == "violet")    { out.mode = 1; out.r = 200; out.g = 140; out.b = 255; out.name = "violet";   return true; }

    // 自定义：--palette 255,60,60        → replace 模式
    //          --palette 255,60,60,tint  → tint 模式
    int rr = 0, gg = 0, bb = 0;
    bool tint = false;
    if (sscanf(spec.c_str(), "%d,%d,%d", &rr, &gg, &bb) == 3) {
        if (spec.find("tint") != std::string::npos) tint = true;
        out = Palette();
        out.mode = tint ? 2 : 1;
        out.r = (Uint8)rr; out.g = (Uint8)gg; out.b = (Uint8)bb;
        out.name = spec;
        return true;
    }
    return false;
}

inline std::string paletteList() {
    return "none, friendly(白), enemy(红), skill(绿), neutral(青), gold(金), violet(紫), 或自定义 R,G,B[,tint]";
}
