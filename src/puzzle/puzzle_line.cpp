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

#include "puzzle/puzzle_line.hpp"

#include "puzzle/puzzle.hpp"

unsigned PuzzleLine::size() const
{
  if (m_type == LineType::row)
    return m_puzzle.width();
  else
    return m_puzzle.height();
}

const PuzzleCell& PuzzleLine::operator[](unsigned index) const
{
  return m_puzzle.at(col(index), row(index));
}

const PuzzleCell& PuzzleLine::at(unsigned index) const
{
  return m_puzzle.at(col(index), row(index));
}

void PuzzleLine::mark_cell(unsigned index, const Color& color)
{
  m_puzzle.mark_cell(col(index), row(index), color);
}

void PuzzleLine::clear_cell(unsigned index)
{
  m_puzzle.clear_cell(col(index), row(index));
}

void PuzzleLine::cross_out_cell(unsigned index)
{
  m_puzzle.cross_out_cell(col(index), row(index));
}

const PuzzleLine::ClueSequence& PuzzleLine::clues() const
{
  if (m_type == LineType::row)
    return m_puzzle.row_clues(m_line);
  else
    return m_puzzle.col_clues(m_line);
}

bool PuzzleLine::is_solved() const
{
  return ConstPuzzleLine(m_puzzle, m_line, m_type).is_solved();
}

unsigned ConstPuzzleLine::size() const
{
  if (m_type == LineType::row)
    return m_puzzle.width();
  else
    return m_puzzle.height();
}

const PuzzleCell& ConstPuzzleLine::at(unsigned index) const
{
  if (m_type == LineType::row)
    return m_puzzle.at(index, m_line);
  else
    return m_puzzle.at(m_line, index);
}

const ConstPuzzleLine::ClueSequence& ConstPuzzleLine::clues() const
{
  if (m_type == LineType::row)
    return m_puzzle.row_clues(m_line);
  else
    return m_puzzle.col_clues(m_line);
}

bool ConstPuzzleLine::is_solved() const
{
  //if only clue is 0, make sure line is empty
  if (clues().size() == 1 && clues()[0].value == 0) {
    for (unsigned i = 0; i < size(); ++i) {
      if (at(i).state == PuzzleCell::State::filled)
        return false;
    }
    return true;
  }

  unsigned pos = 0;
  for (auto clue : clues()) {
    while (pos < size()
           && at(pos).state != PuzzleCell::State::filled)
      ++pos;
    if (pos >= size())
      return false;

    unsigned count = 0;
    while (pos < size()
           && at(pos).state == PuzzleCell::State::filled
           && at(pos).color == clue.color) {
      ++count;
      ++pos;
    }
    if (count != clue.value)
      return false;
  }

  return true;
}
