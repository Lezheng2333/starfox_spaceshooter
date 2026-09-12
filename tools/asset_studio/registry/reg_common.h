#pragma once
// ============================================================
// 各类别注册文件共用的辅助
// ============================================================

#include "preview_harness.h"
#include "chapter_manager.h"

// ---------------- 炮口标记 ----------------
// 预览增强：显示子弹从哪出膛（游戏里看不到这个信息）。
// 走 bypass 通道画，所以不会被阵营配色改掉。
inline void drawMuzzleMarker(SDL_Renderer* r, int mx, int my) {
    setDrawColorRaw(r, 255, 220, 0, 255);
    SDL_RenderDrawLine(r, mx - 3, my, mx + 3, my);
    SDL_RenderDrawLine(r, mx, my - 3, mx, my + 3);
    setDrawColorRaw(r, 255, 220, 0, 90);
    SDL_RenderDrawLine(r, mx - 6, my, mx + 6, my);
    SDL_RenderDrawLine(r, mx, my - 6, mx, my + 6);
}

// ---------------- 章节配置 ----------------
// Ch1Background / Ch1Boss 都持有 ChapterConfig 的引用或指针，
// 所以配置对象必须比它们活得久 —— 这里用 static 保证。
// 直接复用游戏自己的 ChapterManager，拿到的是真实章节参数。
inline ChapterManager& previewChapterMgr() {
    static ChapterManager cm;
    return cm;
}
