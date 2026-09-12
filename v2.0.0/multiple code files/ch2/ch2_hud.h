#pragma once

#include "../font.h"
#include "../renderer.h"

// ============== HUDBase (shared HUD drawing) ==============
class HUDBase {
public:
    // rightEdge = rightmost pixel of the HUD column (all elements aligned)
    static void drawScore(SDL_Renderer* r, const Font& font, int score, int rightEdge, int y) {
        char buf[32];
        snprintf(buf, sizeof(buf), "SCORE:%-4d", score);
        font.drawString(r, buf, rightEdge - 120, y, 2);  // 10 chars * 12px = 120px
    }
    static void drawHPHearts(SDL_Renderer* r, const Font& font, int hp, int maxHp, int rightEdge, int y) {
        for (int i = 0; i < maxHp; ++i) {
            int hx = rightEdge - 12 - i * 14;  // char is 12px wide at scale 2
            SDL_SetRenderDrawColor(r, (i < hp) ? 255 : 70, 20, 20, 255);
            font.drawChar(r, '*', hx, y, 2);
        }
    }
    static void drawEnergyBar(SDL_Renderer* r, int rightEdge, int y, int w, int h, float fill, bool breathing = false) {
        int x = rightEdge - w;
        SDL_SetRenderDrawColor(r, 30, 30, 30, 255);
        SDL_Rect bg = {x, y, w, h}; SDL_RenderFillRect(r, &bg);
        int fw = (int)(fill * w); if (fw > w) fw = w;
        if (fw > 0) {
            int green;
            if (breathing) {
                float b = 0.6f + 0.4f * (float)std::sin(SDL_GetTicks() * 0.008);
                green = (int)(60 + 195 * b);
            } else {
                green = (fill > 0.9f) ? 255 : (fill > 0.3f ? 180 : 100);
            }
            SDL_SetRenderDrawColor(r, 30, (Uint8)green, 50, 255);
            SDL_Rect fr = {x, y, fw, h}; SDL_RenderFillRect(r, &fr);
        }
    }

    // NightElf white energy bar (drawn under the green pulse bar)
    // mode: 0 = charging (white), 1 = decaying (dim), 2 = triple fire (pulsing cyan)
    static void drawEnergyBarWhite(SDL_Renderer* r, int rightEdge, int y, int w, int h, float fill, int mode) {
        int x = rightEdge - w;
        SDL_SetRenderDrawColor(r, 30, 30, 30, 255);
        SDL_Rect bg = {x, y, w, h}; SDL_RenderFillRect(r, &bg);
        int fw = (int)(fill * w); if (fw > w) fw = w;
        if (fw > 0) {
            int R, G, B;
            if (mode == 2) {
                float b = 0.6f + 0.4f * (float)std::sin(SDL_GetTicks() * 0.012);
                R = 120 + (int)(120 * b); G = 200 + (int)(55 * b); B = 255;
            } else if (mode == 0) {
                R = 210; G = 210; B = 230;
            } else {
                R = 120; G = 120; B = 135;
            }
            SDL_SetRenderDrawColor(r, (Uint8)R, (Uint8)G, (Uint8)B, 255);
            SDL_Rect fr = {x, y, fw, h}; SDL_RenderFillRect(r, &fr);
        }
    }

    // Generic boss HP bar (name rendered left of the bar, scale 2)
    static void drawBossBar(SDL_Renderer* r, const Font& font, const char* name,
                            int hp, int maxHp, int x, int y, int w, int h) {
        SDL_SetRenderDrawColor(r, 30, 30, 30, 255);
        SDL_Rect bg = {x, y, w, h}; SDL_RenderFillRect(r, &bg);
        int fw = (int)((double)hp / maxHp * w);
        if (fw > w) fw = w;
        if (fw > 0) {
            SDL_SetRenderDrawColor(r, 220, 30, 30, 255);
            SDL_Rect fr = {x, y, fw, h}; SDL_RenderFillRect(r, &fr);
        }
        SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
        SDL_Rect border = {x - 1, y - 1, w + 2, h + 2};
        SDL_RenderDrawRect(r, &border);
        if (name && name[0]) {
            int nameLen = (int)strlen(name);
            int nameX = x - nameLen * 12 - 8;
            if (nameX < 4) nameX = 4;
            for (int i = 0; name[i]; ++i) {
                SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
                font.drawChar(r, name[i], nameX + i * 12, y + 1, 2);
            }
        }
    }
};
