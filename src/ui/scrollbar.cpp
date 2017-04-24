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
  update_thumb_position();
  m_mouse_hover = false;

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

      if (m_thumb_pos.contains_point(cursor))
        m_mouse_hover = true;
      
      if (input.was_mouse_button_pressed(Mouse::left)
          || input.was_mouse_button_pressed(Mouse::right)) {
        if (m_mouse_hover) {
          m_dragging = true;
          m_drag_pos = m_vertical
            ? (cursor.y() - m_thumb_pos.y())
            : (cursor.x() - m_thumb_pos.x());
        } else if (m_boundary.contains_point(cursor)) {
          m_dragging = true;
          m_drag_pos = m_vertical
            ? (m_thumb_pos.height() / 2)
            : (m_thumb_pos.width() / 2);
          do_thumb_drag(input);
          m_drag_pos = m_vertical
            ? (cursor.y() - m_thumb_pos.y())
            : (cursor.x() - m_thumb_pos.x());
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
    //set thumb color
    if (m_dragging)
      renderer.set_draw_color(thumb_drag);
    else if (m_mouse_hover)
      renderer.set_draw_color(thumb_hover);
    else
      renderer.set_draw_color(thumb);

    thumb_pos.x() += padding;
    thumb_pos.y() += padding;
    thumb_pos.width() -= 2 * padding;
    thumb_pos.height() -= 2 * padding;
    renderer.fill_rect(intersection(thumb_pos, region));
  }
}

void Scrollbar::resize(unsigned width, unsigned height)
{
  UIPanel::resize(width, height);

  if (m_vertical
      && target_height() > m_boundary.height()
      && scroll_position() + m_boundary.height() > target_height())
    m_scroll_target->move(m_scroll_target->boundary().x(),
                          m_boundary.height()
                          - m_scroll_target->boundary().height());
  else if (!m_vertical
           && target_width() > m_boundary.width()
           && scroll_position() + m_boundary.width() > target_width())
    m_scroll_target->move(m_boundary.width()
                          - m_scroll_target->boundary().width(),
                          m_scroll_target->boundary().y());
}

int Scrollbar::scroll_position() const
{
  int result;
  if (m_vertical)
    result = m_boundary.y() - m_scroll_target->boundary().y();
  else
    result = m_boundary.x() - m_scroll_target->boundary().x();

  if (result < 0)
    result = 0;

  return result;
}

void Scrollbar::update_thumb_position()
{
  m_thumb_pos = m_boundary;
  if (m_vertical) {
    m_thumb_pos.y() = m_boundary.y()
      + scroll_position() * m_boundary.height() / target_height();
    m_thumb_pos.height()
      = m_boundary.height() * m_boundary.height() / target_height();
  } else {
    m_thumb_pos.x() = m_boundary.x()
      + scroll_position() * m_boundary.width() / target_width();
    m_thumb_pos.width()
      = m_boundary.width() * m_boundary.width() / target_width();
  }
}

void Scrollbar::do_thumb_drag(InputHandler& input)
{
  if (m_vertical) {
    m_thumb_pos.y() = input.mouse_position().y() - m_drag_pos;
    m_thumb_pos.y() = std::max(m_thumb_pos.y(), m_boundary.y());
    m_thumb_pos.y() = std::min(m_thumb_pos.y(), m_boundary.y()
                               + static_cast<int>(m_boundary.height())
                               - static_cast<int>(m_thumb_pos.height()));

    m_scroll_target->move(m_scroll_target->boundary().x(),
                          m_boundary.y() - target_height()
                          * (m_thumb_pos.y() - m_boundary.y())
                          / m_boundary.height());
  } else {
    m_thumb_pos.x() = input.mouse_position().x() - m_drag_pos;
    m_thumb_pos.x() = std::max(m_thumb_pos.x(), m_boundary.x());
    m_thumb_pos.x() = std::min(m_thumb_pos.x(), m_boundary.x()
                               + static_cast<int>(m_boundary.width())
                               - static_cast<int>(m_thumb_pos.width()));

    m_scroll_target->move(m_boundary.x() - target_width()
                          * (m_thumb_pos.x() - m_boundary.x())
                          / m_boundary.width(),
                          m_scroll_target->boundary().y());
  }
}
