#pragma once

#include <SDL.h>
// ============== Renderer 类 ==============
class Renderer {
    SDL_Renderer* sdlRenderer;
public:
    Renderer(SDL_Window* window) {
        sdlRenderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    }
    ~Renderer() { if (sdlRenderer) SDL_DestroyRenderer(sdlRenderer); }
    SDL_Renderer* get() const { return sdlRenderer; }

    void setColor(int r, int g, int b, int a = 255) {
        SDL_SetRenderDrawColor(sdlRenderer, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
    }
    void clear() { SDL_RenderClear(sdlRenderer); }
    void present() { SDL_RenderPresent(sdlRenderer); }
    void drawPoint(int x, int y) { SDL_RenderDrawPoint(sdlRenderer, x, y); }
    void drawLine(int x1, int y1, int x2, int y2) {
        SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2);
    }
    void drawRect(const SDL_Rect* r) { SDL_RenderFillRect(sdlRenderer, r); }
    void drawRectBorder(const SDL_Rect* r) { SDL_RenderDrawRect(sdlRenderer, r); }
    void setTarget(SDL_Texture* tex) { SDL_SetRenderTarget(sdlRenderer, tex); }
    void resetTarget() { SDL_SetRenderTarget(sdlRenderer, NULL); }
    SDL_Texture* createTexture(Uint32 fmt, int access, int w, int h) {
        return SDL_CreateTexture(sdlRenderer, fmt, access, w, h);
    }
    void copyTexture(SDL_Texture* tex, const SDL_Rect* src, const SDL_Rect* dst) {
        SDL_RenderCopy(sdlRenderer, tex, src, dst);
    }
    void destroyTexture(SDL_Texture* tex) { SDL_DestroyTexture(tex); }
};


