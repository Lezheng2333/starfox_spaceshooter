#pragma once

#include "types.h"

class ChapterManager {
    int currentIdx;
    bool unlocked[5];
    ChapterConfig chapters[5];

    void initChapter(int idx, const char* title, bool ul, int spawnInt, double alienSpd,
                     int fireDly, double bullSpd, int swDmg, int swInt, int bossTrig,
                     BossConfig bc, int hy, int gcR, int gcG, int gcB,
                     int scR, int scG, int scB, int stars, float starB) {
        chapters[idx].chapterNumber = idx + 1;
        chapters[idx].title = title;
        chapters[idx].unlocked = ul;
        chapters[idx].baseSpawnInterval = spawnInt;
        chapters[idx].baseAlienSpeed = alienSpd;
        chapters[idx].baseFireDelay = fireDly;
        chapters[idx].baseBulletSpeed = bullSpd;
        chapters[idx].baseShockwaveDamage = swDmg;
        chapters[idx].baseShockwaveInterval = swInt;
        chapters[idx].bossTriggerScore = bossTrig;
        chapters[idx].bossConfig = bc;
        chapters[idx].horizonY = hy;
        chapters[idx].groundColorR = gcR; chapters[idx].groundColorG = gcG; chapters[idx].groundColorB = gcB;
        chapters[idx].skyColorR = scR; chapters[idx].skyColorG = scG; chapters[idx].skyColorB = scB;
        chapters[idx].starCount = stars;
        chapters[idx].starBrightness = starB;
        chapters[idx].alienTypesMask = 1;
        chapters[idx].fastAlienChance = 0;
        chapters[idx].tankAlienChance = 0;
        chapters[idx].hasMeteorShowers = false;
        chapters[idx].hasMovingBase = false;
        chapters[idx].hasTimeLimit = false;
        chapters[idx].isSideScrolling = false;
        chapters[idx].timeLimitSeconds = 0;
    }

public:
    ChapterManager() : currentIdx(0) {
        for (int i = 0; i < 5; ++i) unlocked[i] = (i == 0);

        // Chapter 1: 默认参数（与当前游戏一致）
        BossConfig bc1 = {1000, 140.0, 25.0, 0.025, 80.0, 150, 70, 50, 420, 30, 60, true, true, "TELAMONDO"};
        initChapter(0, "FIRST FLIGHT", true, 110, 1.75, 7, 60.0, 3, 12, 200,
                   bc1, 200, 80,80,80, 50,50,50, 100, 1.0f);

        // Chapter 2: 侧滚廊桥 (Boss待设计，暂用Ch1配置)
        initChapter(1, "DEEP SPACE", false, 100, 2.0, 6, 65.0, 3, 11, 220,
                   bc1, 200, 60,60,80, 40,40,60, 120, 1.0f);
        chapters[1].isSideScrolling = true;

        // Chapters 3-5: Boss待设计，暂用Ch1配置
        initChapter(2, "ENEMY FORTRESS", false, 90, 2.25, 6, 65.0, 4, 10, 250,
                   bc1, 200, 40,60,40, 30,40,30, 130, 1.0f);
        initChapter(3, "ASTEROID BELT", false, 80, 2.5, 5, 70.0, 4, 9, 250,
                   bc1, 200, 70,50,30, 50,40,30, 140, 1.0f);
        initChapter(4, "FINAL ASSAULT", false, 70, 2.75, 5, 75.0, 5, 8, 280,
                   bc1, 200, 60,30,30, 35,25,25, 150, 1.0f);
    }

    void selectChapter(int idx) { currentIdx = idx; }
    int getCurrentIndex() const { return currentIdx; }
    bool isUnlocked(int idx) const { return unlocked[idx]; }
    void unlockChapter(int idx) { unlocked[idx] = true; }
    const ChapterConfig& getConfig() const { return chapters[currentIdx]; }
    const ChapterConfig& getChapterConfig(int idx) const { return chapters[idx]; }
    void reset() { currentIdx = 0; }
};
