#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>

#include "render.h"

#include "application.h"

const int default_win_width = 800;
const int default_win_height = 600;
const std::string default_win_title = "Nonny";

Application::Application() : m_game{nullptr}, m_window{nullptr},
                             m_renderer{nullptr} {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) SDL_error("SDL_Init");

  m_window = SDL_CreateWindow(default_win_title.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              default_win_width, default_win_height,
                              SDL_WINDOW_RESIZABLE);
  if (!m_window) SDL_error("SDL_CreateWindow");

  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED
                                  | SDL_RENDERER_PRESENTVSYNC);
  if (!m_renderer) SDL_error("SDL_CreateRenderer");
  
  //tell SDL that we want anisotropic filtering if available
  if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best") == SDL_FALSE) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  }

  m_game = new Game();
}

Application::~Application() {  
  cleanup();
}

void Application::SDL_error(const std::string& function) {
  std::string err_msg = function;
  err_msg += ": ";
  err_msg += SDL_GetError();

  cleanup();
  throw std::runtime_error(err_msg);
}

void Application::cleanup() {
  if (m_game) delete m_game;
  if (m_renderer) SDL_DestroyRenderer(m_renderer);
  if (m_window) SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void Application::run() {
  SDL_Event event;
  bool exit = false;

  while (!exit) {
    while (SDL_PollEvent(&event)) {
      //process SDL event
      switch (event.type) {
      case SDL_QUIT:
        exit = true;
        break;
      }
    }

    //handle rendering
    render_game(*m_game, m_window, m_renderer);

    //give control back to the OS
    SDL_Delay(1);
  }
}
