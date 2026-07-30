#pragma once

#include "../audio.h"
#include "../particles.h"
#include "../renderer.h"
#include "../types.h"

// ============== Ch2PulseSystem (green energy bar + Shift pulse) ==============
class Ch2PulseSystem {
public:
    static const int MAX_ENERGY = 30;

    int energy;
    bool unlocked;      // false until skill orb absorbed
    bool draining;      // true when energy is rapidly draining after absorption cancel

    std::vector<Ch2PulseWave> waves;
    int nextWaveID;

    Ch2PulseSystem() : energy(0), unlocked(false), draining(false), nextWaveID(0) {}

    void reset() {
        energy = 0; unlocked = false; draining = false;
        waves.clear(); nextWaveID = 0;
    }

    void addEnergy(int hits) {
        if (!unlocked) return;
        if (hits <= 0) return;
        energy += hits;
        if (energy > MAX_ENERGY) energy = MAX_ENERGY;
    }

    void addAbsorbEnergy() {
        // Gradually fill during orb absorption (target: 30 over 300 frames = every 10 frames)
        energy += 1;
        if (energy > MAX_ENERGY) energy = MAX_ENERGY;
    }

    void startDrain() { draining = true; }
    bool isDraining() const { return draining; }

    bool canAbsorb() const { return !draining && energy == 0; }

    void drainTick() {
        if (!draining) return;
        energy -= 3;
        if (energy <= 0) { energy = 0; draining = false; }
    }

    float getFill() const {
        return (float)energy / MAX_ENERGY;
    }

    bool isFull() const { return unlocked && energy >= MAX_ENERGY; }

    // Breathing: bright green ↔ dark green when full (for HUD rendering)
    float getBreathAlpha() const {
        if (!isFull()) return 1.0f;
        return 0.6f + 0.4f * (float)std::sin(SDL_GetTicks() * 0.008);
    }

    void release(double px, double py, ParticleManager& pm, AudioEngine& audio) {
        energy = 0; // consume all energy

        Ch2PulseWave w;
        w.x = px; w.y = py;
        w.radius = 20.0;
        w.id = nextWaveID++;
        w.active = true;
        waves.push_back(w);

        // White particle burst (similar to Ch1 base shockwave)
        for (int i = 0; i < 50; ++i) {
            double angle = (rand() % 360) * M_PI / 180.0;
            double speed = 2.0 + (rand() % 500) / 100.0;
            pm.spawnWhiteParticle(px, py,
                std::cos(angle) * speed,
                std::sin(angle) * speed,
                20 + rand() % 30);
        }
        audio.sndShockwave();
    }

    void update() {
        // Update expanding shockwave rings
        for (auto& w : waves) {
            if (!w.active) continue;
            w.radius += 8.0;
            if (w.radius > 520.0) w.active = false;
        }
        waves.erase(std::remove_if(waves.begin(), waves.end(),
            [](const Ch2PulseWave& w){ return !w.active; }), waves.end());
    }

    void draw(SDL_Renderer* r) const {
        for (const auto& w : waves) {
            if (!w.active) continue;
            double progress = (w.radius - 20.0) / 500.0; // 0→1 over life
            if (progress > 1.0) progress = 1.0;
            int alpha = (int)(220.0 * (1.0 - progress)); if (alpha < 10) alpha = 10;
            SDL_SetRenderDrawColor(r, 255, 255, 255, (Uint8)alpha);
            const int SEG = 64;
            SDL_Point prev;
            int cx = (int)w.x, cy = (int)w.y;
            for (int i = 0; i <= SEG; ++i) {
                double a = 2.0 * M_PI * i / SEG;
                int sx = cx + (int)(w.radius * std::cos(a));
                int sy = cy + (int)(w.radius * std::sin(a));
                if (i > 0) SDL_RenderDrawLine(r, prev.x, prev.y, sx, sy);
                prev = {sx, sy};
            }
            // Second ring (thinner, slightly smaller)
            double r2 = w.radius * 0.85;
            int alpha2 = (int)(140.0 * (1.0 - progress)); if (alpha2 < 8) alpha2 = 8;
            SDL_SetRenderDrawColor(r, 255, 255, 255, (Uint8)alpha2);
            for (int i = 0; i <= SEG; ++i) {
                double a = 2.0 * M_PI * i / SEG;
                int sx = cx + (int)(r2 * std::cos(a));
                int sy = cy + (int)(r2 * std::sin(a));
                if (i > 0) SDL_RenderDrawLine(r, prev.x, prev.y, sx, sy);
                prev = {sx, sy};
            }
        }
    }

    // Collision: pulse vs enemy bullets → destroy all bullets in radius
    void collideWithBullets(std::vector<Ch2EnemyBullet>& bullets, ParticleManager& pm) {
        for (auto& w : waves) {
            if (!w.active) continue;
            for (auto& b : bullets) {
                if (!b.active) continue;
                double dx = b.x - w.x, dy = b.y - w.y;
                if (dx*dx + dy*dy < (w.radius+4.0)*(w.radius+4.0)) {
                    b.active = false;
                    pm.spawnExplosion(b.x, b.y, 2);
                }
            }
        }
    }

    // Collision: pulse vs Ch2 alien enemies
    void collideWithAliens(Ch2AlienManager& am, ParticleManager& pm, AudioEngine& audio,
                           int& score, int& hitCount) {
        for (auto& w : waves) {
            if (!w.active) continue;
            auto& aliens = const_cast<std::vector<Ch2Alien>&>(am.getAliens());
            for (auto& a : aliens) {
                if (!a.active) continue;
                if (a.lastHitByPulse == w.id) continue;
                double dx = a.x - w.x, dy = a.y - w.y;
                if (dx*dx + dy*dy < (w.radius+22.0)*(w.radius+22.0)) {
                    a.lastHitByPulse = w.id;
                    a.hp -= 1; hitCount++;
                    pm.spawnExplosion(a.x, a.y, 4);
                    audio.sndShockwaveHit();
                    if (a.hp <= 0) {
                        a.defeated = true; a.active = false;
                        pm.spawnExplosion(a.x, a.y, 20);
                        pm.spawnExplosion(a.x-8, a.y-4, 12);
                        pm.spawnExplosion(a.x+8, a.y+4, 12);
                        audio.sndExplosionSmall(); score += 1;
                    }
                }
            }
        }
    }

    // Collision: pulse vs danmaku enemies
    void collideWithDanmaku(Ch2DanmakuManager& dm, ParticleManager& pm, AudioEngine& audio,
                            int& score, int& hitCount) {
        for (auto& w : waves) {
            if (!w.active) continue;
            auto& enemies = const_cast<std::vector<Ch2DanmakuEnemy>&>(dm.getEnemies());
            for (auto& e : enemies) {
                if (!e.active || e.entering) continue;
                if (e.lastHitByPulse == w.id) continue;
                if (e.defeated && e.defeatTimer > 0) continue;
                double dx = e.x - w.x, dy = e.y - w.y;
                double hitR = e.nearLeg + 10.0;
                if (dx*dx + dy*dy < (w.radius+hitR)*(w.radius+hitR)) {
                    e.lastHitByPulse = w.id;
                    e.hp -= 1; hitCount++;
                    pm.spawnExplosion(e.x, e.y, 4);
                    audio.sndShockwaveHit();
                    if (e.hp <= 0) {
                        e.defeated = true; e.invincibleFrames = 300;
                        e.defeatTimer = 70;
                        score += 5;
                        pm.spawnExplosion(e.x, e.y, 20);
                        audio.sndExplosionSmall();
                    }
                }
            }
        }
    }
};
