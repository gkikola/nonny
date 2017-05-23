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

PuzzleLine Puzzle::get_row(unsigned index)
{
  return PuzzleLine(*this, index, LineType::row);
}

ConstPuzzleLine Puzzle::get_row(unsigned index) const
{
  return ConstPuzzleLine(*this, index, LineType::row);
}

PuzzleLine Puzzle::get_col(unsigned index)
{
  return PuzzleLine(*this, index, LineType::column);
}

ConstPuzzleLine Puzzle::get_col(unsigned index) const
{
  return ConstPuzzleLine(*this, index, LineType::column);
}

void Puzzle::mark_cell(unsigned col, unsigned row, const Color& color)
{
  auto& cell = m_grid.at(col, row);
  cell.state = PuzzleCell::State::filled;
  cell.color = color;

  m_rows_changed.insert(row);
  m_cols_changed.insert(col);
}

void Puzzle::clear_cell(unsigned col, unsigned row)
{
  m_grid.at(col, row).state = PuzzleCell::State::blank;

  m_rows_changed.insert(row);
  m_cols_changed.insert(col);
}

void Puzzle::cross_out_cell(unsigned col, unsigned row)
{
  m_grid.at(col, row).state = PuzzleCell::State::crossed_out;

  m_rows_changed.insert(row);
  m_cols_changed.insert(col);
}

void Puzzle::clear_all_cells()
{
  m_grid = PuzzleGrid(width(), height());

  for (unsigned i = 0; i < m_grid.width(); ++i)
    m_cols_changed.insert(i);
  for (unsigned j = 0; j < m_grid.height(); ++j)
    m_rows_changed.insert(j);
}

ConstPuzzleLine Puzzle::operator[](unsigned col) const
{
  return ConstPuzzleLine(*this, col, LineType::column);
}

bool Puzzle::is_solved() const
{
  for (unsigned i = 0; i < m_grid.width(); ++i) {
    if (!is_col_solved(i))
      return false;
  }
  for (unsigned j = 0; j < m_grid.height(); ++j) {
    if (!is_row_solved(j))
      return false;
  }
  return true;
}

bool Puzzle::is_row_solved(unsigned row) const
{
  return get_row(row).is_solved();
}

bool Puzzle::is_col_solved(unsigned col) const
{
  return get_col(col).is_solved();
}

void Puzzle::update_clues(bool edit_mode)
{
  while (!m_rows_changed.empty()) {
    auto it = m_rows_changed.begin();
    update_line(*it, LineType::row, edit_mode);
    m_rows_changed.erase(it);
  }

  while (!m_cols_changed.empty()) {
    auto it = m_cols_changed.begin();
    update_line(*it, LineType::column, edit_mode);
    m_cols_changed.erase(it);
  }
}

Puzzle::ClueSequence& Puzzle::line_clues(unsigned index, LineType type)
{
  if (type == LineType::row)
    return m_row_clues[index];
  else
    return m_col_clues[index];
}

void Puzzle::update_line(unsigned index, LineType type, bool edit_mode)
{
  PuzzleLine line(*this, index, type);
  ClueSequence& clues = line_clues(index, type);

  if (edit_mode) {
    clues.clear();
    
    unsigned cell_pos = 0;
    while (cell_pos < line.size()) {
      while (cell_pos < line.size()
             && line[cell_pos].state != PuzzleCell::State::filled)
        ++cell_pos;

      unsigned count = 0;
      Color color;
      if (cell_pos < line.size())
        color = line[cell_pos].color;
      while (cell_pos < line.size()
             && line[cell_pos].state == PuzzleCell::State::filled
             && line[cell_pos].color == color) {
        ++count;
        ++cell_pos;
      }

      if (count > 0) {
        PuzzleClue clue;
        clue.value = count;
        clue.color = color;
        clue.state = PuzzleClue::State::normal;
        clues.push_back(clue);
      }
    }

    if (clues.empty()) {
      PuzzleClue zero;
      clues.push_back(zero);
    }
  } else {
    
  }
}
