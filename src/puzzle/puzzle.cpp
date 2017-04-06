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

#include "puzzle/puzzle.hpp"

Puzzle::Puzzle(unsigned width, unsigned height)
  : m_grid(width, height)
{
  PuzzleClue zero;
  
  for (unsigned y = 0; y < height; ++y) {
    m_row_clues.push_back(ClueSequence(1, zero)); // add 0 clue for each row
  }

  for (unsigned x = 0; x < width; ++x) {
    m_col_clues.push_back(ClueSequence(1, zero)); // add 0 clue for each column
  }
}

void Puzzle::mark_cell(unsigned row, unsigned col, const Color& color)
{
  auto cell = m_grid.at(row, col);
  cell.state = PuzzleCell::State::filled;
  cell.color = color;
}

Puzzle::PuzzleRow Puzzle::operator[](unsigned row) const
{
  return PuzzleRow(*this, row);
}

const PuzzleCell& Puzzle::PuzzleRow::operator[](unsigned col) const
{
  return m_parent.m_grid.at(m_row, col);
}
