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

#include "ui/button.hpp"

#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

const Color button_background_color(225, 235, 238);
const Color button_label_color(0, 115, 207);
const Color button_label_hover_color(0, 0, 0);
const Color button_label_focus_color(0, 0, 255);
const Color button_label_depressed_color(25, 25, 112);
const Color button_selection_color(0, 0, 0);
constexpr unsigned spacing = 4;

void Button::update(unsigned ticks, InputHandler& input,
                    const Rect& active_region)
{
  Point cursor = input.mouse_position();
  if (m_boundary.contains_point(cursor)
      && active_region.contains_point(cursor))
    m_mouse_hover = true;
  else
    m_mouse_hover = false;

  if (input.was_mouse_button_released(Mouse::Button::left)) {
    if (m_depressed && m_mouse_hover)
      m_operation();
    m_depressed = false;
  }
  
  if (input.was_mouse_button_pressed(Mouse::Button::left)) {
    if (m_mouse_hover) {
      give_focus();
      m_depressed = true;
    } else {
      remove_focus();
    }
  }
}

void Button::draw(Renderer& renderer, const Rect& region) const
{
  //background
  renderer.set_clip_rect(region);
  renderer.set_draw_color(button_background_color);
  renderer.fill_rect(m_boundary);
  
  if (has_focus() || m_mouse_hover) {
    renderer.set_draw_color(button_selection_color);
    renderer.draw_rect(m_boundary);
  }

  //selection rectangle
  if (has_focus()) {
    Rect selection(m_boundary.x() + spacing,
                   m_boundary.y() + spacing,
                   m_boundary.width() - 2 * spacing,
                   m_boundary.height() - 2 * spacing);
    renderer.draw_dotted_rect(selection);
  }

  //label
  Color color;
  if (m_depressed)
    color = button_label_depressed_color;
  else if (m_mouse_hover)
    color = button_label_hover_color;
  else if (has_focus())
    color = button_label_focus_color;
  else
    color = button_label_color;
  renderer.set_draw_color(color);
  Point location(m_boundary.x() + 2 * spacing, m_boundary.y() + 2 * spacing);
  if (m_depressed) {
    location.x() += 1;
    location.y() += 1;
  }
  renderer.draw_text(location, m_font, m_label);
  
  renderer.set_clip_rect();
}

void Button::calc_size()
{
  unsigned width, height;
  m_font.text_size(m_label, &width, &height);

  resize(width + 4 * spacing, height + 4 * spacing);
}
