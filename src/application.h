#ifndef NONNY_APP_H
#define NONNY_APP_H

#include <string>
#include <SDL2/SDL.h>

class Application {
public:
  Application();
  ~Application();

  void run();
private:
  void SDL_error(const std::string& function);
  void SDL_cleanup();

  SDL_Window* window;
};

#endif
