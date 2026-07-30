#pragma once

#include "../audio.h"
#include "../constants.h"
#include "../particles.h"
#include "../renderer.h"
#include "../types.h"

// ============== Ch1AlienManager ==============
class Ch1AlienManager {
    std::vector<Ch1Alien> aliens;
    int spawnTimer;
    double baseSpeed;
    int spawnInterval;
    static const int ALIEN_MIN_HP = 3;
    static const int ALIEN_MAX_HP = 5;
    int hpBonus;  // per-chapter override for alien HP

public:
    Ch1AlienManager() : spawnTimer(0), baseSpeed(1.75), spawnInterval(110), hpBonus(0) {}

    void reset() {
        aliens.clear(); spawnTimer = 0; baseSpeed = 1.75;
        spawnInterval = 110; hpBonus = 0;
    }

    void applyChapterConfig(const ChapterConfig& cfg) {
        baseSpeed = cfg.baseAlienSpeed;
        spawnInterval = cfg.baseSpawnInterval;
        hpBonus = 0;
    }

    void updateMovementParams(int difficultyTimer) {
        if (difficultyTimer % 720 == 0) baseSpeed += 0.4;
    }

    int currentSpawnInterval(int score, int difficultyTimer) {
        int base = 110 - score * 95 / 150;
        if (base < 12) base = 12;
        base -= (difficultyTimer / 720) * 5;
        if (base < 8) base = 8;
        return base;
    }

    void spawnAlien(int score) {
        Ch1Alien a;
        a.targetT = 0.15 + (rand() % 700) / 1000.0;
        a.enterFrame = 0;
        a.enterDuration = 20 + rand() % 25;
        a.entering = true;
        a.enterFromBoss = false;
        a.invincibleFrames = -1;
        a.lastHitBySW = -1; a.lastHealHit = -1;
        a.beingAbsorbed = false;
        a.absorbFrame = 0; a.absorbDuration = 0;
        a.absorbStartX = 0; a.absorbStartY = 0;
        a.alienType = 0;
        int hpBonus = (score >= 60) ? (score / 30 - 1) * 2 : 0;
        a.hp = ALIEN_MIN_HP + hpBonus + rand() % (ALIEN_MAX_HP - ALIEN_MIN_HP + 1);
        a.maxHp = a.hp;
        a.active = true;

        // Entry type
        int entryType = rand() % 3;
        if (entryType < 2) {
            a.enterFromTop = false;
            a.enterStartY = 0; a.enterTargetY = 0;
            a.y = 30.0 + (rand() % 60);
            a.startT = (entryType == 0) ? -0.15 : 1.15;
            a.t = a.startT;
        } else {
            a.enterFromTop = true;
            a.enterStartY = -40.0 - (rand() % 30);
            a.enterTargetY = 30.0 + (rand() % 60);
            a.y = a.enterStartY;
            a.startT = a.targetT;
            a.t = a.targetT;
        }
        aliens.push_back(a);
    }

    void spawnAlienFromBoss(double bossX, double bossY, int score) {
        Ch1Alien a;
        a.startT = (bossX - perspLeft(bossY)) / perspWidth(bossY);
        if (a.startT < 0.1) a.startT = 0.1;
        if (a.startT > 0.9) a.startT = 0.9;
        a.targetT = 0.08 + (rand() % 840) / 1000.0;
        a.t = a.startT;
        a.enterStartY = bossY;
        a.enterTargetY = bossY + 50.0 + (rand() % 180);
        a.y = bossY;
        a.enterFromBoss = true;
        a.enterFromTop = false;
        a.entering = true;
        a.enterFrame = 0;
        a.enterDuration = 16 + rand() % 18;
        a.invincibleFrames = -1;
        a.lastHitBySW = -1; a.lastHealHit = -1;
        a.beingAbsorbed = false;
        a.absorbFrame = 0; a.absorbDuration = 0;
        a.absorbStartX = 0; a.absorbStartY = 0;
        a.alienType = 0;
        int hpBonus = (score >= 60) ? (score / 30 - 1) * 2 : 0;
        a.hp = ALIEN_MIN_HP + hpBonus + rand() % (ALIEN_MAX_HP - ALIEN_MIN_HP + 1);
        a.maxHp = a.hp;
        a.active = true;
        aliens.push_back(a);
    }

