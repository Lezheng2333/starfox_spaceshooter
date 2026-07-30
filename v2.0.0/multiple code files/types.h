#pragma once

#include <cmath>
#include <cstdio>


// ============== 共享数据结构 ==============
struct Star { float x, y; float phase; float twinkleSpeed; float driftSpeed; };

struct FloatingText {
    float x, y;
    int life, totalLife;
    char text[32];
    int r, g, b;
};

struct BossConfig {
    int maxHp;
    double moveAmplitudeX, moveAmplitudeY, moveFrequency, size;
    int enterDuration, absorbDuration, bonusHpPerAlien;
    int healWaveInterval, healHpPerWave, shakeDuration;
    bool hasCh1HealWaves, hasPhase2;
    const char* name;
};

struct ChapterConfig {
    int chapterNumber;
    const char* title;
    bool unlocked;
    int baseSpawnInterval;
    double baseAlienSpeed;
    int baseFireDelay;
    double baseBulletSpeed;
    int baseShockwaveDamage, baseShockwaveInterval;
    int bossTriggerScore;
    int alienTypesMask;
    double fastAlienChance, tankAlienChance;
    BossConfig bossConfig;
    int horizonY, groundColorR, groundColorG, groundColorB;
    int skyColorR, skyColorG, skyColorB;
    int starCount;
    float starBrightness;
    bool hasMeteorShowers, hasMovingBase, hasTimeLimit, isSideScrolling;
    int timeLimitSeconds;
};

struct FontChar { unsigned char rows[7]; };

struct ActiveSound {
    float freq, sweepEnd;
    int totalSamples, samplesLeft;
    float volume;
    int type, band;
    float phase;
};

struct MenuItem {
    const char* label;
    bool enabled;
    bool isToggle;
    bool* toggleValue;
    int* sliderValue;
    int sliderMin, sliderMax;
    bool sliderSymmetric;
    int actionId;
};


// === Chapter 1 data structures ===
struct Ch1Particle {
    double x, y, vx, vy;
    int life;
    bool active;
    bool whiteParticle, greenParticle, redParticle;
};

struct BulletBase {
    double x, y, dx, dy;
    bool active;
};

struct Ch1Bullet : BulletBase {
    double startX, startY;
    bool canDamage, blueBeam;
    bool sideScroll;      // Ch2: simple rightward flight, no range decay
    int beamTargetIndex;
};

// === Enemy base traits (shared by all enemy types) ===
struct EnemyData {
    bool active, entering, defeated;
    int enterFrame, enterDuration;
    int invincibleFrames;
    int hp, maxHp;
};

struct Ch1Alien : EnemyData {
    double y, t;
    double startT, targetT;
    double enterStartY, enterTargetY;
    bool enterFromTop, enterFromBoss;
    int lastHitBySW, lastHealHit;
    bool beingAbsorbed;
    int absorbFrame, absorbDuration;
    double absorbStartX, absorbStartY;
    int alienType;
    int behaviorData[4];
};

struct Ch1Shockwave { double y; int id; bool active; };

struct Ch1HealWave { double radius; int id; bool active; };


// === Chapter 2 enemy bullet (shared base) ===
struct Ch2EnemyBullet {
    double x, y, dx, dy;
    int hp; bool active;
};

// === Chapter 2 pulse wave ===
struct Ch2PulseWave {
    double x, y;      // origin center (player position at release)
    double radius;    // current radius
    int id;           // unique ID for per-enemy collision tracking
    bool active;
};

// === Chapter 2 danmaku data structures ===
struct Ch2DanmakuEnemy : EnemyData {
    double x, y;          // current center position
    double startX, startY, targetX, targetY;  // entrance trajectory
    double baseX, baseY;  // oscillation center (after entrance)
    double movePhase;     // oscillation phase (radians)
    double moveSpeed;     // per-enemy oscillation speed variation
    int leg, farLeg, nearLeg, vpLean;  // computed leg sizes for collision/draw
    int vulnTimer;         // counts down vulnerable time (0 = done)
    int fireTimer;
    int fireInterval;      // frames between spiral shots
    double fireAngle;      // current spiral angle
    int defeatTimer;       // countdown after defeat for animation
    int lastHitByPulse;    // pulse wave ID already applied to this enemy
};
