#ifndef NONNY_RENDER_H
#define NONNY_RENDER_H

#include <SDL2/SDL.h>

#include "game.h"

void render_game(const Game& game, SDL_Window* window, SDL_Renderer* renderer);

#endif
