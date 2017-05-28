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

#include "solver/line_solver.hpp"

Puzzle::Puzzle(int width, int height)
  : m_grid(width, height)
{
  refresh_all_cells();
  update_clues(true);
}

Puzzle::Puzzle(int width, int height, ColorPalette palette)
  : m_grid(width, height), m_palette(palette)
{
  refresh_all_cells();
  update_clues(true);
}

PuzzleLine Puzzle::get_row(int index)
{
  return PuzzleLine(*this, index, LineType::row);
}

ConstPuzzleLine Puzzle::get_row(int index) const
{
  return ConstPuzzleLine(*this, index, LineType::row);
}

PuzzleLine Puzzle::get_col(int index)
{
  return PuzzleLine(*this, index, LineType::column);
}

ConstPuzzleLine Puzzle::get_col(int index) const
{
  return ConstPuzzleLine(*this, index, LineType::column);
}

void Puzzle::mark_cell(int col, int row, const Color& color)
{
  auto& cell = m_grid.at(col, row);
  cell.state = PuzzleCell::State::filled;
  cell.color = color;

  m_rows_changed.insert(row);
  m_cols_changed.insert(col);
}

void Puzzle::clear_cell(int col, int row)
{
  m_grid.at(col, row).state = PuzzleCell::State::blank;

  m_rows_changed.insert(row);
  m_cols_changed.insert(col);
}

void Puzzle::cross_out_cell(int col, int row)
{
  m_grid.at(col, row).state = PuzzleCell::State::crossed_out;

  m_rows_changed.insert(row);
  m_cols_changed.insert(col);
}

void Puzzle::clear_all_cells()
{
  m_grid = PuzzleGrid(width(), height());

  refresh_all_cells();
}

void Puzzle::refresh_all_cells()
{
  for (int i = 0; i < m_grid.width(); ++i)
    m_cols_changed.insert(i);
  for (int j = 0; j < m_grid.height(); ++j)
    m_rows_changed.insert(j);
}

void Puzzle::copy_state(CompressedState& state) const
{
  state.m_width = width();
  state.m_height = height();
  state.m_state.clear();

  int pos = 0;
  PuzzleCell prev_cell;
  while (pos < width() * height()) {
    const PuzzleCell& cur_cell = m_grid.at(pos % width(), pos / width());
    if (pos == 0 || prev_cell != cur_cell) {
      CompressedState::Entry e;
      e.cell = cur_cell;
      e.count = 0;
      state.m_state.push_back(e);
    }
    
    prev_cell = cur_cell;
    ++pos;
    ++state.m_state[state.m_state.size() - 1].count;
  }
}

void Puzzle::load_state(const CompressedState& state)
{
  m_grid = PuzzleGrid(state.m_width, state.m_height);
  int pos = 0;
  for (const auto& e : state.m_state) {
    for (int i = 0; i < e.count; ++i) {
      m_grid.at(pos % width(), pos / width()) = e.cell;
      ++pos;
    }
  }
  refresh_all_cells();
}

ConstPuzzleLine Puzzle::operator[](int col) const
{
  return ConstPuzzleLine(*this, col, LineType::column);
}

bool Puzzle::is_solved() const
{
  for (int i = 0; i < m_grid.width(); ++i) {
    if (!is_col_solved(i))
      return false;
  }
  for (int j = 0; j < m_grid.height(); ++j) {
    if (!is_row_solved(j))
      return false;
  }
  return true;
}

bool Puzzle::is_row_solved(int row) const
{
  return get_row(row).is_solved();
}

bool Puzzle::is_col_solved(int col) const
{
  return get_col(col).is_solved();
}

void Puzzle::update_clues(bool edit_mode)
{
  //make sure clue entries exist
  if (m_row_clues.empty())
    m_row_clues = ClueContainer(width(), ClueSequence());
  if (m_col_clues.empty())
    m_col_clues = ClueContainer(height(), ClueSequence());

  //update changed lines
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

Puzzle::ClueSequence& Puzzle::line_clues(int index, LineType type)
{
  if (type == LineType::row)
    return m_row_clues[index];
  else
    return m_col_clues[index];
}

void Puzzle::update_line(int index, LineType type, bool edit_mode)
{
  PuzzleLine line(*this, index, type);
  ClueSequence& clues = line_clues(index, type);

  if (edit_mode) {
    clues.clear();
    
    int cell_pos = 0;
    while (cell_pos < line.size()) {
      while (cell_pos < line.size()
             && line[cell_pos].state != PuzzleCell::State::filled)
        ++cell_pos;

      int count = 0;
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
    LineSolver solver(line);
    solver.update_clues(clues);
  }
}
