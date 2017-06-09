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

#include "ui/tooltip.hpp"

#include "color/color.hpp"
#include "video/font.hpp"
#include "video/rect.hpp"
#include "video/renderer.hpp"

const Color foreground_color(255, 255, 255);
const Color background_color(64, 64, 64);

const int x_offset = 0;
const int y_offset = 24;
const int padding = 2;

void draw_tooltip(Renderer& renderer,
                  Point position,
                  Font& font,
                  const std::string& text)
{
  int text_wd, text_ht;
  font.text_size(text, &text_wd, &text_ht);

  Rect bound(position.x(), position.y(),
             text_wd + 2 * padding, text_ht + 2 * padding);
  renderer.set_draw_color(background_color);
  renderer.fill_rect(bound);
  renderer.set_draw_color(foreground_color);
  renderer.draw_text(Point(bound.x() + padding, bound.y() + padding),
                     font, text);
}
