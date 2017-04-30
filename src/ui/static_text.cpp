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

#include "ui/static_text.hpp"

#include "color/color.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

void StaticText::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_clip_rect(intersection(m_boundary, region));
  
  Point location(m_boundary.x()
                 + m_boundary.width() / 2
                 - m_label_width / 2,
                 m_boundary.y());
  if (m_font) {
    renderer.set_draw_color(default_colors::black);
    renderer.draw_text(location, *m_font, m_label);
  }

  renderer.set_clip_rect();
}

void StaticText::calc_size()
{
  unsigned width = 0, height = 0;
  if (m_font) {
    m_font->text_size(m_label, &width, &height);
    m_label_width = width;
  }

  resize(width, height);
}
