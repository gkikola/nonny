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

#include <algorithm>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

constexpr int spacing = 8;
const Color background_color = default_colors::white;
const Color foreground_color = default_colors::black;
const Color selection_color = default_colors::blue;

const int cursor_blink_duration = 512;

void TextBox::set_text(const std::string& text)
{
  m_text = text;
  m_cursor = 0;
  m_visible = 0;
}

void TextBox::select_all()
{
  m_sel_start = 0;
  m_cursor = m_sel_length = m_text.size();
}

void TextBox::update(unsigned ticks, InputHandler& input,
                     const Rect& active_region)
{
  handle_mouse_input(input, active_region);

  if (has_focus()) {
    //select text if tabbing onto control
    if (input.was_key_pressed(Keyboard::Key::tab))
      select_all();

    //handle cursor blinking
    m_cursor_duration += ticks;
    int num_blinks = m_cursor_duration / cursor_blink_duration;
    if (num_blinks % 2 != 0)
      m_cursor_visible = !m_cursor_visible;
    m_cursor_duration -= num_blinks * cursor_blink_duration;

    int prev_cursor = m_cursor;
    int prev_size = m_text.size();

    read_chars(input);
    handle_delete_keys(input);
    handle_arrow_keys(input);
    handle_home_end(input);

    //make sure cursor is visible
    if (m_cursor < m_visible)
      m_visible = m_cursor;
    else {
      while (m_visible < static_cast<int>(m_text.size()) - 1
             && pos_to_screen_coord(m_cursor)
             > m_boundary.x()
             + m_boundary.width() - spacing)
        ++m_visible;

      while (m_visible > 0
             && pos_to_screen_coord(m_text.size())
             < m_boundary.x()
             + m_boundary.width() - 2 * spacing)
        --m_visible;
    }

    if (m_cursor != prev_cursor
        || static_cast<int>(m_text.size()) != prev_size) {
      m_cursor_visible = true;
      m_cursor_duration = 0;

      if (m_sel_length == 0)
        m_sel_start = m_cursor;
    }
  } else { //no focus
    m_sel_length = 0;
  }
}

void TextBox::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_clip_rect(intersection(region, m_boundary));

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

    if (m_sel_length > 0) {
      int x1 = pos_to_screen_coord(m_sel_start);
      int x2 = pos_to_screen_coord(m_sel_start + m_sel_length);

      Rect sel(x1, m_boundary.y() + spacing,
               x2 - x1, m_boundary.height() - 2 * spacing);
      Rect clip_region = intersection(intersection(sel, region),
                                      m_boundary);
      renderer.set_clip_rect(clip_region);
      renderer.set_draw_color(selection_color);
      renderer.fill_rect(sel);
      renderer.set_draw_color(background_color);
      renderer.draw_text(text_pos, *m_font, m_text.substr(m_visible));
    }
  }

  renderer.set_clip_rect();
}

void TextBox::calc_size()
{
  int width = 0, height = 0;
  if (m_font)
    m_font->text_size("Sample text", &width, &height);
  width += 2 * spacing;
  height += 2 * spacing;
  resize(width, height);
}

int TextBox::pos_to_screen_coord(int pos) const
{
  if (pos <= m_visible || !m_font)
    return m_boundary.x() + spacing;

  std::string text = m_text.substr(m_visible, pos - m_visible);
  int width;
  m_font->text_size(text, &width, nullptr);
  return m_boundary.x() + spacing + width;
}

int TextBox::screen_coord_to_pos(int x) const
{
  if (!m_font)
    return 0;

  if (x <= m_boundary.x())
    return m_visible;

  if (x > m_boundary.x() + m_boundary.width())
    x = m_boundary.x() + m_boundary.width();

  int cur_pos = m_visible;
  while (cur_pos < static_cast<int>(m_text.size())
         && x > m_boundary.x() + spacing) {
    int width;
    m_font->text_size(m_text.substr(cur_pos, 1), &width, nullptr);
    x -= width;
    ++cur_pos;
  }
  return cur_pos;
}

void TextBox::read_chars(InputHandler& input)
{
  std::string text = input.chars_entered();
  if (!text.empty()) {
    if (m_sel_length > 0) {
      m_text.erase(m_sel_start, m_sel_length);
      m_sel_length = 0;
      m_cursor = m_sel_start;
    }

    m_text.insert(m_cursor, text);
    m_cursor += text.size();
  }
}

