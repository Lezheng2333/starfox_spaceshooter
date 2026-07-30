#pragma once

#include "../audio.h"
#include "../bullets.h"
#include "../constants.h"
#include "../font.h"
#include "../particles.h"
#include "../renderer.h"
#include "../types.h"
#include "../ch1/ch1_aliens.h"

// ============== Ch1Boss ==============
class Ch1Boss {
    // Core state
    double y;
    int hp, maxHp, bonusHp;
    bool active;
    double x;
    double moveTime;
    int flashTimer;
    int lastHitBySW;

    // Enter
    bool entering;
    int enterFrame, enterDuration;

    // Phase
    bool phase2Triggered;
    int shakeTimer;
    int shakeX, shakeY;

public:
    enum AbsorbState { IDLE, BEAM_FLYING, SPIRALING, COOLDOWN };
private:
    // Absorb state machine
    int absorbTimer;
    int absorbIndex;
    int absorbCooldown;
    AbsorbState absorbState;
    int absorbTargetIdx;
    int postAbsorbTimer;
    bool healWavesEnabled;

    // Heal wave system
    std::vector<Ch1HealWave> healWaves;
    int healWaveTimer;
    int nextCh1HealWaveID;

    // Config
    const BossConfig* cfg;

public:
    Ch1Boss() : y(-300), hp(1000), maxHp(1000), bonusHp(0), active(false),
             x(CENTER_X), moveTime(0), flashTimer(0), lastHitBySW(-1),
             entering(false), enterFrame(0), enterDuration(150),
             phase2Triggered(false), shakeTimer(0), shakeX(0), shakeY(0),
             absorbTimer(0), absorbIndex(0), absorbCooldown(0),
             absorbState(IDLE), absorbTargetIdx(-1), postAbsorbTimer(0),
             healWavesEnabled(false), healWaveTimer(0), nextCh1HealWaveID(0), cfg(nullptr) {}

    void setConfig(const BossConfig* c) { cfg = c; }

    void reset() {
        y = -300; hp = 1000; maxHp = 1000; bonusHp = 0;
        active = false; x = CENTER_X; moveTime = 0; flashTimer = 0; lastHitBySW = -1;
        entering = false; enterFrame = 0; enterDuration = 150;
        phase2Triggered = false; shakeTimer = 0; shakeX = shakeY = 0;
        absorbTimer = 0; absorbIndex = 0; absorbCooldown = 0;
        absorbState = IDLE; absorbTargetIdx = -1; postAbsorbTimer = 0;
        healWavesEnabled = false; healWaveTimer = 0; nextCh1HealWaveID = 0;
        healWaves.clear();
    }

    // Accessors
    double getX() const { return x; }
    double getY() const { return y; }
    void setY(double ny) { y = ny; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getBonusHp() const { return bonusHp; }
    int getLastHitBySW() const { return lastHitBySW; }
    int& lastHitBySWRef() { return lastHitBySW; }
    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }
    void setMaxHp(int m) { maxHp = m; }
    bool isEntering() const { return entering; }
    int getShakeTimer() const { return shakeTimer; }
    int getShakeX() const { return shakeX; }
    int getShakeY() const { return shakeY; }
    int getFlashTimer() const { return flashTimer; }
    int& bonusHpRef() { return bonusHp; }
    int& hpRef() { return hp; }
    bool isPhase2Triggered() const { return phase2Triggered; }
    bool isCh1HealWavesEnabled() const { return healWavesEnabled; }
    std::vector<Ch1HealWave>& getCh1HealWaves() { return healWaves; }

    void takeDamage(int dmg) {
        flashTimer = 5;
        if (bonusHp > 0) {
            if (dmg <= bonusHp) { bonusHp -= dmg; return; }
            dmg -= bonusHp; bonusHp = 0;
        }
        hp -= dmg;
    }

    void trigger() {
        active = true;
        y = -300;
        hp = cfg ? cfg->maxHp : 1000;
        maxHp = cfg ? cfg->maxHp : 1000;
        bonusHp = 0;
        lastHitBySW = -1;
        entering = true;
        enterFrame = 0;
        enterDuration = cfg ? cfg->enterDuration : 150;
        phase2Triggered = false;
        absorbTimer = 0; absorbIndex = 0; absorbCooldown = 0;
        absorbState = IDLE; absorbTargetIdx = -1; postAbsorbTimer = 0;
        healWavesEnabled = false; healWaveTimer = 0; nextCh1HealWaveID = 0;
        healWaves.clear();
        x = CENTER_X; moveTime = 0;
    }

