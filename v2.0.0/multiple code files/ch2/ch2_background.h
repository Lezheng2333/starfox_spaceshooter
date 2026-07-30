#pragma once

#include "../constants.h"
#include "../renderer.h"
#include "../types.h"

// ============== Ch2Background ==============
class Ch2Background {
private:
    struct Star2 { double wx, wy; float phase, twinkleSpeed; bool isCross; };
    struct Pillar { double wx; bool isMajor; };

    std::vector<Star2> stars;
    std::vector<Pillar> pillars;
    double scrollX, scrollSpeed, genNext;
    static constexpr double STAR_PARALLAX = 0.015;
    static constexpr double PILLAR_SPACING = 330.0; // ~1.5s between pillars
    static const int VP_X = CENTER_X;    // pillar/glass wall vanishing point x
    static const int VP_Y = -60;        // pillar/glass wall vanishing point y (above screen)
    static const int WALL_Y = 280;      // floor meets glass wall

    // Longitudinal floor seams (horizontal lines along corridor, at fixed depths)
    // Each track = a fixed y position on the floor, seams scroll left at same speed
    struct SeamTrack {
        double y;           // fixed screen y for this depth track
        double spacing;     // world spacing between seams at this depth (near=wider, far=narrower)
        double genNext;     // next world-x to generate
        std::vector<double> wxs; // world-x positions of seams
    };
    static const int N_SEAM_TRACKS = 5;
    SeamTrack seamTracks[N_SEAM_TRACKS];

    void genStars() {
        for (int i = 0; i < 400; ++i) {
            Star2 s;
            s.wx = rand() % 6000; s.wy = rand() % WIN_HEIGHT;
            s.phase = (rand() % 628) / 100.0f;
            s.twinkleSpeed = 0.008f + (rand() % 50) / 1000.0f;
            s.isCross = (rand() % 100 < 10);
            stars.push_back(s);
        }
    }

    void genAhead() {
        double ahead = scrollX + WIN_WIDTH + 600.0; // margin: generate well off-screen to avoid visible pop-in
        // Pillars
        while (genNext < ahead) {
            pillars.push_back({genNext, (int)(genNext / PILLAR_SPACING) % 5 == 0});
            genNext += PILLAR_SPACING;
        }
        // Floor seams per track
        for (int t = 0; t < N_SEAM_TRACKS; ++t) {
            while (seamTracks[t].genNext < ahead) {
                seamTracks[t].wxs.push_back(seamTracks[t].genNext);
                seamTracks[t].genNext += seamTracks[t].spacing;
            }
        }
    }

    template<typename T>
    void pruneVec(std::vector<T>& v, double cutoff) {
        int w = 0;
        for (int i = 0; i < (int)v.size(); ++i)
            if (v[i].wx >= cutoff) v[w++] = v[i];
        v.resize(w);
    }
    template<typename T>
    void pruneDoubleVec(std::vector<T>& v, double cutoff) {
        int w = 0;
        for (int i = 0; i < (int)v.size(); ++i)
            if (v[i] >= cutoff) v[w++] = v[i];
        v.resize(w);
    }
    void pruneBehind() {
        double cutoff = scrollX - 500.0;
        pruneVec(pillars, cutoff);
        for (int t = 0; t < N_SEAM_TRACKS; ++t)
            pruneDoubleVec(seamTracks[t].wxs, cutoff);
    }

    void pruneStars() {
        double cutoff = scrollX * STAR_PARALLAX - 400.0;
        double wrap = scrollX * STAR_PARALLAX + WIN_WIDTH + 400.0;
        for (auto& s : stars) {
            if (s.wx < cutoff) { s.wx += 6000.0; s.phase = (rand() % 628) / 100.0f; }
            if (s.wx > wrap) { s.wx -= 6000.0; }
        }
    }

public:
    Ch2Background() : scrollX(0), scrollSpeed(3.5), genNext(-200.0) {
        genStars();
        for (int t = 0; t < N_SEAM_TRACKS; ++t) {
            double frac = (double)(t + 1) / (N_SEAM_TRACKS + 1);
            seamTracks[t].y = WALL_Y + (WIN_HEIGHT - WALL_Y) * frac * frac;
            seamTracks[t].spacing = 45.0 + t * 22.0;
            seamTracks[t].genNext = -200.0;
        }
    }
    void reset() {
        scrollX = 0; scrollSpeed = 3.5; genNext = -200.0;
        pillars.clear(); stars.clear(); genStars();
        for (int t = 0; t < N_SEAM_TRACKS; ++t) {
            seamTracks[t].wxs.clear();
            seamTracks[t].genNext = -200.0;
        }
    }
    void setSpeed(double s) { scrollSpeed = s; }
    double getScrollX() const { return scrollX; }

