#pragma once

// ============================================================
// 文本化检视通道 —— 让 AI 在看不到图片时仍能闭环验证素材
//
// 两条通道：
//   1. 字符画（ASCII）：把渲染结果降采样成文本，形状/比例/对称性肉眼可读
//   2. 像素分析：包围盒、内容像素数、线宽估计、左右对称度、颜色分布
//
// 关键实现细节：降采样用「块内最大值」而不是平均值。
// 线稿只有 1 像素宽，用平均值会让细线直接消失（整张图变空白）。
// ============================================================

#include <SDL.h>
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <cmath>

struct AsAnalysis {
    bool   empty = true;
    int    bboxX = 0, bboxY = 0, bboxW = 0, bboxH = 0;
    int    contentPixels = 0;
    double fillRatio = 0.0;      // 内容像素 / 包围盒面积
    double symmetryLR = 0.0;     // 左右镜像重合率 0..1（俯视素材看这个）
    double symmetryTB = 0.0;     // 上下镜像重合率 0..1（侧视素材看这个）
    double symmetry = 0.0;       // 两者取大：粗略判断"这素材对称吗"
    int    lineWidth = 0;        // 线宽估计（内容像素 / 骨架长度 的近似）
    std::vector<std::pair<unsigned int, int>> topColors;  // ARGB -> 像素数
};

inline bool asIsBackground(unsigned int p, unsigned int bg) {
    return (p & 0x00FFFFFFu) == (bg & 0x00FFFFFFu);
}

inline AsAnalysis asAnalyze(const unsigned int* px, int w, int h, unsigned int bg) {
    AsAnalysis a;
    int minX = w, minY = h, maxX = -1, maxY = -1;
    std::map<unsigned int, int> colors;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned int p = px[(size_t)y * w + x];
            if (asIsBackground(p, bg)) continue;
            ++a.contentPixels;
            if (x < minX) minX = x;
            if (x > maxX) maxX = x;
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
            colors[p & 0x00FFFFFFu]++;
        }
    }

    if (a.contentPixels == 0) return a;
    a.empty   = false;
    a.bboxX   = minX;
    a.bboxY   = minY;
    a.bboxW   = maxX - minX + 1;
    a.bboxH   = maxY - minY + 1;
    a.fillRatio = (double)a.contentPixels / (double)(a.bboxW * a.bboxH);

    // 左右镜像重合率（俯视素材看这个；侧视素材本来就不该左右对称）
    int matchLR = 0, total = 0;
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            bool l = !asIsBackground(px[(size_t)y * w + x], bg);
            int mx = minX + maxX - x;
            bool r = !asIsBackground(px[(size_t)y * w + mx], bg);
            ++total;
            if (l == r) ++matchLR;
        }
    }
    a.symmetryLR = total > 0 ? (double)matchLR / (double)total : 0.0;

    // 上下镜像重合率（侧视素材看这个）
    int matchTB = 0;
    for (int y = minY; y <= maxY; ++y) {
        int my = minY + maxY - y;
        for (int x = minX; x <= maxX; ++x) {
            bool u = !asIsBackground(px[(size_t)y * w + x], bg);
            bool d = !asIsBackground(px[(size_t)my * w + x], bg);
            if (u == d) ++matchTB;
        }
    }
    a.symmetryTB = total > 0 ? (double)matchTB / (double)total : 0.0;
    a.symmetry = std::max(a.symmetryLR, a.symmetryTB);

    // 线宽估计：内容像素数 / 包围盒对角线长度（斜线近似）
    double diag = std::sqrt((double)(a.bboxW * a.bboxW + a.bboxH * a.bboxH));
    a.lineWidth = diag > 0 ? (int)std::lround((double)a.contentPixels / diag) : 0;

    for (std::map<unsigned int, int>::iterator it = colors.begin(); it != colors.end(); ++it)
        a.topColors.push_back(*it);
    std::sort(a.topColors.begin(), a.topColors.end(),
              [](const std::pair<unsigned int, int>& A, const std::pair<unsigned int, int>& B) {
                  return A.second > B.second;
              });
    if (a.topColors.size() > 6) a.topColors.resize(6);
    return a;
}

