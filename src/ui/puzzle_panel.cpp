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

#include "ui/puzzle_panel.hpp"

#include <cmath>
#include <string>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "puzzle/puzzle.hpp"
#include "utility/utility.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

const Color cell_border_color(128, 128, 128);
const Color blank_cell_color(255, 255, 255);
const Color shaded_cell_color(230, 230, 255);
const Color lightly_shaded_cell_color(240, 240, 255);

constexpr unsigned cell_animation_duration = 100;
constexpr unsigned time_for_mouse_unlock = 96;

PuzzlePanel::PuzzlePanel(Font& clue_font, const Texture& cell_texture,
                         Puzzle& puzzle)
  : m_clue_font(clue_font), m_cell_texture(cell_texture)
{
  attach_puzzle(puzzle);
  calc_grid_pos();
}

void PuzzlePanel::attach_puzzle(Puzzle& puzzle)
{
  m_puzzle = &puzzle;
  unsigned size = puzzle.width() * puzzle.height();
  m_cell_time.resize(size, 0);
  m_prev_cell_state.resize(size, PuzzleCell::State::blank);
}

void PuzzlePanel::update(unsigned ticks, InputHandler& input,
                         const Rect& active_region)
{
  if (m_puzzle) {
    update_cells(ticks);

    if (!m_mouse_dragging && !m_kb_dragging)
      m_puzzle->update_clues(false);

    handle_mouse_selection(ticks, input, active_region);
    handle_kb_selection(ticks, input);
  }
}

void PuzzlePanel::draw(Renderer& renderer, const Rect& region) const
{
  if (m_puzzle) {
    renderer.set_clip_rect(region);
    draw_cells(renderer);
    draw_grid_lines(renderer);
    draw_clues(renderer);
    draw_selection(renderer);
  
    renderer.set_clip_rect();
  }
}

void PuzzlePanel::calc_grid_pos()
{
  if (m_puzzle) {
    m_clue_font.resize(m_cell_size * 3 / 5);
    m_grid_pos.x() = m_boundary.x();
    m_grid_pos.y() = m_boundary.y();

    for (unsigned i = 0; i < m_puzzle->width(); ++i) {
      int bottom = m_boundary.y() + col_clue_height(i);
      if (bottom > m_grid_pos.y())
        m_grid_pos.y() = bottom;
    }

    for (unsigned j = 0; j < m_puzzle->height(); ++j) {
      int side = m_boundary.x() + row_clue_width(j);
      if (side > m_grid_pos.x())
        m_grid_pos.x() = side;
    }

    m_boundary.width()
      = m_grid_pos.x() + m_puzzle->width() * (m_cell_size + 1) + 1;
    m_boundary.height()
      = m_grid_pos.y() + m_puzzle->height() * (m_cell_size + 1) + 1;
  }
}

unsigned PuzzlePanel::row_clue_width(unsigned row) const
{
  unsigned width = 0;
  for (auto clue : m_puzzle->row_clues(row)) {
    unsigned text_wd, text_ht;
    m_clue_font.text_size(std::to_string(clue.value), &text_wd, &text_ht);
    width += text_wd + clue_spacing();
  }
  return width;
}

unsigned PuzzlePanel::col_clue_height(unsigned col) const
{
  unsigned height = 0;
  for (auto clue : m_puzzle->col_clues(col)) {
    unsigned text_wd, text_ht;
    m_clue_font.text_size(std::to_string(clue.value), &text_wd, &text_ht);
    height += text_ht + clue_spacing();
  }
  return height;
}

void PuzzlePanel::draw_grid_lines(Renderer& renderer) const
{
  renderer.set_draw_color(cell_border_color);
  unsigned width = m_puzzle->width();
  unsigned height = m_puzzle->height();
  for (unsigned x = 0; x <= width; ++x) {
    Point start(m_grid_pos.x() + x * (m_cell_size + 1), m_grid_pos.y());
    Point end(start.x(), m_grid_pos.y() + height * (m_cell_size + 1));

    if (x % 5 == 0)
      renderer.draw_thick_line(start, end.y() - start.y(), 3, true);
    else
      renderer.draw_line(start, end);
  }
  for (unsigned y = 0; y <= height; ++y) {
    Point start(m_grid_pos.x(), m_grid_pos.y() + y * (m_cell_size + 1));
    Point end(m_grid_pos.x() + width * (m_cell_size + 1), start.y());

    if (y % 5 == 0)
      renderer.draw_thick_line(start, end.x() - start.x(), 3, false);
    else
      renderer.draw_line(start, end);
  }
}