void TextBox::handle_arrow_keys(InputHandler& input)
{
  int num_press = input.num_key_presses(Keyboard::Key::left);
  if (num_press) {
    if (input.is_shift_down()) {
      if (m_sel_start > 0 && m_cursor == m_sel_start) {
        --m_sel_start;
        ++m_sel_length;
      } else if (m_cursor == m_sel_start + m_sel_length) {
        --m_sel_length;
      }
    } else {
      m_sel_start = m_cursor;
      m_sel_length = 0;
    }
  }
  if (m_cursor >= num_press)
    m_cursor -= num_press;
  else
    m_cursor = 0;

  num_press = input.num_key_presses(Keyboard::Key::right);
  if (num_press) {
    if (input.is_shift_down()) {
      if (m_sel_start + m_sel_length < static_cast<int>(m_text.size())
          && m_cursor == m_sel_start + m_sel_length) {
        ++m_sel_length;
      } else if (m_cursor == m_sel_start && m_sel_length > 0) {
        ++m_sel_start;
        --m_sel_length;
      }
    } else {
      m_sel_start = m_cursor;
      m_sel_length = 0;
    }
  }
  if (m_cursor + num_press <= static_cast<int>(m_text.size()))
    m_cursor += num_press;
  else
    m_cursor = m_text.size();
}

void TextBox::handle_delete_keys(InputHandler& input)
{
  int num_press = input.num_key_presses(Keyboard::Key::backspace);
  if (num_press && m_sel_length > 0) {
    m_text.erase(m_sel_start, m_sel_length);
    m_sel_length = 0;
    m_cursor = m_sel_start;
    --num_press;
  }
  if (num_press) {
    if (m_cursor >= num_press) {
      m_text.erase(m_cursor - num_press, num_press);
      m_cursor -= num_press;
    } else {
      m_text.erase(0, m_cursor);
      m_cursor = 0;
    }
  }

  num_press = input.num_key_presses(Keyboard::Key::del);
  if (num_press && m_sel_length > 0) {
    m_text.erase(m_sel_start, m_sel_length);
    m_sel_length = 0;
    m_cursor = m_sel_start;
    --num_press;
  }
  if (num_press) {
    if (m_cursor + num_press <= static_cast<int>(m_text.size()))
      m_text.erase(m_cursor, num_press);
    else
      m_text.erase(m_cursor);
  }
}

void TextBox::handle_home_end(InputHandler& input)
{
  int old_sel_start = m_sel_start;
  if (input.was_key_pressed(Keyboard::Key::home)) {
    m_cursor = 0;
    m_visible = 0;

    if (input.is_shift_down()) {
      m_sel_start = 0;
      m_sel_length += old_sel_start;
    } else {
      m_sel_start = 0;
      m_sel_length = 0;
    }
  } else if (input.was_key_pressed(Keyboard::Key::end)) {
    m_cursor = m_sel_start = m_text.size();

    if (input.is_shift_down()) {
      m_sel_start = old_sel_start;
      m_sel_length = m_text.size() - old_sel_start;
    } else {
      m_sel_length = 0;
    }
  }
}

void TextBox::handle_mouse_input(InputHandler& input, const Rect& region)
{
  if (input.was_mouse_button_released(Mouse::Button::left))
    m_dragging_sel = false;

  if (input.was_mouse_button_pressed(Mouse::Button::left)
      || input.was_mouse_button_pressed(Mouse::Button::right)) {
    Point mouse_pos = input.mouse_position();
    if (intersection(region, m_boundary).contains_point(mouse_pos)) {
      if (input.was_mouse_button_double_clicked(Mouse::Button::left)) {
        select_all();
        m_cursor = m_text.length();
      } else {
        m_sel_length = 0;
      }
      give_focus();
    } else {
      remove_focus();
    }

    if (has_focus() && m_sel_length == 0) {
      m_cursor = screen_coord_to_pos(mouse_pos.x());
      m_cursor_visible = true;
      m_cursor_duration = 0;
      m_dragging_sel = true;
      m_drag_start_pos = m_cursor;
    }
  } //end if left button clicked

  if (has_focus() && m_dragging_sel) {
    int new_point = screen_coord_to_pos(input.mouse_position().x());
    m_cursor = new_point;
    if (new_point <= m_drag_start_pos) {
      m_sel_start = new_point;
      m_sel_length = m_drag_start_pos - new_point;
    } else {
      m_sel_start = m_drag_start_pos;
      m_sel_length = new_point - m_drag_start_pos;
    }
  }
}
