#pragma once

#include "font.h"
#include "renderer.h"
#include "audio.h"

class FloatingTextManager;
class ParticleManager;
class TrainingPlane;
class Ch2Trainer;
class NightElf;
class BulletManager;
class Ch1ShockwaveManager;
class Ch1AlienManager;
class Ch1Boss;
class ChapterManager;
class Ch1Background;
class Ch2SphereBoss;
class Ch2Background;
class Ch2DanmakuManager;
class Ch2AlienManager;
class UIRenderer;
class MenuStateMachine;

#include "audio.h"
#include "bullets.h"
#include "constants.h"
#include "floating_text.h"
#include "font.h"
#include "particles.h"
#include "player.h"
#include "renderer.h"
#include "types.h"
#include "ch1/ch1_aliens.h"
#include "ch2/ch2_hud.h"

// ============== Game 类 ==============

enum GamePhase { PHASE_PLAY, PHASE_BOSS_INTRO, PHASE_BOSS_FIGHT, PHASE_BOSS_PHASE2, PHASE_BOSS_DEFEAT };

class Game {
    Renderer& renderer;
    AudioEngine& audio;
    Font font;

    TrainingPlane trainingPlane;
    Ch2Trainer ch2Trainer;
    NightElf nightElf;
    Player* player;
    BulletManager bulletMgr;
    Ch1AlienManager alienMgr;
    ParticleManager particleMgr;
    Ch1ShockwaveManager shockwaveMgr;
    Ch1Boss boss;
    FloatingTextManager floatingTextMgr;
    NarrationSystem narration;
    DialogueSystem dialogueSys;
    ChapterManager chapterMgr;

    SDL_Texture* shakeTex;

    // Core state
    GamePhase phase;
    int score;
    int baseHP;
    int difficultyTimer;
    bool paused;
    bool gameOver;
    bool aimAssistOn;
    bool inNarration;
    bool ch1DialogueDone;
    bool bossPhase2DialogueTriggered;
    bool triggeredScores[256];
    int baseFireTimer;
    int lastScore;
    bool pauseHistoryFocused;
    bool enemiesEnabled;

    // Screens
    bool atStartScreen, atTestSelect, atChapterSelect, atOptionScreen, atSoundMenu;
    bool optionFromPause;
    bool optionJustEntered;

    // Menu state
    int startMenuSelection, testScoreSelection, chapterSelection, menuSelection;
    int pauseMenuSelection, optionCursor;
    int testChapterSelection;
    bool testAtChapterSelect;

    // Countdown
    int countdown, countdownFrame;

    // Sound menu cursor
    int soundCursor;

    // Ch1Boss defeat sequence
    int bossDefeatTimer, defeatAlienTimer, defeatReturnTimer, defeatFWTimer;
    int defeatMCDelay, defeatFadeTimer;
    bool missionCompleteShown, missionComplete;
    int mcMenuSelection;

    // Chapter unlock tracking
    bool isNormalPlay;

    // Ch2 energy barrier (right wall only)
    int wallFlashTimer;
    int wallContactY;      // Y position where plane touched the barrier
    int wallAnimFrame;     // animates lightning/sparks while touching

    // Ch2 enemy systems
    int ch2PlayerHP; bool ch2GameOver;
    Ch2AlienManager ch2AlienMgr;
    Ch2DanmakuManager dmMgr;
    int dmFireCooldown;    // player fire cooldown in side-scroll mode
    Ch2SphereBoss sphereBoss;
    bool sphereBossActive;
    NightElfEnergy nightElfEnergy;
    int playerHitCount;
    int tripleBeepCounter;
    Ch2PulseSystem pulseSystem;
    Ch2SkillOrb skillOrb;
    bool pulseOrbDropped; // first danmaku has dropped the orb
    bool shiftWas;        // Shift key edge detection
    bool shiftJustPressed; // true for one frame when Shift first pressed

    // Timing
    Uint32 lastTime;

    // Edge detection helpers for menus
    bool upWas, downWas, enterWas, escWas, leftWas, rightWas;
    int autoSpawnPhase;    // 0=idle,1=spawnW1(3),2=fightW1,3=spawnW2(5),4=fightW2,5=wave3,6=danmaku+done
    int autoSpawnQueued;   // aliens left to spawn in current spawning phase
    int autoSpawnTimer;    // countdown frames to next spawn (0.2s=12)
    int autoSpawnWave3Reinf;  // wave3 reinforcement rounds done (0-4)
    int autoSpawnScoreBase;   // score when auto-spawn activated (for kill counting across all waves)
    int autoSpawnAliveLast;   // alive+queued snapshot for delta escape detection
    int autoSpawnKillsLast;   // kills snapshot for delta
    int lastShockwaveLevel;

    // Ch1Background (per chapter, persistent)
    Ch1Background* background;
    Ch2Background* sideBg;

public:
    Game(Renderer& r, AudioEngine& a, SDL_Window*)
        : renderer(r), audio(a),
          player(&trainingPlane), shakeTex(nullptr),
          phase(PHASE_PLAY), score(0), baseHP(10), difficultyTimer(0),
          paused(false), gameOver(false), aimAssistOn(false), inNarration(false), ch1DialogueDone(false), bossPhase2DialogueTriggered(false), baseFireTimer(0), lastScore(-1), pauseHistoryFocused(false), enemiesEnabled(false),
          atStartScreen(true), atTestSelect(false), atChapterSelect(false),
          atOptionScreen(false), atSoundMenu(false), optionFromPause(false), optionJustEntered(true),
          startMenuSelection(0), testScoreSelection(0), chapterSelection(0), menuSelection(0),
          pauseMenuSelection(0), optionCursor(0), testChapterSelection(0), testAtChapterSelect(true),
          countdown(-1), countdownFrame(0), soundCursor(0),
          bossDefeatTimer(0), defeatAlienTimer(0), defeatReturnTimer(0), defeatFWTimer(0),
          defeatMCDelay(0), defeatFadeTimer(0),
          missionCompleteShown(false), missionComplete(false), mcMenuSelection(0),
          isNormalPlay(false),
          wallFlashTimer(0), wallContactY(0), wallAnimFrame(0),
          ch2PlayerHP(3), ch2GameOver(false),
          ch2AlienMgr(ch2PlayerHP, ch2GameOver), dmMgr(ch2PlayerHP, ch2GameOver),
          dmFireCooldown(0), sphereBossActive(false), playerHitCount(0), tripleBeepCounter(0),
          pulseOrbDropped(false), shiftWas(true), shiftJustPressed(false),
          lastTime(0), upWas(false), downWas(false), enterWas(false), escWas(false),
          leftWas(false), rightWas(false),
          autoSpawnPhase(0), autoSpawnQueued(0), autoSpawnTimer(0),
          autoSpawnWave3Reinf(0), autoSpawnScoreBase(0),
          autoSpawnAliveLast(0), autoSpawnKillsLast(0),
          lastShockwaveLevel(0),
          background(nullptr), sideBg(nullptr) {
        boss.setConfig(&chapterMgr.getConfig().bossConfig);
        background = new Ch1Background(chapterMgr.getConfig());
        sideBg = new Ch2Background();
    }

    ~Game() { delete background; delete sideBg; }

    void resetGame() {
        score = 0;
        gameOver = false;
        atStartScreen = true;
        atTestSelect = false;
        atChapterSelect = false;
        atOptionScreen = false;
        atSoundMenu = false;
        menuSelection = 0;
        startMenuSelection = 0;
        testScoreSelection = 0;
        testChapterSelection = 0;
        testAtChapterSelect = true;
        chapterSelection = 0;
        pauseMenuSelection = 0;
        difficultyTimer = 0;
        lastShockwaveLevel = 0;
        baseHP = 10;
        if (chapterMgr.getConfig().isSideScrolling) {
            ch2Trainer.reset(); player = &ch2Trainer;
        } else {
            trainingPlane.reset(); player = &trainingPlane;
        }
        bulletMgr.reset();
        alienMgr.reset();
        particleMgr.all().clear();
        shockwaveMgr.reset();
        boss.reset();
        boss.setConfig(&chapterMgr.getConfig().bossConfig);
        floatingTextMgr.clear();
                narration.reset(); dialogueSys.reset(); inNarration = false; ch1DialogueDone = false; bossPhase2DialogueTriggered = false;
        memset(triggeredScores, 0, sizeof(triggeredScores)); baseFireTimer = 0; lastScore = -1; enemiesEnabled = false;
        phase = PHASE_PLAY;
        paused = false;
        missionComplete = false; missionCompleteShown = false; mcMenuSelection = 0;
        wallFlashTimer = 0; wallContactY = 0; wallAnimFrame = 0;
        ch2AlienMgr.reset(); dmMgr.reset(); dmFireCooldown = 0;
        sphereBoss.reset(); sphereBossActive = false;
        nightElfEnergy.reset(); playerHitCount = 0; tripleBeepCounter = 0;
        pulseSystem.reset(); skillOrb.reset(); pulseOrbDropped = false; shiftWas = true;
        autoSpawnPhase = 0; autoSpawnQueued = 0; autoSpawnTimer = 0;
        autoSpawnWave3Reinf = 0; autoSpawnScoreBase = 0;
        autoSpawnAliveLast = 0; autoSpawnKillsLast = 0;
        bossDefeatTimer = 0; defeatAlienTimer = 0; defeatReturnTimer = 0;
        defeatFWTimer = 0; defeatMCDelay = 0; defeatFadeTimer = 0;
        countdown = -1; countdownFrame = 0;
        soundCursor = 0;
        if (background) { delete background; background = new Ch1Background(chapterMgr.getConfig()); }
        if (sideBg) sideBg->reset();
    }

    void run() {
        lastTime = SDL_GetTicks();
        bool running = true;
        SDL_Event e;

        while (running) {
            bool escPressed = false;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) running = false;
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) escPressed = true;
            }

            const Uint8* keys = SDL_GetKeyboardState(NULL);
            audio.setBossMusic(phase == PHASE_BOSS_FIGHT && boss.isActive());
            audio.setBgmOff(atStartScreen || atChapterSelect || atTestSelect || atOptionScreen || atSoundMenu || paused || gameOver || missionComplete);
            audio.setCh2Bgm(chapterMgr.getConfig().isSideScrolling &&
                !atStartScreen && !atChapterSelect && !atTestSelect && !atOptionScreen && !atSoundMenu && !gameOver);
            if (background) background->update();
            if (sideBg) sideBg->update();

            // ======== Esc key global ========
            if (escPressed && inNarration) {
                // ESC skips entire opening narration
                narration.reset(); inNarration = false;
            } else if (escPressed && !inNarration && !gameOver && !atStartScreen && !atTestSelect && !atChapterSelect
                && !atOptionScreen && !atSoundMenu && !missionComplete) {
                if (paused && countdown == -1) {
                    countdown = 3; countdownFrame = 0;
                } else {
                    paused = !paused;
                    pauseMenuSelection = 0;
                    if (paused) { pauseHistoryFocused = false; dialogueSys.history.resetView(); }
                }
            }

            // ======== Narrations ========
            if (inNarration) {
                updateNarration(keys);
                drawNarrationFrame();
            } else if (atStartScreen) {
                updateStartScreen(keys, running);
                drawStartScreen();
            } else if (atChapterSelect) {
                updateChapterScreen(keys);
                drawChapterScreen();
            } else if (atTestSelect) {
                updateTestScreen(keys);
                drawTestScreen();
            } else if (atOptionScreen && !atSoundMenu) {
                updateOptionScreen(keys);
                drawOptionScreen();
            } else if (atSoundMenu) {
                updateSoundMenu(keys);
                drawSoundMenu();
            } else if (gameOver) {
                updateGameOverScreen(keys, running);
                drawGameplayFrame();
                drawGameOverScreen();
            } else if (paused) {
                updatePaused(keys, running);
                drawGameplayFrame();
                if (countdown >= 0) drawCountdown();
                else if (paused) drawPauseMenu();
            } else {
                // ======== GAMEPLAY ========
                updateGameplay(keys);
                drawGameplayFrame();
                if (missionComplete) drawMissionComplete();
                if (paused && countdown >= 0) drawCountdown();
                else if (paused) drawPauseMenu();
            }

            renderer.present();

            Uint32 now = SDL_GetTicks();
            Uint32 elapsed = now - lastTime;
            if (elapsed < 16) SDL_Delay(16 - elapsed);
            lastTime = SDL_GetTicks();
        }
    }