void PuzzlePanel::draw_clues(Renderer& renderer) const
{
  int x, y;
  for (unsigned i = 0; i < m_puzzle->width(); ++i) {
    x = m_grid_pos.x() + i * (m_cell_size + 1);
    y = m_grid_pos.y() - col_clue_height(i);
    for (auto clue : m_puzzle->col_clues(i)) {
      Color color;
      if (clue.state == PuzzleClue::State::finished) {
        color = Color(clue.color.red() / 2,
                      clue.color.green() / 2,
                      clue.color.blue() / 2);
      } else {
        color = clue.color;
      }
      renderer.set_draw_color(color);
      std::string value = std::to_string(clue.value);
      unsigned wd, ht;
      m_clue_font.text_size(value, &wd, &ht);
      Point pos(x + (m_cell_size + 1) / 2 - wd / 2, y);
      renderer.draw_text(pos, m_clue_font, value);
      y += ht + clue_spacing();
    }
  }

  for (unsigned j = 0; j < m_puzzle->height(); ++j) {
    x = m_grid_pos.x() - row_clue_width(j);
    y = m_grid_pos.y() + j * (m_cell_size + 1);
    for (auto clue : m_puzzle->row_clues(j)) {
      Color color;
      if (clue.state == PuzzleClue::State::finished) {
        color = Color(clue.color.red() / 2,
                      clue.color.green() / 2,
                      clue.color.blue() / 2);
      } else {
        color = clue.color;
      }
      renderer.set_draw_color(color);
      std::string value = std::to_string(clue.value);
      unsigned wd, ht;
      m_clue_font.text_size(value, &wd, &ht);
      Point pos(x, y + (m_cell_size + 1) / 2 - ht / 2);
      renderer.draw_text(pos, m_clue_font, value);
      x += wd + clue_spacing();
    }
  }
}

void PuzzlePanel::draw_cells(Renderer& renderer) const
{
  unsigned index = 0;
  for (unsigned y = 0; y < m_puzzle->height(); ++y) {
    for (unsigned x = 0; x < m_puzzle->width(); ++x, ++index) {
      Rect dest(m_grid_pos.x() + x * (m_cell_size + 1) + 1,
                m_grid_pos.y() + y * (m_cell_size + 1) + 1,
                m_cell_size, m_cell_size);
      const PuzzleCell& cell = (*m_puzzle)[x][y];

      if (x % 2 != y % 2)
        renderer.set_draw_color(lightly_shaded_cell_color);
      else if (x % 2 == 0)
        renderer.set_draw_color(shaded_cell_color);
      else
        renderer.set_draw_color(blank_cell_color);
      renderer.fill_rect(dest);

      //change size of square based on time elapsed
      if (cell.state != m_prev_cell_state[index]) {
        if (m_cell_time[index] < cell_animation_duration) {
          int size_reduction = (m_cell_size / 2)
            * (cell_animation_duration - m_cell_time[index])
            / cell_animation_duration;
          if (size_reduction < 0)
            size_reduction = 0;
          else if (size_reduction >= static_cast<int>(m_cell_size) / 2)
            size_reduction = m_cell_size / 2 - 1;

          if (cell.state == PuzzleCell::State::blank)
            size_reduction = m_cell_size / 2 - 1 - size_reduction;
        
          dest.x() += size_reduction;
          dest.y() += size_reduction;
          dest.width() -= 2 * size_reduction;
          dest.height() -= 2 * size_reduction;
        }
      }

      if (cell.state == PuzzleCell::State::filled) {
        renderer.set_draw_color(cell.color);
        renderer.fill_rect(dest);
      } else if (cell.state == PuzzleCell::State::crossed_out) {
        renderer.copy_texture(m_cell_texture, Rect(), dest);
      }

      if (cell.state == PuzzleCell::State::blank
          && m_cell_time[index] < cell_animation_duration) {
        if (m_prev_cell_state[index] == PuzzleCell::State::filled) {
          renderer.set_draw_color(cell.color);
          renderer.fill_rect(dest);
        } else if (m_prev_cell_state[index]
                   == PuzzleCell::State::crossed_out) {
          renderer.copy_texture(m_cell_texture, Rect(), dest);
        }
      }
    }
  }
}

