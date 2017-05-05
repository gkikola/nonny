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

#include "ui/text_box.hpp"

#include "color/color.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

constexpr unsigned spacing = 8;
const Color background_color = default_colors::white;
const Color foreground_color = default_colors::black;

void TextBox::update(unsigned ticks, InputHandler& input,
                     const Rect& active_region)
{
}

void TextBox::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_clip_rect(region);

  renderer.set_draw_color(background_color);
  renderer.fill_rect(m_boundary);
  
  renderer.set_clip_rect();
}

void TextBox::calc_size()
{
  unsigned width = 0, height = 0;
  if (m_font)
    m_font->text_size("Sample text", &width, &height);
  width += 2 * spacing;
  height += 2 * spacing;
  resize(width, height);
}