private:
    // ======== START SCREEN ========
    void updateStartScreen(const Uint8* keys, bool& running) {
        static bool sJustEntered = true;
        bool upNow = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
        bool downNow = keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN];
        bool enterNow = keys[SDL_SCANCODE_RETURN];

        if (sJustEntered) {
            upWas = upNow; downWas = downNow; enterWas = enterNow;
            sJustEntered = false;
        }
        if (upNow && !upWas && startMenuSelection > 0)     startMenuSelection--;
        if (downNow && !downWas && startMenuSelection < 4) startMenuSelection++;
        if (enterNow && !enterWas) {
            if (startMenuSelection == 0) {
                resetGame(); atStartScreen = false;
                isNormalPlay = true;
                alienMgr.applyChapterConfig(chapterMgr.getConfig());
                bulletMgr.updateParams(0);
                shockwaveMgr.updateParams(0);
                startChapterNarration();
                sJustEntered = true;
            } else if (startMenuSelection == 1) {
                atStartScreen = false; atChapterSelect = true;
                chapterSelection = 0; sJustEntered = true;
            } else if (startMenuSelection == 2) {
                atStartScreen = false; atTestSelect = true;
                testScoreSelection = 0; testChapterSelection = 0;
                testAtChapterSelect = true; sJustEntered = true;
            } else if (startMenuSelection == 3) {
                atStartScreen = false; atOptionScreen = true;
                optionFromPause = false; optionCursor = 0;
                optionJustEntered = true; sJustEntered = true;
            } else running = false;
        }
        upWas = upNow; downWas = downNow; enterWas = enterNow;
    }

    void drawStartScreen() {
        SDL_Renderer* r = renderer.get();
        renderer.setColor(0, 0, 0); renderer.clear();
        if (background) background->drawStarsFullscreen(r);

        font.drawString(r, "STAR FOX", CENTER_X - 96, 70, 4);
        font.drawString(r, "SPACE SHOOTER", CENTER_X - 117, 120, 3);
        SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
        SDL_RenderDrawLine(r, CENTER_X - 200, 140, CENTER_X + 200, 140);
        SDL_RenderDrawLine(r, CENTER_X - 200, 142, CENTER_X + 200, 142);

        const char* items[5] = {"PLAY", "CHAPTER", "TEST", "OPTIONS", "EXIT"};
        const int MENU_Y0 = 210, GAP = 48;
        for (int i = 0; i < 5; ++i) {
            int itemW = (int)strlen(items[i]) * 6 * 4;
            int itemX = CENTER_X - itemW / 2;
            int itemY = MENU_Y0 + i * GAP;
            font.drawString(r, items[i], itemX, itemY, 4);
            if (i == startMenuSelection) {
                UIRenderer::drawMenuCursor(r, itemX - 30, itemY + 14, 12);
                UIRenderer::drawMenuUnderline(r, itemX, itemY + 32, itemW);
            }
        }
        font.drawString(r, "W/S:select  ENTER:confirm", CENTER_X - 150, 490, 2);
        SDL_SetRenderDrawColor(r, 120, 120, 120, 255);
        font.drawString(r, "Ver 1.2.20", 15, WIN_HEIGHT - 30, 2);
    }

    // ======== CHAPTER SCREEN ========
    void updateChapterScreen(const Uint8* keys) {
        static bool cJustEntered = true;
        MenuKeys mk(keys);
        if (cJustEntered) { upWas=mk.up; downWas=mk.down; enterWas=mk.enter; escWas=mk.esc; cJustEntered=false; }
        if (mk.up && !upWas)    { int cs = chapterSelection; while (cs > 0) { cs--; if (chapterMgr.isUnlocked(cs)) { chapterSelection = cs; break; } } }
        if (mk.down && !downWas) { int cs = chapterSelection; while (cs < 4) { cs++; if (chapterMgr.isUnlocked(cs)) { chapterSelection = cs; break; } } }
        if (mk.enter && !enterWas) {
            if (chapterMgr.isUnlocked(chapterSelection)) {
                chapterMgr.selectChapter(chapterSelection);
                resetGame(); atStartScreen = false; atChapterSelect = false;
                isNormalPlay = true;
                alienMgr.applyChapterConfig(chapterMgr.getConfig());
                bulletMgr.updateParams(0);
                shockwaveMgr.updateParams(0);
                startChapterNarration();
            }
        }
        if (mk.esc && !escWas) { atChapterSelect = false; atStartScreen = true; cJustEntered = true; }
        upWas=mk.up; downWas=mk.down; enterWas=mk.enter; escWas=mk.esc;
    }

    void drawChapterScreen() {
        SDL_Renderer* r = renderer.get();
        renderer.setColor(0, 0, 0); renderer.clear();
        if (background) background->drawStarsFullscreen(r);
        font.drawString(r, "SELECT CHAPTER", CENTER_X - 180, 60, 4);
        SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
        SDL_RenderDrawLine(r, CENTER_X - 180, 100, CENTER_X + 180, 100);
        const char* chLabels[5] = {"CHAPTER 1", "CHAPTER 2", "CHAPTER 3", "CHAPTER 4", "CHAPTER 5"};
        const int Y0 = 150, GAP = 55;
        for (int i = 0; i < 5; ++i) {
            int itemW = (int)strlen(chLabels[i]) * 6 * 3;
            int itemX = CENTER_X - itemW / 2;
            int itemY = Y0 + i * GAP;
            bool locked = !chapterMgr.isUnlocked(i);
            SDL_SetRenderDrawColor(r, locked ? 80 : 255, locked ? 80 : 255, locked ? 80 : 255, 255);
            font.drawString(r, chLabels[i], itemX, itemY, 3);
            if (i == chapterSelection) {
                UIRenderer::drawMenuCursor(r, itemX - 24, itemY + 10, 10);
                UIRenderer::drawMenuUnderline(r, itemX, itemY + 24, itemW);
            }
            if (locked) font.drawString(r, "(LOCKED)", itemX + itemW + 10, itemY, 2);
        }
        font.drawString(r, "W/S:select  ENTER:start  ESC:back", CENTER_X - 210, 490, 2);
    }

    // ======== TEST SCREEN ========
    void updateTestScreen(const Uint8* keys) {
        static bool tJustEntered = true;
        MenuKeys mk(keys);
        if (tJustEntered) { upWas=mk.up; downWas=mk.down; enterWas=mk.enter; escWas=mk.esc; tJustEntered=false; return; }
        if (testAtChapterSelect) {
            // Level 1: Chapter selection
            if (mk.esc && !escWas) { atTestSelect = false; atStartScreen = true; tJustEntered = true; }
            if (mk.up && !upWas && testChapterSelection > 0)       testChapterSelection--;
            if (mk.down && !downWas && testChapterSelection < 4)   testChapterSelection++;
            if (mk.enter && !enterWas) {
                if (testChapterSelection == 0) { chapterMgr.selectChapter(0); testAtChapterSelect = false; tJustEntered = true; }
                else if (testChapterSelection == 1) {
                    chapterMgr.selectChapter(1);
                    testAtChapterSelect = false; testScoreSelection = 0; tJustEntered = true;
                }
            }
        } else {
            // Level 2: sub-menu for selected chapter
            bool isCh2 = chapterMgr.getConfig().isSideScrolling;
            int maxSel = isCh2 ? 2 : 9;
            if (mk.esc && !escWas) { testAtChapterSelect = true; tJustEntered = true; }
            if (mk.up && !upWas && testScoreSelection > 0)     testScoreSelection--;
            if (mk.down && !downWas && testScoreSelection < maxSel) testScoreSelection++;
        if (mk.enter && !enterWas) {
            int savedSel = testScoreSelection;
            if (isCh2) {
                // Chapter 2 sub-menu: two entry points
                resetGame(); atStartScreen = false; atTestSelect = false;
                isNormalPlay = false;
                alienMgr.applyChapterConfig(chapterMgr.getConfig());
                bulletMgr.updateParams(0);
                shockwaveMgr.updateParams(0);
                dmFireCooldown = 0;
                if (savedSel == 0) {
                    // Option 0: Full boss entry (entrance animation → fight → debris → combat)
                    sphereBoss.init(sideBg, player);
                    sphereBossActive = true;
                    sphereBoss.startEntering();
                } else if (savedSel == 1) {
                    // Option 1: Skip to combat (boss done, auto-spawn wave 1)
                    autoSpawnPhase = 1; autoSpawnQueued = 3; autoSpawnTimer = 0;
                } else {
                    // Option 2: Score 25 + first danmaku spawn (for pulse orb testing)
                    score = 25; autoSpawnPhase = 6; autoSpawnScoreBase = 0;
                    dmMgr.spawnEnemy();
                }
                tJustEntered = true; return;
            }
            // Chapter 1 sub-menu: score/target selection
            resetGame();
            atTestSelect = false; tJustEntered = true;
            atStartScreen = false;
            isNormalPlay = false;
            alienMgr.applyChapterConfig(chapterMgr.getConfig());
            bulletMgr.updateParams(0);
            shockwaveMgr.updateParams(0);
            const int testScores[7] = {0, 30, 60, 90, 120, 150, 180};
            if (savedSel < 7) {
                score = testScores[savedSel];
                shockwaveMgr.setPending(true);
                bulletMgr.updateParams(score / 30);
                shockwaveMgr.updateParams(score / 30);
                // Score 0: use opening delay like normal play
                if (score == 0) { lastScore = -1; }
                else { lastScore = score; enemiesEnabled = true; }
            } else if (savedSel == 7) {
                // 200 BOSS: 直接进入Boss登场动画
                score = 200; enemiesEnabled = true;
                bulletMgr.updateParams(score / 30);
                shockwaveMgr.updateParams(score / 30);
                boss.trigger();
                phase = PHASE_BOSS_INTRO;
                alienMgr.setAllInvincible();
                for (int i = 0; i < 5; ++i) {
                    Ch1Alien a;
                    a.targetT = 0.15 + (rand() % 700) / 1000.0;
                    a.t = a.targetT; a.y = 120.0 + (rand() % 180);
                    a.entering = false; a.enterFromTop = false; a.enterFromBoss = false;
                    a.invincibleFrames = -1; a.lastHitBySW = -1; a.lastHealHit = -1;
                    a.absorbFrame = 0; a.absorbDuration = 0;
                    a.absorbStartX = 0; a.absorbStartY = 0; a.beingAbsorbed = false;
                    a.alienType = 0;
                    a.hp = 3 + rand() % 3; a.maxHp = a.hp;
                    a.active = true;
                    alienMgr.pushAlien(a);
                }
            } else if (savedSel == 8) {
                // BOSS PH.2: 直接进入Boss二阶段
                score = 200; enemiesEnabled = true;
                bulletMgr.updateParams(score / 30);
                shockwaveMgr.updateParams(score / 30);
                boss.setY(90);
                boss.hpRef() = 500; boss.setMaxHp(1000); boss.bonusHpRef() = 0;
                boss.setActive(true);
                boss.enteringRef() = false;
                boss.shakeTimerRef() = 0;
                boss.phase2TriggeredRef() = true;
                boss.flashTimerRef() = 0;
                phase = PHASE_BOSS_PHASE2;
                boss.setCh1HealWavesEnabled(false);
                for (int i = 0; i < 5; ++i) {
                    Ch1Alien a;
                    a.targetT = 0.15 + (rand() % 700) / 1000.0;
                    a.t = a.targetT; a.y = 120.0 + (rand() % 180);
                    a.entering = false; a.enterFromTop = false; a.enterFromBoss = false;
                    a.invincibleFrames = -1; a.lastHitBySW = -1; a.lastHealHit = -1;
                    a.absorbFrame = 0; a.absorbDuration = 0;
                    a.absorbStartX = 0; a.absorbStartY = 0; a.beingAbsorbed = false;
                    a.alienType = 0;
                    a.hp = 3 + rand() % 3; a.maxHp = a.hp;
                    a.active = true;
                    alienMgr.pushAlien(a);
                }
            } else {
                // BOSS 1HP: 快速检验战败流程
                score = 200; enemiesEnabled = true;
                bulletMgr.updateParams(score / 30);
                shockwaveMgr.updateParams(score / 30);
                boss.setY(90);
                boss.hpRef() = 1; boss.setMaxHp(1000); boss.bonusHpRef() = 0;
                boss.setActive(true);
                boss.enteringRef() = false;
                boss.shakeTimerRef() = 0;
                boss.phase2TriggeredRef() = true;
                boss.flashTimerRef() = 0;
                phase = PHASE_BOSS_FIGHT;
                boss.setCh1HealWavesEnabled(true);
                shockwaveMgr.setPending(true);
            }
            // Pre-populate Ch1 dialogue history for test mode
            if (chapterMgr.getConfig().chapterNumber == 1 && score > 0) {
                #define PH(scr, spk, ...) \
                    if (score > scr) { \
                        triggeredScores[scr] = true; \
                        dialogueSys.history.add(spk, __VA_ARGS__); \
                    }
                PH(0, "Ally (ai copilot)", {"Martha, you're the only one in the air.", "Hold on as long as you can. The base shockwave cannon is charging."}, 2);
                PH(3, "Ally (ai copilot)", {"These enemies are made of energy.", "Destroy them. We can collect the energy."}, 2);
                PH(15, "", {"Tower communication restored."}, 1);
                PH(20, "Tower (ai)", {"Shockwave cannon ready."}, 1);
                if (score > 20) dialogueSys.history.add("Bryssa from Tower", {"A little more energy!"}, 1);
                PH(30, "Tower (ai)", {"Defense system charged.", "More enemies incoming. Keep gathering energy."}, 2);
                PH(40, "Ally (ai copilot)", {"Stay strong, Martha!"}, 1);
                if (score > 40) {
                    dialogueSys.history.add("Bryssa from Tower", {"The trainer shares energy with the base.", "You and the base will upgrade together."}, 2);
                }
                PH(50, "Tower (ai)", {"Keep gathering energy."}, 1);
                PH(55, "Ally (ai copilot)", {"System checking.", "Done."}, 2);
                PH(61, "Tower (ai)", {"Base upgraded again."}, 1);
                if (score > 61) dialogueSys.history.add("Bryssa from Tower", {"Radar shows even more enemies! Watch out!"}, 1);
                PH(70, "Ally (ai copilot)", {"System checking result:", "Aiming assist system on this plane.", "You shall find it somewhere."}, 3);
                PH(80, "Ally (ai copilot)", {"I've lost contact with the tower!", "But you and the base can upgrade again soon."}, 2);
                PH(105, "", {"Tower communication restored."}, 1);
                if (score > 105) {
                    dialogueSys.history.add("Tower (ai)", {"Massive energy signature detected.", "Analyzing source..."}, 2);
                }
                PH(120, "Bryssa from Tower", {"It's a capital ship.", "Telamondo-class. Martha, this is what the defense system was built for."}, 2);
                PH(160, "Tower (ai)", {"Enemy capital ship approaching.", "Entering weapons range in 40 seconds."}, 2);
                PH(180, "Bryssa from Tower", {"Shockwave Defense System is fully charged", "Martha, just keep them off us!"}, 2);
                PH(195, "Ally (ai copilot)", {"Here it comes...!"}, 1);
                #undef PH
            }
            lastScore = score;
        }
    }
        upWas=mk.up; downWas=mk.down; enterWas=mk.enter; escWas=mk.esc;
    }

    void drawTestScreen() {
        SDL_Renderer* r = renderer.get();
        renderer.setColor(0, 0, 0); renderer.clear();
        if (background) background->drawStarsFullscreen(r);

        if (testAtChapterSelect) {
            // Level 1: Chapter selection
            font.drawString(r, "TEST - SELECT CHAPTER", CENTER_X - 234, 60, 4);
            SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
            SDL_RenderDrawLine(r, CENTER_X - 180, 100, CENTER_X + 180, 100);
            const char* chLabels[5] = {"CHAPTER 1", "CHAPTER 2", "CHAPTER 3", "CHAPTER 4", "CHAPTER 5"};
            const int Y0 = 150, GAP = 55;
            for (int i = 0; i < 5; ++i) {
                int itemW = (int)strlen(chLabels[i]) * 6 * 3;
                int itemX = CENTER_X - itemW / 2;
                int itemY = Y0 + i * GAP;
                bool locked = (i > 1); // Chapter 1 & 2 unlocked for testing
                SDL_SetRenderDrawColor(r, locked ? 80 : 255, locked ? 80 : 255, locked ? 80 : 255, 255);
                font.drawString(r, chLabels[i], itemX, itemY, 3);
                if (i == testChapterSelection) {
                    UIRenderer::drawMenuCursor(r, itemX - 24, itemY + 10, 10);
                    UIRenderer::drawMenuUnderline(r, itemX, itemY + 24, itemW);
                }
                if (locked) font.drawString(r, "(LOCKED)", itemX + itemW + 10, itemY, 2);
            }
            font.drawString(r, "W/S:select  ENTER:enter  ESC:back", CENTER_X - 216, 490, 2);
        } else {
            // Level 2: Sub-menu for selected chapter
            bool isCh2 = chapterMgr.getConfig().isSideScrolling;
            if (isCh2) {
                font.drawString(r, "TEST - CHAPTER 2", CENTER_X - 192, 50, 4);
                SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
                SDL_RenderDrawLine(r, CENTER_X - 180, 90, CENTER_X + 180, 90);
                const char* labels[3] = {"SPHERE BOSS FULL", "COMBAT ONLY", "PULSE ORB TEST"};
                const int MENU_Y0 = 140, GAP = 52;
                for (int i = 0; i < 3; ++i) {
                    int itemW = (int)strlen(labels[i]) * 6 * 3;
                    int itemX = CENTER_X - itemW / 2;
                    int itemY = MENU_Y0 + i * GAP;
                    font.drawString(r, labels[i], itemX, itemY, 3);
                    if (i == testScoreSelection) {
                        UIRenderer::drawMenuCursor(r, itemX - 24, itemY + 10, 10);
                        UIRenderer::drawMenuUnderline(r, itemX, itemY + 24, itemW);
                    }
                }
                font.drawString(r, "W/S:select  ENTER:start  ESC:back", CENTER_X - 210, 490, 2);
            } else {
                font.drawString(r, "TEST - CHAPTER 1", CENTER_X - 192, 50, 4);
                SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
                SDL_RenderDrawLine(r, CENTER_X - 180, 90, CENTER_X + 180, 90);
                const char* testLabels[10] = {"0", "30", "60", "90", "120", "150", "180", "200 BOSS", "BOSS PH.2", "BOSS 1HP"};
                const int MENU_Y0 = 130, GAP = 48;
                for (int i = 0; i < 10; ++i) {
                    int itemW = (int)strlen(testLabels[i]) * 6 * 3;
                    int itemX = CENTER_X - itemW / 2;
                    int itemY = MENU_Y0 + i * GAP;
                    font.drawString(r, testLabels[i], itemX, itemY, 3);
                    if (i == testScoreSelection) {
                        UIRenderer::drawMenuCursor(r, itemX - 24, itemY + 10, 10);
                        UIRenderer::drawMenuUnderline(r, itemX, itemY + 24, itemW);
                    }
                }
                font.drawString(r, "W/S:select  ENTER:start  ESC:back", CENTER_X - 210, 490, 2);
            }
        }
    }

    // ======== OPTIONS SCREEN ========
    void updateOptionScreen(const Uint8* keys) {
        static bool oJustEntered = true;
        MenuKeys mk(keys);
        if (oJustEntered) { upWas=mk.up; downWas=mk.down; enterWas=mk.enter; escWas=mk.esc; oJustEntered=false; }
        if (mk.up && !upWas && optionCursor > 0)    optionCursor--;
        if (mk.down && !downWas && optionCursor < 1) optionCursor++;
        if (mk.enter && !enterWas) {
            if (optionCursor == 0) aimAssistOn = !aimAssistOn;
            else if (optionCursor == 1) { atSoundMenu = true; oJustEntered = true; }
        }
        if (mk.esc && !escWas) {
            optionCursor = 0; atOptionScreen = false; oJustEntered = true;
            if (optionFromPause) paused = true;
            else atStartScreen = true;
        }
        upWas=mk.up; downWas=mk.down; enterWas=mk.enter; escWas=mk.esc;
    }

    void drawOptionScreen() {
        SDL_Renderer* r = renderer.get();
        renderer.setColor(0, 0, 0); renderer.clear();
        if (background) background->drawStarsFullscreen(r);
        font.drawString(r, "OPTIONS", CENTER_X - 84, 40, 4);
        SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
        SDL_RenderDrawLine(r, CENTER_X - 180, 78, CENTER_X + 180, 78);
        const char* labels[2] = {"AIM ASSIST", "SOUND"};
        const int Y0 = 130, GAP = 70;
        for (int i = 0; i < 2; ++i) {
            int ly = Y0 + i * GAP;
            int lx = CENTER_X - 120;
            if (i == optionCursor) UIRenderer::drawMenuCursor(r, lx - 22, ly + 7, 10);
            font.drawString(r, labels[i], lx, ly, 3);
            if (i == 0) {
                SDL_SetRenderDrawColor(r, aimAssistOn ? 100 : 200, aimAssistOn ? 255 : 60, 100, 255);
                SDL_Rect tg = {CENTER_X + 80, ly - 2, 56, 26};
                SDL_RenderFillRect(r, &tg);
                font.drawString(r, aimAssistOn ? "ON" : "OFF", CENTER_X + 88, ly + 4, 2);
            } else {
                font.drawString(r, ">", CENTER_X + 80, ly, 3);
            }
            if (i == optionCursor) {
                int itemW = (int)strlen(labels[i]) * 6 * 3;
                UIRenderer::drawMenuUnderline(r, lx, ly + 24, itemW);
            }
        }
        font.drawString(r, "W/S:select  ENTER:confirm  ESC:back", CENTER_X - 216, 490, 2);
    }

    // ======== SOUND MENU ========
    void updateSoundMenu(const Uint8* keys) {
        static bool sJustEntered = true;
        MenuKeys mk(keys);
        bool leftNow = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
        bool rightNow = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];
        if (sJustEntered) {
            upWas=mk.up; downWas=mk.down; escWas=mk.esc; enterWas=mk.enter;
            leftWas=leftNow; rightWas=rightNow; sJustEntered=false;
        }
        if (mk.up && !upWas && soundCursor > 0)    soundCursor--;
        if (mk.down && !downWas && soundCursor < 5) soundCursor++;
        if (mk.esc && !escWas)  { atSoundMenu = false; sJustEntered = true; }
        if (mk.enter && !enterWas && soundCursor == 5) { atSoundMenu = false; sJustEntered = true; }

        if (leftNow && !leftWas) {
            if (soundCursor == 0) audio.adjBgmVolume(-1);
            if (soundCursor == 1) audio.adjSfxVolume(-1);
            if (soundCursor == 2) audio.adjEqLow(-1);
            if (soundCursor == 3) audio.adjEqMid(-1);
            if (soundCursor == 4) audio.adjEqHigh(-1);
        }
        if (rightNow && !rightWas) {
            if (soundCursor == 0) audio.adjBgmVolume(1);
            if (soundCursor == 1) audio.adjSfxVolume(1);
            if (soundCursor == 2) audio.adjEqLow(1);
            if (soundCursor == 3) audio.adjEqMid(1);
            if (soundCursor == 4) audio.adjEqHigh(1);
        }
        upWas=mk.up; downWas=mk.down; escWas=mk.esc; enterWas=mk.enter;
        leftWas=leftNow; rightWas=rightNow;
    }

    void drawSoundMenu() {
        SDL_Renderer* r = renderer.get();
        renderer.setColor(0, 0, 0); renderer.clear();
        if (background) background->drawStarsFullscreen(r);
        font.drawString(r, "SOUND", CENTER_X - 60, 40, 4);
        SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
        SDL_RenderDrawLine(r, CENTER_X - 180, 78, CENTER_X + 180, 78);
        const char* labels[6] = {"BGM VOL", "SFX VOL", "EQ LOW", "EQ MID", "EQ HIGH", "BACK"};
        const int Y0 = 110, GAP = 48;
        for (int i = 0; i < 6; ++i) {
            int ly = Y0 + i * GAP;
            int lx = CENTER_X - 120;
            if (i == soundCursor) UIRenderer::drawMenuCursor(r, lx - 22, ly + 7, 10);
            font.drawString(r, labels[i], lx, ly, 3);
            if (i == soundCursor) {
                int itemW = (int)strlen(labels[i]) * 6 * 3;
                UIRenderer::drawMenuUnderline(r, lx, ly + 24, itemW);
            }
            int sx = CENTER_X + 20, sw = 160;
            char vbuf[8];
            switch (i) {
                case 0: snprintf(vbuf, sizeof(vbuf), "%d", audio.getBgmVolume());
                        UIRenderer::drawSlider(r, sx, ly+6, sw, audio.getBgmVolume(), 1, 10, false); break;
                case 1: snprintf(vbuf, sizeof(vbuf), "%d", audio.getSfxVolume());
                        UIRenderer::drawSlider(r, sx, ly+6, sw, audio.getSfxVolume(), 1, 10, false); break;
                case 2: snprintf(vbuf, sizeof(vbuf), "%+d", audio.getEqLow());
                        UIRenderer::drawSlider(r, sx, ly+6, sw, audio.getEqLow(), -5, 5, true); break;
                case 3: snprintf(vbuf, sizeof(vbuf), "%+d", audio.getEqMid());
                        UIRenderer::drawSlider(r, sx, ly+6, sw, audio.getEqMid(), -5, 5, true); break;
                case 4: snprintf(vbuf, sizeof(vbuf), "%+d", audio.getEqHigh());
                        UIRenderer::drawSlider(r, sx, ly+6, sw, audio.getEqHigh(), -5, 5, true); break;
            }
            if (i < 5) font.drawString(r, vbuf, sx + sw + 10, ly, 2);
        }
        font.drawString(r, "W/S:select  A/D:adjust  ENTER/ESC:back", CENTER_X - 232, 490, 2);
    }

    // ======== GAMEPLAY UPDATE ========
    void updateGameplay(const Uint8* keys) {
        bool isSide = chapterMgr.getConfig().isSideScrolling;
        if (isSide) {
            // ======== Chapter 2 side-scrolling update ========
            bool moveLeft  = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
            bool moveRight = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];
            bool moveUp    = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
            bool moveDown  = keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN];
            bool shoot     = keys[SDL_SCANCODE_SPACE];
            int px = player->getX(), py = player->getY();
            if (moveLeft)  px -= 6;
            if (moveRight) px += 6;
            if (moveUp)    py -= 6;
            if (moveDown)  py += 6;
            if (px < 10) px = 10;
            if (py < 10) py = 10;
            if (py > WIN_HEIGHT - 10) py = WIN_HEIGHT - 10;

            int noseX = px + player->getNoseOffset();
            // Energy wall only during combat (after auto-spawn begins)
            if (autoSpawnPhase > 0 && noseX > 642) {
                px = 642 - player->getNoseOffset();
                wallFlashTimer = 28; wallContactY = py; wallAnimFrame++;
            } else if (wallFlashTimer > 0) { wallFlashTimer--; wallAnimFrame++; }

            player->setX(px); player->setY(py);

            // TrainingPlane shooting (Chapter 1 original fire rate)
            if (dmFireCooldown > 0) dmFireCooldown--;
            if (shoot && dmFireCooldown <= 0) {
                int nGuns = player->getGunCount();
                for (int g = 0; g < nGuns; ++g) {
                    int ox, oy;
                    player->getGunOffset(g, ox, oy);
                    bulletMgr.addBulletSideScrollAt(*player, ox, oy, &audio);
                }
                dmFireCooldown = 7;
            }
            bulletMgr.update(alienMgr.all());
            bulletMgr.removeInactive();

            // ==== Ch2 auto-spawn wave system ====
            // Start after sphere boss finishes all animations (DONE state)
            if (autoSpawnPhase == 0 && sphereBossActive && sphereBoss.getState() == Ch2SphereBoss::DONE) {
                autoSpawnPhase = 1; autoSpawnQueued = 3; autoSpawnTimer = 0;
                autoSpawnScoreBase = score;  // record baseline for kill counting
                autoSpawnAliveLast = 0; autoSpawnKillsLast = 0;
            }
            // Spawn pump: processes any queued aliens (initial waves + escape replacements + reinforcements)
            if (autoSpawnQueued > 0) {
                if (autoSpawnTimer > 0) autoSpawnTimer--;
                if (autoSpawnTimer <= 0) {
                    ch2AlienMgr.forceSpawn();
                    autoSpawnQueued--;
                    autoSpawnTimer = 12; // 0.2s = 12 frames
                }
                // Auto-transition for waves 1,2 when initial batch fully spawned
                if (autoSpawnQueued <= 0 && (autoSpawnPhase == 1 || autoSpawnPhase == 3))
                    autoSpawnPhase++;
            }
            // Unified escape detection (fight phases 2,4,5): alive delta not from kills = escaped
            if (autoSpawnPhase == 2 || autoSpawnPhase == 4 || autoSpawnPhase == 5) {
                if (autoSpawnQueued <= 0) {
                    int alive = ch2AlienMgr.countLiving();
                    int kills = score - autoSpawnScoreBase;
                    int aliveLost = autoSpawnAliveLast - alive;
                    int killsGained = kills - autoSpawnKillsLast;
                    int escaped = aliveLost - killsGained;
                    if (escaped > 0) { autoSpawnQueued += escaped; autoSpawnTimer = 12; }
                    autoSpawnAliveLast = alive + autoSpawnQueued;
                    autoSpawnKillsLast = kills;
                }
            }
            // Wave1→Wave2 transition: field empty + score ≥ 3
            if (autoSpawnPhase == 2 && autoSpawnQueued <= 0) {
                if (ch2AlienMgr.countLiving() == 0 && (score - autoSpawnScoreBase) >= 3) {
                    autoSpawnPhase = 3; autoSpawnQueued = 5; autoSpawnTimer = 12;
                }
            }
            // Wave2→Wave3 transition: field empty + score ≥ 8
            if (autoSpawnPhase == 4 && autoSpawnQueued <= 0) {
                if (ch2AlienMgr.countLiving() == 0 && (score - autoSpawnScoreBase) >= 8) {
                    autoSpawnPhase = 5; autoSpawnQueued = 5; autoSpawnTimer = 12;
                    autoSpawnWave3Reinf = 0;
                }
            }
            // Wave 3: reinforcement + danmaku gate (phase 5)
            if (autoSpawnPhase == 5 && autoSpawnQueued <= 0) {
                int alive = ch2AlienMgr.countLiving();
                int kills = score - autoSpawnScoreBase - 8;  // kills within wave3 only
                // Reinforcement: every 3 wave3 kills, spawn 3 more (up to 4 rounds)
                while (autoSpawnWave3Reinf < 4 && kills >= 3 * (autoSpawnWave3Reinf + 1)) {
                    autoSpawnQueued += 3; autoSpawnWave3Reinf++;
                }
                // Danmaku gate: 4 reinf rounds done + field clear + no pending spawns
                if (autoSpawnWave3Reinf >= 4 && alive == 0 && autoSpawnQueued <= 0) {
                    dmMgr.spawnEnemy();
                    autoSpawnPhase = 6;
                }
            }
            player->updateInvFrames();
            ch2AlienMgr.update(bulletMgr, particleMgr, audio, score, *player, floatingTextMgr, playerHitCount);
            dmMgr.update(bulletMgr, particleMgr, audio, score, *player, floatingTextMgr, playerHitCount);
            if (ch2GameOver) gameOver = true;

            // Sphere boss update + scroll sync (only during ENTERING) + bg speed + bullet collision
            if (sphereBossActive) {
                if (sphereBoss.getState() == Ch2SphereBoss::ENTERING)
                    sphereBoss.syncScreenPos(sideBg->getScrollX());
                sphereBoss.update();
                double ts = sphereBoss.getBgTargetSpeed();
                if (ts >= 0 && sideBg) sideBg->setSpeed(ts);
            }
            if (sphereBossActive && sphereBoss.getState() == Ch2SphereBoss::FIGHT) {
                for (auto& b : bulletMgr.all()) {
                    if (!b.active || !b.canDamage) continue;
                    double dx = b.x - sphereBoss.getCx();
                    double dy = b.y - sphereBoss.getCy();
                    if (dx*dx + dy*dy < sphereBoss.getRadius() * sphereBoss.getRadius()) {
                        b.active = false;
                        sphereBoss.takeDamage(1);
                        sphereBoss.popDiamonds(1);
                        particleMgr.spawnExplosion(b.x, b.y, 2);
                    }
                }
            }

            // ==== Pulse energy / skill orb / Shift input ====
            bool shiftNow = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];
            shiftJustPressed = (shiftNow && !shiftWas);
            shiftWas = shiftNow;

            // Energy fill from bullet hits this frame
            pulseSystem.addEnergy(playerHitCount);
            playerHitCount = 0;

            // Skill orb: spawn on first danmaku defeat (after defeat animation ends)
            if (!pulseOrbDropped && !pulseSystem.unlocked) {
                for (const auto& de : dmMgr.getEnemies()) {
                    if (de.defeated && de.defeatTimer == 1) { // last frame of defeat animation
                        skillOrb.spawn(de.x, de.y);
                        pulseOrbDropped = true;
                        break;
                    }
                }
            }

            // Skill orb: update + bullet collision (shield break after 18 hits)
            if (skillOrb.isActive()) {
                skillOrb.update();
                if (skillOrb.state == Ch2SkillOrb::FLOATING) {
                    for (auto& b : bulletMgr.all()) {
                        if (!b.active || !b.canDamage) continue;
                        double dx = b.x - skillOrb.x, dy = b.y - skillOrb.y;
                        double hitR = skillOrb.radius + 5.0;
                        if (dx*dx + dy*dy < hitR*hitR) {
                            b.active = false; b.canDamage = false;
                            skillOrb.registerHit(particleMgr, audio);
                            break; // one hit per frame
                        }
                    }
                }
                // Orb absorption: hold Shift near core for 5 sec → fill energy bar
                // Release Shift during absorption → energy rapidly drains to 0 before retry
                if (skillOrb.isCore() && !skillOrb.pulseUnlocked()) {
                    double dx = skillOrb.x - player->getX();
                    double dy = skillOrb.y - player->getY();
                    bool nearPlayer = (dx*dx + dy*dy < 200.0*200.0);
                    // Drain energy to 0 if player released Shift or moved away
                    if (skillOrb.state == Ch2SkillOrb::ABSORBING && (!shiftNow || !nearPlayer) && !pulseSystem.isDraining()) {
                        skillOrb.stopAbsorb();
                        pulseSystem.startDrain();
                    }
                    // Start absorption: must be CORE, holding Shift, near player, energy at 0
                    if (skillOrb.state == Ch2SkillOrb::CORE && shiftNow && nearPlayer && pulseSystem.canAbsorb()) {
                        skillOrb.startAbsorb();
                    }
                    // Continuing absorption: each frame tick timer + fill energy
                    if (skillOrb.state == Ch2SkillOrb::ABSORBING && shiftNow && nearPlayer) {
                        skillOrb.tickAbsorb();
                        if (skillOrb.absorbTimer % 10 == 0) pulseSystem.addAbsorbEnergy();
                        for (int i = 0; i < 5; ++i) {
                            double sx = skillOrb.x + (rand()%14-7);
                            double sy = skillOrb.y + (rand()%14-7);
                            double vx = (player->getX() - sx) * 0.05 + (rand()%40-20)/15.0;
                            double vy = (player->getY() - sy) * 0.05 + (rand()%40-20)/15.0;
                            particleMgr.spawnWhiteParticle(sx, sy, vx, vy, 18 + rand() % 25);
                        }
                        if (skillOrb.pulseUnlocked()) {
                            pulseSystem.unlocked = true;
                            pulseSystem.energy = Ch2PulseSystem::MAX_ENERGY; // snap to full
                            autoSpawnQueued += 10;
                        }
                    }
                    // Energy drain: -3 per frame until 0
                    if (pulseSystem.isDraining()) {
                        pulseSystem.drainTick();
                    }
                }
            }

            // Pulse release: single Shift press when energy full
            if (pulseSystem.unlocked && pulseSystem.isFull() && shiftJustPressed) {
                pulseSystem.release((float)player->getX(), (float)player->getY(), particleMgr, audio);
            }
            pulseSystem.update();

            // Pulse wave collision: destroy all enemy bullets
            pulseSystem.collideWithBullets(
                const_cast<std::vector<Ch2EnemyBullet>&>(ch2AlienMgr.getBullets()), particleMgr);
            pulseSystem.collideWithBullets(
                const_cast<std::vector<Ch2EnemyBullet>&>(dmMgr.getBullets()), particleMgr);
            // Pulse wave collision: 1 damage per enemy per wave
            pulseSystem.collideWithAliens(ch2AlienMgr, particleMgr, audio, score, playerHitCount);
            pulseSystem.collideWithDanmaku(dmMgr, particleMgr, audio, score, playerHitCount);

            floatingTextMgr.update();
            particleMgr.update();
            particleMgr.removeInactive();
            return;
        }

        if (phase != PHASE_BOSS_DEFEAT) {
            // Ch1Boss trigger
            if (score >= chapterMgr.getConfig().bossTriggerScore && phase == PHASE_PLAY) {
                boss.trigger();
                phase = PHASE_BOSS_INTRO;
                alienMgr.setAllInvincible();
            }

            // Difficulty growth
            if (phase == PHASE_PLAY) {
                difficultyTimer++;
                alienMgr.updateMovementParams(difficultyTimer);
            }

            // TrainingPlane input
            player->handleInput(keys);

            // Shooting
            if (keys[SDL_SCANCODE_SPACE] && bulletMgr.canFire()) {
                bulletMgr.addBullet((TrainingPlane&)*player, &audio);
                bulletMgr.setCooldown();
            }
            bulletMgr.decrementCooldown();

            // Enable enemies after score-0 dialogue finishes (Ch1 only; Ch2+ immediately)
            if (!enemiesEnabled && (chapterMgr.getConfig().chapterNumber != 1 || (triggeredScores[0] && !dialogueSys.isActive())))
                enemiesEnabled = true;

            // Spawn
            if (!enemiesEnabled) {
                if (phase == PHASE_PLAY) alienMgr.spawnTimerRef()--;
            } else if (phase == PHASE_BOSS_FIGHT && boss.isActive()) {
                if (alienMgr.spawnTimerRef() <= 0) {
                    alienMgr.spawnAlienFromBoss(boss.getX(), boss.getY(), score);
                    alienMgr.spawnTimerRef() = alienMgr.currentSpawnInterval(score, difficultyTimer) + (rand() % 20);
                }
                alienMgr.spawnTimerRef()--;
            } else if (phase == PHASE_PLAY) {
                if (alienMgr.spawnTimerRef() <= 0) {
                    alienMgr.spawnAlien(score);
                    alienMgr.spawnTimerRef() = alienMgr.currentSpawnInterval(score, difficultyTimer) + (rand() % 30);
                }
                alienMgr.spawnTimerRef()--;
            }

            // Ch1Shockwave
            if (score >= 30 && (phase == PHASE_PLAY || phase == PHASE_BOSS_FIGHT)) {
                shockwaveMgr.attemptAutoRelease(score, *player, floatingTextMgr, &audio, &particleMgr);
            }

            // Updates
            bulletMgr.update(alienMgr.all());
            if (phase != PHASE_BOSS_INTRO && phase != PHASE_BOSS_PHASE2) {
                alienMgr.update(false, 0, gameOver, baseHP, particleMgr, &audio);
                if (gameOver) menuSelection = 0;
            }
            // Base fire effects (burning wreckage on the ground)
            baseFireTimer++;
            if (baseFireTimer % 4 == 0) {
                // Fire particles at 2-3 random spots on the base
                for (int fi = 0; fi < 3; ++fi) {
                    int fx = 100 + rand() % 600;
                    int fy = WIN_HEIGHT - 25 - rand() % 45;
                    particleMgr.spawnExplosion(fx, fy, 2 + rand() % 3);
                }
            }
            if (baseFireTimer % 30 == 0) {
                // Larger burst at a random base position
                int bx = 120 + rand() % 560;
                int by = WIN_HEIGHT - 20 - rand() % 35;
                particleMgr.spawnExplosion(bx, by, 8 + rand() % 10);
            }
            // Smoke: white particles rising from base
            if (baseFireTimer % 6 == 0) {
                for (int si = 0; si < 2; ++si) {
                    int sx = 80 + rand() % 640;
                    int sy = WIN_HEIGHT - 15 - rand() % 40;
                    double svx = (rand() % 30 - 15) / 20.0;
                    double svy = -(0.6 + (rand() % 40) / 100.0);
                    particleMgr.spawnWhiteParticle(sx, sy, svx, svy, 30 + rand() % 20);
                }
            }
            particleMgr.update();
            shockwaveMgr.update();
            floatingTextMgr.update();
            if (chapterMgr.getConfig().chapterNumber == 1) {
            if (!triggeredScores[0] && lastScore < 0 && score >= 0 && !dialogueSys.isActive()) {
                triggeredScores[0] = true;
                dialogueSys.queueDialogue("Ally (ai copilot)", "Martha, you're the only one in the air.");
                dialogueSys.queueDialogue("Ally (ai copilot)", "Hold on as long as you can. The base shockwave cannon is charging.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[3] && lastScore < 3 && score >= 3 && !dialogueSys.isActive()) {
                triggeredScores[3] = true;
                dialogueSys.queueDialogue("Ally (ai copilot)", "These enemies are made of energy.");
                dialogueSys.queueDialogue("Ally (ai copilot)", "Destroy them. We can collect the energy.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[15] && lastScore < 15 && score >= 15 && !dialogueSys.isActive()) {
                triggeredScores[15] = true;
                dialogueSys.queueDialogue("", "Tower communication restored.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[20] && lastScore < 20 && score >= 20 && !dialogueSys.isActive()) {
                triggeredScores[20] = true;
                dialogueSys.queueDialogue("Tower (ai)", "Shockwave cannon ready.");
                dialogueSys.queueDialogue("Bryssa from Tower", "A little more energy!");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[30] && lastScore < 30 && score >= 30 && !dialogueSys.isActive()) {
                triggeredScores[30] = true;
                dialogueSys.queueDialogue("Tower (ai)", "Defense system charged.");
                dialogueSys.queueDialogue("Tower (ai)", "More enemies incoming. Keep gathering energy.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[40] && lastScore < 40 && score >= 40 && !dialogueSys.isActive()) {
                triggeredScores[40] = true;
                dialogueSys.queueDialogue("Bryssa from Tower", "The trainer shares energy with the base.");
                dialogueSys.queueDialogue("Bryssa from Tower", "You and the base will upgrade together.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[50] && lastScore < 50 && score >= 50 && !dialogueSys.isActive()) {
                triggeredScores[50] = true;
                dialogueSys.queueDialogue("Tower (ai)", "Keep gathering energy.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[55] && lastScore < 55 && score >= 55 && !dialogueSys.isActive()) {
                triggeredScores[55] = true;
                dialogueSys.queueDialogue("Ally (ai copilot)", "System checking.");
                dialogueSys.queueDialogue("Ally (ai copilot)", "Done.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[61] && lastScore < 61 && score >= 61 && !dialogueSys.isActive()) {
                triggeredScores[61] = true;
                dialogueSys.queueDialogue("Tower (ai)", "Base upgraded again.");
                dialogueSys.queueDialogue("Bryssa from Tower", "Radar shows even more enemies! Watch out!");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[70] && lastScore < 70 && score >= 70 && !dialogueSys.isActive()) {
                triggeredScores[70] = true;
                dialogueSys.queueDialogue("Ally (ai copilot)", "System checking result:");
                dialogueSys.queueDialogue("Ally (ai copilot)", "Aim assist system on this plane.");
                dialogueSys.queueDialogue("Ally (ai copilot)", "You shall find it somewhere.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[80] && lastScore < 80 && score >= 80 && !dialogueSys.isActive()) {
                triggeredScores[80] = true;
                dialogueSys.queueDialogue("Ally (ai copilot)", "I've lost contact with the tower!");
                dialogueSys.queueDialogue("Ally (ai copilot)", "But you and the base can upgrade again soon.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[105] && lastScore < 105 && score >= 105 && !dialogueSys.isActive()) {
                triggeredScores[105] = true;
                dialogueSys.queueDialogue("", "Tower communication restored.");
                dialogueSys.queueDialogue("Tower (ai)", "Massive energy signature detected.");
                dialogueSys.queueDialogue("Tower (ai)", "Analyzing source...");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[120] && lastScore < 120 && score >= 120 && !dialogueSys.isActive()) {
                triggeredScores[120] = true;
                dialogueSys.queueDialogue("Bryssa from Tower", "It's a capital ship.");
                dialogueSys.queueDialogue("Bryssa from Tower", "Telamondo-class. Martha, this is what the defense system was built for.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[160] && lastScore < 160 && score >= 160 && !dialogueSys.isActive()) {
                triggeredScores[160] = true;
                dialogueSys.queueDialogue("Tower (ai)", "Enemy capital ship approaching.");
                dialogueSys.queueDialogue("Tower (ai)", "Entering weapons range in 40 seconds.");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[180] && lastScore < 180 && score >= 180 && !dialogueSys.isActive()) {
                triggeredScores[180] = true;
                dialogueSys.queueDialogue("Bryssa from Tower", "Shockwave Defense System is fully charged");
                dialogueSys.queueDialogue("Bryssa from Tower", "Martha, just keep them off us!");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[195] && lastScore < 195 && score >= 195 && !dialogueSys.isActive()) {
                triggeredScores[195] = true;
                dialogueSys.queueDialogue("Ally (ai copilot)", "Here it comes...!");
                dialogueSys.start(); lastScore = score;
            }
            if (!triggeredScores[210] && lastScore < 210 && score >= 210 && !dialogueSys.isActive()) {
                triggeredScores[210] = true;
                dialogueSys.queueDialogue("Bryssa from Tower", "Base defense systems are strengthening.");
                dialogueSys.queueDialogue("Ally (ai copilot)", "Firepower systems being enhanced.");
                dialogueSys.start(); lastScore = score;
            }
            if (bossPhase2DialogueTriggered && !dialogueSys.isActive()) {
                bossPhase2DialogueTriggered = false;
                dialogueSys.queueDialogue("Ally (ai copilot)", "Telamondo can absorb energy!");
                dialogueSys.queueDialogue("Ally (ai copilot)", "Our firepower can match it!");
                dialogueSys.start(); lastScore = score;
            }
            } // chapter 1 dialogue triggers
            if (!dialogueSys.isActive()) lastScore = score;
            dialogueSys.update(false);  // no ENTER skip for dialogue
            int ticks = dialogueSys.popTicks();
            const std::string& spk = dialogueSys.currentSpeaker();
            while (ticks-- > 0) {
                if (spk.find("Ally") != std::string::npos) audio.sndAllyTalk();
                else if (spk.find("Bryssa") != std::string::npos) audio.sndBryssaTalk();
                else if (spk.find("Tower") != std::string::npos) audio.sndTowerTalk();
                else if (spk.empty()) audio.sndSystemTalk();
                else audio.sndTeletype();
            }

            // Ch1Boss movement
            if (phase == PHASE_BOSS_FIGHT || phase == PHASE_BOSS_PHASE2) {
                boss.updateMovement();
            }
            if (phase == PHASE_BOSS_FIGHT && boss.isActive() && boss.isCh1HealWavesEnabled()) {
                boss.updateCh1HealWaves(particleMgr, alienMgr, &audio);
            }

            // Phase2 trigger
            int totalHP = boss.getHp() + boss.getBonusHp();
            if (phase == PHASE_BOSS_FIGHT && !boss.isPhase2Triggered() && boss.isActive() &&
                totalHP <= (boss.getMaxHp() + boss.getBonusHp()) / 2) {
                boss.triggerPhase2();
                phase = PHASE_BOSS_PHASE2;
                alienMgr.setAllInvincible();
                bossPhase2DialogueTriggered = true;
            }

            // ======== PHASE_BOSS_INTRO ========
            if (phase == PHASE_BOSS_INTRO) {
                boss.updateEnterAnimation(&audio);
                boss.updateShake(&audio);
                alienMgr.update(true, 0.2, gameOver, baseHP, particleMgr, &audio);
                alienMgr.setAllInvincible(); // keep all aliens blue after entry completes
                // Absorb logic
                if (!boss.isEntering() && boss.getShakeTimer() == 0 && boss.absorbTimerRef() >= 0) {
                    if (boss.updateAbsorbStateMachine(alienMgr, bulletMgr, particleMgr, &audio)) {
                        alienMgr.setAllVulnerable();
                    }
                    boss.updateAbsorbAnimations(alienMgr, particleMgr);
                }
                if (boss.postAbsorbTimerRef() > 0) {
                    boss.updatePostAbsorbShake(&audio);
                    if (boss.postAbsorbTimerRef() == 0) phase = PHASE_BOSS_FIGHT;
                }
            }

            // ======== PHASE_BOSS_PHASE2 ========
            if (phase == PHASE_BOSS_PHASE2) {
                boss.updateShake(&audio);
                alienMgr.update(true, 0.2, gameOver, baseHP, particleMgr, &audio);
                alienMgr.setAllInvincible(); // keep all aliens blue after entry completes
                if (boss.getShakeTimer() == 0 && boss.absorbTimerRef() >= 0) {
                    if (boss.updateAbsorbStateMachine(alienMgr, bulletMgr, particleMgr, &audio)) {
                        alienMgr.setAllVulnerable();
                        boss.setCh1HealWavesEnabled(true);
                    }
                    boss.updateAbsorbAnimations(alienMgr, particleMgr);
                }
                if (boss.postAbsorbTimerRef() > 0) {
                    boss.updatePostAbsorbShake(&audio);
                    if (boss.postAbsorbTimerRef() == 0) phase = PHASE_BOSS_FIGHT;
                }
            }

            // ======== COLLISION DETECTION ========
            // Ch1Bullet vs Ch1Alien
            for (auto& b : bulletMgr.all()) {
                if (!b.active || !b.canDamage) continue;
                for (auto& a : alienMgr.all()) {
                    if (!a.active) continue;
                    double ax = perspLeft(a.y) + a.t * perspWidth(a.y);
                    double dx = b.x - ax, dy = b.y - a.y;
                    double depthBelow = (a.y - HORIZON_Y) / (WIN_HEIGHT - HORIZON_Y);
                    double alienScale = (depthBelow < 0) ? 0.08 : 0.08 + 0.92 * depthBelow;
                    if (alienScale < 0.08) alienScale = 0.08;
                    if (alienScale > 1.0)  alienScale = 1.0;
                    double hitRadius = 28.0 * alienScale + 10.0;
                    // far (scale≈0.08) → hitR≈12, near (scale≈1.0) → hitR≈38
                    if (dx*dx + dy*dy < hitRadius * hitRadius) {
                        b.active = false; b.canDamage = false;
                        if (a.invincibleFrames != 0) {
                            particleMgr.spawnExplosion(b.x, b.y, 3);
                            break;
                        }
                        a.hp--;
                        audio.sndHit();
                        if (a.hp <= 0) {
                            a.active = false;
                            particleMgr.spawnExplosion(ax, a.y, 22);
                            audio.sndExplosionBig();
                            score++;
                            bulletMgr.updateParams(score / 30);
                            shockwaveMgr.updateParams(score / 30);
                        } else {
                            particleMgr.spawnExplosion(b.x, b.y, 4);
                        }
                        break;
                    }
                }
            }
            bulletMgr.removeInactive();

            // Ch1Shockwave vs Ch1Alien
            for (auto& a : alienMgr.all()) {
                if (!a.active || a.invincibleFrames != 0) continue;
                int scoreBefore = score;
                shockwaveMgr.collideWithAlien(a, particleMgr, &audio, score);
                if (score != scoreBefore) {
                    bulletMgr.updateParams(score / 30);
                    shockwaveMgr.updateParams(score / 30);
                }
            }

            // Blue beam hit detection
            for (auto& b : bulletMgr.all()) {
                if (!b.active || !b.blueBeam) continue;
                if (b.beamTargetIndex < 0 || b.beamTargetIndex >= (int)alienMgr.all().size()) continue;
                Ch1Alien& a = alienMgr.all()[b.beamTargetIndex];
                if (!a.active || a.beingAbsorbed) { b.active = false; continue; }
                double ax = perspLeft(a.y) + a.t * perspWidth(a.y);
                double dx = b.x - ax, dy = b.y - a.y;
                if (dx*dx + dy*dy < 28.0 * 28.0) {
                    b.active = false;
                    for (int i = 0; i < 12; ++i) {
                        particleMgr.spawnWhiteParticle(ax + (rand()%14-7), a.y + (rand()%14-7),
                            (rand()%30-15)/6.0, (rand()%30-15)/6.0, 15 + rand()%10);
                    }
                    a.beingAbsorbed = true;
                    audio.sndBossAbsorb();
                    a.absorbFrame = 0;
                    a.absorbDuration = 70;
                    a.absorbStartX = ax;
                    a.absorbStartY = a.y;
                    boss.absorbStateRef() = Ch1Boss::SPIRALING;
                }
            }

            // Ch1Bullet vs Ch1Boss
            if (boss.isActive() && phase != PHASE_BOSS_INTRO && phase != PHASE_BOSS_PHASE2 && phase != PHASE_BOSS_DEFEAT) {
                for (auto& b : bulletMgr.all()) {
                    if (!b.active || !b.canDamage) continue;
                    double dx = b.x - boss.getX(), dy = b.y - boss.getY();
                    if (dx*dx + dy*dy < 55.0 * 55.0) {
                        b.active = false; b.canDamage = false;
                        boss.flashTimerRef() = 5;
                        audio.sndBossHit();
                        particleMgr.spawnExplosion(b.x, b.y, 4);
                        boss.takeDamage(1);
                        if (boss.getHp() <= 0) {
                            phase = PHASE_BOSS_DEFEAT;
                            bossDefeatTimer = 0;
                        }
                    }
                }
                // Ch1Shockwave vs Ch1Boss
                shockwaveMgr.collideWithBoss(boss.getX(), boss.getY(), boss.lastHitBySWRef(),
                    boss.bonusHpRef(), boss.hpRef(), boss.flashTimerRef(), particleMgr, &audio);
                if (boss.getHp() <= 0) {
                    phase = PHASE_BOSS_DEFEAT;
                    bossDefeatTimer = 0;
                }
            }

            // Cleanup
            bulletMgr.removeInactive();
            alienMgr.removeInactive();
            particleMgr.removeInactive();
            shockwaveMgr.removeInactive();
        } else {
            // ======== PHASE_BOSS_DEFEAT ========
            updateBossDefeat(keys);
        }
    }

    void updateBossDefeat(const Uint8* keys) {
        bossDefeatTimer++;
        particleMgr.update();
        floatingTextMgr.update();

        if (bossDefeatTimer < 300) {
            if (bossDefeatTimer % 6 == 0)
                particleMgr.spawnExplosion(boss.getX() + (rand()%90-45), boss.getY() + (rand()%55-27), 10);
            if (bossDefeatTimer % 20 == 0)
                particleMgr.spawnExplosion(boss.getX() + (rand()%120-60), boss.getY() + (rand()%70-35), 20);
        }
        if (bossDefeatTimer == 300) {
            particleMgr.spawnExplosion(boss.getX(), boss.getY(), 120);
            audio.sndExplosionBig();
            boss.setActive(false);
            defeatAlienTimer = 0;
        }

        if (bossDefeatTimer > 300) {
            defeatAlienTimer++;
            if (defeatAlienTimer >= 60) {
                defeatAlienTimer = 0;
                for (auto& a : alienMgr.all()) {
                    if (a.active) {
                        double ax = perspLeft(a.y) + a.t * perspWidth(a.y);
                        particleMgr.spawnExplosion(ax, a.y, 22);
                        audio.sndExplosionBig();
                        a.active = false;
                        break;
                    }
                }
            }
        }

        bool anyAlien = false;
        for (const auto& a : alienMgr.all()) if (a.active) anyAlien = true;
        if (!anyAlien && bossDefeatTimer > 300 && !missionCompleteShown) {
            defeatMCDelay++;
            if (defeatMCDelay >= 60) {
                missionCompleteShown = true;
                floatingTextMgr.spawn((float)player->getX(), (float)(player->getY() - 30),
                                      "MISSION COMPLETE!", 255, 255, 50);
            }
        }

        if (missionCompleteShown) {
            defeatReturnTimer++;
            if (defeatReturnTimer >= 120 && defeatReturnTimer < 200) {
                double t = (defeatReturnTimer - 120) / 80.0;
                double eased = t * t * (3.0 - 2.0 * t);
                int px = player->getX() + (int)((CENTER_X - player->getX()) * eased * 0.25);
                player->setX(px);
                if (std::abs(player->getX() - CENTER_X) < 2) player->setX(CENTER_X);
            }
            if (defeatReturnTimer == 200) {
                player->setRollTarget(0); player->setRollAngle(0);
            }
            if (defeatReturnTimer >= 200) {
                player->setY(player->getY() - 7);
            }
            if (player->getY() < -50) {
                defeatFWTimer++;
                if (defeatFWTimer % 8 == 0) {
                    particleMgr.spawnFireworks(CENTER_X, WIN_HEIGHT);
                }
                if (defeatFWTimer >= 120) {
                    missionComplete = true;
                    defeatFadeTimer++;
                    if (isNormalPlay && defeatFWTimer == 120) {
                        int cur = chapterMgr.getCurrentIndex();
                        if (cur < 4 && !chapterMgr.isUnlocked(cur + 1))
                            chapterMgr.unlockChapter(cur + 1);
                    }
                }
            }
        }

        if (missionComplete) {
            bool upNow = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
            bool downNow = keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN];
            bool enterNow = keys[SDL_SCANCODE_RETURN];
            static bool upWasM = false, downWasM = false, enterWasM = false;
            if (upNow && !upWasM && mcMenuSelection > 0) mcMenuSelection--;
            if (downNow && !downWasM && mcMenuSelection < 1) mcMenuSelection++;
            if (enterNow && !enterWasM) {
                if (mcMenuSelection == 0) {
                    int cur = chapterMgr.getCurrentIndex();
                    if (cur < 4) {
                        chapterMgr.selectChapter(cur + 1);
                        resetGame();
                        atStartScreen = false;
                        isNormalPlay = true;
                        alienMgr.applyChapterConfig(chapterMgr.getConfig());
                        bulletMgr.updateParams(0);
                        shockwaveMgr.updateParams(0);
                        startChapterNarration();
                    } else {
                        resetGame(); atStartScreen = true;
                    }
                } else {
                    resetGame(); atStartScreen = true;
                }
            }
            upWasM = upNow; downWasM = downNow; enterWasM = enterNow;
        }

        particleMgr.removeInactive();
        shockwaveMgr.removeInactive();
    }

    // ======== DRAW GAMEPLAY ========
    void drawGameplayFrame() {
        bool isSide = chapterMgr.getConfig().isSideScrolling;
        bool useShake = (boss.getShakeTimer() > 0);
        if (useShake) {
            shakeTex = SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888,
                                          SDL_TEXTUREACCESS_TARGET, WIN_WIDTH, WIN_HEIGHT);
            SDL_SetRenderTarget(renderer.get(), shakeTex);
        }
        renderer.setColor(0, 0, 0);
        renderer.clear();

        if (!gameOver) {
            if (isSide && sideBg) {
                sideBg->draw(renderer.get());
            } else if (background) {
                background->drawBackground(renderer.get());
                background->drawBase(renderer.get());
            }

            if (!isSide) {
                for (const auto& sw : shockwaveMgr.all()) if (sw.active) shockwaveMgr.draw(renderer.get());
            }
            particleMgr.draw(renderer.get());
            dialogueSys.draw(renderer.get(), font);
            if (!isSide) {
                alienMgr.draw(renderer.get());
                bulletMgr.draw(renderer.get());
            }
            if (isSide) {
                if (sphereBossActive) sphereBoss.draw(renderer.get());
                bulletMgr.draw(renderer.get());
                player->draw(renderer.get());
                drawWallFlash();
                ch2AlienMgr.drawEnemy(renderer.get());
                ch2AlienMgr.drawBullets(renderer.get());
                dmMgr.drawEnemy(renderer.get());
                dmMgr.drawBullets(renderer.get());
                // Pulse waves on top of enemies
                pulseSystem.draw(renderer.get());
                skillOrb.draw(renderer.get());
                // Ch2 HUD: all aligned to rightEdge = WIN_WIDTH - 10
                HUDBase::drawScore(renderer.get(), font, score, WIN_WIDTH - 10, 10);
                HUDBase::drawHPHearts(renderer.get(), font, ch2PlayerHP, 3, WIN_WIDTH - 10, 28);
                float eFill = pulseSystem.getFill();
                HUDBase::drawEnergyBar(renderer.get(), WIN_WIDTH - 10, 46, 10*14, 6,
                    eFill, pulseSystem.isFull());
                if (aimAssistOn) drawAimAssistSide();
            } else player->draw(renderer.get());

            // Floating texts
            drawFloatingTexts();

            // Aim assist
            if (!isSide && aimAssistOn) drawAimAssist();

            if (!isSide && boss.isActive()) {
                if (phase == PHASE_BOSS_PHASE2) boss.drawCircularShockwave(renderer.get());
                boss.drawCh1HealWaves(renderer.get());
                boss.drawBody(renderer.get());
                boss.drawHPBar(renderer.get(), font);
            }

            if (!isSide) drawScoreHUD();
        } else {
            if (isSide && sideBg) {
                sideBg->draw(renderer.get());
            } else if (background) {
                background->drawBackground(renderer.get());
                background->drawBase(renderer.get());
            }
            particleMgr.draw(renderer.get());
        }

        if (useShake) {
            SDL_SetRenderTarget(renderer.get(), NULL);
            SDL_Rect dst = {boss.getShakeX(), boss.getShakeY(), WIN_WIDTH, WIN_HEIGHT};
            SDL_RenderCopy(renderer.get(), shakeTex, NULL, &dst);
            SDL_DestroyTexture(shakeTex);
        }
    }

    void drawScoreHUD() {
        SDL_Renderer* r = renderer.get();
        HUDBase::drawScore(r, font, score, WIN_WIDTH - 10, 10);
        HUDBase::drawHPHearts(r, font, baseHP, 10, WIN_WIDTH - 10, 28);
        int lev = score / 30;
        float eFill = (lev >= 6) ? 1.0f : (float)(score % 30) / 30.0f;
        HUDBase::drawEnergyBar(r, WIN_WIDTH - 10, 46, 10*14, 6, eFill);
    }

    void drawFloatingTexts() {
        for (const auto& ft : floatingTextMgr.all()) {
            float t = (float)ft.life / ft.totalLife;
            if (t < 0.05f) continue;
            int rv = (int)(ft.r * t), gv = (int)(ft.g * t), bv = (int)(ft.b * t);
            SDL_SetRenderDrawColor(renderer.get(), rv, gv, bv, 255);
            int cx = (int)(ft.x - strlen(ft.text) * 6 * 3 / 2);
            for (const char* p = ft.text; *p; ++p) {
                if (*p != ' ') font.drawChar(renderer.get(), *p, cx, (int)ft.y, 3);
                cx += 6 * 3;
            }
        }
    }


    void drawWallFlash() {
        if (wallFlashTimer <= 0) return;
        SDL_Renderer* r = renderer.get();
        float t = (float)wallFlashTimer / 30.0f;
        int alpha = (int)(255.0f * t);
        int bx = 642, cy = wallContactY;  // wall at nose-tip max reach (px=630 + nose 12)

        // === Barrier core: white-hot line at contact, fading vertically ===
        int coreH = 70;
        for (int dy = -coreH; dy <= coreH; ++dy) {
            int yy = cy + dy;
            if (yy < 0 || yy >= WIN_HEIGHT) continue;
            float dr = (float)std::abs(dy) / coreH;
            int la = (int)(alpha * (1.0f - dr * 0.82f));
            if (la < 8) continue;
            SDL_SetRenderDrawColor(r, 220, 240, 255, (Uint8)la);
            SDL_RenderDrawLine(r, bx, yy, bx, yy);
        }

        // === Inner glow: cyan-blue layers extending right (energy discharge) ===
        for (int g = 1; g <= 5; ++g) {
            int gx = bx + g;  // rightward
            float gt = (float)g / 5.0f;
            int ga = (int)(alpha * (1.0f - gt * 0.65f));
            if (ga < 14) continue;
            int gh = (int)(coreH * (1.0f - gt * 0.45f)) + 25;
            for (int dy = -gh; dy <= gh; ++dy) {
                int yy = cy + dy;
                if (yy < 0 || yy >= WIN_HEIGHT) continue;
                float dr = (float)std::abs(dy) / gh;
                int la = (int)(ga * (1.0f - dr * 0.78f));
                if (la < 6) continue;
                int rr = (int)(60 * (1.0f - gt));
                int gg = (int)(190 - gt * 85);
                int bb = (int)(215 + gt * 40);
                SDL_SetRenderDrawColor(r, (Uint8)rr, (Uint8)gg, (Uint8)bb, (Uint8)la);
                SDL_RenderDrawPoint(r, gx, yy);
            }
        }

        // === Lightning branches: jagged lines discharging rightward ===
        if (t > 0.20f) {
            int nBranches = 5;
            for (int b = 0; b < nBranches; ++b) {
                int seed = b * 41 + wallAnimFrame * 17;
                int lx = bx, ly = cy;
                int segs = 3 + (seed % 3);
                for (int s = 0; s < segs; ++s) {
                    int nx = lx + (5 + (seed + s * 7) % 16);  // rightward
                    int ny = ly + ((seed * 3 + s * 13) % 23 - 11);
                    if (nx > WIN_WIDTH + 30 || ny < 5 || ny >= WIN_HEIGHT - 5) break;
                    int sa = (int)(alpha * (1.0f - (float)s / segs) * 0.75f);
                    if (sa < 12) break;
                    SDL_SetRenderDrawColor(r, 180, 210, 255, (Uint8)sa);
                    SDL_RenderDrawLine(r, lx, ly, nx, ny);
                    SDL_SetRenderDrawColor(r, 255, 255, 240, (Uint8)(sa / 2));
                    SDL_RenderDrawLine(r, lx + 1, ly, nx + 1, ny);
                    lx = nx; ly = ny;
                }
            }
        }

        // === Energy sparks: bright dots discharging rightward ===
        if (t > 0.12f) {
            int nSparks = 22;
            for (int s = 0; s < nSparks; ++s) {
                int seed = s * 59 + wallAnimFrame * 37;
                int sx = bx + (3 + (seed % 32));  // rightward
                int sy = cy + ((seed * 7 + 19) % 130 - 65);
                if (sx > WIN_WIDTH + 20 || sy < 5 || sy >= WIN_HEIGHT - 5) continue;
                int sa = (int)(alpha * (0.55f + 0.45f * ((float)((seed + s * 11) % 100) / 100.0f)));
                if (sa < 22) continue;
                SDL_SetRenderDrawColor(r, 255, 255, 240, (Uint8)sa);
                SDL_RenderDrawPoint(r, sx, sy);
                if ((seed % 5) == 0) {
                    SDL_RenderDrawPoint(r, sx + 1, sy);
                    SDL_RenderDrawPoint(r, sx, sy - 1);
                }
            }
        }
    }

    void drawAimAssistSide() {
        SDL_Renderer* r = renderer.get();
        int px = player->getX(), py = player->getY();
        // Default: 200px ahead along the horizontal shooting ray
        double defaultDist = 200.0;
        double aimX = px + defaultDist, aimY = py;
        double bestT = 999.0;
        double snapX = aimX, snapY = aimY;
        // Ray: horizontal from player (dx=1, dy=0). Check targets along this ray.
        // No range limit (Ch2 bullets deal damage at any range).

        // Check Ch2 regular aliens (vulnerable state)
        for (const auto& ca : ch2AlienMgr.getAliens()) {
            if (ca.active && !ca.entering && ca.invincibleFrames <= 0) {
                double t = ca.x - px;
                if (t > 0) {
                    double lateral = std::fabs(ca.y - py);
                    if (lateral < 18.0 && t < bestT) { bestT = t; snapX = ca.x; snapY = ca.y; }
                }
            }
        }

        // Check Ch2 crystal bullets (from regular aliens)
        auto& cbullets = ch2AlienMgr.getBullets();
        for (const auto& cb : cbullets) {
            if (!cb.active) continue;
            double t = cb.x - px;
            if (t > 0) {
                double lateral = std::fabs(cb.y - py);
                if (lateral < 16.0 && t < bestT) { bestT = t; snapX = cb.x; snapY = cb.y; }
            }
        }

        // Check danmaku enemies (vulnerability window)
        for (const auto& de : dmMgr.getEnemies()) {
            if (de.active && !de.entering && de.invincibleFrames <= 0 && de.vulnTimer > 0) {
                double t = de.x - px;
                if (t > 0) {
                    double lateral = std::fabs(de.y - py);
                    if (lateral < 24.0 && t < bestT) { bestT = t; snapX = de.x; snapY = de.y; }
                }
            }
        }

        // Check danmaku enemy bullets
        auto& dbullets = dmMgr.getBullets();
        for (const auto& db : dbullets) {
            if (!db.active) continue;
            double t = db.x - px;
            if (t > 0) {
                double lateral = std::fabs(db.y - py);
                if (lateral < 16.0 && t < bestT) { bestT = t; snapX = db.x; snapY = db.y; }
            }
        }

        player->aimAssist.update(bestT < 999.0);
        double drawX = (bestT < 999.0) ? snapX : aimX;
        double drawY = (bestT < 999.0) ? snapY : aimY;
        player->aimAssist.draw(r, drawX, drawY, 3, 2);
    }

    void drawAimAssist() {
        SDL_Renderer* r = renderer.get();
        double tPlane = (player->getX() - perspLeft(player->getY())) / perspWidth(player->getY());
        if (tPlane < 0.0) tPlane = 0.0; if (tPlane > 1.0) tPlane = 1.0;
        double range = BulletManager::getBulletRange();
        double ty = player->getY() - range;
        double tx = perspLeft(ty) + tPlane * perspWidth(ty);
        double dx = tx - player->getX(), dy = ty - player->getY();
        double fullDist = std::sqrt(dx*dx + dy*dy);
        if (fullDist < 1.0) return;
        double defaultDist = fullDist * 0.40;

        double ex = player->getX() + dx / fullDist * defaultDist;
        double ey = player->getY() + dy / fullDist * defaultDist;

        // Snap to nearest enemy
        double bestT = 999.0;
        double snapX = ex, snapY = ey;
        if (std::fabs(dy) > 0.001) {
            for (const auto& a : alienMgr.all()) {
                if (!a.active || a.entering || a.invincibleFrames != 0) continue;
                double ax = perspLeft(a.y) + a.t * perspWidth(a.y);
                double t = (a.y - player->getY()) / dy;
                if (t < 0.0 || t > 0.80) continue;
                double hitX = player->getX() + dx * t;
                double dist = std::fabs(hitX - ax);
                double depthBelow = (a.y - HORIZON_Y) / (WIN_HEIGHT - HORIZON_Y);
                double alienScale = (depthBelow < 0) ? 0.08 : 0.08 + 0.92 * depthBelow;
                if (alienScale < 0.08) alienScale = 0.08;
                double snapR = 28.0 * alienScale + 10.0;  // identical to bullet hitRadius
                if (dist < snapR && t < bestT) { bestT = t; snapX = ax; snapY = a.y; }
            }
            if (boss.isActive() && phase != PHASE_BOSS_INTRO && phase != PHASE_BOSS_PHASE2) {
                double bt = (boss.getY() - player->getY()) / dy;
                if (bt > 0.0 && bt < 0.80) {
                    double hitX = player->getX() + dx * bt;
                    if (std::fabs(hitX - boss.getX()) < 55.0 && bt < bestT) {  // same as bullet hit radius
                        bestT = bt; snapX = boss.getX(); snapY = boss.getY();
                    }
                }
            }
        }

        player->aimAssist.update(bestT < 999.0);
        double drawX = (bestT < 999.0) ? snapX : ex;
        double drawY = (bestT < 999.0) ? snapY : ey;
        player->aimAssist.draw(r, drawX, drawY, 5, 3);
    }

    // ======== PAUSED ========
    void updatePaused(const Uint8* keys, bool& running) {
        if (countdown >= 0) {
            if (countdownFrame == 0) {
                particleMgr.spawnDigitShatter(font, '0' + countdown, 18, CENTER_X, WIN_HEIGHT/2);
            }
            countdownFrame++;
            particleMgr.update();
            particleMgr.removeInactive();
            if (countdownFrame >= 45) {
                countdownFrame = 0;
                countdown--;
                if (countdown <= 0) {
                    paused = false; countdown = -1;
                    if (score >= 30) shockwaveMgr.setPending(true);
                }
            }
        } else {
            static bool pUpWas = false, pDownWas = false, pEnterWas = false, pLeftWas = false, pRightWas = false;
            bool upNow = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
            bool downNow = keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN];
            bool leftNow = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
            bool rightNow = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];
            bool enterNow = keys[SDL_SCANCODE_RETURN];

            // Left/Right: switch focus between menu and history
            if (leftNow && !pLeftWas)  pauseHistoryFocused = false;
            if (rightNow && !pRightWas) pauseHistoryFocused = true;

            if (!pauseHistoryFocused) {
                // Menu focused
                if (upNow && !pUpWas && pauseMenuSelection > 0)     pauseMenuSelection--;
                if (downNow && !pDownWas && pauseMenuSelection < 4) pauseMenuSelection++;
                if (enterNow && !pEnterWas) {
                    if (pauseMenuSelection == 0)      { countdown = 3; countdownFrame = 0; }
                    else if (pauseMenuSelection == 1) { resetGame(); paused = false; }
                    else if (pauseMenuSelection == 2) { paused = false; atOptionScreen = true; optionFromPause = true; }
                    else if (pauseMenuSelection == 3) { resetGame(); atStartScreen = true; paused = false; }
                    else                              running = false;
                }
            } else {
                // History focused: focus moves, then scrolls at boundaries
                if (upNow && !pUpWas) dialogueSys.history.moveUp();
                if (downNow && !pDownWas) dialogueSys.history.moveDown();
            }
            pUpWas = upNow; pDownWas = downNow; pEnterWas = enterNow;
            pLeftWas = leftNow; pRightWas = rightNow;
        }
    }

    void drawPauseMenu() {
        SDL_Renderer* r = renderer.get();
        UIRenderer::drawHalfTransparentOverlay(r, 180);

        // === Left half: menu ===
        bool menuFocus = !pauseHistoryFocused;
        font.drawString(r, "PAUSED", 50, 100, 3);
        const char* items[5] = {"RESUME", "RESTART", "OPTIONS", "BACK TO MAIN MENU", "EXIT"};
        const int MENU_Y0 = 160, GAP = 40;
        for (int i = 0; i < 5; ++i) {
            int itemW = (int)strlen(items[i]) * 6 * 3;
            int itemX = 50;
            int itemY = MENU_Y0 + i * GAP;
            font.drawString(r, items[i], itemX, itemY, 3);
            if (menuFocus && i == pauseMenuSelection) {
                UIRenderer::drawMenuCursor(r, itemX - 20, itemY + 10, 8);
                UIRenderer::drawMenuUnderline(r, itemX, itemY + 24, itemW);
            }
        }
        font.drawString(r, "A/D:switch  W/S:menu  ENTER:confirm", 30, 480, 2);

        // === Right half: dialogue history ===
        const int RX = 420;
        font.drawString(r, "HISTORY DIALOGUE", RX, 100, 3);

        auto& hist = dialogueSys.history;
        int total = (int)hist.size();
        if (total == 0) return;

        const int CH_W = 12, CH_H = 14;
        const int RY_TOP = 160;
        const int LINE_GAP = 14;  // one blank line between blocks
        const int TR = 50, TG = 155, TB = 70;
        const int SR = 180, SG = 200, SB = 160;

        int bottomIdx = total - 1 - dialogueSys.history.scroll;
        if (bottomIdx < 0) { bottomIdx = 0; dialogueSys.history.scroll = total - 1; }
        int maxSlot = (total >= 3) ? 2 : total - 1;
        int fSlot = dialogueSys.history.focusSlot;
        if (fSlot > maxSlot) fSlot = maxSlot;

        // Draw items top to bottom with LINE_GAP between
        const int HIST_WRAP = 30;
        int curY = RY_TOP;
        for (int s = 0; s <= maxSlot; ++s) {
            int idx = bottomIdx - (maxSlot - s);
            if (idx < 0 || idx >= total) continue;
            auto& hl = hist[idx];

            // Opacity based on distance from focus
            int dist = (s > fSlot) ? s - fSlot : fSlot - s;
            int alpha = 100;
            if (dist == 1) alpha = 50;
            else if (dist >= 2) alpha = 30;
            if (!pauseHistoryFocused) alpha = 30;

            int rr = (int)(TR * alpha / 100.0);
            int gg = (int)(TG * alpha / 100.0);
            int bb = (int)(TB * alpha / 100.0);
            int sr2 = (int)(SR * alpha / 100.0);
            int sg2 = (int)(SG * alpha / 100.0);
            int sb2 = (int)(SB * alpha / 100.0);

            // Re-wrap lines for narrow history panel
            std::vector<std::string> wlines;
            for (auto& line : hl.lines) {
                std::string s = line;
                while ((int)s.length() > HIST_WRAP) {
                    int brk = HIST_WRAP;
                    while (brk > 0 && s[brk] != ' ') brk--;
                    if (brk == 0) brk = HIST_WRAP;
                    wlines.push_back(s.substr(0, brk));
                    s = s.substr(brk + 1);
                }
                if (!s.empty()) wlines.push_back(s);
            }
            int wNum = (int)wlines.size();
            int itemH = wNum * CH_H + (wNum - 1) * 4;
            int speakerOff = 0;
            if (!hl.speaker.empty()) { speakerOff = CH_H + 2; itemH += speakerOff; }

            int itemY = curY;

            // Speaker
            if (!hl.speaker.empty()) {
                SDL_SetRenderDrawColor(r, (Uint8)sr2, (Uint8)sg2, (Uint8)sb2, 255);
                int tx = RX, ty = itemY;
                for (char c : hl.speaker) {
                    if (c == ' ') { tx += CH_W; continue; }
                    font.drawChar(r, c, tx, ty, 1, 2);
                    tx += CH_W;
                }
                itemY += CH_H + 2;
            }
            // Content
            SDL_SetRenderDrawColor(r, (Uint8)rr, (Uint8)gg, (Uint8)bb, 255);
            for (int li = 0; li < wNum; ++li) {
                auto& line = wlines[li];
                int tx = RX;
                int ty = itemY + li * (CH_H + 4);
                for (char c : line) {
                    if (c == ' ') { tx += CH_W; continue; }
                    font.drawChar(r, c, tx, ty, 1, 2);
                    tx += CH_W;
                }
            }
            curY += itemH + LINE_GAP;
        }
    }

    void drawCountdown() {
        SDL_Renderer* r = renderer.get();
        UIRenderer::drawHalfTransparentOverlay(r, 160);
        double progress = countdownFrame / 45.0;
        if (progress > 1.0) progress = 1.0;
        double eased = 1.0 - std::pow(1.0 - progress, 2.5);
        char digit = '0' + countdown;
        int mainScale = 3 + (int)(eased * 15);
        for (int i = 2; i >= 0; --i) {
            double t = (double)(i + 1) / 3.0;
            int gs = mainScale + (int)(t * eased * 28.0);
            int alpha = (int)(180.0 * (1.0 - t) * (1.0 - eased * 0.4));
            if (alpha < 8) continue;
            int gw = 5 * gs, gh = 7 * gs;
            int gx = CENTER_X - gw / 2, gy = WIN_HEIGHT / 2 - gh / 2;
            SDL_SetRenderDrawColor(r, (Uint8)alpha, (Uint8)alpha, (Uint8)alpha, 255);
            font.drawChar(r, digit, gx, gy, gs);
        }
        int dw = 5 * mainScale, dh = 7 * mainScale;
        int dx = CENTER_X - dw / 2, dy = WIN_HEIGHT / 2 - dh / 2;
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        font.drawChar(r, digit, dx, dy, mainScale);
    }

    // ======== GAME OVER ========
    void updateGameOverScreen(const Uint8* keys, bool&) {
        static bool upWas2 = false, downWas2 = false, enterWas2 = false;
        bool upNow2 = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
        bool downNow2 = keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN];
        bool enterNow2 = keys[SDL_SCANCODE_RETURN];
        if (upNow2 && !upWas2 && menuSelection > 0)   menuSelection--;
        if (downNow2 && !downWas2 && menuSelection < 1) menuSelection++;
        if (enterNow2 && !enterWas2) {
            if (menuSelection == 0) {
                // PLAY AGAIN: restart current chapter
                bool wasNormal = isNormalPlay;
                resetGame();
                atStartScreen = false; atChapterSelect = false;
                isNormalPlay = wasNormal;
                alienMgr.applyChapterConfig(chapterMgr.getConfig());
                bulletMgr.updateParams(0);
                shockwaveMgr.updateParams(0);
            } else {
                // BACK TO MAIN MENU
                resetGame(); atStartScreen = true;
            }
        }
        upWas2 = upNow2; downWas2 = downNow2; enterWas2 = enterNow2;
        particleMgr.update();
        particleMgr.removeInactive();
    }

    void drawGameOverScreen() {
        SDL_Renderer* r = renderer.get();
        UIRenderer::drawHalfTransparentOverlay(r, 200);
        font.drawString(r, "GAME OVER", CENTER_X - 118, 140, 4);
        char buf[32];
        snprintf(buf, sizeof(buf), "SCORE:%-4d", score);
        int scoreW = (int)strlen(buf) * 6 * 4;
        font.drawString(r, buf, CENTER_X - scoreW/2, 210, 4);
        const char* items[2] = {"PLAY AGAIN", "BACK TO MAIN MENU"};
        const int MENU_Y0 = 340;
        for (int i = 0; i < 2; ++i) {
            int itemW = (int)strlen(items[i]) * 6 * 3;
            int itemX = CENTER_X - itemW / 2;
            int itemY = MENU_Y0 + i * 60;
            font.drawString(r, items[i], itemX, itemY, 3);
            if (i == menuSelection) {
                UIRenderer::drawMenuCursor(r, itemX - 20, itemY + 10, 8);
                UIRenderer::drawMenuUnderline(r, itemX, itemY + 24, itemW);
            }
        }
        font.drawString(r, "W/S:select  ENTER:confirm", CENTER_X - 150, 490, 2);
    }

    // ======== MISSION COMPLETE ========
    void updateMissionComplete(const Uint8* keys) {
        bool enterNow = keys[SDL_SCANCODE_RETURN];
        static bool entWas = false;
        if (enterNow && !entWas) { resetGame(); atStartScreen = true; }
        entWas = enterNow;
    }

    void drawMissionComplete() {
        SDL_Renderer* r = renderer.get();
        int fadeAlpha = defeatFadeTimer * 3;
        if (fadeAlpha > 220) fadeAlpha = 220;
        SDL_SetRenderDrawColor(r, 0, 0, 0, (Uint8)fadeAlpha);
        SDL_Rect dr = {0, 0, WIN_WIDTH, WIN_HEIGHT};
        SDL_RenderFillRect(r, &dr);
        if (fadeAlpha > 160) {
            const int TR = 50, TG = 155, TB = 70;
            // Narration-style text box
            const char* title = "MISSION COMPLETE";
            int titleLen = (int)strlen(title);
            int titleW = titleLen * 18 + 50;
            int titleH = 21 + 40;
            int boxX = CENTER_X - titleW / 2, boxY = 200;
            // Background
            SDL_SetRenderDrawColor(r, 10, 25, 15, 220);
            SDL_Rect bgRect = {boxX, boxY, titleW, titleH};
            SDL_RenderFillRect(r, &bgRect);
            SDL_SetRenderDrawColor(r, TR, TG, TB, 180);
            SDL_RenderDrawRect(r, &bgRect);
            // Title text
            SDL_SetRenderDrawColor(r, TR, TG, TB, 255);
            int tx = boxX + 25;
            for (const char* p = title; *p; ++p) {
                if (*p != ' ') font.drawChar(r, *p, tx, boxY + 22, 3);
                tx += 18;
            }
            // Menu options
            const int MENU_Y = 310, MENU_GAP = 40;
            const char* items[2] = {"NEXT CHAPTER", "BACK TO MAIN MENU"};
            for (int i = 0; i < 2; ++i) {
                int iy = MENU_Y + i * MENU_GAP;
                SDL_SetRenderDrawColor(r, TR, TG, TB, (i == mcMenuSelection) ? 255 : 140);
                int itemW = (int)strlen(items[i]) * 12; // scale=2, charW=6*2
                int ix = CENTER_X - itemW / 2;
                font.drawString(r, items[i], ix, iy, 2);
                if (i == mcMenuSelection) {
                    SDL_SetRenderDrawColor(r, 255, 255, 0, 200);
                    SDL_RenderDrawLine(r, ix, iy + 20, ix + itemW, iy + 20);
                }
            }
        }
    }

    // ======== NARRATION ========
    void startChapterNarration() {
        narration.reset();
        int ch = chapterMgr.getCurrentIndex();
        switch (ch) {
            case 0:
                narration.queue("Stellar Calendar 24th.\nIn the deep space.\nThe \"Life\" base.");
                narration.queue("Martha aces her final test.\nShe's now a member of \"Huntress\".");
                narration.queue("When she is just off the plane.\nHeading for the rest area.\nAnnouncement sounds.");
                narration.queue("Tower: \"Hostile objects approaching!\nAll fighters scramble!\"");
                narration.queue("\"Repeat: Scramble! Scramble!\"");
                narration.queue("At the same moment,\na series of massive explosions behind her.\nShe turns --");
                narration.queue("All fighters become wreckage...");
                narration.queue("Martha runs back to the hangar.\nOnly one old trainer left.");
                narration.queue("Into the cockpit. Engines up.\nShe reports:\n\"I'm taking off. Flight code:21395\"");
                narration.queue("No response from the tower.\nOnly a sharp, harsh noise.");
                narration.queue("Martha heads into space.\nAgain and alone.");
                break;
            case 1:
                narration.queue("A hard-won victory. But the cost is plain.\nThe \"Life\" base lies scarred and broken.\nRepairs will take weeks.");
                narration.queue("The shockwave defense system,\npushed far beyond its limits,\nneeds a full overhaul\nbefore it can fire again.");
                narration.queue("The base's long-range comms array\nis destroyed.\nMartha must fly to the\n\"Moonwell\" R&D center,\nwith Bryssa in her backseat,\nto bring home new equipment.");
                narration.queue("Ally enters its standby mode.\nBryssa, exhausted, drifts off.\nThe old trainer's engine hums\na tired, weary vibration.");
                narration.queue("Space is terribly quiet...");
                narration.queue("......");
                narration.queue("............");
                narration.queue("After an eternity in the void,\nthe R&D center finally\ndrifts into Martha's comms range.");
                narration.queue("Martha opens the channel.\n\"Moonwell, Moonwell, this is Martha\nfrom \'Life\' base. Flight code 21395.\"");
                narration.queue("\"......\"");
                narration.queue("Silence.");
                narration.queue("\"Moonwell, Moonwell, 21395.\nRequesting landing clearance.\"");
                narration.queue("\"......\"");
                narration.queue("Nothing but still silence.");
                narration.queue("Martha has already flown to the center's gate.\nA massive vacuum door blocks her way.");
                break;
            case 2:
                narration.queue("CH.3  ENEMY FORTRESS");
                narration.queue("We have located the enemy stronghold.");
                narration.queue("Break through their defense line!");
                break;
            case 3:
                narration.queue("CH.4  ASTEROID BELT");
                narration.queue("Danger lurks among the asteroids.");
                narration.queue("Fly carefully and stay alert.");
                break;
            case 4:
                narration.queue("CH.5  FINAL ASSAULT");
                narration.queue("This is the final battle.");
                narration.queue("Give it everything you've got!");
                break;
        }
        narration.start();
        inNarration = true;
    }

    void updateNarration(const Uint8* keys) {
        bool enterNow = keys[SDL_SCANCODE_RETURN];
        narration.update(enterNow);
        int cticks = narration.popTicks();
        while (cticks-- > 0) audio.sndTeletype();
        if (!narration.isActive()) inNarration = false;
        if (background) background->update();
        if (sideBg) sideBg->update();
    }

    void drawNarrationFrame() {
        SDL_Renderer* r = renderer.get();
        renderer.setColor(0, 0, 0);
        renderer.clear();
        if (background) background->drawStarsFullscreen(r);
        narration.draw(r, font);
        SDL_SetRenderDrawColor(r, 130, 130, 130, 255);
        font.drawString(r, "ENTER:continue", CENTER_X - 78, WIN_HEIGHT - 40, 2);
    }

};