void PuzzlePanel::draw_selection(Renderer& renderer) const
{
  if (m_selected) {
    Rect cell(m_grid_pos.x() + m_selection_x * (m_cell_size + 1),
              m_grid_pos.y() + m_selection_y * (m_cell_size + 1),
              m_cell_size + 1, m_cell_size + 1);
    renderer.set_draw_color(m_color);
    renderer.draw_thick_rect(cell, 3);

    renderer.draw_thick_line(Point(m_boundary.x(), cell.y()),
                             m_grid_pos.x() - m_boundary.x(), 3, false);
    renderer.draw_thick_line(Point(m_boundary.x(), cell.y() + m_cell_size + 1),
                             m_grid_pos.x() - m_boundary.x(), 3, false);
    renderer.draw_thick_line(Point(m_boundary.x(), cell.y()),
                             m_cell_size + 1, 3, true);

    renderer.draw_thick_line(Point(cell.x(), m_boundary.y()),
                             m_grid_pos.y() - m_boundary.y(), 3, true);
    renderer.draw_thick_line(Point(cell.x() + m_cell_size + 1, m_boundary.y()),
                             m_grid_pos.y() - m_boundary.y(), 3, true);
    renderer.draw_thick_line(Point(cell.x(), m_boundary.y()),
                             m_cell_size + 1, 3, false);
  }
}

void PuzzlePanel::move(int x, int y)
{
  int delta_x = x - m_boundary.x();
  int delta_y = y - m_boundary.y();
  UIPanel::move(x, y);
  m_grid_pos.x() += delta_x;
  m_grid_pos.y() += delta_y;
}

void PuzzlePanel::update_cells(unsigned ticks)
{
  for (auto& time : m_cell_time) {
    if (time < cell_animation_duration)
      time += ticks;
  }
}

void PuzzlePanel::set_cell(unsigned x, unsigned y, PuzzleCell::State state)
{
  unsigned index = x + y * m_puzzle->width();
  m_prev_cell_state[index] = (*m_puzzle)[x][y].state;
  m_cell_time[index] = 0;
  switch (state) {
  case PuzzleCell::State::filled:
    m_puzzle->mark_cell(x, y, m_color);
    break;
  default:
  case PuzzleCell::State::blank:
    m_puzzle->clear_cell(x, y);
    break;
  case PuzzleCell::State::crossed_out:
    m_puzzle->cross_out_cell(x, y);
    break;
  };
}

void PuzzlePanel::drag_over_cell(unsigned x, unsigned y)
{
  PuzzleCell::State cur_state = (*m_puzzle)[x][y].state;
  PuzzleCell::State new_state;
  bool set = false;
  switch (m_mouse_drag_type) {
  default:
  case DragType::fill:
    new_state = PuzzleCell::State::filled;
    set = (cur_state == PuzzleCell::State::blank);
    break;
  case DragType::cross:
    new_state = PuzzleCell::State::crossed_out;
    set = (cur_state == PuzzleCell::State::blank);
    break;
  case DragType::blanking_fill:
    new_state = PuzzleCell::State::blank;
    set = (cur_state == PuzzleCell::State::filled);
    break;
  case DragType::blanking_cross:
    new_state = PuzzleCell::State::blank;
    set = (cur_state == PuzzleCell::State::crossed_out);
    break;
  }
  if (set)
    set_cell(x, y, new_state);
}

