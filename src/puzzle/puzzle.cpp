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

#include <algorithm>
#include <set>
#include "solver/line_solver.hpp"

Puzzle::Puzzle(int width, int height)
  : m_grid(width, height)
{
  refresh_all_cells();
  update(true);
}

Puzzle::Puzzle(int width, int height, ColorPalette palette)
  : m_grid(width, height), m_palette(palette)
{
  refresh_all_cells();
  update(true);
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

void Puzzle::shift_cells(int x, int y)
{
  int wd = width(), ht = height();
  PuzzleGrid copy(m_grid);
  m_grid = PuzzleGrid(wd, ht);

  for (int row = 0; row < ht; ++row) {
    for (int col = 0; col < wd; ++col) {
      int new_row = row + y;
      int new_col = col + x;

      if (new_row >= 0 && new_row < ht
          && new_col >= 0 && new_col < wd)
        m_grid.at(new_col, new_row) = copy.at(col, row);
    }
  }
  refresh_all_cells();
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
  int old_size = width() * height();
  m_grid = PuzzleGrid(state.m_width, state.m_height);
  int pos = 0;
  for (const auto& e : state.m_state) {
    for (int i = 0; i < e.count; ++i) {
      m_grid.at(pos % m_grid.width(), pos / m_grid.width()) = e.cell;
      ++pos;
    }
  }

  if (old_size != m_grid.width() * m_grid.height())
    handle_size_change();
  refresh_all_cells();
}

ConstPuzzleLine Puzzle::operator[](int col) const
{
  return ConstPuzzleLine(*this, col, LineType::column);
}

bool Puzzle::is_solved() const
{
  return static_cast<int>(m_rows_solved.size()) == m_grid.height()
    && static_cast<int>(m_cols_solved.size()) == m_grid.width();
}

bool Puzzle::is_row_solved(int row) const
{
  return m_rows_solved.count(row);
}

bool Puzzle::is_col_solved(int col) const
{
  return m_cols_solved.count(col);
}

bool Puzzle::is_clear() const
{
  for (int row = 0; row != height(); ++row)
    for (int col = 0; col != width(); ++col)
      if (m_grid.at(col, row).state != PuzzleCell::State::blank)
        return false;

  return true;
}

void Puzzle::update(bool edit_mode)
{
  //make sure clue entries exist
  if (m_row_clues.empty())
    m_row_clues = ClueContainer(height(), ClueSequence());
  if (m_col_clues.empty())
    m_col_clues = ClueContainer(width(), ClueSequence());

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

void Puzzle::reset_palette()
{
  m_palette = ColorPalette::default_palette();
}

void Puzzle::purge_unused_colors()
{
  std::set<Color> used_colors;
  for (auto clues : m_row_clues)
    for (auto clue : clues)
      used_colors.insert(clue.color);

  //erase the unused colors
  auto it = m_palette.begin();
  while (it != m_palette.end()) {
    if (it->name != "background"
        && used_colors.find(it->color) == used_colors.end()) {
      m_palette.remove(it->name);
      it = m_palette.begin();
    } else {
      ++it;
    }
  }
}

void Puzzle::set_property(const std::string& property,
                          const std::string& value)
{
  m_properties[property] = value;
}

void Puzzle::resize(int width, int height)
{
  PuzzleGrid copy(m_grid);
  m_grid = PuzzleGrid(width, height);

  int common_width = std::min(width, copy.width());
  int common_height = std::min(height, copy.height());
  for (int row = 0; row < common_height; ++row) {
    for (int col = 0; col < common_width; ++col) {
      m_grid.at(col, row) = copy.at(col, row);
    }
  }

  handle_size_change();
}

void Puzzle::handle_size_change()
{
  m_row_clues.clear();
  m_col_clues.clear();
  m_rows_changed.clear();
  m_cols_changed.clear();
  m_rows_solved.clear();
  m_cols_solved.clear();
  refresh_all_cells();
  update(true);
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
    bool solved = solver.update_clues(clues);
    if (type == LineType::row) {
      if (solved)
        m_rows_solved.insert(index);
      else
        m_rows_solved.erase(index);
    } else {
      if (solved)
        m_cols_solved.insert(index);
      else
        m_cols_solved.erase(index);
    }
  }
}
