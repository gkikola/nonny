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

#include "puzzle/puzzle.hpp"
#include "solver/line_solver.hpp"

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