void PuzzlePanel::handle_mouse_selection(unsigned ticks, InputHandler& input,
                                         const Rect& region)
{
  //figure out which cell is under the mouse cursor
  Point cursor = input.mouse_position();
  unsigned x = 0, y = 0;
  cell_at_point(cursor, &x, &y);

  PuzzleCell::State cur_state;
  bool cursor_over_grid
    = is_point_in_grid(cursor) && region.contains_point(cursor);
  if (cursor_over_grid) {
    cur_state = (*m_puzzle)[x][y].state;
  }
  
  if (input.rel_mouse_x() != 0 || input.rel_mouse_y() != 0) {
    m_selected = cursor_over_grid;
    if (m_selected) {
      m_selection_x = x;
      m_selection_y = y;

      if (m_mouse_locked) {
        if (m_mouse_lock_type == MouseLockType::to_row)
          m_selection_y = m_mouse_lock_pos;
        else
          m_selection_x = m_mouse_lock_pos;
      }
    }
  }

  //check mouse buttons for drags
  if (cursor_over_grid) {
    bool left_pressed
      = input.was_mouse_button_pressed(Mouse::Button::left);
    bool right_pressed
      = input.was_mouse_button_pressed(Mouse::Button::right);
    if (left_pressed || right_pressed) {
      if (cur_state == PuzzleCell::State::filled)
        m_mouse_drag_type = DragType::blanking_fill;
      else if (cur_state == PuzzleCell::State::blank)
        m_mouse_drag_type = left_pressed ? DragType::fill : DragType::cross;
      else if (cur_state == PuzzleCell::State::crossed_out)
        m_mouse_drag_type = DragType::blanking_cross;
      m_mouse_dragging = true;
      input.capture_mouse();

      //keep track of current cell in order to lock mouse later on
      m_drag_start_x = x;
      m_drag_start_y = y;
    }
  }

  if (input.was_mouse_button_released(Mouse::Button::left)
      || input.was_mouse_button_released(Mouse::Button::right)) {
    m_mouse_dragging = false;
    m_mouse_locked = false;
    input.release_mouse();
  }
        
  if (m_mouse_dragging) {    
    Point old_cursor = input.prev_mouse_position();
    unsigned old_x, old_y;
    cell_at_point(old_cursor, &old_x, &old_y);

    if (old_x == x && old_y == y)
      m_ticks_on_cur_cell += ticks;
    else
      m_ticks_on_cur_cell = 0;

    if ((m_mouse_lock_type == MouseLockType::to_row
         && (m_mouse_lock_pos + 1 == y || m_mouse_lock_pos == y + 1))
        || (m_mouse_lock_type == MouseLockType::to_col
            && (m_mouse_lock_pos + 1 == x || m_mouse_lock_pos == x + 1))) {
      if (m_ticks_on_cur_cell >= time_for_mouse_unlock) {
        m_mouse_locked = false;
        m_drag_start_x = x;
        m_drag_start_y = y;
      }
    }

    if (m_mouse_locked) {
      if (m_mouse_lock_type == MouseLockType::to_row) {
        y = m_mouse_lock_pos;
        old_y = m_mouse_lock_pos;
      } else {
        x = m_mouse_lock_pos;
        old_x = m_mouse_lock_pos;
      }
    } else {
      if (x == m_drag_start_x && y != m_drag_start_y) {
        m_mouse_lock_type = MouseLockType::to_col;
        m_mouse_lock_pos = x;
        m_mouse_locked = true;
      } else if (x != m_drag_start_x && y == m_drag_start_y) {
        m_mouse_lock_type = MouseLockType::to_row;
        m_mouse_lock_pos = y;
        m_mouse_locked = true;
      } else {
        m_drag_start_x = x;
        m_drag_start_y = y;
      }
    }
    
    //mark current cell and any cells that the mouse passed over
    drag_over_cell(x, y);
    
    while (old_x != x || old_y != y) {
      drag_over_cell(old_x, old_y);
      if (old_x < x)
        ++old_x;
      else if (old_x > x)
        --old_x;
      if (old_y < y)
        ++old_y;
      else if (old_y > y)
        --old_y;
    }
  }
}

