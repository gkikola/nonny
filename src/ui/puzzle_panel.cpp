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
#include <queue>
#include <string>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "puzzle/puzzle.hpp"
#include "utility/utility.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"
#include "video/texture.hpp"

const Color cell_border_color(128, 128, 128);
const Color blank_cell_color(255, 255, 255);
const Color shaded_cell_color(230, 230, 255);
const Color lightly_shaded_cell_color(240, 240, 255);

constexpr int cell_animation_duration = 100;
constexpr int time_for_mouse_unlock = 96;
constexpr int max_undo_size = 64;

const std::vector<int> zoom_levels = { 6, 8, 12, 16, 20, 24, 27, 32, 48,
                                       64, 96,
                                       128, 150};
constexpr int zoom_speed = 500; //pixels per second

PuzzlePanel::PuzzlePanel(Font& clue_font, const Texture& cell_texture,
                         Puzzle& puzzle)
  : m_clue_font(clue_font), m_cell_texture(cell_texture)
{
  m_cur_state = m_state_history.begin();
  update_clue_font();
  attach_puzzle(puzzle);
}

void PuzzlePanel::attach_puzzle(Puzzle& puzzle)
{
  m_puzzle = &puzzle;
  m_cur_puzzle_size = puzzle.width() * puzzle.height();
  m_cell_time.resize(m_cur_puzzle_size, 0);
  m_prev_cell_state.resize(m_cur_puzzle_size, PuzzleCell::State::blank);
  calc_grid_pos();
}

void PuzzlePanel::zoom(int steps)
{
  zoom(steps,
       m_boundary.x() + m_boundary.width() / 2,
       m_boundary.y() + m_boundary.height() / 2);
}

void PuzzlePanel::zoom(int steps, int x, int y)
{
  int cur_step;
  int num_levels = zoom_levels.size();
  for (cur_step = 0; cur_step < num_levels; ++cur_step) {
    if (zoom_levels[cur_step] >= m_target_cell_size)
      break;
  }

  int target = cur_step + steps;
  if (target >= 0 && target < num_levels)
    zoom_to(zoom_levels[target], x, y);
  else if (target < 0)
    zoom_to(zoom_levels[0], x, y);
  else if (target >= num_levels)
    zoom_to(zoom_levels[num_levels - 1], x, y);
}

void PuzzlePanel::set_edit_mode(bool edit_mode)
{
  m_edit_mode = edit_mode;
  m_state_history.clear();
  m_cur_state = m_state_history.begin();
  m_has_state_changed = true;
}

void PuzzlePanel::clear_puzzle()
{
  m_puzzle->clear_all_cells();
  m_has_state_changed = true;
}

void PuzzlePanel::shift_left()
{
  m_puzzle->shift_cells(-1, 0);
  m_has_state_changed = true;
}

void PuzzlePanel::shift_right()
{
  m_puzzle->shift_cells(1, 0);
  m_has_state_changed = true;
}

void PuzzlePanel::shift_up()
{
  m_puzzle->shift_cells(0, -1);
  m_has_state_changed = true;
}

void PuzzlePanel::shift_down()
{
  m_puzzle->shift_cells(0, 1);
  m_has_state_changed = true;
}

void PuzzlePanel::update(unsigned ticks, InputHandler& input,
                         const Rect& active_region)
{
  if (m_puzzle) {
    int size = m_puzzle->width() * m_puzzle->height();
    if (size != m_cur_puzzle_size) { //size has changed
      handle_resize();
      m_has_state_changed = true;
    }
    
    update_cells(ticks);

    if (m_target_cell_size != m_cell_size)
      update_zoom(ticks);

    if (!m_mouse_dragging && !m_kb_dragging) {
      m_puzzle->update(m_edit_mode);
      if (m_has_state_changed) {
        m_need_save = !m_state_history.empty();
        save_undo_state();
      }
    }

    handle_mouse_selection(ticks, input, active_region);
    handle_kb_shortcuts(ticks, input);
    handle_kb_selection(ticks, input);
    handle_mouse_wheel(ticks, input);
  }
}

