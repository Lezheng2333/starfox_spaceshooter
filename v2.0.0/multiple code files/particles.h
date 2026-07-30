#pragma once

#include "font.h"
#include "renderer.h"
#include "types.h"

// ============== ParticleManager ==============
class ParticleManager {
    std::vector<Ch1Particle> particles;
public:
    void spawnExplosion(double x, double y, int count) {
        for (int i = 0; i < count; ++i) {
            Ch1Particle p;
            p.x = x; p.y = y;
            double angle = (rand() % 360) * M_PI / 180.0;
            double speed = 1.2 + (rand() % 400) / 100.0;
            p.vx = std::cos(angle) * speed;
            p.vy = std::sin(angle) * speed;
            p.life = 15 + rand() % 20;
            p.active = true;
            p.whiteParticle = false;
            p.greenParticle = false;
            p.redParticle = false;
            particles.push_back(p);
        }
    }

    void spawnWhiteParticle(double x, double y, double vx, double vy, int life) {
        Ch1Particle p;
        p.x = x; p.y = y; p.vx = vx; p.vy = vy;
        p.life = life; p.active = true;
        p.whiteParticle = true;
        p.greenParticle = false; p.redParticle = false;
        particles.push_back(p);
    }

    void spawnGreenParticle(double x, double y, double vx, double vy, int life) {
        Ch1Particle p;
        p.x = x; p.y = y; p.vx = vx; p.vy = vy;
        p.life = life; p.active = true;
        p.whiteParticle = false;
        p.greenParticle = true; p.redParticle = false;
        particles.push_back(p);
    }

    void spawnRedParticle(double x, double y, double vx, double vy, int life) {
        Ch1Particle p;
        p.x = x; p.y = y; p.vx = vx; p.vy = vy;
        p.life = life; p.active = true;
        p.whiteParticle = false;
        p.greenParticle = false; p.redParticle = true;
        particles.push_back(p);
    }

    void spawnDigitShatter(const Font& font, char digit, int digitScale, int cx, int cy) {
        const FontChar& fc = font.getChar(digit);
        int dw = 5 * digitScale, dh = 7 * digitScale;
        int dx = cx - dw / 2, dy = cy - dh / 2;
        for (int row = 0; row < 7; ++row) {
            unsigned char bits = fc.rows[row];
            for (int col = 0; col < 5; ++col) {
                if (!(bits & (1 << (4 - col)))) continue;
                double px = dx + col * digitScale + digitScale / 2.0;
                double py = dy + row * digitScale + digitScale / 2.0;
                int n = 4 + rand() % 3;
                for (int k = 0; k < n; ++k) {
                    Ch1Particle p;
                    p.x = px + (rand() % (digitScale + 1)) - digitScale / 2.0;
                    p.y = py + (rand() % (digitScale + 1)) - digitScale / 2.0;
                    double angle = (rand() % 360) * M_PI / 180.0;
                    double speed = 2.0 + (rand() % 500) / 100.0;
                    p.vx = std::cos(angle) * speed;
                    p.vy = std::sin(angle) * speed;
                    p.life = 22 + rand() % 24;
                    p.active = true;
                    p.whiteParticle = true;
                    particles.push_back(p);
                }
            }
        }
    }

    void spawnFireworks(double cx, double cy) {
        for (int k = 0; k < 12; ++k) {
            Ch1Particle p;
            double angle = (rand() % 360) * M_PI / 180.0;
            double speed = 2.5 + (rand() % 500) / 100.0;
            p.x = cx + (rand() % 240 - 120);
            p.y = cy - (rand() % 40);
            p.vx = std::cos(angle) * speed;
            p.vy = -std::fabs(std::sin(angle) * speed) - 1.5;
            p.life = 45 + rand() % 35;
            p.active = true;
            p.whiteParticle = (rand()%2 == 0);
            p.greenParticle = (rand()%2 == 0);
            p.redParticle = !p.whiteParticle && !p.greenParticle;
            particles.push_back(p);
        }
    }

    void update() {
        for (auto& p : particles) {
            if (!p.active) continue;
            p.x += p.vx; p.y += p.vy;
            p.vx *= 0.96; p.vy *= 0.96;
            p.life--;
            if (p.life <= 0) p.active = false;
        }
    }

    void draw(SDL_Renderer* renderer) const {
        for (const auto& p : particles) {
            if (!p.active) continue;
            double fade = (double)p.life / 35.0;
            if (fade < 0.0) fade = 0.0;
            int brightness = (int)(255 * fade);
            if (p.whiteParticle) {
                SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);
            } else if (p.greenParticle) {
                SDL_SetRenderDrawColor(renderer, (int)(brightness * 0.2), brightness, (int)(brightness * 0.3), 255);
            } else if (p.redParticle) {
                SDL_SetRenderDrawColor(renderer, brightness, (int)(brightness * 0.2), (int)(brightness * 0.2), 255);
            } else {
                int r = brightness, g = (int)(brightness * 0.55), b = (int)(brightness * 0.1);
                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            }
            int sz = (int)((p.whiteParticle || p.greenParticle) ? 5.0 * fade : 3.5 * fade);
            if (sz < 1) sz = 1;
            SDL_Rect rect = {(int)(p.x - sz/2), (int)(p.y - sz/2), sz, sz};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    void removeInactive() {
        particles.erase(std::remove_if(particles.begin(), particles.end(),
            [](const Ch1Particle& p){ return !p.active; }), particles.end());
    }

    std::vector<Ch1Particle>& all() { return particles; }
};