    void update(bool duringBossAbsorb, double speedFactor, bool& gameOver, int& baseHP,
                ParticleManager& pm, AudioEngine* audio) {
        const double A = WIN_WIDTH / 2.0 - 15.0;
        const double B = 75.0;
        for (auto& a : aliens) {
            if (!a.active) continue;

            if (a.entering) {
                a.enterFrame++;
                double raw = (double)a.enterFrame / a.enterDuration;
                double eased = 1.0 - std::pow(1.0 - raw, 3.0);
                if (raw >= 1.0) {
                    raw = 1.0; eased = 1.0;
                    a.entering = false; a.invincibleFrames = 3;
                    double ax = perspLeft(a.y) + a.t * perspWidth(a.y);
                    pm.spawnExplosion(ax, a.y, 8);
                }
                if (a.enterFromBoss) {
                    a.y = a.enterStartY + (a.enterTargetY - a.enterStartY) * eased;
                    a.t = a.startT + (a.targetT - a.startT) * eased;
                } else if (a.enterFromTop) {
                    a.y = a.enterStartY + (a.enterTargetY - a.enterStartY) * eased;
                } else {
                    a.t = a.startT + (a.targetT - a.startT) * eased;
                }
            }

            if (!a.entering) {
                if (a.invincibleFrames > 0) a.invincibleFrames--;
                double depthBelow = (a.y - HORIZON_Y) / (WIN_HEIGHT - HORIZON_Y);
                double sf = (depthBelow < 0) ? 0.08 : 0.08 + 0.92 * depthBelow;
                a.y += baseSpeed * sf * (duringBossAbsorb ? speedFactor : 1.0);
            }

            if (a.y > WIN_HEIGHT + 30) { a.active = false; continue; }

            // Base collision
            double ax = perspLeft(a.y) + a.t * perspWidth(a.y);
            double dx = (ax - CENTER_X) / A;
            double dy = (WIN_HEIGHT - a.y) / B;
            if (dx*dx + dy*dy < 1.0) {
                pm.spawnExplosion(ax, a.y, 35);
                a.active = false;
                if (a.invincibleFrames == 0) {
                    baseHP--;
                    if (audio) audio->sndBaseDamage();
                    if (baseHP <= 0) { gameOver = true; }
                }
            }
        }
    }

    void draw(SDL_Renderer* renderer) const {
        for (const auto& a : aliens) {
            if (!a.active) continue;
            double ax = perspLeft(a.y) + a.t * perspWidth(a.y);
            double depthBelow = (a.y - HORIZON_Y) / (WIN_HEIGHT - HORIZON_Y);
            double scale = (depthBelow < 0) ? 0.17 : 0.17 + 0.83 * depthBelow;
            if (scale < 0.14) scale = 0.14;
            if (scale > 1.0)  scale = 1.0;
            int s = (int)(28.0 * scale);
            if (s < 3) s = 3;

            // Entry trail
            if (a.entering && a.enterFrame > 3) {
                for (int k = 1; k <= 3; ++k) {
                    double pastRaw = (double)(a.enterFrame - k * 3) / a.enterDuration;
                    if (pastRaw < 0) continue;
                    double pastEased = 1.0 - std::pow(1.0 - pastRaw, 3.0);
                    double pastX, pastY;
                    if (a.enterFromBoss) {
                        pastY = a.enterStartY + (a.enterTargetY - a.enterStartY) * pastEased;
                        double pastT2 = a.startT + (a.targetT - a.startT) * pastEased;
                        pastX = perspLeft(pastY) + pastT2 * perspWidth(pastY);
                    } else if (a.enterFromTop) {
                        pastY = a.enterStartY + (a.enterTargetY - a.enterStartY) * pastEased;
                        pastX = perspLeft(pastY) + a.t * perspWidth(pastY);
                    } else {
                        pastY = a.y;
                        double pastT = a.startT + (a.targetT - a.startT) * pastEased;
                        pastX = perspLeft(pastY) + pastT * perspWidth(pastY);
                    }
                    int alpha = 150 - k * 40;
                    SDL_SetRenderDrawColor(renderer, 100, 180, 255, (unsigned char)alpha);
                    SDL_RenderDrawPoint(renderer, (int)pastX, (int)pastY);
                }
            }

            double hpRatio = (double)a.hp / a.maxHp;
            bool inv = a.entering || a.invincibleFrames != 0;
            int r, g, b;
            if (inv) { r = 100; g = 180; b = 255; }
            else { r = 255; g = (int)(150 * hpRatio + 80 * (1 - hpRatio)); b = (int)(100 * hpRatio + 30 * (1 - hpRatio)); }
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);

            int ix = (int)ax, iy = (int)a.y;
            SDL_Point pts[4] = {
                {ix, iy - s}, {ix + s * 2 / 3, iy},
                {ix, iy + s}, {ix - s * 2 / 3, iy}
            };
            SDL_RenderDrawLines(renderer, pts, 4);
            SDL_RenderDrawLine(renderer, pts[3].x, pts[3].y, pts[0].x, pts[0].y);
            SDL_RenderDrawLine(renderer, ix - s*2/3, iy, ix + s*2/3, iy);
        }
    }

    void removeInactive() {
        aliens.erase(std::remove_if(aliens.begin(), aliens.end(),
            [](const Ch1Alien& a){ return !a.active; }), aliens.end());
    }

    std::vector<Ch1Alien>& all() { return aliens; }
    void pushAlien(const Ch1Alien& a) { aliens.push_back(a); }
    int countAlive() const {
        int c = 0;
        for (const auto& a : aliens) if (a.active) c++;
        return c;
    }
    void setAllInvincible() { for (auto& a : aliens) a.invincibleFrames = -1; }
    void setAllVulnerable() { for (auto& a : aliens) a.invincibleFrames = 0; }
    int& spawnTimerRef() { return spawnTimer; }
};