    void updateEnterAnimation(AudioEngine* audio) {
        if (!entering) return;
        enterFrame++;
        double raw = (double)enterFrame / enterDuration;
        if (raw > 1.0) raw = 1.0;
        double eased = 1.0 - std::pow(1.0 - raw, 3.0);
        y = -300.0 + (90.0 + 300.0) * eased;
        if (raw >= 1.0) {
            entering = false;
            shakeTimer = cfg ? cfg->shakeDuration : 60;
            if (audio) audio->sndBossEntrance();
        }
    }

    void updateShake(AudioEngine* audio) {
        if (shakeTimer > 0) {
            shakeTimer--;
            shakeX = (rand() % 10) - 5;
            shakeY = (rand() % 10) - 5;
            if (shakeTimer % 12 == 0 && audio) audio->sndShake();
        } else { shakeX = shakeY = 0; }
    }

    void updatePostAbsorbShake(AudioEngine* audio) {
        if (postAbsorbTimer > 0) {
            postAbsorbTimer--;
            shakeX = (rand() % 10) - 5; shakeY = (rand() % 10) - 5;
            if (postAbsorbTimer % 10 == 0 && audio) audio->sndShake();
        }
    }

    void triggerPhase2() {
        phase2Triggered = true;
        shakeTimer = 120;
        absorbTimer = 0;
        postAbsorbTimer = 0;
        absorbCooldown = 0;
        absorbState = IDLE;
        absorbTargetIdx = -1;
        setCh1HealWavesEnabled(false);
    }

    void updateMovement() {
        moveTime += 0.025;
        x = CENTER_X + std::sin(moveTime) * (cfg ? cfg->moveAmplitudeX : 140.0);
        y = 90.0 + std::sin(moveTime * 2.0) * std::cos(moveTime) * (cfg ? cfg->moveAmplitudeY : 25.0);
        if (flashTimer > 0) flashTimer--;
    }

    void updateCh1HealWaves(ParticleManager& pm, Ch1AlienManager& aliens, AudioEngine* audio) {
        if (!healWavesEnabled) return;
        healWaveTimer++;
        int interval = cfg ? cfg->healWaveInterval : 420;
        if (healWaveTimer >= interval) {
            healWaveTimer = 0;
            Ch1HealWave hw; hw.radius = 15; hw.id = nextCh1HealWaveID++; hw.active = true;
            healWaves.push_back(hw);
        }
        for (auto& hw : healWaves) {
            if (!hw.active) continue;
            hw.radius += 4.0;
            if (hw.radius > 700) hw.active = false;
        }
        // Heal wave collision
        for (auto& hw : healWaves) {
            if (!hw.active) continue;
            for (auto& a : aliens.all()) {
                if (!a.active || a.entering || a.beingAbsorbed) continue;
                if (a.lastHealHit == hw.id) continue;
                double ax = perspLeft(a.y) + a.t * perspWidth(a.y);
                double dist = std::sqrt((ax - x)*(ax - x) + (a.y - y)*(a.y - y));
                if (std::abs(dist - hw.radius) < 25.0) {
                    a.lastHealHit = hw.id;
                    if (audio) audio->sndBossHeal();
                    int healAmt = cfg ? cfg->healHpPerWave : 30;
                    bonusHp += healAmt;
                    int barEndX = CENTER_X - 200 + (int)((double)(hp + bonusHp) / maxHp * 400);
                    if (barEndX > WIN_WIDTH) barEndX = WIN_WIDTH;
                    for (int i = 0; i < 10; ++i) {
                        Ch1Particle p;
                        p.x = ax + (rand()%10-5); p.y = a.y + (rand()%10-5);
                        p.vx = (barEndX - p.x) / 60.0 + (rand()%10-5)/10.0;
                        p.vy = (16 - p.y) / 30.0 + (rand()%10-5)/10.0;
                        p.life = 30 + rand()%15; p.active = true;
                        p.whiteParticle = false; p.greenParticle = false; p.redParticle = true;
                        pm.all().push_back(p);
                    }
                }
            }
        }
        healWaves.erase(std::remove_if(healWaves.begin(), healWaves.end(),
            [](const Ch1HealWave& hw){ return !hw.active; }), healWaves.end());
    }

