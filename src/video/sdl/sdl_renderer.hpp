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

#ifndef NONNY_SDL_RENDERER_HPP
#define NONNY_SDL_RENDERER_HPP

#include <SDL2/SDL.h>
#include "video/renderer.hpp"

class Window;

class SDLRenderer : public Renderer {
public:
  SDLRenderer(Window& window);
  ~SDLRenderer();

  void present() { SDL_RenderPresent(m_renderer); }
  
  void set_draw_color(const Color& color);

  void clear() { SDL_RenderClear(m_renderer); }
  void draw_point(const Point& point);
  void draw_line(const Point& point1, const Point& point2);
  void draw_rect(const Rect& rect);
  void draw_dotted_rect(const Rect& rect);
  void fill_rect(const Rect& rect);

  void set_viewport();
  void set_viewport(const Rect& rect);

private:
  SDL_Renderer* m_renderer;
};

#endif
