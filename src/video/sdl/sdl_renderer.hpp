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

#include <string>
#include "SDL.h"
#include "video/renderer.hpp"

class Font;
class Point;
class Rect;
class Window;

class SDLRenderer : public Renderer {
public:
  SDLRenderer(Window& window);
  ~SDLRenderer();

  void present() override { SDL_RenderPresent(m_renderer); }
  
  void set_draw_color(const Color& color) override;

  void clear() override { SDL_RenderClear(m_renderer); }
  void draw_point(const Point& point) override;

  void draw_line(const Point& point1, const Point& point2) override;
  void draw_dotted_line(const Point& start,
                        int length, bool vertical = true) override;
  
  void draw_rect(const Rect& rect) override;
  void fill_rect(const Rect& rect) override;

  Rect draw_text(const Point& point, const Font& font,
                 const std::string& text) override;

  void copy_texture(const Texture& src,
                    const Rect& src_rect, const Rect& dest_rect) override;

  void set_clip_rect() override;
  void set_clip_rect(const Rect& rect) override;
  void set_viewport() override;
  void set_viewport(const Rect& rect) override;

  SDL_Renderer* get_sdl_handle() { return m_renderer; }

private:
  SDL_Renderer* m_renderer;
  SDL_Color m_draw_color;
};

#endif
