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

#include <algorithm>
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
  Point cursor = input.mouse_position();
  m_thumb_pos = thumb_bounds();

  if (m_dragging) {
    do_thumb_drag(input);
    
    if (input.was_mouse_button_released(Mouse::left)
        || input.was_mouse_button_released(Mouse::right)) {
      m_dragging = false;
      input.release_mouse();
    }
  } else {
    //check to see if the user is attempting to drag the scroll thumb
    if (active_region.contains_point(cursor)) {
      
      if (input.was_mouse_button_pressed(Mouse::left)
          || input.was_mouse_button_pressed(Mouse::right)) {
        if (m_thumb_pos.contains_point(cursor)) {
          m_dragging = true;
          m_drag_pos = m_vertical
            ? (cursor.y() - m_thumb_pos.y())
            : (cursor.x() - m_thumb_pos.x());
        } else if (m_boundary.contains_point(cursor)) {
          m_dragging = true;
          m_drag_pos = m_vertical
            ? (m_thumb_pos.y() + m_thumb_pos.height() / 2)
            : (m_thumb_pos.x() + m_thumb_pos.width() / 2);
        }

        if (m_dragging)
          input.capture_mouse();
      }
    }
  }
}

void Scrollbar::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_draw_color(background);
  renderer.fill_rect(intersection(m_boundary, region));

  //add spacing around thumb
  Rect thumb_pos = m_thumb_pos;
  if (thumb_pos.width() > 2 * padding && thumb_pos.height() > 2 * padding) {
    thumb_pos.x() += padding;
    thumb_pos.y() += padding;
    thumb_pos.width() -= 2 * padding;
    thumb_pos.height() -= 2 * padding;
    renderer.set_draw_color(thumb);
    renderer.fill_rect(intersection(thumb_pos, region));
  }
}

int Scrollbar::thumb_position() const
{
  if (m_vertical) {
    unsigned target_ht = m_scroll_target->boundary().height();
    return m_boundary.y() + m_boundary.height()
      * m_scroll_pos.y() / target_ht;
  } else {
    unsigned target_wd = m_scroll_target->boundary().width();
    return m_boundary.x() + m_boundary.width()
      * m_scroll_pos.x() / target_wd;
  }
}

int Scrollbar::thumb_length() const
{
  if (m_vertical) {
    return m_boundary.height() * m_boundary.height()
      / m_scroll_target->boundary().height();
  } else {
    return m_boundary.width() * m_boundary.width()
      / m_scroll_target->boundary().width();
  }
}

Rect Scrollbar::thumb_bounds() const
{
  Rect result = m_boundary;
  if (m_vertical) {
    result.y() = thumb_position();
    result.height() = thumb_length();
  } else {
    result.x() = thumb_position();
    result.width() = thumb_length();
  }
  return result;
}

void Scrollbar::do_thumb_drag(const InputHandler& input)
{
  if (m_vertical) {
    m_thumb_pos.y() = input.mouse_position().y() - m_drag_pos;
    m_thumb_pos.y() = std::max(m_boundary.y(), m_thumb_pos.y());
    m_thumb_pos.y() = std::min(m_boundary.y()
                               + static_cast<int>(m_boundary.height())
                               - static_cast<int>(m_thumb_pos.height()),
                               m_thumb_pos.y());
    int prev_pos = m_scroll_pos.y();
    m_scroll_pos.y() = (m_thumb_pos.y() - m_boundary.y())
      * m_scroll_target->boundary().height() / m_boundary.height();
    m_scroll_target->scroll(0, prev_pos - m_scroll_pos.y());
  } else {
    m_thumb_pos.x() = input.mouse_position().x() - m_drag_pos;
    m_thumb_pos.x() = std::max(m_boundary.x(), m_thumb_pos.x());
    m_thumb_pos.x() = std::min(m_boundary.x()
                               + static_cast<int>(m_boundary.width())
                               - static_cast<int>(m_thumb_pos.width()),
                               m_thumb_pos.x());
    int prev_pos = m_scroll_pos.x();
    m_scroll_pos.x() = (m_thumb_pos.x() - m_boundary.x())
      * m_scroll_target->boundary().width() / m_boundary.width();
    m_scroll_target->scroll(prev_pos - m_scroll_pos.x(), 0);
  }
}
