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

#include "video/renderer.hpp"

#include <algorithm>
#include <cstddef>
#include "video/font.hpp"
#include "video/rect.hpp"

void Renderer::draw_thick_line(const Point& start,
                                 int length, int thickness, bool vertical)
{
  Rect region;
  if (vertical)
    region = Rect(start.x() - thickness / 2, start.y(), thickness, length + 1);
  else
    region = Rect(start.x(), start.y() - thickness / 2, length + 1, thickness);

  fill_rect(region);
}

void Renderer::draw_thick_rect(const Rect& rect, int thickness)
{
  draw_thick_line(Point(rect.x(), rect.y()),
                  rect.width(), thickness,  false);
  draw_thick_line(Point(rect.x(), rect.y()),
                  rect.height(), thickness, true);
  draw_thick_line(Point(rect.x(), rect.y() + rect.height()),
                  rect.width(), thickness, false);
  draw_thick_line(Point(rect.x() + rect.width(), rect.y()),
                  rect.height(), thickness, true);
}

void Renderer::draw_dotted_rect(const Rect& rect)
{
  draw_dotted_line(Point(rect.x(), rect.y()), rect.width(), false);
  draw_dotted_line(Point(rect.x(), rect.y()), rect.height(), true);
  draw_dotted_line(Point(rect.x(), rect.y() + rect.height()),
                   rect.width(), false);
  draw_dotted_line(Point(rect.x() + rect.width(), rect.y()),
                   rect.height(), true);
}

Rect Renderer::draw_text_wrapped(const Point& point, const Font& font,
                                 std::string text, unsigned width,
                                 bool center)
{
  Rect bounds(point.x(), point.y(), 0, 0);
  text += "\n";
  std::size_t pos = 0;
  std::string line, word;
  Point pt = point;
  while (pos != text.size()) {
    if (text[pos] != '\n' && text[pos] != ' ') {
      word += text[pos];
    } else {
      unsigned txt_wd, txt_ht;
      if (line.empty())
        font.text_size(word, &txt_wd, &txt_ht);
      else
        font.text_size(line + " " + word, &txt_wd, &txt_ht);

      if (txt_wd > width) {
        font.text_size(line, &txt_wd, &txt_ht);
        int x = pt.x();
        if (center)
          x += width / 2 - txt_wd / 2;
        Rect r = draw_text(Point(x, pt.y()), font, line);
        bounds.width() = std::max(bounds.width(), r.width());
        bounds.height() += r.height();
        pt.y() += r.height();
        line = word;
        word = "";
      } else {
        if (!line.empty() && !word.empty())
          line += " ";
        line += word;
        word = "";
      }

      if (text[pos] == '\n') {
        if (word.empty())
          font.text_size(line, &txt_wd, &txt_ht);
        else if (line.empty())
          font.text_size(word, &txt_wd, &txt_ht);
        else
          font.text_size(line + " " + word, &txt_wd, &txt_ht);
        int x = pt.x();
        if (center)
          x += width / 2 - txt_wd / 2;
        Rect r = draw_text(Point(x, pt.y()), font, line + " " + word);
        bounds.width() = std::max(bounds.width(), r.width());
        bounds.height() += r.height();
        pt.y() += r.height();
        line = "";
        word = "";
      }
    }
    
    ++pos;
  }
  
  return bounds;
}
