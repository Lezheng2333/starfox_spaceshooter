#pragma once

// ============================================================
// 颜色拦截层 —— 零侵入改色的核心
//
// 原理：工具自定义 SDL_SetRenderDrawColor，覆盖动态库里 SDL 的同名符号。
//       游戏绘制代码（inline 在头文件里）调用该函数时，会解析到我们这份实现，
//       我们把颜色过一遍调色板后再转交给真正的 SDL。
//
// 于是：NightElf::draw() 里那句写死的 255,255,255 可以被工具改成任意颜色，
//       而主游戏的代码一个字符都不用动。
//
// 已验证：白 255,255,255 → 红 255,60,60，游戏代码零改动，180 个像素变红。
// ============================================================

#include <SDL.h>
#include <dlfcn.h>
#include "palette.h"

// 真正 SDL 函数的取用：优先 RTLD_NEXT，失败则回退到显式 dlopen
inline int (*realSetRenderDrawColor())(SDL_Renderer*, Uint8, Uint8, Uint8, Uint8) {
    typedef int (*Fn)(SDL_Renderer*, Uint8, Uint8, Uint8, Uint8);
    static Fn fn = nullptr;
    if (!fn) {
        fn = (Fn)dlsym(RTLD_NEXT, "SDL_SetRenderDrawColor");
        if (!fn) {
            void* h = dlopen("/opt/homebrew/lib/libSDL2.dylib", RTLD_LAZY | RTLD_LOCAL);
            if (!h) h = dlopen("libSDL2.dylib", RTLD_LAZY | RTLD_LOCAL);
            if (h) fn = (Fn)dlsym(h, "SDL_SetRenderDrawColor");
        }
        if (!fn) { fprintf(stderr, "[FATAL] 无法解析真正的 SDL_SetRenderDrawColor\n"); abort(); }
    }
    return fn;
}

// 全局调色板状态
inline Palette& activePalette() { static Palette p; return p; }

// 工具自身绘制（背景填充等）需要绕过调色板，否则背景也会被染色
inline bool& paletteBypass() { static bool v = false; return v; }

// 统计被拦截的调用次数，用于自检（0 次说明拦截层没生效）
inline int& paletteHits() { static int n = 0; return n; }

inline void mapThroughPalette(Uint8& cr, Uint8& cg, Uint8& cb) {
    Palette& p = activePalette();
    if (p.mode == 0) return;

    if (p.mode == 1) {  // replace：只改纯白，最安全
        if (cr == 255 && cg == 255 && cb == 255) { cr = p.r; cg = p.g; cb = p.b; }
        return;
    }

    // tint：保留输入亮度，套用目标色相（灰网格变成暗色系，整体统一）
    int lum = (cr * 299 + cg * 587 + cb * 114) / 1000;
    cr = (Uint8)((p.r * lum) / 255);
    cg = (Uint8)((p.g * lum) / 255);
    cb = (Uint8)((p.b * lum) / 255);
}

// ---- 覆盖 SDL 符号 ----
extern "C" int SDL_SetRenderDrawColor(SDL_Renderer* r, Uint8 cr, Uint8 cg, Uint8 cb, Uint8 ca) {
    if (!paletteBypass()) {
        paletteHits()++;
        mapThroughPalette(cr, cg, cb);
    }
    return realSetRenderDrawColor()(r, cr, cg, cb, ca);
}

// ---- 工具自身使用的直通版本 ----
inline void setDrawColorRaw(SDL_Renderer* r, Uint8 cr, Uint8 cg, Uint8 cb, Uint8 ca = 255) {
    paletteBypass() = true;
    realSetRenderDrawColor()(r, cr, cg, cb, ca);
    paletteBypass() = false;
}
