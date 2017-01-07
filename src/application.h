#ifndef NONNY_APP_H
#define NONNY_APP_H

#include <string>
#include <SDL2/SDL.h>

class Game;
class Renderer;
class InputHandler;

class Application {
 public:
  Application();
  ~Application();

  void run();
 private:
  void cleanup();
  void SDL_error(const std::string& function);

  SDL_Window* m_window;

  Game* m_game;
  Renderer* m_renderer;
  InputHandler* m_input_handler;

  std::string m_data_dir;
  std::string m_save_dir;
};

#endif