    // Shared absorb state machine (deduplicated from INTRO and PHASE2)
    bool updateAbsorbStateMachine(Ch1AlienManager& aliens, BulletManager& bullets,
                                   ParticleManager&, AudioEngine*) {
        if (absorbCooldown > 0) absorbCooldown--;

        if (absorbState == IDLE) {
            int idx = -1;
            for (int i = 0; i < (int)aliens.all().size(); ++i) {
                if (aliens.all()[i].active && !aliens.all()[i].beingAbsorbed) { idx = i; break; }
            }
            if (idx >= 0) {
                Ch1Alien& a = aliens.all()[idx];
                double ax = perspLeft(a.y) + a.t * perspWidth(a.y);
                bullets.addBossBeam(x, y, ax, a.y, idx);
                absorbTargetIdx = idx;
                absorbState = BEAM_FLYING;
            } else if (absorbCooldown <= 0) {
                for (auto& aa : aliens.all()) {
                    if (aa.active) { bonusHp += (cfg ? cfg->bonusHpPerAlien : 50); aa.active = false; }
                }
                aliens.all().clear();
                postAbsorbTimer = 30;
                absorbTimer = -1;
                absorbState = IDLE;
                return true;  // all done
            }
        }
        // Safety net: beam lost target
        if (absorbState == BEAM_FLYING) {
            bool beamAlive = false;
            for (auto& bb : bullets.all())
                if (bb.active && bb.blueBeam && bb.beamTargetIndex == absorbTargetIdx)
                    { beamAlive = true; break; }
            if (!beamAlive) { absorbState = COOLDOWN; absorbCooldown = 18; }
        }
        // Spiral complete → cooldown
        if (absorbState == SPIRALING && absorbTargetIdx >= 0) {
            if (!aliens.all()[absorbTargetIdx].active || !aliens.all()[absorbTargetIdx].beingAbsorbed) {
                absorbState = COOLDOWN; absorbCooldown = 18;
            }
        }
        if (absorbState == COOLDOWN && absorbCooldown <= 0) absorbState = IDLE;
        return false;
    }

    // Shared absorb animation (deduplicated)
    void updateAbsorbAnimations(Ch1AlienManager& aliens, ParticleManager& pm) {
        for (auto& a : aliens.all()) {
            if (!a.active || !a.beingAbsorbed) continue;
            a.absorbFrame++;
            double raw = (double)a.absorbFrame / a.absorbDuration;
            if (raw >= 1.0) {
                raw = 1.0; a.active = false;
                int gain = cfg ? cfg->bonusHpPerAlien : 50;
                bonusHp += gain;
                absorbCooldown = 18;
                int barEndX = CENTER_X - 200 + (int)((double)(hp + bonusHp) / maxHp * 400);
                if (barEndX > WIN_WIDTH) barEndX = WIN_WIDTH;
                for (int i = 0; i < 12; ++i) {
                    Ch1Particle p;
                    p.x = x + (rand()%30-15); p.y = y + (rand()%20-10);
                    p.vx = (barEndX - p.x) / 60.0 + (rand()%10-5)/10.0;
                    p.vy = (16 - p.y) / 60.0 + (rand()%10-5)/10.0;
                    p.life = 60 + rand()%20; p.active = true;
                    p.whiteParticle = false; p.greenParticle = false; p.redParticle = true;
                    pm.all().push_back(p);
                }
            } else {
                double eased = 1.0 - std::pow(1.0 - raw, 2.0);
                double startDist = std::sqrt(
                    (a.absorbStartX - x)*(a.absorbStartX - x) +
                    (a.absorbStartY - y)*(a.absorbStartY - y));
                double startAngle = std::atan2(a.absorbStartY - y, a.absorbStartX - x);
                double angle = startAngle + raw * M_PI * 5.0;
                double radius = startDist * (1.0 - eased);
                a.y = y + std::sin(angle) * radius;
                double tx = x + std::cos(angle) * radius;
                a.t = (tx - perspLeft(a.y)) / perspWidth(a.y);
            }
        }
    }

    void drawCircularShockwave(SDL_Renderer* renderer) const {
        if (shakeTimer <= 0) return;
        double progress = 1.0 - (double)shakeTimer / 120.0;
        if (progress > 1.0) progress = 1.0;
        double radius = 40.0 + progress * 520.0;
        int alpha = (int)(180.0 * (1.0 - progress));
        if (alpha < 8) return;
        SDL_SetRenderDrawColor(renderer, 255, 80, 80, (unsigned char)alpha);
        const int SEG = 64;
        SDL_Point prev;
        for (int i = 0; i <= SEG; ++i) {
            double angle = 2.0 * M_PI * i / SEG;
            double sy = y + std::sin(angle) * radius;
            double rx = radius * perspWidth(sy) / perspWidth(y);
            int sx = (int)(x + std::cos(angle) * rx);
            if (i > 0) SDL_RenderDrawLine(renderer, prev.x, prev.y, sx, (int)sy);
            prev = {sx, (int)sy};
        }
    }

    void drawCh1HealWaves(SDL_Renderer* renderer) const {
        for (const auto& hw : healWaves) {
            if (!hw.active) continue;
            SDL_SetRenderDrawColor(renderer, 255, 60, 60, 80);
            const int SEG = 60;
            SDL_Point prev;
            for (int i = 0; i <= SEG; ++i) {
                double angle = 2.0 * M_PI * i / SEG;
                double sy = y + std::sin(angle) * hw.radius;
                double rx = hw.radius * perspWidth(sy) / perspWidth(y);
                int sx = (int)(x + std::cos(angle) * rx);
                if (i > 0) SDL_RenderDrawLine(renderer, prev.x, prev.y, sx, (int)sy);
                prev = {sx, (int)sy};
            }
        }
    }