// 字符画：块内取最大亮度，保证 1 像素细线不会消失
inline std::string asAsciiArt(const unsigned int* px, int w, int h,
                              int cols, unsigned int bg, bool cropToContent = true) {
    AsAnalysis a = asAnalyze(px, w, h, bg);
    int x0 = 0, y0 = 0, x1 = w - 1, y1 = h - 1;
    if (cropToContent && !a.empty) {
        int pad = 1;
        x0 = std::max(0, a.bboxX - pad); y0 = std::max(0, a.bboxY - pad);
        x1 = std::min(w - 1, a.bboxX + a.bboxW - 1 + pad);
        y1 = std::min(h - 1, a.bboxY + a.bboxH - 1 + pad);
    }
    int cw = x1 - x0 + 1, ch = y1 - y0 + 1;
    if (cw <= 0 || ch <= 0) return "(空白)\n";

    if (cols <= 0) cols = std::min(110, cw);   // 自动：小素材 1 字符 = 1 像素
    if (cols < 1) cols = 1;

    int rows;
    if (cols >= cw) {
        rows = ch;                              // 1:1，细节无损，不做上采样
    } else {
        // 终端字符高宽比约 2:1，降采样时行数按一半算才不变形
        rows = std::max(1, (int)std::lround((double)ch / ((double)cw / cols) * 0.5));
    }
    if (rows > 80) rows = 80;

    const char* ramp = " .:-=+*#%@";
    std::string out;
    char line[512];

    for (int r = 0; r < rows; ++r) {
        int sy0 = y0 + (int)((long long)r * ch / rows);
        int sy1 = y0 + (int)((long long)(r + 1) * ch / rows);
        if (sy1 <= sy0) sy1 = sy0 + 1;
        for (int c = 0; c < cols; ++c) {
            int sx0 = x0 + (int)((long long)c * cw / cols);
            int sx1 = x0 + (int)((long long)(c + 1) * cw / cols);
            if (sx1 <= sx0) sx1 = sx0 + 1;

            int bestLum = -1;
            for (int y = sy0; y < sy1 && y <= y1; ++y) {
                for (int x = sx0; x < sx1 && x <= x1; ++x) {
                    unsigned int p = px[(size_t)y * w + x];
                    if (asIsBackground(p, bg)) continue;
                    int lum = (int)((p >> 16 & 0xFF) * 299 + (p >> 8 & 0xFF) * 587 + (p & 0xFF) * 114) / 1000;
                    if (lum > bestLum) bestLum = lum;
                }
            }
            out += bestLum < 0 ? ' ' : ramp[std::min(9, std::max(1, bestLum * 10 / 256))];
        }
        snprintf(line, sizeof(line), "  |%d\n", sy0);
        out += line;
    }
    return out;
}

inline std::string asAnalysisText(const AsAnalysis& a, int w, int h) {
    char buf[1024];
    if (a.empty) return "  内容: 空白（渲染结果全为背景色）\n";

    std::string s;
    snprintf(buf, sizeof(buf), "  画布: %dx%d\n", w, h); s += buf;
    snprintf(buf, sizeof(buf), "  包围盒: x=%d y=%d w=%d h=%d\n", a.bboxX, a.bboxY, a.bboxW, a.bboxH); s += buf;
    snprintf(buf, sizeof(buf), "  内容像素: %d   填充率: %.1f%%   线宽估计: %d px\n",
             a.contentPixels, a.fillRatio * 100.0, a.lineWidth); s += buf;
    snprintf(buf, sizeof(buf), "  镜像对称  左右: %.1f%%   上下: %.1f%%\n",
             a.symmetryLR * 100.0, a.symmetryTB * 100.0); s += buf;
    s += "  主要颜色:";
    for (size_t i = 0; i < a.topColors.size(); ++i) {
        unsigned int c = a.topColors[i].first;
        snprintf(buf, sizeof(buf), "  #%02X%02X%02X(%d)",
                 (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF, a.topColors[i].second);
        s += buf;
    }
    s += "\n";
    return s;
}
