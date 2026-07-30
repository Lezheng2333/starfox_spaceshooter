#pragma once

#include "audio.h"
#include "constants.h"
#include "player.h"
#include "renderer.h"
#include "types.h"

// ============== BulletManager ==============
class BulletManager {
    std::vector<Ch1Bullet> bullets;
    int fireCooldown;
    int fireDelay;
    double bulletSpeed;
    static const int BASE_FIRE_DELAY = 7;
    static constexpr double BASE_BULLET_SPEED = 60.0;
    static constexpr double BULLET_RANGE = 370.0;

public:
    BulletManager() : fireCooldown(0), fireDelay(BASE_FIRE_DELAY), bulletSpeed(BASE_BULLET_SPEED) {}

    void reset() { bullets.clear(); fireCooldown = 0; updateParams(0); }

    void updateParams(int lv) {
        if (lv < 1) lv = 1;
        double factor = std::pow(0.9, lv - 1);
        fireDelay = (int)(BASE_FIRE_DELAY * factor);
        if (fireDelay < 3) fireDelay = 3;
        bulletSpeed = BASE_BULLET_SPEED * std::pow(1.1, lv - 1);
    }

    void addBullet(TrainingPlane& pl, AudioEngine* audio) {
        double tPlane = pl.getT();
        double spread = (rand() % 40 - 20) / 1000.0;
        double tBullet = tPlane + spread;
        if (tBullet < 0.0) tBullet = 0.0;
        if (tBullet > 1.0) tBullet = 1.0;

        double ty = pl.getY() - BULLET_RANGE;
        double tx = perspLeft(ty) + tBullet * perspWidth(ty);

        double dirX = tx - pl.getX();
        double dirY = ty - pl.getY();
        double len = std::sqrt(dirX * dirX + dirY * dirY);
        if (len < 0.001) return;

        Ch1Bullet b;
        b.x = (double)pl.getX();
        b.y = (double)pl.getY();
        b.startX = pl.getX(); b.startY = pl.getY();
        b.dx = dirX / len; b.dy = dirY / len;
        b.active = true; b.canDamage = true;
        b.sideScroll = false; b.blueBeam = false; b.beamTargetIndex = -1;
        bullets.push_back(b);
        if (audio) audio->sndShoot();
    }

    void addBossBeam(double bossX, double bossY, double ax, double ay, int targetIdx) {
        Ch1Bullet beam;
        beam.x = bossX; beam.y = bossY;
        double bdx = ax - bossX, bdy = ay - bossY;
        double blen = std::sqrt(bdx*bdx + bdy*bdy);
        if (blen > 1.0) { beam.dx = bdx / blen; beam.dy = bdy / blen; }
        else { beam.dx = 0; beam.dy = -1; }
        beam.startX = bossX; beam.startY = bossY;
        beam.active = true; beam.canDamage = false; beam.sideScroll = false;
        beam.blueBeam = true; beam.beamTargetIndex = targetIdx;
        bullets.push_back(beam);
    }

    void addBulletSideScroll(Player& pl, AudioEngine* audio) {
        addBulletSideScrollAt(pl, 14, 0, audio);
    }
    void addBulletSideScrollAt(Player& pl, int ox, int oy, AudioEngine* audio) {
        Ch1Bullet b;
        b.x = (double)pl.getX() + ox;
        b.y = (double)pl.getY() + oy;
        b.startX = b.x; b.startY = b.y;
        double scatter = (rand() % 16 - 8) / 50.0;
        b.dx = 11.0; b.dy = scatter;
        b.active = true; b.canDamage = true;
        b.sideScroll = true;
        b.blueBeam = false; b.beamTargetIndex = -1;
        bullets.push_back(b);
        if (audio) audio->sndShoot();
    }

    bool canFire() const { return fireCooldown == 0; }
    void decrementCooldown() { if (fireCooldown > 0) fireCooldown--; }
    void setCooldown() { fireCooldown = fireDelay; }

