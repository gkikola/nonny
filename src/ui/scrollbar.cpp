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

#include "ui/scrollbar.hpp"

#include "color/color.hpp"
#include "video/rect.hpp"

void Scrollbar::update(unsigned ticks, InputHandler& input)
{
}

void Scrollbar::draw(Renderer& renderer) const
{
  renderer.set_draw_color(Color(196, 196, 196));

  int x, y;
  unsigned width, height;
  if (vertical) {
    x = m_boundary.x() + m_boundary.width() - scrollbar_width;
    y = m_boundary.y();
    width = scrollbar_width;
    height = m_boundary.height();
    //    if (m_hscroll.visible) area.height -= scrollbar_width;
  } else {
    x = m_boundary.x();
    y = m_boundary.y() + m_boundary.height() - scrollbar_width;
    width = m_boundary.width();
    //    if (m_vscroll.visible) area.width -= scrollbar_width;
    height = scrollbar_width;
  }
  renderer.fill_rect(Rect(x, y, width, height));
}
