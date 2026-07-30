#pragma once

#include "renderer.h"
#include "types.h"

// ============== FloatingTextManager ==============
class FloatingTextManager {
    std::vector<FloatingText> texts;
public:
    void spawn(float x, float y, const char* txt, int r=50, int g=255, int b_=80) {
        FloatingText ft;
        ft.x = x; ft.y = y;
        ft.life = 90; ft.totalLife = 90;
        ft.r = r; ft.g = g; ft.b = b_;
        snprintf(ft.text, sizeof(ft.text), "%s", txt);
        texts.push_back(ft);
    }

    void update() {
        for (auto& ft : texts) ft.y -= 0.7f;
        for (auto& ft : texts) ft.life--;
        texts.erase(std::remove_if(texts.begin(), texts.end(),
            [](const FloatingText& ft){ return ft.life <= 0; }), texts.end());
    }

    void draw(SDL_Renderer* renderer) const {
        for (const auto& ft : texts) {
            float t = (float)ft.life / ft.totalLife;
            if (t < 0.05f) continue;
            int r = (int)(ft.r * t), g = (int)(ft.g * t), b_ = (int)(ft.b * t);
            SDL_SetRenderDrawColor(renderer, r, g, b_, 255);
            // Use external font reference — handled in Game::draw
        }
    }

    const std::vector<FloatingText>& all() const { return texts; }
    void clear() { texts.clear(); }
};
