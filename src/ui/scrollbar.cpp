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
#include "input/input_handler.hpp"
#include "video/rect.hpp"
#include "video/renderer.hpp"

constexpr unsigned padding = 4;
const Color background(196, 196, 196);
const Color thumb(128, 128, 128);
const Color thumb_hover(88, 88, 88);
const Color thumb_drag(42, 118, 198);

void Scrollbar::update(unsigned ticks, InputHandler& input,
                       const Rect& active_region)
{
}

void Scrollbar::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_draw_color(background);
  renderer.fill_rect(intersection(m_boundary, region));

  Rect thumb_pos = m_boundary;
  if (m_vertical) {
    unsigned target_ht = m_scroll_target->boundary().height();
    thumb_pos.move(m_boundary.x(),
                   m_boundary.y() + m_boundary.height()
                   * m_scroll_pos.y() / target_ht);
    thumb_pos.resize(m_boundary.width(),
                     m_boundary.height() * m_boundary.height() / target_ht);
  } else {
    unsigned target_wd = m_scroll_target->boundary().width();
    thumb_pos.move(m_boundary.x() + m_boundary.width()
                   * m_scroll_pos.y() / target_wd,
                   m_boundary.y());
    thumb_pos.resize(m_boundary.width() * m_boundary.width() / target_wd,
                     m_boundary.height());
  }

  if (thumb_pos.width() > 2 * padding && thumb_pos.height() > 2 * padding) {
    thumb_pos.move(thumb_pos.x() + padding, thumb_pos.y() + padding);
    thumb_pos.resize(thumb_pos.width() - 2 * padding,
                     thumb_pos.height() - 2 * padding);
    renderer.set_draw_color(thumb);
    renderer.fill_rect(intersection(thumb_pos, region));
  }
}
