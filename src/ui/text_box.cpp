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
#include "input/input_handler.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

constexpr unsigned spacing = 8;
const Color background_color = default_colors::white;
const Color foreground_color = default_colors::black;

const unsigned cursor_blink_duration = 512;

void TextBox::update(unsigned ticks, InputHandler& input,
                     const Rect& active_region)
{
  if (input.was_mouse_button_pressed(Mouse::Button::left)
      || input.was_mouse_button_pressed(Mouse::Button::right)) {
    Point mouse_pos = input.mouse_position();
    if (intersection(active_region, m_boundary).contains_point(mouse_pos))
      give_focus();
    else
      remove_focus();
  }
  
  if (has_focus()) {
    //handle cursor blinking
    m_cursor_duration += ticks;
    unsigned num_blinks = m_cursor_duration / cursor_blink_duration;
    if (num_blinks % 2 != 0)
      m_cursor_visible = !m_cursor_visible;
    m_cursor_duration -= num_blinks * cursor_blink_duration;
    
    unsigned prev_cursor = m_cursor;
    unsigned prev_size = m_text.size();
    
    std::string text = input.chars_entered();
    m_text.insert(m_cursor, text);
    m_cursor += text.size();

    unsigned num_press = input.num_key_presses(Keyboard::Key::backspace);
    if (num_press) {
      if (m_cursor >= num_press) {
        m_text.erase(m_cursor - num_press, num_press);
        m_cursor -= num_press;
      } else {
        m_text.erase(0, m_cursor);
        m_cursor = 0;
      }
    }

    num_press = input.num_key_presses(Keyboard::Key::del)
      + input.num_key_presses(Keyboard::Key::kp_del);
    if (num_press) {
      if (m_cursor + num_press <= m_text.size())
        m_text.erase(m_cursor, num_press);
      else
        m_text.erase(m_cursor);
    }

    num_press = input.num_key_presses(Keyboard::Key::left)
      + input.num_key_presses(Keyboard::Key::kp_left);
    if (m_cursor >= num_press)
      m_cursor -= num_press;
    else
      m_cursor = 0;

    num_press = input.num_key_presses(Keyboard::Key::right)
      + input.num_key_presses(Keyboard::Key::kp_right);
    if (m_cursor + num_press <= m_text.size())
      m_cursor += num_press;
    else
      m_cursor = m_text.size();

    //make sure cursor is visible
    if (m_cursor < m_visible)
      m_visible = m_cursor;
    else {
      while (m_visible < m_text.size() - 1
             && pos_to_screen_coord(m_cursor)
             > m_boundary.x()
             + static_cast<int>(m_boundary.width() - spacing))
        ++m_visible;

      while (m_visible > 0
             && pos_to_screen_coord(m_text.size())
             < m_boundary.x()
             + static_cast<int>(m_boundary.width() - 2 * spacing))
        --m_visible;
    }

    if (m_cursor != prev_cursor || m_text.size() != prev_size) {
      m_cursor_visible = true;
      m_cursor_duration = 0;
    }
  }
}

void TextBox::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_clip_rect(region);

  renderer.set_draw_color(background_color);
  renderer.fill_rect(m_boundary);

  if (m_font) {
    renderer.set_draw_color(foreground_color);
    Point text_pos(spacing + m_boundary.x(), spacing + m_boundary.y());
    renderer.draw_text(text_pos, *m_font, m_text.substr(m_visible));

    if (has_focus() && m_cursor_visible) {
      int x = pos_to_screen_coord(m_cursor);
      Point pt1(x, m_boundary.y() + spacing);
      Point pt2(x, m_boundary.y() + m_boundary.height() - spacing);
      renderer.draw_line(pt1, pt2);
    }
  }
  
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

int TextBox::pos_to_screen_coord(unsigned pos) const
{
  if (pos <= m_visible || !m_font)
    return m_boundary.x() + spacing;

  std::string text = m_text.substr(m_visible, pos - m_visible);
  unsigned width;
  m_font->text_size(text, &width, nullptr);
  return m_boundary.x() + spacing + width;
}

unsigned TextBox::screen_coord_to_pos(int x) const
{
  if (!m_font)
    return 0;

  if (x <= m_boundary.x())
    return m_visible;

  if (x > m_boundary.x() + static_cast<int>(m_boundary.width()))
    x = m_boundary.x() + m_boundary.width();

  unsigned cur_pos = m_visible;
  while (cur_pos < m_text.size()
         && x > m_boundary.x() + static_cast<int>(spacing)) {
    unsigned width;
    m_font->text_size(m_text.substr(cur_pos, 1), &width, nullptr);
    x -= width;
    ++cur_pos;
  }
  return cur_pos;
}
