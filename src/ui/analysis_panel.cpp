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

#include "ui/analysis_panel.hpp"

#include <algorithm>
#include <string>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "utility/utility.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

const Color background_color(192, 192, 192);

constexpr int panel_spacing = 16;
constexpr int text_spacing = 8;
constexpr int preview_size = 256;
constexpr int button_width = 150;
constexpr unsigned solution_cycle_duration = 1000;

AnalysisPanel::AnalysisPanel(const Font& font, const Puzzle& puzzle)
  : m_puzzle(puzzle), m_solver(m_puzzle), m_font(font)
{
  setup_buttons();
  calc_size();
  m_puzzle.clear_all_cells();
}

void AnalysisPanel::update(unsigned ticks, InputHandler& input,
                           const Rect& active_region)
{
  if (m_solver_running) {
    m_run_time += ticks;
    if (m_solver.step()) {
      m_done_solving = true;
      m_solver_running = false;
    } else if (m_solver.was_contradiction_found()) {
      m_inconsistent = true;
      m_solver_running = false;
    }
  }

  if (m_solver.is_finished()) {
    m_sol_cycle_time += ticks;
    if (m_sol_cycle_time >= solution_cycle_duration) {
      m_solver.cycle_solution();
      m_sol_cycle_time = 0;
    }
  }

  if (input.was_key_pressed(Keyboard::Key::tab)) {
    if (input.is_shift_down())
      focus_prev();
    else
      focus_next();
  }

  if (input.was_key_pressed(Keyboard::Key::left)
      || input.was_key_pressed(Keyboard::Key::kp_left)
      || input.was_key_pressed(Keyboard::Key::up)
      || input.was_key_pressed(Keyboard::Key::kp_up))
    focus_prev();

  if (input.was_key_pressed(Keyboard::Key::right)
      || input.was_key_pressed(Keyboard::Key::kp_right)
      || input.was_key_pressed(Keyboard::Key::down)
      || input.was_key_pressed(Keyboard::Key::kp_down))
    focus_next();
  
  m_preview.update(ticks, input, active_region);
  m_solve_button.update(ticks, input, active_region);
  m_close_button.update(ticks, input, active_region);
}

void AnalysisPanel::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_draw_color(background_color);
  renderer.fill_rect(m_boundary);
  renderer.set_draw_color(default_colors::black);
  renderer.draw_rect(m_boundary);

  Rect r;
  int x = m_boundary.x() + panel_spacing;
  int y = m_boundary.y() + panel_spacing;
  if (m_solver_running)
    r = renderer.draw_text(Point(x, y), m_font, "Status: solving");
  else if (m_solver.is_finished())
    r = renderer.draw_text(Point(x, y), m_font, "Status: solved");
  else
    r = renderer.draw_text(Point(x, y), m_font, "Status: ready");
  y += r.height() + text_spacing;

  std::string time_str = time_to_string(m_run_time, true);
  r = renderer.draw_text(Point(x, y), m_font, "Time: " + time_str);
  y += r.height() + text_spacing;

  std::string unique_str = "Unique solution: ";
  if (!m_solver.is_finished())
    unique_str += "?";
  else if (m_solver.num_solutions() == 1)
    unique_str += "Yes";
  else
    unique_str += "No";
  r = renderer.draw_text(Point(x, y), m_font, unique_str);
  y += r.height() + text_spacing;

  std::string lsolvable_str = "Line solvable: ";
  if (!m_solver.is_finished())
    lsolvable_str += "?";
  else if (m_solver.is_line_solvable())
    lsolvable_str += "Yes";
  else
    lsolvable_str += "No";
  r = renderer.draw_text(Point(x, y), m_font, lsolvable_str);
  y += r.height() + text_spacing;

  r = renderer.draw_text(Point(x, y), m_font, "Difficulty: ?");
  y += r.height() + text_spacing;

  std::string sol_str;
  if (m_solver.is_finished() && m_solver.was_contradiction_found())
    sol_str = "No solution";
  else if (m_solver.num_solutions() == 1)
    sol_str = "Found 1 solution";
  else if (m_solver.num_solutions() > 1)
    sol_str = "Found " + std::to_string(m_solver.num_solutions())
      + " solutions";
  if (!sol_str.empty()) {
    r = renderer.draw_text(Point(x, y), m_font, sol_str);
    y += r.height() + text_spacing;
  }

  std::string guess_str;
  if (m_solver.num_guesses() == 1)
    guess_str = "Made 1 guess";
  else if (m_solver.num_guesses() > 1)
    guess_str = "Made " + std::to_string(m_solver.num_guesses())
      + " guesses";
  if (!guess_str.empty()) {
    r = renderer.draw_text(Point(x, y), m_font, guess_str);
    y += r.height() + text_spacing;
  }

  m_preview.draw(renderer, region);
  m_solve_button.draw(renderer, region);
  m_close_button.draw(renderer, region);
}

