#pragma once

#include "renderer.h"

class Player;  // forward decl — AimAssist stores no Player, only takes refs

// ============== AimAssist (owned by Player) ==============
class AimAssist {
    double snapProgress;
    static void updateProgress(double& sp, bool hasTarget) {
        if (hasTarget) sp += 0.05; else sp -= 0.12;
        if (sp > 1.0) sp = 1.0; if (sp < 0.0) sp = 0.0;
    }
public:
    AimAssist() : snapProgress(0) {}
    void update(bool hasTarget) { updateProgress(snapProgress, hasTarget); }
    double getSnapProgress() const { return snapProgress; }
    void draw(SDL_Renderer* r, double drawX, double drawY, int dotBig, int dotSmall) const {
        double ss = 8.0 * (1.0 - snapProgress);
        if (ss > 0.5) {
            SDL_SetRenderDrawColor(r, 255, 255, 255, (Uint8)(140 * (1.0 - snapProgress)));
            int sx = (int)(drawX - ss), side = (int)(ss * 2);
            int gapPx = side * 6 / 10; if (gapPx < 2) gapPx = 2;
            int edgePx = (side - gapPx) / 2; if (edgePx < 1) edgePx = 1;
            int sy = (int)(drawY - ss);
            if (side >= 3) {
                SDL_RenderDrawLine(r, sx, sy, sx+edgePx, sy);
                SDL_RenderDrawLine(r, sx+edgePx+gapPx, sy, sx+side, sy);
                SDL_RenderDrawLine(r, sx, sy+side, sx+edgePx, sy+side);
                SDL_RenderDrawLine(r, sx+edgePx+gapPx, sy+side, sx+side, sy+side);
                SDL_RenderDrawLine(r, sx, sy, sx, sy+edgePx);
                SDL_RenderDrawLine(r, sx, sy+edgePx+gapPx, sx, sy+side);
                SDL_RenderDrawLine(r, sx+side, sy, sx+side, sy+edgePx);
                SDL_RenderDrawLine(r, sx+side, sy+edgePx+gapPx, sx+side, sy+side);
            }
        }
        int dotSize = (snapProgress > 0.5) ? dotBig : dotSmall;
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_Rect dot = {(int)(drawX - dotSize/2), (int)(drawY - dotSize/2), dotSize, dotSize};
        SDL_RenderFillRect(r, &dot);
    }
};