void PuzzlePanel::handle_kb_selection(unsigned ticks, InputHandler& input)
{
  bool fill_pressed = input.was_key_pressed(Keyboard::Key::space)
    || input.was_key_pressed(Keyboard::Key::enter)
    || input.was_key_pressed(Keyboard::Key::kp_enter)
    || input.was_key_pressed(Keyboard::Key::ins)
    || input.was_key_pressed(Keyboard::Key::kp_ins);
  bool cross_pressed = input.was_key_pressed(Keyboard::Key::lctrl)
    || input.was_key_pressed(Keyboard::Key::rctrl)
    || input.was_key_pressed(Keyboard::Key::backspace)
    || input.was_key_pressed(Keyboard::Key::del)
    || input.was_key_pressed(Keyboard::Key::kp_del);
  if (m_selected && (fill_pressed || cross_pressed)) {
    m_kb_dragging = true;
    PuzzleCell::State cur_state
      = (*m_puzzle)[m_selection_x][m_selection_y].state;
    PuzzleCell::State new_state;
    switch (cur_state) {
    case PuzzleCell::State::filled:
      m_kb_drag_type = DragType::blanking_fill;
      new_state = PuzzleCell::State::blank;
      break;
    default:
    case PuzzleCell::State::blank:
      if (fill_pressed) {
        m_kb_drag_type = DragType::fill;
        new_state = PuzzleCell::State::filled;
      } else {
        m_kb_drag_type = DragType::cross;
        new_state = PuzzleCell::State::crossed_out;
      }
      break;
    case PuzzleCell::State::crossed_out:
      m_kb_drag_type = DragType::blanking_cross;
      new_state = PuzzleCell::State::blank;
      break;
    };

    set_cell(m_selection_x, m_selection_y, new_state);
  }

  if (input.was_key_released(Keyboard::Key::space)
      || input.was_key_released(Keyboard::Key::enter)
      || input.was_key_released(Keyboard::Key::kp_enter)
      || input.was_key_released(Keyboard::Key::lctrl)
      || input.was_key_released(Keyboard::Key::rctrl)
      || input.was_key_released(Keyboard::Key::backspace)
      || input.was_key_released(Keyboard::Key::del)
      || input.was_key_released(Keyboard::Key::kp_del))
    m_kb_dragging = false;

  unsigned num_press = input.num_key_presses(Keyboard::Key::left)
    + input.num_key_presses(Keyboard::Key::kp_left);
  if (num_press)
    move_selection(Direction::left, num_press);

  num_press = input.num_key_presses(Keyboard::Key::right)
    + input.num_key_presses(Keyboard::Key::kp_right);
  if (num_press)
    move_selection(Direction::right, num_press);

  num_press = input.num_key_presses(Keyboard::Key::down)
    + input.num_key_presses(Keyboard::Key::kp_down);
  if (num_press)
    move_selection(Direction::down, num_press);

  num_press = input.num_key_presses(Keyboard::Key::up)
    + input.num_key_presses(Keyboard::Key::kp_up);
  if (num_press)
    move_selection(Direction::up, num_press);  
}

void PuzzlePanel::move_selection(Direction dir, unsigned count)
{
  for (unsigned i = 0; i < count; ++i) {
    if (!m_selected)
      m_selected = true;
    else {
      switch (dir) {
      case Direction::left:
        if (m_selection_x > 0)
          --m_selection_x;
        else if (!m_kb_dragging)
          m_selection_x = m_puzzle->width() - 1;
        break;
      case Direction::right:
        if (m_selection_x < m_puzzle->width() - 1)
          ++m_selection_x;
        else if (!m_kb_dragging)
          m_selection_x = 0;
        break;
      case Direction::up:
        if (m_selection_y > 0)
          --m_selection_y;
        else if (!m_kb_dragging)
          m_selection_y = m_puzzle->height() - 1;
        break;
      case Direction::down:
        if (m_selection_y < m_puzzle->height() - 1)
          ++m_selection_y;
        else if (!m_kb_dragging)
          m_selection_y = 0;
        break;
      }

      if (m_kb_dragging) {
        auto cur_state = (*m_puzzle)[m_selection_x][m_selection_y].state;
        decltype(cur_state) new_state;
        bool set_state = false;
        switch (m_kb_drag_type) {
        default:
        case DragType::fill:
          if (cur_state == PuzzleCell::State::blank) {
            new_state = PuzzleCell::State::filled;
            set_state = true;
          }
          break;
        case DragType::cross:
          if (cur_state == PuzzleCell::State::blank) {
            new_state = PuzzleCell::State::crossed_out;
            set_state = true;
          }
          break;
        case DragType::blanking_fill:
          if (cur_state == PuzzleCell::State::filled) {
            new_state = PuzzleCell::State::blank;
            set_state = true;
          }
          break;
        case DragType::blanking_cross:
          if (cur_state == PuzzleCell::State::crossed_out) {
            new_state = PuzzleCell::State::blank;
            set_state = true;
          }
          break;
        }

        if (set_state)
          set_cell(m_selection_x, m_selection_y, new_state);
      } //end if kb_dragging
    }
  } //end for
}
