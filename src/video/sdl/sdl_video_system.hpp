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

#ifndef NONNY_SDL_VIDEO_SYSTEM_HPP
#define NONNY_SDL_VIDEO_SYSTEM_HPP

#include <memory>
#include "video/video_system.hpp"

class Window;
struct WindowSettings;
class Renderer;

class SDLVideoSystem : public VideoSystem {
public:
  SDLVideoSystem();
  ~SDLVideoSystem();
  
  SDLVideoSystem(const SDLVideoSystem&) = delete;
  SDLVideoSystem& operator=(const SDLVideoSystem&) = delete;

  std::unique_ptr<Window> new_window(const WindowSettings& ws) const;
  std::unique_ptr<Renderer> new_renderer() const;
private:
};

#endif
