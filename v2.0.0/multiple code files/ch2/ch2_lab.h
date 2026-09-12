#pragma once

#include "../constants.h"
#include "../renderer.h"

// ============== Ch2LabScene (central research lab — NightElf pedestal room) ==============
// Circular room overlay drawn over the (frozen) corridor background.
// The NightElf prototype rests on a pedestal at the room center.
class Ch2LabScene {
public:
    double cx, cy, radius;
    int timer;

    // Pedestal / NightElf parking spot (Game aligns the parked plane here)
    static const int PARK_X = 430;
    static const int PARK_Y = 300;

    Ch2LabScene() { reset(); }

    void reset() { cx = PARK_X; cy = PARK_Y; radius = 250.0; timer = 0; }
    void update() { timer++; }

    // 存档：研究室计时（房间几何是常量，只需存 timer 保证动画帧一致）
    template <class Ar> void visit(Ar& ar) {
        ar.ioNum(cx); ar.ioNum(cy); ar.ioNum(radius); ar.ioNum(timer);
    }

    void draw(SDL_Renderer* r) const {
        // ---- Circular room wall ----
        const int SEG = 72;
        SDL_Point prev;
        // Outer wall (double stroke)
        for (int pass = 0; pass < 2; ++pass) {
            double rr = radius + pass * 3.0;
            int alpha = pass == 0 ? 110 : 60;
            SDL_SetRenderDrawColor(r, 105, 118, 140, (Uint8)alpha);
            for (int i = 0; i <= SEG; ++i) {
                double a = 2.0 * M_PI * i / SEG;
                int sx = (int)(cx + rr * std::cos(a));
                int sy = (int)(cy + rr * std::sin(a));
                if (i > 0) SDL_RenderDrawLine(r, prev.x, prev.y, sx, sy);
                prev = {sx, sy};
            }
        }

        // Radial wall ribs (very dim)
        SDL_SetRenderDrawColor(r, 90, 100, 120, 38);
        for (int i = 0; i < 24; ++i) {
            double a = 2.0 * M_PI * i / 24.0;
            int x0 = (int)(cx + (radius - 18) * std::cos(a));
            int y0 = (int)(cy + (radius - 18) * std::sin(a));
            int x1 = (int)(cx + radius * std::cos(a));
            int y1 = (int)(cy + radius * std::sin(a));
            SDL_RenderDrawLine(r, x0, y0, x1, y1);
        }

        // ---- Rotating light sweep (radar-like) ----
        double sa = timer * 0.012;
        SDL_SetRenderDrawColor(r, 140, 200, 235, 70);
        int sx1 = (int)(cx + (radius - 12) * std::cos(sa));
        int sy1 = (int)(cy + (radius - 12) * std::sin(sa));
        SDL_RenderDrawLine(r, (int)cx, (int)cy, sx1, sy1);

        // ---- Ceiling light cone onto the pedestal ----
        double coneTopY = cy - radius;
        SDL_SetRenderDrawColor(r, 150, 190, 220, 60);
        SDL_RenderDrawLine(r, (int)(cx - 90), (int)coneTopY, (int)(cx - 60), (int)(cy + 150));
        SDL_RenderDrawLine(r, (int)(cx + 90), (int)coneTopY, (int)(cx + 60), (int)(cy + 150));
        for (int i = 1; i <= 5; ++i) {
            double f = i / 6.0;
            int ly = (int)(cy + 150 * f);
            int hw = (int)(70 * (1.0 - f * 0.55));
            SDL_SetRenderDrawColor(r, 150, 190, 220, (Uint8)(14 + (int)(f * 30)));
            SDL_RenderDrawLine(r, (int)(cx - hw), ly, (int)(cx + hw), ly);
        }

        // ---- Pedestal ----
        SDL_SetRenderDrawColor(r, 120, 128, 148, 220);
        // Column
        SDL_RenderDrawLine(r, (int)cx - 46, (int)(cy + 150), (int)cx - 46, (int)(cy + 34));
        SDL_RenderDrawLine(r, (int)cx + 46, (int)(cy + 150), (int)cx + 46, (int)(cy + 34));
        // Platform ellipse
        const int PE = 48;
        SDL_Point plat[PE];
        for (int i = 0; i < PE; ++i) {
            double a = M_PI + M_PI * i / (PE - 1);   // bottom half (upper surface line)
            plat[i] = {(int)(cx + 92 * std::cos(a)), (int)(cy + 34 + 14 * std::sin(a))};
        }
        SDL_RenderDrawLines(r, plat, PE);
        // Platform base line
        SDL_RenderDrawLine(r, (int)cx - 92, (int)(cy + 34), (int)cx + 92, (int)(cy + 34));

        // ---- Pedestal glow ring under the NightElf ----
        double gr = 40.0 + 6.0 * std::sin(timer * 0.06);
        SDL_SetRenderDrawColor(r, 130, 220, 180, 150);
        SDL_Point gprev;
        for (int i = 0; i <= 40; ++i) {
            double a = 2.0 * M_PI * i / 40;
            int gx = (int)(cx + gr * std::cos(a));
            int gy = (int)(cy + 8 + 6 * std::sin(a));
            if (i > 0) SDL_RenderDrawLine(r, gprev.x, gprev.y, gx, gy);
            gprev = {gx, gy};
        }
        // Vertical light shaft from the ceiling to the plane
        SDL_SetRenderDrawColor(r, 170, 225, 240, 55);
        SDL_RenderDrawLine(r, (int)cx, (int)coneTopY, (int)cx, (int)(cy - 26));
    }
};
