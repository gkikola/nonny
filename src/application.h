#ifndef NONNY_APP_H
#define NONNY_APP_H

#include <string>
#include <SDL2/SDL.h>

#include "game.h"

class Application {
 public:
  Application();
  ~Application();

  void run();
 private:
  void SDL_error(const std::string& function);
  void cleanup();

  SDL_Window* m_window;
  SDL_Renderer* m_renderer;

  Game* m_game;
};

#endif