void PuzzlePanel::draw(Renderer& renderer, const Rect& region) const
{
  if (m_puzzle) {
    renderer.set_clip_rect(region);
    draw_cells(renderer);
    draw_grid_lines(renderer);
    draw_selection(renderer);
    draw_clues(renderer);
  
    renderer.set_clip_rect();
  }
}

void PuzzlePanel::calc_grid_pos()
{
  if (m_puzzle) {
    m_clue_font.resize(m_cell_size * 3 / 5);
    m_grid_pos.x() = m_boundary.x();
    m_grid_pos.y() = m_boundary.y();
    m_clue_pos.x() = m_boundary.x();
    m_clue_pos.y() = m_boundary.y();

    for (int i = 0; i < m_puzzle->width(); ++i) {
      int bottom = m_boundary.y() + col_clue_height(i);
      if (bottom > m_grid_pos.y())
        m_grid_pos.y() = bottom;
    }

    for (int j = 0; j < m_puzzle->height(); ++j) {
      int side = m_boundary.x() + row_clue_width(j);
      if (side > m_grid_pos.x())
        m_grid_pos.x() = side;
    }

    int grid_width = m_puzzle->width() * (m_cell_size + 1) + 1;
    int grid_height = m_puzzle->height() * (m_cell_size + 1) + 1;
    m_boundary.width()
      = (m_grid_pos.x() - m_boundary.x()) + grid_width;
    m_boundary.height()
      = (m_grid_pos.y() - m_boundary.y()) + grid_height;

    //add extra space
    int x_padding = grid_width;
    int y_padding = grid_height;
    m_grid_pos.x() += x_padding;
    m_grid_pos.y() += y_padding;
    m_clue_pos.x() += x_padding;
    m_clue_pos.y() += y_padding;
    m_boundary.width() += 2 * x_padding;
    m_boundary.height() += 2 * y_padding;
  }
}

int PuzzlePanel::row_clue_width(int row) const
{
  int width = 0;
  for (auto clue : m_puzzle->row_clues(row)) {
    int text_wd, text_ht;
    m_clue_font.text_size(std::to_string(clue.value), &text_wd, &text_ht);
    width += text_wd + clue_spacing();
  }
  return width;
}

int PuzzlePanel::col_clue_height(int col) const
{
  int height = 0;
  for (auto clue : m_puzzle->col_clues(col)) {
    int text_wd, text_ht;
    m_clue_font.text_size(std::to_string(clue.value), &text_wd, &text_ht);
    height += text_ht + clue_spacing();
  }
  return height;
}