    void update() {
        scrollX += scrollSpeed;
        for (auto& s : stars) {
            s.phase += s.twinkleSpeed;
            if (s.phase > 2.0f * M_PI) s.phase -= 2.0f * M_PI;
        }
        pruneStars();
        genAhead();
        pruneBehind();
    }

    void draw(SDL_Renderer* r) {
        drawSky(r);
        drawFloor(r);
        drawGlassPanels(r);
        drawPillars(r);
    }

private:
    void drawSky(SDL_Renderer* r) {
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_Rect bg = {0, 0, WIN_WIDTH, WIN_HEIGHT};
        SDL_RenderFillRect(r, &bg);
        for (const auto& s : stars) {
            int sx = (int)(s.wx - scrollX * STAR_PARALLAX);
            int sy = (int)s.wy;
            if (sx < -20 || sx > WIN_WIDTH + 20) continue;
            float bright = 0.35f + 0.65f * std::fabs(std::sin(s.phase));
            int b = (int)(bright * 240); if (b < 40) b = 40; if (b > 255) b = 255;
            SDL_SetRenderDrawColor(r, b, b, b, 255);
            SDL_RenderDrawPoint(r, sx, sy);
            if (s.isCross && bright > 0.6f) {
                int cb = (int)(bright * 160); if (cb > 160) cb = 160;
                SDL_SetRenderDrawColor(r, cb, cb, cb, 255);
                int len = (bright > 0.82f) ? 4 : 2;
                SDL_RenderDrawLine(r, sx - len, sy, sx + len, sy);
                SDL_RenderDrawLine(r, sx, sy - len, sx, sy + len);
            }
        }
    }

    // ======== FLOOR ========
    void drawFloor(SDL_Renderer* r) {
        // Floor fill
        SDL_SetRenderDrawColor(r, 22, 24, 30, 255);
        SDL_Rect floorRect = {0, WALL_Y, WIN_WIDTH, WIN_HEIGHT - WALL_Y};
        SDL_RenderFillRect(r, &floorRect);

        // Longitudinal floor seams (horizontal lines along corridor, at fixed depths)
        // Each track has a fixed y, seams scroll left at same speed as pillars
        for (int t = 0; t < N_SEAM_TRACKS; ++t) {
            int y = (int)seamTracks[t].y;
            int alpha = 180 - t * 22; // nearer = brighter
            SDL_SetRenderDrawColor(r, 45, 48, 58, alpha);
            for (double wx : seamTracks[t].wxs) {
                double sx = wx - scrollX;
                if (sx < -40 || sx > WIN_WIDTH + 40) continue;
                // Each seam is a short horizontal dash at its fixed y
                int halfW = 15 + t * 8; // wider near camera
                SDL_RenderDrawLine(r, (int)sx - halfW, y, (int)sx + halfW, y);
            }
        }

        // Horizon junction line (where floor meets glass wall)
        SDL_SetRenderDrawColor(r, 35, 38, 45, 255);
        SDL_RenderDrawLine(r, 0, WALL_Y, WIN_WIDTH, WALL_Y);
        SDL_SetRenderDrawColor(r, 55, 58, 68, 255);
        SDL_RenderDrawLine(r, 0, WALL_Y + 2, WIN_WIDTH, WALL_Y + 2);
    }

    // ======== GLASS PANELS ========
    void pillarScreenX(const Pillar& p, double& botX, double& topX) const {
        botX = p.wx - scrollX;
        topX = VP_X + (botX - VP_X) * 0.85;
    }

