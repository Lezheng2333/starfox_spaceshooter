#pragma once

// ============== SaveMenu — 读档/存档/文件浏览器界面 ==============
// 纯 UI + 输入层：不含任何游戏状态读写。Game 负责拿到 Action 后执行真正的存/读档。
//   SCR_LOAD    读档槽位列表（AUTO + SLOT 1..6 + BROWSE FILE）
//   SCR_SAVE    存档槽位列表（SLOT 1..6 + SAVE AS）
//   SCR_BROWSER 游戏内迷你文件浏览器（自由读档 / 另存为任意路径）
// 确认框与状态提示（GAME SAVED / 读取失败原因）也在这里绘制。

#include <string>
#include <vector>

#include "constants.h"
#include "font.h"
#include "renderer.h"
#include "save_system.h"
#include "ui.h"

class SaveMenu {
public:
    enum Screen { SCR_LOAD, SCR_SAVE, SCR_BROWSER };
    enum Action {
        ACT_NONE,
        ACT_PICK_LOAD,        // 选了槽位要读档（pickedSlot）
        ACT_PICK_SAVE,        // 选了槽位要存档（pickedSlot）
        ACT_BROWSE_LOAD,      // 打开文件浏览器（读档）
        ACT_BROWSE_SAVE,      // 打开文件浏览器（另存为）
        ACT_LOAD_FILE,        // 浏览器里选中文件 → 读档（pickedPath）
        ACT_SAVE_FILE,        // 浏览器里选中/新建文件 → 存档（pickedPath）
        ACT_CONFIRM,          // 确认框按了 ENTER
        ACT_CANCEL,           // 确认框按了 ESC
        ACT_BACK              // 返回上一层界面
    };

    Screen screen;
    int cursor;
    int scroll;
    bool confirmActive;
    std::string confirmText;
    std::string statusText;
    int statusTimer;
    int pickedSlot;
    std::string pickedPath;
    FileBrowser browser;

    SaveMenu() : screen(SCR_LOAD), cursor(0), scroll(0), confirmActive(false),
                 statusTimer(0), pickedSlot(-1) {}

    // ---- 打开界面 ----
    void openLoad() {
        screen = SCR_LOAD;
        cursor = 0; scroll = 0;
        confirmActive = false;
    }
    void openSave() {
        screen = SCR_SAVE;
        cursor = 0; scroll = 0;
        confirmActive = false;
    }
    void openBrowser(bool saveAs) {
        screen = SCR_BROWSER;
        browser.newName = SaveSystem::autoFileName();
        browser.open(SaveSystem::baseDir(), saveAs);
    }

    void setStatus(const std::string& msg) { statusText = msg; statusTimer = 150; }
    void tickStatus() { if (statusTimer > 0) statusTimer--; }

    void askConfirm(const std::string& text) { confirmActive = true; confirmText = text; }

    // ---- 槽位列表行数（由界面决定，绘制与输入共用） ----
    int loadRowCount() const { return SaveSystem::SLOT_COUNT + 2; }   // AUTO+6 槽 + BROWSE + BACK
    int saveRowCount() const { return 6 + 2; }                        // 6 槽 + SAVE AS + BACK

    static std::string fit(const std::string& s, int maxChars) {
        if ((int)s.size() <= maxChars) return s;
        if (maxChars <= 3) return s.substr(0, maxChars);
        return s.substr(0, maxChars - 2) + "..";
    }

    // ---- 每帧输入处理 ----
    // keys: SDL 键盘状态；edge 结构由 Game 维护（避免连按）
    Action update(bool upPressed, bool downPressed, bool enterPressed, bool escPressed, bool bkspPressed) {
        if (confirmActive) {
            if (enterPressed) { confirmActive = false; return ACT_CONFIRM; }
            if (escPressed)   { confirmActive = false; return ACT_CANCEL; }
            return ACT_NONE;
        }
        if (screen == SCR_BROWSER) return updateBrowser(upPressed, downPressed, enterPressed, escPressed, bkspPressed);

        int rows = (screen == SCR_LOAD) ? loadRowCount() : saveRowCount();
        if (downPressed) { cursor++; if (cursor >= rows) cursor = rows - 1; }
        if (upPressed)   { cursor--; if (cursor < 0) cursor = 0; }
        if (escPressed)  return ACT_BACK;
        if (!enterPressed) return ACT_NONE;

        if (screen == SCR_LOAD) {
            if (cursor < SaveSystem::SLOT_COUNT) { pickedSlot = cursor; return ACT_PICK_LOAD; }
            if (cursor == SaveSystem::SLOT_COUNT) return ACT_BROWSE_LOAD;
            return ACT_BACK;
        }
        // SCR_SAVE
        if (cursor < 6) { pickedSlot = cursor + 1; return ACT_PICK_SAVE; }
        if (cursor == 6) return ACT_BROWSE_SAVE;
        return ACT_BACK;
    }

