#pragma once

#include <SDL.h>
#include "types.h"
// ============== UIRenderer ==============
class UIRenderer {
public:
    static void drawMenuCursor(SDL_Renderer* r, int x, int y, int size = 0) {
        int s = size > 0 ? size : 10;
        SDL_SetRenderDrawColor(r, 255, 255, 100, 255);
        SDL_RenderDrawLine(r, x, y, x + s, y + s/2);
        SDL_RenderDrawLine(r, x, y, x + s, y - s/2);
        SDL_RenderDrawLine(r, x + s, y - s/2, x + s, y + s/2);
    }

    static void drawMenuUnderline(SDL_Renderer* r, int x, int y, int w) {
        SDL_SetRenderDrawColor(r, 255, 255, 100, 255);
        SDL_RenderDrawLine(r, x, y, x + w, y);
    }

    static void drawSlider(SDL_Renderer* r, int x, int y, int w, int val, int lo, int hi, bool sym) {
        int range = hi - lo;
        int fillW = (val - lo) * w / range;
        SDL_SetRenderDrawColor(r, 50, 50, 50, 255);
        SDL_Rect bg = {x, y, w, 12};
        SDL_RenderFillRect(r, &bg);
        if (sym) {
            int zeroX = x + w / 2;
            if (fillW >= w / 2) {
                SDL_SetRenderDrawColor(r, 100, 220, 100, 255);
                SDL_Rect fg = {zeroX, y, fillW - w/2, 12};
                SDL_RenderFillRect(r, &fg);
            } else {
                SDL_SetRenderDrawColor(r, 220, 100, 100, 255);
                SDL_Rect fg = {zeroX + fillW - w/2, y, w/2 - fillW, 12};
                SDL_RenderFillRect(r, &fg);
            }
            SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
            SDL_RenderDrawLine(r, zeroX, y, zeroX, y + 12);
        } else {
            SDL_SetRenderDrawColor(r, 100, 200, 100, 255);
            SDL_Rect fg = {x, y, fillW, 12};
            SDL_RenderFillRect(r, &fg);
        }
        SDL_SetRenderDrawColor(r, 150, 150, 150, 255);
        SDL_RenderDrawRect(r, &bg);
    }

    static void drawHalfTransparentOverlay(SDL_Renderer* r, int alpha) {
        SDL_SetRenderDrawColor(r, 0, 0, 0, (Uint8)alpha);
        for (int yy = 0; yy < WIN_HEIGHT; yy += 3) {
            SDL_Rect rect = {0, yy, WIN_WIDTH, 2};
            SDL_RenderFillRect(r, &rect);
        }
    }
};



// ============== MenuStateMachine ==============
class MenuStateMachine {
public:
    enum ScreenType { NONE, START, CHAPTER, TEST, OPTIONS, SOUND, PAUSE, GAMEOVER, MISSION_COMPLETE };
    enum Action { ACT_NONE, ACT_START_GAME, ACT_SELECT_CHAPTER, ACT_BACK_TO_MENU, ACT_QUIT,
                  ACT_RESUME, ACT_RESTART, ACT_TOGGLE_OPTION, ACT_OPTIONS, ACT_SOUND,
                  ACT_BACK_TO_MAIN, ACT_TEST_START, ACT_CHAPTER_START, ACT_EXIT_PAUSE_OPTIONS };

    struct StateSnapshot {
        ScreenType screen;
        int cursor;
        int extraData;
    };

private:
    ScreenType currentScreen;
    int cursor;
    bool justEntered;
    // Edge detection
    bool upWas, downWas, enterWas, escWas, leftWas, rightWas;
    // Extra
    int soundCursor;
    bool optionFromPause;

public:
    MenuStateMachine() : currentScreen(START), cursor(0), justEntered(true),
        upWas(false), downWas(false), enterWas(false), escWas(false),
        leftWas(false), rightWas(false), soundCursor(0), optionFromPause(false) {}

    void enter(ScreenType s) {
        currentScreen = s;
        cursor = 0;
        justEntered = true;
        upWas = downWas = enterWas = escWas = leftWas = rightWas = false;
    }

    void enterSound() { soundCursor = 0; justEntered = true; }

    ScreenType getScreen() const { return currentScreen; }
    int getCursor() const { return cursor; }
    int getSoundCursor() const { return soundCursor; }

    void update(const Uint8* keys) {
        bool upNow   = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
        bool downNow = keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN];
        bool enterNow = keys[SDL_SCANCODE_RETURN];
        bool escNow   = keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_BACKSPACE];
        bool leftNow  = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
        bool rightNow = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];

        if (justEntered) {
            upWas = upNow; downWas = downNow; enterWas = enterNow;
            escWas = escNow; leftWas = leftNow; rightWas = rightNow;
            justEntered = false;
            return;
        }

        int maxItems = 5;
        switch (currentScreen) {
            case START:    maxItems = 5; break;
            case CHAPTER:  maxItems = 5; break;
            case TEST:     maxItems = 9; break;
            case OPTIONS:  maxItems = 2; break;
            case SOUND:    maxItems = 6; break;
            case PAUSE:    maxItems = 5; break;
            case GAMEOVER: maxItems = 2; break;
            default: return;
        }

        if (upNow && !upWas) {
            if (currentScreen == SOUND && soundCursor > 0) soundCursor--;
            else cursor = (cursor - 1 + maxItems) % maxItems;
        }
        if (downNow && !downWas) {
            if (currentScreen == SOUND && soundCursor < maxItems - 1) soundCursor++;
            else cursor = (cursor + 1) % maxItems;
        }
        upWas = upNow; downWas = downNow; enterWas = enterNow;
        escWas = escNow; leftWas = leftNow; rightWas = rightNow;
    }

    bool isEnterPressed() const {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        bool enterNow = keys[SDL_SCANCODE_RETURN];
        return enterNow && !enterWas;
    }

    bool isEscPressed() const {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        bool escNow = keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_BACKSPACE];
        return escNow && !escWas;
    }

    bool isLeftPressed() const {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        bool leftNow = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
        return leftNow && !leftWas;
    }

    bool isRightPressed() const {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        bool rightNow = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];
        return rightNow && !rightWas;
    }

    void setOptionFromPause(bool v) { optionFromPause = v; }
    bool getOptionFromPause() const { return optionFromPause; }
};


// ============== Game 类 ==============

struct MenuKeys {
    bool up, down, left, right, enter, esc;
    bool upWas, downWas, enterWas, escWas;
    MenuKeys(const Uint8* keys) : up(false), down(false), left(false), right(false), enter(false), esc(false),
        upWas(false), downWas(false), enterWas(false), escWas(false) {
        up = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
        down = keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN];
        left = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
        right = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];
        enter = keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_KP_ENTER];
        esc = keys[SDL_SCANCODE_ESCAPE];
    }
};