    void update(const std::vector<Ch1Alien>& aliens) {
        for (auto& b : bullets) {
            if (!b.active) continue;
            if (b.sideScroll) {
                b.x += b.dx; b.y += b.dy;
                if (b.x > WIN_WIDTH + 30) b.active = false;
                continue;
            }
            if (b.blueBeam) {
                if (b.beamTargetIndex >= 0 && b.beamTargetIndex < (int)aliens.size()) {
                    const Ch1Alien& ta = aliens[b.beamTargetIndex];
                    if (ta.active) {
                        double ax = perspLeft(ta.y) + ta.t * perspWidth(ta.y);
                        double tdx = ax - b.x, tdy = ta.y - b.y;
                        double tlen = std::sqrt(tdx*tdx + tdy*tdy);
                        if (tlen > 1.0) { b.dx = tdx / tlen; b.dy = tdy / tlen; }
                    }
                }
                b.x += b.dx * 4.0;
                b.y += b.dy * 4.0;
                if (b.x < -60 || b.x > WIN_WIDTH + 60 || b.y < -60 || b.y > WIN_HEIGHT + 60)
                    b.active = false;
                continue;
            }
            double dist = std::sqrt((b.x - b.startX) * (b.x - b.startX) +
                                    (b.y - b.startY) * (b.y - b.startY));
            double remainRatio = 1.0 - dist / BULLET_RANGE;
            if (remainRatio <= 0.03) { b.active = false; continue; }
            if (remainRatio < 0.20) b.canDamage = false;
            double speed = bulletSpeed * remainRatio * remainRatio;
            b.x += b.dx * speed;
            b.y += b.dy * speed;
            if (b.x < -20 || b.x > WIN_WIDTH + 20 || b.y < -30 || b.y > WIN_HEIGHT + 10)
                b.active = false;
        }
    }

    void draw(SDL_Renderer* renderer) const {
        for (const auto& b : bullets) {
            if (!b.active) continue;
            if (b.sideScroll) {
                // Rightward bullet: white line + short trail
                SDL_SetRenderDrawColor(renderer, 80, 80, 80, 100);
                SDL_RenderDrawLine(renderer, (int)b.x-8, (int)b.y, (int)b.x-2, (int)b.y);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawLine(renderer, (int)b.x-2, (int)b.y, (int)b.x+14, (int)b.y);
                continue;
            }
            double dist = std::sqrt((b.x - b.startX) * (b.x - b.startX) +
                                    (b.y - b.startY) * (b.y - b.startY));
            double remainRatio = 1.0 - dist / BULLET_RANGE;
            if (remainRatio < 0.0) remainRatio = 0.0;
            double bodyLen = 16.0 * remainRatio;
            if (bodyLen < 1.5) bodyLen = 1.5;

            int alpha = 255;
            if (!b.canDamage && remainRatio < 0.15) {
                alpha = (int)(255.0 * (remainRatio - 0.03) / 0.12);
                if (alpha < 20) alpha = 20;
                if (alpha > 255) alpha = 255;
            }

            double hx = b.x + b.dx * bodyLen * 0.5;
            double hy = b.y + b.dy * bodyLen * 0.5;
            double tx = b.x - b.dx * bodyLen * 0.5;
            double ty = b.y - b.dy * bodyLen * 0.5;
            double trailLen = bodyLen * 0.7;
            double trx = tx - b.dx * trailLen;
            double try_ = ty - b.dy * trailLen;
            int trailAlpha = (int)(alpha * 0.35);
            if (trailAlpha < 8) trailAlpha = 8;

            if (b.blueBeam)
                SDL_SetRenderDrawColor(renderer, 60, 160, 255, (unsigned char)alpha);
            else
                SDL_SetRenderDrawColor(renderer, 90, 90, 90, (unsigned char)trailAlpha);
            SDL_RenderDrawLine(renderer, (int)tx, (int)ty, (int)trx, (int)try_);
            if (b.blueBeam)
                SDL_SetRenderDrawColor(renderer, 100, 200, 255, (unsigned char)alpha);
            else
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, (unsigned char)alpha);
            SDL_RenderDrawLine(renderer, (int)tx, (int)ty, (int)hx, (int)hy);
        }
    }

    void removeInactive() {
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Ch1Bullet& b){ return !b.active; }), bullets.end());
    }

    std::vector<Ch1Bullet>& all() { return bullets; }
    int getFireDelay() const { return fireDelay; }
    double getBulletSpeed() const { return bulletSpeed; }
    static double getBulletRange() { return BULLET_RANGE; }
};
