#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>

#include "renderer.h"

const std::string font_filename = "FreeSans.ttf";

Renderer::Renderer(SDL_Window* window, const std::string& data_dir)
  : m_window{window}, m_data_dir{data_dir} {
  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED
                                  | SDL_RENDERER_PRESENTVSYNC);
  if (!m_renderer) SDL_error("SDL_CreateRenderer");
  
  //tell SDL that we want anisotropic filtering if available
  if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best") == SDL_FALSE) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  }

  std::string font_path = m_data_dir + font_filename;
  m_menu_font = TTF_OpenFont(font_path.c_str(), 16);
  m_info_font = TTF_OpenFont(font_path.c_str(), 12);
}

Renderer::~Renderer() {
  if (m_menu_font) TTF_CloseFont(m_menu_font);
  if (m_info_font) TTF_CloseFont(m_info_font);
  if (m_rule_font) TTF_CloseFont(m_rule_font);
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

void Renderer::render_game(Game& game) {
  SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
  SDL_RenderClear(m_renderer);
  
  if (game.has_size_changed()) {
    reload_font(game.cell_size() * 3 / 5);
  }
  
  //drawing code goes here
  
  SDL_RenderPresent(m_renderer);
}

void Renderer::reload_font(int font_size) {
  if (m_rule_font)
    TTF_CloseFont(m_rule_font);

  std::string font_path = m_data_dir + font_filename;
  m_rule_font = TTF_OpenFont(font_path.c_str(), font_size);

  if (!m_rule_font)
    throw std::runtime_error("TTF_OpenFont: could not open font file");
}