    Action updateBrowser(bool upPressed, bool downPressed, bool enterPressed, bool escPressed, bool bkspPressed) {
        if (downPressed) browser.move(1);
        if (upPressed)   browser.move(-1);
        if (bkspPressed) { browser.goUp(); return ACT_NONE; }
        if (escPressed)  { return ACT_BACK; }
        if (!enterPressed) return ACT_NONE;
        const FileBrowser::Row* row = browser.current();
        if (!row) return ACT_NONE;
        if (row->isNew) { pickedPath = row->path; return ACT_SAVE_FILE; }
        if (row->isDir) { browser.enterDir(row->path); return ACT_NONE; }
        pickedPath = row->path;
        return browser.saveMode ? ACT_SAVE_FILE : ACT_LOAD_FILE;
    }

    // ---- 绘制 ----
    void drawSlots(SDL_Renderer* r, const Font& font, const SaveMeta* metas, bool fromPause) {
        if (fromPause) UIRenderer::drawHalfTransparentOverlay(r, 210);   // 暂停画面作为背景
        else rendererBg(r);
        bool load = (screen == SCR_LOAD);
        drawCentered(font, r, load ? "LOAD GAME" : "SAVE GAME", 48, 4);
        SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
        SDL_RenderDrawLine(r, CENTER_X - 200, 92, CENTER_X + 200, 92);

        const int Y0 = 122, GAP = 42;
        int rows = load ? loadRowCount() : saveRowCount();
        for (int i = 0; i < rows; ++i) {
            int y = Y0 + i * GAP;
            std::string label;
            std::string info;
            bool dim = false;

            if (load && i < SaveSystem::SLOT_COUNT) {
                label = SaveSystem::slotLabel(i);
                fillSlotInfo(metas[i], info);
                dim = !metas[i].valid;
            } else if (!load && i < 6) {
                label = SaveSystem::slotLabel(i + 1);
                fillSlotInfo(metas[i + 1], info);
                dim = !metas[i + 1].valid;
            } else if (load && i == SaveSystem::SLOT_COUNT) {
                label = "BROWSE FILE...";
                info = "load any .sav on this computer";
            } else if (!load && i == 6) {
                label = "SAVE AS...";
                info = "export to any folder";
            } else {
                label = "BACK";
            }

            SDL_SetRenderDrawColor(r, dim ? 120 : 255, dim ? 120 : 255, dim ? 120 : 255, 255);
            font.drawString(r, label.c_str(), 60, y, 3);
            if (!info.empty()) {
                SDL_SetRenderDrawColor(r, 140, 180, 140, 255);
                font.drawString(r, fit(info, 44).c_str(), 250, y + 4, 2);
            }
            if (i == cursor) {
                int w = (int)label.size() * 6 * 3;
                UIRenderer::drawMenuCursor(r, 40, y + 10, 10);
                UIRenderer::drawMenuUnderline(r, 60, y + 26, w);
            }
        }
        const char* hint = fromPause ? "W/S:select  ENTER:confirm  ESC:back to pause"
                                     : "W/S:select  ENTER:confirm  ESC:back";
        font.drawString(r, hint, CENTER_X - 216, 545, 2);
        drawStatus(r, font);
        drawConfirm(r, font);
    }

    void fillSlotInfo(const SaveMeta& m, std::string& info) {
        if (!m.valid) { info = m.broken ? "UNREADABLE / WRONG VERSION" : emptyText(); return; }
        std::string chap = m.chapterTitle[0] ? std::string(m.chapterTitle) : std::string("?");
        std::string seg = segmentName(m.flow);
        info = chap;
        if (!seg.empty()) info += " / " + seg;
        info += "  SCORE " + SaveSystem::toStr(m.score);
        if (m.playerHP > 0) info += "  HP " + SaveSystem::toStr(m.playerHP);
        info += "  v" + m.versionText();
        info += "  " + m.timeText();
    }

