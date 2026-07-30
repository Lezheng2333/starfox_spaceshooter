#pragma once

#include "../constants.h"
#include "../renderer.h"
#include "../types.h"

// ============== Ch1Background ==============
class Ch1Background {
    std::vector<Star> stars;
    const ChapterConfig& chapterCfg;

public:
    Ch1Background(const ChapterConfig& cfg) : chapterCfg(cfg) {
        for (int i = 0; i < cfg.starCount; ++i)
            stars.push_back({(float)(rand() % WIN_WIDTH), (float)(rand() % cfg.horizonY),
                             (rand() % 628) / 100.0f, 0.01f + (rand() % 40) / 1000.0f,
                             (rand() % 30 - 15) / 200.0f});
    }

    void update() {
        for (auto& s : stars) {
            s.phase += s.twinkleSpeed;
            if (s.phase > 2.0f * M_PI) s.phase -= 2.0f * M_PI;
            s.x += s.driftSpeed;
            if (s.x < -10) s.x = WIN_WIDTH + 10;
            if (s.x > WIN_WIDTH + 10) s.x = -10;
        }
    }

    void drawStars(SDL_Renderer* renderer) const {
        float brightness = chapterCfg.starBrightness;
        for (const auto& s : stars) {
            float bright = 0.45f + 0.55f * std::fabs(std::sin(s.phase));
            int b = (int)(bright * 200 * brightness);
            if (b < 60) b = 60;
            SDL_SetRenderDrawColor(renderer, b, b, b, 255);
            SDL_RenderDrawPoint(renderer, (int)s.x, (int)s.y);
        }
    }

    void drawStarsFullscreen(SDL_Renderer* renderer) const {
        float brightness = chapterCfg.starBrightness;
        int hy = chapterCfg.horizonY;
        // Sky portion: existing stars [0, hy)
        for (const auto& s : stars) {
            float bright = 0.45f + 0.55f * std::fabs(std::sin(s.phase));
            int b = (int)(bright * 200 * brightness);
            if (b < 60) b = 60;
            SDL_SetRenderDrawColor(renderer, b, b, b, 255);
            SDL_RenderDrawPoint(renderer, (int)s.x, (int)s.y);
        }
        // Ground portion: remap stars to [hy, WIN_HEIGHT]
        float remap = (float)(WIN_HEIGHT - hy) / hy;
        for (const auto& s : stars) {
            float y2 = hy + s.y * remap;
            if (y2 >= WIN_HEIGHT) continue;
            float bright = 0.40f + 0.60f * std::fabs(std::sin(s.phase + 2.3f));
            int b = (int)(bright * 200 * brightness);
            if (b < 55) b = 55;
            SDL_SetRenderDrawColor(renderer, b, b, b, 255);
            SDL_RenderDrawPoint(renderer, (int)(s.x + s.driftSpeed * 300), (int)y2);
        }
    }

    void drawBackground(SDL_Renderer* renderer) const {
        int hy = chapterCfg.horizonY;
        // Ground
        SDL_SetRenderDrawColor(renderer, chapterCfg.groundColorR, chapterCfg.groundColorG, chapterCfg.groundColorB, 255);
        SDL_RenderDrawLine(renderer, 0, hy, WIN_WIDTH, hy);
        drawStars(renderer);

        // Perspective radiate lines
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        const int NUM_RADIATE = 17;
        for (int i = 0; i < NUM_RADIATE; ++i) {
            double t = (double)i / (NUM_RADIATE - 1);
            int bx = (int)(t * WIN_WIDTH);
            int hx = (int)(HORIZON_LEFT + t * (HORIZON_RIGHT - HORIZON_LEFT));
            SDL_RenderDrawLine(renderer, bx, WIN_HEIGHT, hx, hy);
        }

        // Depth lines
        SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
        const int NUM_DEPTH = 16;
        for (int i = 0; i < NUM_DEPTH; ++i) {
            double ratio = (double)(i + 1) / (NUM_DEPTH + 1);
            double dy = hy + (WIN_HEIGHT - hy) * (1.0 - ratio) * (1.0 - ratio);
            SDL_RenderDrawLine(renderer, 0, (int)dy, WIN_WIDTH, (int)dy);
        }
    }

    void drawBase(SDL_Renderer* renderer) const {
        const int BASE_H = 75;
        const double A = WIN_WIDTH / 2.0 - 15.0;
        const double B = BASE_H;
        SDL_SetRenderDrawColor(renderer, 50, 180, 80, 255);
        const int SEG = 80;
        SDL_Point prev;
        for (int i = 0; i <= SEG; ++i) {
            double t = (double)i / SEG;
            int sx = (int)(CENTER_X + A * (2.0 * t - 1.0));
            double ratio = (double)(sx - CENTER_X) / A;
            if (ratio > 1.0) ratio = 1.0;
            if (ratio < -1.0) ratio = -1.0;
            int sy = (int)(WIN_HEIGHT - B * std::sqrt(1.0 - ratio * ratio));
            if (i > 0) SDL_RenderDrawLine(renderer, prev.x, prev.y, sx, sy);
            prev = {sx, sy};
        }
        SDL_SetRenderDrawColor(renderer, 30, 130, 55, 255);
        for (int i = 0; i < 30; ++i) {
            double t = (double)i / 29.0;
            int sx = (int)(CENTER_X + A * 0.85 * (2.0 * t - 1.0));
            double ratio = (double)(sx - CENTER_X) / A;
            if (ratio > 1.0) ratio = 1.0;
            if (ratio < -1.0) ratio = -1.0;
            int topY = (int)(WIN_HEIGHT - B * std::sqrt(1.0 - ratio * ratio));
            SDL_RenderDrawLine(renderer, sx, topY + 3, sx, WIN_HEIGHT);
        }
        SDL_SetRenderDrawColor(renderer, 50, 180, 80, 255);
        SDL_RenderDrawLine(renderer, CENTER_X - (int)A, WIN_HEIGHT, CENTER_X + (int)A, WIN_HEIGHT);
    }
};
