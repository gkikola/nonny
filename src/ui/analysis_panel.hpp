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

#ifndef NONNY_ANALYSIS_PANEL_HPP
#define NONNY_ANALYSIS_PANEL_HPP

#include <functional>
#include "puzzle/puzzle.hpp"
#include "solver/solver.hpp"
#include "ui/button.hpp"
#include "ui/puzzle_preview.hpp"
#include "ui/ui_panel.hpp"

class Font;

/*
 * Displays solution information for a puzzle, such as number of
 * solutions, difficulty, and a solution preview.
 */
class AnalysisPanel : public UIPanel {
public:
  AnalysisPanel(const Font& font, const Puzzle& puzzle);

  typedef std::function<void()> Callback;
  void on_close(Callback fn) { m_close_button.register_callback(fn); }

  using UIPanel::update;
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

  void move(int x, int y) override;

private:
  void setup_buttons();
  void calc_size();
  void focus_prev();
  void focus_next();

  Puzzle m_puzzle;
  Solver m_solver;
  const Font& m_font;

  PuzzlePreview m_preview;
  Button m_solve_button;
  Button m_close_button;
  bool m_solver_running = false;
  bool m_done_solving = false;
  bool m_inconsistent = false;
  unsigned m_run_time = 0;
  unsigned m_sol_cycle_time = 0;
};

#endif