void PuzzlePanel::draw_grid_lines(Renderer& renderer) const
{
  renderer.set_draw_color(cell_border_color);
  int width = m_puzzle->width();
  int height = m_puzzle->height();
  for (int x = 0; x <= width; ++x) {
    Point start(m_grid_pos.x() + x * (m_cell_size + 1), m_grid_pos.y());
    Point end(start.x(), m_grid_pos.y() + height * (m_cell_size + 1));

    if (x % 5 == 0)
      renderer.draw_thick_line(start, end.y() - start.y(), 3, true);
    else
      renderer.draw_line(start, end);
  }
  for (int y = 0; y <= height; ++y) {
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
  constexpr double finished_fade = 0.33;
  
  int x, y;
  for (int i = 0; i < m_puzzle->width(); ++i) {
    x = m_grid_pos.x() + i * (m_cell_size + 1);
    y = m_grid_pos.y() - col_clue_height(i);
    for (auto clue : m_puzzle->col_clues(i)) {
      Color color;
      if (clue.state == PuzzleClue::State::finished) {
        color = clue.color.fade(finished_fade);
      } else {
        color = clue.color;
      }
      renderer.set_draw_color(color);
      std::string value = std::to_string(clue.value);
      int wd, ht;
      m_clue_font.text_size(value, &wd, &ht);
      Point pos(x + (m_cell_size + 1) / 2 - wd / 2, y);
      renderer.draw_text(pos, m_clue_font, value);
      y += ht + clue_spacing();
    }
  }

  for (int j = 0; j < m_puzzle->height(); ++j) {
    x = m_grid_pos.x() - row_clue_width(j);
    y = m_grid_pos.y() + j * (m_cell_size + 1);
    for (auto clue : m_puzzle->row_clues(j)) {
      Color color;
      if (clue.state == PuzzleClue::State::finished) {
        color = clue.color.fade(finished_fade);
      } else {
        color = clue.color;
      }
      renderer.set_draw_color(color);
      std::string value = std::to_string(clue.value);
      int wd, ht;
      m_clue_font.text_size(value, &wd, &ht);
      Point pos(x, y + (m_cell_size + 1) / 2 - ht / 2);
      renderer.draw_text(pos, m_clue_font, value);
      x += wd + clue_spacing();
    }
  }
}

void PuzzlePanel::draw_cells(Renderer& renderer) const
{
  int index = 0;
  for (int y = 0; y < m_puzzle->height(); ++y) {
    for (int x = 0; x < m_puzzle->width(); ++x, ++index) {
      draw_cell(renderer, x, y, (*m_puzzle)[x][y].state,
                m_prev_cell_state[index], (*m_puzzle)[x][y].color,
                m_cell_time[index]);
    }
  }
}

void PuzzlePanel::draw_cell(Renderer& renderer, int x, int y,
                            PuzzleCell::State state,
                            PuzzleCell::State prev_state,
                            const Color& color,
                            unsigned animation_time) const
{
  Rect dest(m_grid_pos.x() + x * (m_cell_size + 1) + 1,
            m_grid_pos.y() + y * (m_cell_size + 1) + 1,
            m_cell_size, m_cell_size);
  Rect src(0, 0, m_cell_texture.height(), m_cell_texture.height());

  if (x % 2 != y % 2)
    renderer.set_draw_color(lightly_shaded_cell_color);
  else if (x % 2 == 0)
    renderer.set_draw_color(shaded_cell_color);
  else
    renderer.set_draw_color(blank_cell_color);
  renderer.fill_rect(dest);

  //change size of square based on time elapsed
  if (state != prev_state) {
    if (animation_time < cell_animation_duration) {
      int size_reduction = (m_cell_size / 2)
        * (cell_animation_duration - animation_time)
        / cell_animation_duration;
      if (size_reduction < 0)
        size_reduction = 0;
      else if (size_reduction >= m_cell_size / 2)
        size_reduction = m_cell_size / 2 - 1;

      if (state == PuzzleCell::State::blank)
        size_reduction = m_cell_size / 2 - 1 - size_reduction;
        
      dest.x() += size_reduction;
      dest.y() += size_reduction;
      dest.width() -= 2 * size_reduction;
      dest.height() -= 2 * size_reduction;
    }
  }

  if (state == PuzzleCell::State::filled) {
    renderer.set_draw_color(color);
    renderer.fill_rect(dest);
  } else if (state == PuzzleCell::State::crossed_out) {
    renderer.copy_texture(m_cell_texture, src, dest);
  }

  if (state == PuzzleCell::State::blank
      && animation_time < cell_animation_duration) {
    if (prev_state == PuzzleCell::State::filled) {
      renderer.set_draw_color(color);
      renderer.fill_rect(dest);
    } else if (prev_state == PuzzleCell::State::crossed_out) {
      renderer.copy_texture(m_cell_texture, src, dest);
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

    int src_size = m_cell_texture.height();
    Rect dest(m_grid_pos.x() - m_cell_size - 1, cell.y(),
              m_cell_size, m_cell_size);
    Rect src(src_size, 0, src_size, src_size);
    renderer.copy_texture(m_cell_texture, src, dest);

    dest = Rect(cell.x(), m_grid_pos.y() - m_cell_size - 1,
                m_cell_size, m_cell_size);
    src = Rect(src_size * 2, 0, src_size, src_size);
    renderer.copy_texture(m_cell_texture, src, dest);
  }

  if ((m_mouse_dragging || m_kb_dragging)
      && m_draw_tool != DrawTool::paint
      && m_draw_tool != DrawTool::fill) {
    if (m_drag_marks)
      renderer.set_draw_color(m_color);
    else
      renderer.set_draw_color(default_colors::white);
    auto fn = [this, &renderer](int x, int y) {
      auto state = (m_drag_marks
                    ? PuzzleCell::State::filled
                    : PuzzleCell::State::blank);
      draw_cell(renderer, x, y, state, state,
                m_color, cell_animation_duration);
    };
    for_each_point_on_selection(fn);
  }
}

void PuzzlePanel::move(int x, int y)
{
  int delta_x = x - m_boundary.x();
  int delta_y = y - m_boundary.y();
  UIPanel::move(x, y);
  m_grid_pos.x() += delta_x;
  m_grid_pos.y() += delta_y;
  m_clue_pos.x() += delta_x;
  m_clue_pos.y() += delta_y;
}

void PuzzlePanel::update_cells(unsigned ticks)
{
  for (auto& time : m_cell_time) {
    if (time < cell_animation_duration)
      time += ticks;
  }
}

void PuzzlePanel::set_cell(int x, int y, PuzzleCell::State state)
{
  int index = x + y * m_puzzle->width();
  m_prev_cell_state[index] = (*m_puzzle)[x][y].state;
  m_cell_time[index] = 0;

  if (m_prev_cell_state[index] != state)
    m_has_state_changed = true;
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

void PuzzlePanel::drag_over_cell(int x, int y)
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
  int x = 0, y = 0;
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

      if (m_mouse_locked && !m_edit_mode) {
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
      //or for edit mode
      m_drag_start_x = x;
      m_drag_start_y = y;
      m_drag_marks = left_pressed;

      if (m_draw_tool == DrawTool::fill)
        do_draw_action(left_pressed);
    }
  }

  if (input.was_mouse_button_released(Mouse::Button::left)
      || input.was_mouse_button_released(Mouse::Button::right)) {
    if (m_mouse_dragging && m_edit_mode
        && m_draw_tool != DrawTool::paint
        && m_draw_tool != DrawTool::fill)
      do_draw_action(input.was_mouse_button_released(Mouse::Button::left));
    
    m_mouse_dragging = false;
    m_mouse_locked = false;
    input.release_mouse();
  }

  //check for scrolling drag
  if (input.was_mouse_button_pressed(Mouse::Button::middle)
      && region.contains_point(input.mouse_position())) {
    m_dragging_grid = true;
    input.capture_mouse();
  }

  if (input.was_mouse_button_released(Mouse::Button::middle)) {
    m_dragging_grid = false;
    input.release_mouse();
  }

  if (m_mouse_dragging) {    
    Point old_cursor = input.prev_mouse_position();
    int old_x, old_y;
    cell_at_point(old_cursor, &old_x, &old_y);

    if (old_x == x && old_y == y)
      m_ticks_on_cur_cell += ticks;
    else
      m_ticks_on_cur_cell = 0;

    generalized_cell_at_point(cursor, &m_drag_end_x, &m_drag_end_y);

    if ((m_mouse_lock_type == MouseLockType::to_row
         && (m_mouse_lock_pos + 1 == y || m_mouse_lock_pos == y + 1))
        || (m_mouse_lock_type == MouseLockType::to_col
            && (m_mouse_lock_pos + 1 == x || m_mouse_lock_pos == x + 1))) {
      if (!m_edit_mode && m_ticks_on_cur_cell >= time_for_mouse_unlock) {
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
    } else if (!m_edit_mode) {
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
    if (m_draw_tool == DrawTool::paint) {
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

  if (m_dragging_grid) {
    int relx = input.rel_mouse_x();
    int rely = input.rel_mouse_y();
    scroll(relx, rely);
  }
}

void PuzzlePanel::handle_kb_shortcuts(unsigned ticks, InputHandler& input)
{
  if (input.is_ctrl_down()) {
    if (input.was_key_pressed(Keyboard::Key::letter_z))
      undo();
    else if (input.was_key_pressed(Keyboard::Key::letter_y))
      redo();
  }
}

void PuzzlePanel::handle_kb_selection(unsigned ticks, InputHandler& input)
{
  bool fill_pressed = input.was_key_pressed(Keyboard::Key::space)
    || input.was_key_pressed(Keyboard::Key::enter)
    || input.was_key_pressed(Keyboard::Key::kp_enter)
    || input.was_key_pressed(Keyboard::Key::ins)
    || input.was_key_pressed(Keyboard::Key::kp_ins);
  bool cross_pressed = input.was_key_pressed(Keyboard::Key::backspace)
    || input.was_key_pressed(Keyboard::Key::del)
    || input.was_key_pressed(Keyboard::Key::kp_del);
  if (m_selected && (fill_pressed || cross_pressed)) {
    m_kb_dragging = true;
    m_drag_start_x = m_selection_x;
    m_drag_start_y = m_selection_y;
    m_drag_end_x = m_drag_start_x;
    m_drag_end_y = m_drag_start_y;
    m_drag_marks = fill_pressed;
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

    if (m_draw_tool == DrawTool::paint)
      set_cell(m_selection_x, m_selection_y, new_state);
    else if (m_draw_tool == DrawTool::fill)
      do_draw_action(fill_pressed);
  }

  bool fill_released = input.was_key_released(Keyboard::Key::space)
    || input.was_key_released(Keyboard::Key::enter)
    || input.was_key_released(Keyboard::Key::kp_enter);
  bool cross_released = input.was_key_released(Keyboard::Key::lctrl)
    || input.was_key_released(Keyboard::Key::rctrl)
    || input.was_key_released(Keyboard::Key::backspace)
    || input.was_key_released(Keyboard::Key::del)
    || input.was_key_released(Keyboard::Key::kp_del);
  if (fill_released || cross_released) {
    m_kb_dragging = false;

    if (m_edit_mode && m_selected
        && m_draw_tool != DrawTool::paint
        && m_draw_tool != DrawTool::fill)
      do_draw_action(fill_released);
  }

  int num_press = input.num_key_presses(Keyboard::Key::left)
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

void PuzzlePanel::handle_mouse_wheel(unsigned ticks, InputHandler& input)
{
  int steps = input.vert_mouse_wheel_scroll();
  if (steps != 0)
    zoom(steps, input.mouse_position().x(), input.mouse_position().y());
}

void PuzzlePanel::handle_resize()
{
  m_cur_puzzle_size = m_puzzle->width() * m_puzzle->height();
  m_cell_time.resize(m_cur_puzzle_size, cell_animation_duration);
  m_prev_cell_state.resize(m_cur_puzzle_size, PuzzleCell::State::blank);

  m_selection_x = 0;
  m_selection_y = 0;
  m_selected = false;

  calc_grid_pos();

  //update function will check for size change,
  //so we don't need to save undo state just yet
  m_has_state_changed = false;
  m_need_save = true;

  if (m_resize_callback)
    m_resize_callback();
}

void PuzzlePanel::do_draw_action(bool mark)
{
  switch (m_draw_tool) {
  case DrawTool::paint:
    //do nothing
    break;
  case DrawTool::line:
  case DrawTool::rect:
  case DrawTool::ellipse:
    {
      auto f = [this, mark](int x, int y) {
        if (mark)
          m_puzzle->mark_cell(x, y, m_color);
        else
          m_puzzle->clear_cell(x, y);
        m_has_state_changed = true;
      };
      for_each_point_on_selection(f);
    }
    break;
  case DrawTool::fill:
    {
      auto fill = PuzzleCell::State::filled;
      auto clear = PuzzleCell::State::blank;
      auto& start = (*m_puzzle)[m_selection_x][m_selection_y];

      auto target_state = start.state;
      auto replace_state = mark ? fill : clear;
      Color target_color = start.color;
      Color replace_color = m_color;

      if ((target_state != fill
           && target_state == replace_state)
          || (target_state == fill
              && replace_state == fill
              && target_color == replace_color))
        break;
      
      std::queue<Point> queue;
      queue.push(Point(m_selection_x, m_selection_y));

      auto test_point = [=](int x, int y) -> bool {
        if (x < 0 || x >= m_puzzle->width()
            || y < 0 || y >= m_puzzle->height())
          return false;
        if ((*m_puzzle)[x][y].state != target_state)
          return false;
        if (target_state == fill
            && (*m_puzzle)[x][y].color != target_color)
          return false;
        return true;
      };

      while (!queue.empty()) {
        Point n = queue.front();
        queue.pop();

        Point w = n, e = n;
        while (test_point(w.x(), w.y()))
          --w.x();
        while (test_point(e.x(), e.y()))
          ++e.x();

        for (int x = w.x() + 1; x < e.x(); ++x) {
          set_cell(x, w.y(), replace_state);
          if (test_point(x, w.y() - 1))
            queue.push(Point(x, w.y() - 1));
          if (test_point(x, w.y() + 1))
            queue.push(Point(x, w.y() + 1));
        }
      }
    }
    break;
  }
}

void PuzzlePanel::for_each_point_on_selection(CellFunction fn) const
{
  int x0 = m_drag_start_x;
  int y0 = m_drag_start_y;
  int x1 = m_drag_end_x;
  int y1 = m_drag_end_y;
  
  switch (m_draw_tool) {
  case DrawTool::paint:
  case DrawTool::fill:
    //do nothing
    break;
  case DrawTool::line:
    {
      int dx = std::abs(x1 - x0);
      int dy = -std::abs(y1 - y0);
      int sx = x0 < x1 ? 1 : -1;
      int sy = y0 < y1 ? 1 : -1;
      int err = dx + dy;
      int x = x0, y = y0;

      while (true) {
        call_on_point(fn, x, y);

        if (x == x1 && y == y1)
          break;

        int err2 = 2 * err;
        if (err2 >= dy) {
          err += dy;
          x += sx;
        }
        if (err2 <= dx) {
          err += dx;
          y += sy;
        }
      }
    }
    break;
  case DrawTool::rect:
    {
      int x = x0, y = y0;
      int sx = x0 < x1 ? 1 : -1;
      int sy = y0 < y1 ? 1 : -1;
      while (true) {
        call_on_point(fn, x, y0);
        call_on_point(fn, x, y1);
        if (x == x1) break;
        x += sx;
      }
      while (true) {
        call_on_point(fn, x0, y);
        call_on_point(fn, x1, y);
        if (y == y1) break;
        y += sy;
      }
    }
    break;
  case DrawTool::ellipse:
    {
      //thanks to Alois Zingl for ellipse algorithm
      int a = std::abs(x1 - x0), b = std::abs(y1 - y0), b1 = b & 1;
      int dx = 4 * (1 - a) * b * b;
      int dy = 4 * (b1 + 1) * a * a;
      int err = dx + dy + b1 * a * a, err2 = 0;

      if (x0 > x1) { x0 = x1; x1 += a; }
      if (y0 > y1) { y0 = y1; }
      y0 += (b + 1) / 2;
      y1 = y0 - b1;
      a *= 8 * a;
      b1 = 8 * b * b;

      do {
        call_on_point(fn, x1, y0); //quad 1
        call_on_point(fn, x0, y0); //quad 2
        call_on_point(fn, x0, y1); //quad 3
        call_on_point(fn, x1, y1); //quad 4
        err2 = 2 * err;
        if (err2 <= dy) {
          ++y0;
          --y1;
          dy += a;
          err += dy;
        }
        if (err2 >= dx || 2 * err > dy) {
          ++x0;
          --x1;
          dx += b1;
          err += dx;
        }
      } while (x0 <= x1);

      while (y0 - y1 < b) {
        call_on_point(fn, x0 - 1, y0);
        call_on_point(fn, x1 + 1, y0);
        call_on_point(fn, x0 - 1, y1);
        call_on_point(fn, x1 + 1, y1);
        ++y0;
        --y1;
      }
    }
    break;
  }
}

void PuzzlePanel::call_on_point(CellFunction fn, int x, int y) const
{
  if (x >= 0 && x < m_puzzle->width()
      && y >= 0 && y < m_puzzle->height())
    fn(x, y);
}

void PuzzlePanel::move_selection(Direction dir, int count)
{
  for (int i = 0; i < count; ++i) {
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

      if (m_kb_dragging && m_draw_tool == DrawTool::paint) {
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

  if (m_kb_dragging) {
    m_drag_end_x = m_selection_x;
    m_drag_end_y = m_selection_y;
  }
}

void PuzzlePanel::update_clue_font()
{
  int size = m_cell_size * 3 / 5;
  if (size <= 1)
    size = 1;
  m_clue_font.resize(size);
}

void PuzzlePanel::update_zoom(unsigned ticks)
{
  int delta = zoom_speed * ticks / 1000;
  if (delta == 0)
    delta = 1;

  //calculate grid pos of zoom target
  double zoom_pos_x = (m_zoom_target_x - m_grid_pos.x())
    / static_cast<double>(m_cell_size + 1);
  double zoom_pos_y = (m_zoom_target_y - m_grid_pos.y())
    / static_cast<double>(m_cell_size + 1);

  //update cell size
  if (m_cell_size < m_target_cell_size) {
    m_cell_size += delta;
    if (m_cell_size > m_target_cell_size)
      m_cell_size = m_target_cell_size;
  } else if (m_cell_size > m_target_cell_size) {
    m_cell_size -= delta;
    if (m_cell_size < m_target_cell_size)
      m_cell_size = m_target_cell_size;
  }

  //resize the font and recalculate boundaries
  update_clue_font();
  calc_grid_pos();

  int new_target_x = m_grid_pos.x() + zoom_pos_x * (m_cell_size + 1);
  int new_target_y = m_grid_pos.y() + zoom_pos_y * (m_cell_size + 1);
  scroll(m_zoom_target_x - new_target_x, m_zoom_target_y - new_target_y);
}

void PuzzlePanel::zoom_to(int cell_size, int x, int y)
{
  m_target_cell_size = cell_size;
  m_zoom_target_x = x;
  m_zoom_target_y = y;
}

void PuzzlePanel::save_undo_state()
{
  m_has_state_changed = false;

  if (!m_state_history.empty())
    ++m_cur_state;
  if (m_cur_state != m_state_history.end())
    m_state_history.erase(m_cur_state, m_state_history.end());
  if (m_state_history.size() >= max_undo_size)
    m_state_history.erase(m_state_history.begin());
  m_cur_state = m_state_history.insert(m_state_history.end(),
                                       CompressedState());
  m_puzzle->copy_state(*m_cur_state);
}

void PuzzlePanel::load_undo_state()
{
  if (m_cur_state != m_state_history.end()) {
    m_puzzle->load_state(*m_cur_state);
    if (m_cur_puzzle_size != m_puzzle->width() * m_puzzle->height())
      handle_resize();
  }
}

void PuzzlePanel::undo()
{
  if (m_cur_state != m_state_history.begin()) {
    --m_cur_state;
    load_undo_state();
  }
}

void PuzzlePanel::redo()
{
  if (m_cur_state != m_state_history.end()) {
    ++m_cur_state;
    if (m_cur_state != m_state_history.end())
      load_undo_state();
    else
      --m_cur_state;
  }
}
