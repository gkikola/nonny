#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>

#include "renderer.h"

Renderer::Renderer(SDL_Window* window) : m_window{window} {
  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED
                                  | SDL_RENDERER_PRESENTVSYNC);
  if (!m_renderer) SDL_error("SDL_CreateRenderer");
  
  //tell SDL that we want anisotropic filtering if available
  if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best") == SDL_FALSE) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  }
}

Renderer::~Renderer() {
  if (m_renderer) SDL_DestroyRenderer(m_renderer);
}

void Renderer::SDL_error(const std::string& function) {
  std::string err_msg = function;
  err_msg += ": ";
  err_msg += SDL_GetError();

  if (m_renderer) SDL_DestroyRenderer(m_renderer);
  if (m_window) SDL_DestroyWindow(m_window);
  SDL_Quit();
  
  throw std::runtime_error(err_msg);
}

void Renderer::render_game(const Game& game) {
  SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
  SDL_RenderClear(m_renderer);

  //drawing code goes here
  
  SDL_RenderPresent(m_renderer);
}
