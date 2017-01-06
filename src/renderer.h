#ifndef NONNY_RENDERER_H
#define NONNY_RENDERER_H

#include <SDL2/SDL.h>

#include "game.h"

class Renderer {
 public:
  explicit Renderer(SDL_Window* window);
  ~Renderer();

  void render_game(const Game& game);
 private:
  void SDL_error(const std::string& function);

  SDL_Window* m_window;
  SDL_Renderer* m_renderer;
};

#endif
