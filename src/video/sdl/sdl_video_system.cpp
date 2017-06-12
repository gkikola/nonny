/* Nonny -- Play and create nonogram puzzles.
 * Copyright (C) 2017 Gregory Kikola.
 *
 * This file is part of Nonny.
 *
 * Nonny is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nonny is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nonny.  If not, see <http://www.gnu.org/licenses/>.
 */
/* Written by Gregory Kikola <gkikola@gmail.com>. */

#include "video/sdl/sdl_video_system.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "utility/sdl/sdl_error.hpp"
#include "video/sdl/sdl_font.hpp"
#include "video/sdl/sdl_renderer.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/sdl/sdl_window.hpp"

SDLVideoSystem::SDLVideoSystem()
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    throw SDLError("SDL_Init");

  int img_flags = IMG_INIT_PNG | IMG_INIT_TIF;
  if ((IMG_Init(img_flags) & img_flags) != img_flags) {
    SDL_Quit();
    throw IMGError("IMG_Init");
  }
  
  if (TTF_Init() == -1) {
    IMG_Quit();
    SDL_Quit();
    throw TTFError("TTF_Init");
  }
}

SDLVideoSystem::~SDLVideoSystem()
{
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

std::unique_ptr<Window>
SDLVideoSystem::new_window(const WindowSettings& ws) const
{
  return std::make_unique<SDLWindow>(ws);
}

std::unique_ptr<Renderer>
SDLVideoSystem::new_renderer(Window& window) const
{
  return std::make_unique<SDLRenderer>(window);
}

std::unique_ptr<Font>
SDLVideoSystem::new_font(const std::string& filename, int pt_size) const
{
  return std::make_unique<SDLFont>(filename, pt_size);
}

std::unique_ptr<Texture>
SDLVideoSystem::load_image(Renderer& renderer,
                           const std::string& filename) const
{
  SDLRenderer* sdl_renderer = dynamic_cast<SDLRenderer*>(&renderer);
  if (!sdl_renderer)
    throw std::runtime_error("SDLVideoSystem::load_image: "
                             "given Renderer is not an SDLRenderer");

  SDL_Surface* surface = IMG_Load(filename.c_str());
  std::unique_ptr<Texture> result
    = std::make_unique<SDLTexture>(sdl_renderer->get_sdl_handle(),
                                   surface);
  SDL_FreeSurface(surface);

  return result;
}