void AnalysisPanel::move(int x, int y)
{
  int old_x = m_boundary.x(), old_y = m_boundary.y();
  UIPanel::move(x, y);
  m_preview.scroll(x - old_x, y - old_y);
  m_solve_button.scroll(x - old_x, y - old_y);
  m_close_button.scroll(x - old_x, y - old_y);
}

void AnalysisPanel::setup_buttons()
{
  m_solve_button = Button(m_font, "Solve");
  m_solve_button.resize(button_width, m_solve_button.boundary().height());
  m_solve_button.register_callback([this]() {
      if (!m_solver_running && !m_puzzle.is_solved())
        m_solver_running = true; });
  m_close_button = Button(m_font, "Close");
  m_close_button.resize(button_width, m_close_button.boundary().height());
  m_preview.attach_puzzle(m_puzzle);
}

void AnalysisPanel::calc_size()
{
  int width = 2 * panel_spacing;
  int height = panel_spacing;

  int text_wd, text_ht;
  m_font.text_size("Status: ready", &text_wd, &text_ht);
  width = std::max(width, text_wd + 2 * panel_spacing);
  height += text_ht + text_spacing;

  m_font.text_size("Time: 00:00.0", &text_wd, &text_ht);
  width = std::max(width, text_wd + 2 * panel_spacing);
  height += text_ht + text_spacing;
  
  m_font.text_size("Unique solution: Yes", &text_wd, &text_ht);
  width = std::max(width, text_wd + 2 * panel_spacing);
  height += text_ht + text_spacing;

  m_font.text_size("Line solvable: Yes", &text_wd, &text_ht);
  width = std::max(width, text_wd + 2 * panel_spacing);
  height += text_ht + text_spacing;

  m_font.text_size("Difficulty: moderate", &text_wd, &text_ht);
  width = std::max(width, text_wd + 2 * panel_spacing);
  height += text_ht + text_spacing;

  m_font.text_size("Found m solutions", &text_wd, &text_ht);
  width = std::max(width, text_wd + 2 * panel_spacing);
  height += text_ht + panel_spacing;
  
  m_font.text_size("Made n guesses", &text_wd, &text_ht);
  width = std::max(width, text_wd + 2 * panel_spacing);
  height += text_ht + panel_spacing;

  int preview_width = preview_size;
  int preview_height = preview_width
    * m_puzzle.height() / m_puzzle.width();
  if (preview_height > preview_size)
    preview_height = preview_size;
  m_preview.move(m_boundary.x() + width, m_boundary.y() + panel_spacing);
  m_preview.resize(preview_width, preview_height);
  width += preview_width + panel_spacing;
  height = std::max(height, preview_height + 2 * panel_spacing);

  int button_height = m_close_button.boundary().height();
  int x = m_boundary.x() + width / 2
    - (button_width * 2 + panel_spacing) / 2;
  int y = m_boundary.y() + height;
  m_solve_button.move(x, y);
  m_close_button.move(x + button_width + panel_spacing, y);
  height += button_height + panel_spacing;

  resize(width, height);
}

void AnalysisPanel::focus_prev()
{
  if (m_solve_button.has_focus()) {
    m_solve_button.remove_focus();
    m_close_button.give_focus();
  } else if (m_close_button.has_focus()) {
    m_close_button.remove_focus();
    m_solve_button.give_focus();
  } else {
    m_close_button.give_focus();
  }
}

void AnalysisPanel::focus_next()
{
  if (m_solve_button.has_focus()) {
    m_solve_button.remove_focus();
    m_close_button.give_focus();
  } else if (m_close_button.has_focus()) {
    m_close_button.remove_focus();
    m_solve_button.give_focus();
  } else {
    m_solve_button.give_focus();
  }
}
