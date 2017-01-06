#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "puzzle.h"

#include "renderer.h"

const std::string cell_sheet_filename = "cell.png";
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

  //load sprite sheet
  std::string sprite_path = m_data_dir + cell_sheet_filename;
  m_cell_sheet = IMG_LoadTexture(m_renderer, sprite_path.c_str());
  if (!m_cell_sheet)
    throw std::runtime_error("IMG_LoadTexture: failed to load texture");

  std::string font_path = m_data_dir + font_filename;
  m_menu_font = TTF_OpenFont(font_path.c_str(), 30);
  m_info_font = TTF_OpenFont(font_path.c_str(), 12);
}

Renderer::~Renderer() {
  if (m_menu_font) TTF_CloseFont(m_menu_font);
  if (m_info_font) TTF_CloseFont(m_info_font);
  if (m_rule_font) TTF_CloseFont(m_rule_font);
  if (m_cell_sheet) SDL_DestroyTexture(m_cell_sheet);
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

SDL_Texture* Renderer::rule_entry_to_texture(const RuleEntry& e,
                                             int* w, int* h) {
  std::string str = std::to_string(e.value);
  SDL_Color color = { 0, 0, 0, 255 };
  
  if (e.completed)
    color = { 96, 96, 96, 255 };
  if (e.hint)
    color = { 0, 0, 255, 255 };
  if (e.error)
    color = { 255, 0, 0, 255 };
      
  SDL_Surface* surface = TTF_RenderText_Blended(m_rule_font,
                                                str.c_str(), color);
  SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surface);
  SDL_FreeSurface(surface);

  //get texture size if requested
  if (w && h) {
    Uint32 fmt;
    int access;
    SDL_QueryTexture(tex, &fmt, &access, w, h);
  }

  return tex;
}

void Renderer::render_game(Game& game) {
  SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
  SDL_RenderClear(m_renderer);
  
  if (game.has_size_changed()) {
    reload_font(game.cell_size() * 3 / 5);
  }

  int spacing = game.cell_size() / 3;
  
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
