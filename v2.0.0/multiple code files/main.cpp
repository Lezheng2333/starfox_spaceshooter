#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "types.h"
#include "constants.h"
#include "font.h"
#include "renderer.h"
#include "audio.h"
#include "floating_text.h"
#include "dialogue.h"
#include "narration.h"
#include "particles.h"
#include "aim_assist.h"
#include "player.h"
#include "bullets.h"
#include "ch1/ch1_shockwave.h"
#include "ch1/ch1_aliens.h"
#include "ch1/ch1_boss.h"
#include "ch1/ch1_background.h"
#include "ch2/ch2_sphere_boss.h"
#include "ch2/ch2_background.h"
#include "ch2/ch2_shooter_base.h"
#include "ch2/ch2_hud.h"
#include "ch2/ch2_danmaku.h"
#include "ch2/ch2_aliens.h"
#include "ch2/ch2_skill_orb.h"
#include "ch2/ch2_pulse.h"
#include "chapter_manager.h"
#include "ui.h"
#include "game.h"

// ============== MAIN ==============
int main() {
    srand((unsigned)time(nullptr));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("星际火狐 极简版",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIN_WIDTH, WIN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) { SDL_Quit(); return 1; }

    Renderer renderer(window);
    AudioEngine audio;
    Game game(renderer, audio, window);
    game.run();

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
