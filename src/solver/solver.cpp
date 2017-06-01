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

#include "solver/solver.hpp"

#include <algorithm>
#include "solver/line_solver.hpp"

Solver::Solver(Puzzle& puzzle)
  : m_puzzle(puzzle)
{
  for (int i = 0; i < m_puzzle.width(); ++i)
    m_cols_to_check.insert(i);
  for (int j = 0; j < m_puzzle.height(); ++j)
    m_rows_to_check.insert(j);
}

bool Solver::step()
{
  if (m_inconsistent)
    return false;

  while (!m_rows_to_check.empty()) {
    int row = *m_rows_to_check.begin();
    PuzzleLine line = m_puzzle.get_row(row);
    if (!solve_line(line, m_complete)) {
      m_inconsistent = true;
      return false;
    }
  }
  while (!m_cols_to_check.empty()) {
    int col = *m_cols_to_check.begin();
    PuzzleLine line = m_puzzle.get_col(col);
    if (!solve_line(line, m_complete)) {
      m_inconsistent = true;
      return false;
    }
  }

  if (!m_complete && m_rows_to_check.empty() && m_cols_to_check.empty()) {
    m_complete = true;
    for (int i = 0; i < m_puzzle.width(); ++i)
      m_cols_to_check.insert(i);
    for (int j = 0; j < m_puzzle.height(); ++j)
      m_rows_to_check.insert(j);
  }
  
  return false;
}

void Solver::operator()()
{
  int count = 0;
  while (count < 32) {
    for (int row = 0; row < m_puzzle.height(); ++row) {
      PuzzleLine line = m_puzzle.get_row(row);
      LineSolver solver(line);
      solver();
    }
    for (int col = 0; col < m_puzzle.width(); ++col) {
      PuzzleLine line = m_puzzle.get_col(col);
      LineSolver solver(line);
      solver();
    }

    ++count;
  }
}

bool Solver::solve_line(PuzzleLine& line, bool complete)
{
  LineSolver solver(line);
  m_solved_line.clear();
  if (complete) {
    if (!solver.solve_complete(m_solved_line))
      return false;
  } else {
    if (!solver.solve_fast(m_solved_line))
      return false;
  }

  for (int i = 0; i < line.size(); ++i) {
    if (line[i] != m_solved_line[i]
        && m_solved_line[i].state != PuzzleCell::State::blank) {
      if (line.type() == LineType::row)
        m_cols_to_check.insert(i);
      else
        m_rows_to_check.insert(i);

      switch (m_solved_line[i].state) {
      case PuzzleCell::State::filled:
        line.mark_cell(i, m_solved_line[i].color);
        break;
      case PuzzleCell::State::crossed_out:
        line.cross_out_cell(i);
        break;
      default:
        break;
      }
    }
  }

  if (line.type() == LineType::row)
    m_rows_to_check.erase(line.index());
  else
    m_cols_to_check.erase(line.index());
  
  return true;
}