    void drawBody(SDL_Renderer* renderer) const {
        if (!active || y < -100) return;
        double scale = 0.5 + 0.5 * ((y + 100) / 200.0);
        if (scale > 1.0) scale = 1.0;
        int s = (int)((cfg ? cfg->size : 80.0) * scale);
        if (s < 20) s = 20;
        int ix = (int)x, iy = (int)y;

        if (flashTimer > 0)
            SDL_SetRenderDrawColor(renderer, 255, 200, 200, 255);
        else
            SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        SDL_Point pts[4] = {
            {ix, iy - s}, {ix + s*2/3, iy},
            {ix, iy + s/2}, {ix - s*2/3, iy}
        };
        SDL_RenderDrawLines(renderer, pts, 4);
        SDL_RenderDrawLine(renderer, pts[3].x, pts[3].y, pts[0].x, pts[0].y);
        SDL_RenderDrawLine(renderer, ix - s*2/3, iy, ix + s*2/3, iy);
        SDL_RenderDrawLine(renderer, ix, iy - s, ix, iy + s/2);

        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 100);
        int r = (int)(s * 0.9);
        for (int i = 0; i < 2; ++i) {
            int rr = r + i * 10;
            SDL_Point ring[6] = {
                {ix, iy - rr}, {ix + rr*3/4, iy - rr/2},
                {ix + rr*3/4, iy + rr/2}, {ix, iy + rr},
                {ix - rr*3/4, iy + rr/2}, {ix - rr*3/4, iy - rr/2}
            };
            SDL_RenderDrawLines(renderer, ring, 6);
            SDL_RenderDrawLine(renderer, ring[5].x, ring[5].y, ring[0].x, ring[0].y);
        }
    }

    void drawHPBar(SDL_Renderer* renderer, const Font& font) const {
        if (!active) return;
        const int BAR_W = 400, BAR_H = 14;
        const int BAR_X = CENTER_X - BAR_W / 2, BAR_Y = 8;
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_Rect bg = {BAR_X, BAR_Y, BAR_W, BAR_H};
        SDL_RenderFillRect(renderer, &bg);

        int baseW = (int)((double)hp / maxHp * BAR_W);
        if (baseW > BAR_W) baseW = BAR_W;
        int bonusW = (int)((double)bonusHp / maxHp * BAR_W);

        SDL_SetRenderDrawColor(renderer, 220, 30, 30, 255);
        SDL_Rect baseR = {BAR_X, BAR_Y, baseW, BAR_H};
        SDL_RenderFillRect(renderer, &baseR);
        if (bonusHp > 0 && bonusW > 0) {
            SDL_SetRenderDrawColor(renderer, 255, 140, 140, 255);
            SDL_Rect bonusR = {BAR_X + baseW, BAR_Y, bonusW, BAR_H};
            SDL_RenderFillRect(renderer, &bonusR);
        }
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        int borderW = baseW + bonusW + 2;
        if (borderW < BAR_W + 2) borderW = BAR_W + 2;
        SDL_Rect border = {BAR_X-1, BAR_Y-1, borderW, BAR_H+2};
        SDL_RenderDrawRect(renderer, &border);
        if (cfg && cfg->name) {
            int nameLen = (int)strlen(cfg->name);
            int namePxW = nameLen * 6 * 2;  // font size 2, each char 12px
            int nameX = BAR_X - namePxW - 8;
            for (int i = 0; cfg->name[i]; ++i) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                font.drawChar(renderer, cfg->name[i], nameX + i * 12, BAR_Y + 1, 2);
            }
        }
    }

    // Writable refs for game loop
    int& absorbTimerRef() { return absorbTimer; }
    int& postAbsorbTimerRef() { return postAbsorbTimer; }
    int& shakeTimerRef() { return shakeTimer; }
    int& shakeXRef() { return shakeX; }
    int& shakeYRef() { return shakeY; }
    int& flashTimerRef() { return flashTimer; }
    int& absorbCooldownRef() { return absorbCooldown; }
    bool& phase2TriggeredRef() { return phase2Triggered; }
    bool& enteringRef() { return entering; }
    AbsorbState& absorbStateRef() { return absorbState; }
    int& absorbTargetIdxRef() { return absorbTargetIdx; }
    void setCh1HealWavesEnabled(bool v) { healWavesEnabled = v; }
    void setLastHitBySW(int id) { lastHitBySW = id; }
};
