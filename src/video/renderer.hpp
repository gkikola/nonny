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

#ifndef NONNY_RENDERER_HPP
#define NONNY_RENDERER_HPP

#include <string>
#include "color/color.hpp"

class Font;
class Point;
class Rect;
class Texture;

/*
 * Responsible for all drawing operations. Provides basic primitives
 * which interface elements can use to draw themselves.
 */
class Renderer {
public:
  Renderer() { }
  virtual ~Renderer() { }

  virtual void present() = 0;

  virtual void clear() = 0;
  virtual void draw_point(const Point& point) = 0;

  virtual void draw_line(const Point& point1, const Point& point2) = 0;
  virtual void draw_thick_line(const Point& start,
                               int length, int thickness,
                               bool vertical = true);
  virtual void draw_dotted_line(const Point& start,
                                int length, bool vertical = true) = 0;

  virtual void draw_rect(const Rect& rect) = 0;
  virtual void draw_thick_rect(const Rect& rect, int thickness);
  virtual void draw_dotted_rect(const Rect& rect);
  virtual void fill_rect(const Rect& rect) = 0;

  virtual Rect draw_text(const Point& point, const Font& font,
                         const std::string& text) = 0;
  virtual Rect draw_text_with_bg(const Point& point, const Font& font,
                                 const std::string& text,
                                 const Color& bg_color) = 0;
  virtual Rect draw_text_wrapped(const Point& point, const Font& font,
                                 std::string text, int width,
                                 bool center = false);

  virtual void copy_texture(const Texture& src,
                            const Rect& src_rect,
                            const Rect& dest_rect) = 0;

  virtual void set_draw_color(const Color& color) = 0;
  virtual void set_clip_rect() = 0;
  virtual void set_clip_rect(const Rect& rect) = 0;
  virtual void set_viewport() = 0;
  virtual void set_viewport(const Rect& rect) = 0;
};

#endif