    static std::string segmentName(int flow) {
        switch (flow) {
            case 0: return "GATE";
            case 1: return "CORRIDOR";
            case 2: return "SPHERE";
            case 3: return "CHASE";
            case 4: return "LAB";
            case 5: return "BOSS";
            case 6: return "ENDED";
            default: return "";
        }
    }
    static const char* emptyText() { return "- EMPTY -"; }

    void drawBrowser(SDL_Renderer* r, const Font& font) {
        rendererBg(r);
        drawCentered(font, r, browser.saveMode ? "SAVE AS - CHOOSE FILE" : "LOAD FROM FILE", 40, 3);
        SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
        SDL_RenderDrawLine(r, 30, 76, WIN_WIDTH - 30, 76);
        font.drawString(r, browser.displayDir().c_str(), 34, 86, 2);

        const int Y0 = 116, GAP = 40, VISIBLE = 9;
        for (int s = 0; s < VISIBLE; ++s) {
            int idx = browser.scroll + s;
            if (idx >= (int)browser.rows.size()) break;
            const FileBrowser::Row& row = browser.rows[idx];
            int y = Y0 + s * GAP;
            if (row.isNew)            SDL_SetRenderDrawColor(r, 120, 255, 140, 255);
            else if (row.isDir)       SDL_SetRenderDrawColor(r, 120, 200, 255, 255);
            else if (row.isSav)       SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
            else                      SDL_SetRenderDrawColor(r, 150, 150, 150, 255);
            font.drawString(r, fit(row.label, 46).c_str(), 60, y, 2);
            if (idx == browser.cursor)
                UIRenderer::drawMenuCursor(r, 42, y + 7, 10);
        }
        if (browser.rows.empty()) {
            SDL_SetRenderDrawColor(r, 200, 120, 120, 255);
            font.drawString(r, "(EMPTY FOLDER)", 60, Y0, 2);
        }
        font.drawString(r, "W/S:move  ENTER:open/select  BKSP:up dir  ESC:cancel",
                        CENTER_X - 288, 545, 2);
        drawStatus(r, font);
    }

    void rendererBg(SDL_Renderer* r) {
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderClear(r);
    }

    void drawCentered(const Font& font, SDL_Renderer* r, const std::string& text, int y, int scale) {
        int w = (int)text.size() * 6 * scale;
        font.drawString(r, text.c_str(), CENTER_X - w / 2, y, scale);
    }

    void drawStatus(SDL_Renderer* r, const Font& font) {
        if (statusTimer <= 0 || statusText.empty()) return;
        int w = (int)statusText.size() * 6 * 2;
        SDL_SetRenderDrawColor(r, 20, 40, 20, 220);
        SDL_Rect box = {CENTER_X - w / 2 - 12, WIN_HEIGHT - 62, w + 24, 26};
        SDL_RenderFillRect(r, &box);
        SDL_SetRenderDrawColor(r, 120, 255, 140, 255);
        SDL_RenderDrawRect(r, &box);
        font.drawString(r, statusText.c_str(), CENTER_X - w / 2, WIN_HEIGHT - 56, 2);
    }

    void drawConfirm(SDL_Renderer* r, const Font& font) {
        if (!confirmActive) return;
        SDL_SetRenderDrawColor(r, 0, 0, 0, 200);
        SDL_Rect full = {0, 0, WIN_WIDTH, WIN_HEIGHT};
        SDL_RenderFillRect(r, &full);
        int w = 520, h = 110;
        int x = CENTER_X - w / 2, y = WIN_HEIGHT / 2 - h / 2;
        SDL_SetRenderDrawColor(r, 15, 25, 15, 255);
        SDL_Rect box = {x, y, w, h};
        SDL_RenderFillRect(r, &box);
        SDL_SetRenderDrawColor(r, 120, 255, 140, 255);
        SDL_RenderDrawRect(r, &box);
        int tw = (int)confirmText.size() * 6 * 2;
        font.drawString(r, confirmText.c_str(), CENTER_X - tw / 2, y + 26, 2);
        const char* hint = "ENTER:YES    ESC:NO";
        int hw = (int)strlen(hint) * 6 * 2;
        font.drawString(r, hint, CENTER_X - hw / 2, y + 66, 2);
    }

    // 状态提示在任意界面顶部绘制（LOAD 失败信息等）
    void drawStatusOverlay(SDL_Renderer* r, const Font& font) { drawStatus(r, font); }
};
