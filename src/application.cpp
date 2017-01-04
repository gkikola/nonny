#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>

#include "application.h"

Application::Application() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::string err_msg("SDL_Init: ");
    err_msg += SDL_GetError();
    throw std::runtime_error(err_msg);
  }
}

Application::~Application() {
  SDL_Quit();
}

void Application::run() {
}
