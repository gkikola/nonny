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
#include <SDL2/SDL.h>
#include "utility/sdl/sdl_error.hpp"
#include "video/sdl/sdl_renderer.hpp"
#include "video/sdl/sdl_window.hpp"

SDLVideoSystem::SDLVideoSystem()
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    throw SDLError("SDL_Init");
}

SDLVideoSystem::~SDLVideoSystem()
{
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