    void drawGlassPanels(SDL_Renderer* r) {
        for (size_t i = 0; i + 1 < pillars.size(); ++i) {
            double lbx, ltx, rbx, rtx;
            pillarScreenX(pillars[i], lbx, ltx);
            pillarScreenX(pillars[i+1], rbx, rtx);
            double glassT = 0, glassB = WALL_Y;

            // Cull only if ENTIRE pane is off-screen
            double minX = std::min(std::min(lbx, ltx), std::min(rbx, rtx));
            double maxX = std::max(std::max(lbx, ltx), std::max(rbx, rtx));
            if (maxX < -100 || minX > WIN_WIDTH + 100) continue;

            // Border colour
            SDL_SetRenderDrawColor(r, 170, 200, 230, 95);
            // Left edge (follows pillar angle)
            SDL_RenderDrawLine(r, (int)lbx, (int)glassB, (int)ltx, (int)glassT);
            // Right edge
            SDL_RenderDrawLine(r, (int)rbx, (int)glassB, (int)rtx, (int)glassT);
            // Top edge
            SDL_RenderDrawLine(r, (int)ltx, (int)glassT, (int)rtx, (int)glassT);
            // Bottom edge
            SDL_RenderDrawLine(r, (int)lbx, (int)glassB, (int)rbx, (int)glassB);

            // Internal horizontal seams (2-3 subtle lines, Minecraft pane style)
            SDL_SetRenderDrawColor(r, 165, 195, 225, 55);
            for (int hi = 1; hi <= 3; ++hi) {
                double t = (double)hi / 4.0;
                int ly = (int)(glassT + (glassB - glassT) * t);
                int lx = (int)(ltx + (lbx - ltx) * t);
                int rx = (int)(rtx + (rbx - rtx) * t);
                SDL_RenderDrawLine(r, lx, ly, rx, ly);
            }

            // Internal vertical seams (1-2 subtle lines)
            for (int vi = 1; vi <= 2; ++vi) {
                double t = (double)vi / 3.0;
                int bx = (int)(lbx + (rbx - lbx) * t);
                int tx = (int)(ltx + (rtx - ltx) * t);
                SDL_RenderDrawLine(r, bx, (int)glassB, tx, (int)glassT);
            }

            // Corner highlights (Minecraft signature)
            SDL_SetRenderDrawColor(r, 220, 235, 255, 130);
            // Top-left corner
            SDL_RenderDrawLine(r, (int)ltx, (int)glassT, (int)ltx + 6, (int)glassT);
            SDL_RenderDrawLine(r, (int)ltx, (int)glassT, (int)ltx, (int)glassT + 6);
            // Top-right corner
            SDL_RenderDrawLine(r, (int)rtx, (int)glassT, (int)rtx - 6, (int)glassT);
            SDL_RenderDrawLine(r, (int)rtx, (int)glassT, (int)rtx, (int)glassT + 6);
            // Bottom-left corner
            SDL_RenderDrawLine(r, (int)lbx, (int)glassB, (int)lbx + 6, (int)glassB);
            SDL_RenderDrawLine(r, (int)lbx, (int)glassB, (int)lbx, (int)glassB - 6);
            // Bottom-right corner
            SDL_RenderDrawLine(r, (int)rbx, (int)glassB, (int)rbx - 6, (int)glassB);
            SDL_RenderDrawLine(r, (int)rbx, (int)glassB, (int)rbx, (int)glassB - 6);

            // Center sheen (subtle diagonal glow)
            double cx = (ltx + rtx + lbx + rbx) * 0.25;
            double cy = glassB * 0.35;
            SDL_SetRenderDrawColor(r, 200, 225, 250, 45);
            SDL_RenderDrawLine(r, (int)(cx - 15), (int)(cy - 8), (int)(cx + 10), (int)(cy + 6));
        }
    }

    // ======== PILLARS ========
    void drawPillars(SDL_Renderer* r) {
        for (const auto& p : pillars) {
            double sx = p.wx - scrollX;
            if (sx < -120 || sx > WIN_WIDTH + 120) continue; // full off-screen cull

            double botX = sx;
            double botY = WALL_Y;
            // Top leans slightly toward vanishing point (above screen)
            double topX = VP_X + (botX - VP_X) * 0.85;
            double topY = 0;

            int pw = p.isMajor ? 5 : 3;
            double dx = topX - botX, dy = topY - botY;
            double len = std::sqrt(dx*dx + dy*dy);
            if (len < 1.0) continue;
            double nx = -dy / len, ny = dx / len;

            // Pillar body
            SDL_SetRenderDrawColor(r, 55, 55, 65, 230);
            for (int w = -pw; w <= pw; ++w) {
                int x0 = (int)(botX + nx * w), y0 = (int)(botY + ny * w);
                int x1 = (int)(topX + nx * w), y1 = (int)(topY + ny * w);
                SDL_RenderDrawLine(r, x0, y0, x1, y1);
            }

            // Highlight edges
            SDL_SetRenderDrawColor(r, 80, 90, 120, 180);
            SDL_RenderDrawLine(r, (int)(botX+nx*pw), (int)(botY+ny*pw),
                               (int)(topX+nx*pw), (int)(topY+ny*pw));
            SDL_SetRenderDrawColor(r, 110, 120, 145, 180);
            SDL_RenderDrawLine(r, (int)(botX-nx*pw), (int)(botY-ny*pw),
                               (int)(topX-nx*pw), (int)(topY-ny*pw));

            // Gusset
            int gs = pw + 3;
            SDL_SetRenderDrawColor(r, 65, 65, 75, 180);
            SDL_RenderDrawLine(r, (int)(botX-nx*gs), (int)(botY-ny*gs), (int)botX, (int)botY + 6);
            SDL_RenderDrawLine(r, (int)(botX+nx*gs), (int)(botY+ny*gs), (int)botX, (int)botY + 6);
            SDL_RenderDrawLine(r, (int)(botX-nx*gs), (int)(botY-ny*gs), (int)(botX+nx*gs), (int)(botY+ny*gs));
        }
    }
};
