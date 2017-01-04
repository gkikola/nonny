#include "render.h"

void render_game(const Game& game, SDL_Window* window, SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  //drawing code goes here
  
  SDL_RenderPresent(renderer);
}
