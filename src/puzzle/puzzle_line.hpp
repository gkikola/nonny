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

#ifndef NONNY_PUZZLE_LINE_HPP
#define NONNY_PUZZLE_LINE_HPP

#include <vector>
#include "puzzle/puzzle_clue.hpp"

enum class LineType { row, column };

class Puzzle;
class PuzzleCell;

/*
 * Holds a reference to a line (row or column) of puzzle cells.
 */
class PuzzleLine {
public:
  typedef std::vector<PuzzleClue> ClueSequence;
  
  PuzzleLine(Puzzle& puzzle, unsigned line, LineType type)
    : m_puzzle(puzzle), m_line(line), m_type(type) { }

  // Which row or column does this line represent?
  unsigned index() const { return m_line; }

  // Is this a row or a column?
  LineType type() const { return m_type; }

  // Number of cells in the line
  unsigned size() const;

  // Access cells
  const PuzzleCell& operator[](unsigned index) const;
  const PuzzleCell& at(unsigned index) const;

  void mark_cell(unsigned index, const Color& color = Color());
  void clear_cell(unsigned index);
  void cross_out_cell(unsigned index);

  // Get clue sequence for the current line
  const ClueSequence& clues() const;

  // Is the line solved?
  bool is_solved() const;
private:
  inline unsigned row(unsigned index) const;
  inline unsigned col(unsigned index) const;
  
  Puzzle& m_puzzle;
  unsigned m_line;
  LineType m_type;
};


/*
 * Holds a const reference to a line of puzzle cells.
 */
class ConstPuzzleLine {
public:
  typedef std::vector<PuzzleClue> ClueSequence;

  ConstPuzzleLine(const Puzzle& puzzle, unsigned line, LineType type)
    : m_puzzle(puzzle), m_line(line), m_type(type) { }

  unsigned index() const { return m_line; }
  LineType type() const { return m_type; }
  unsigned size() const;

  const PuzzleCell& operator[](unsigned index) const { return at(index); }
  const PuzzleCell& at(unsigned index) const;

  const ClueSequence& clues() const;

  bool is_solved() const;
  
private:
  const Puzzle& m_puzzle;
  unsigned m_line;
  LineType m_type;
};


/* implementation */

inline unsigned PuzzleLine::row(unsigned index) const
{
  return m_type == LineType::row ? m_line : index;
}

inline unsigned PuzzleLine::col(unsigned index) const
{
  return m_type == LineType::column ? m_line : index;
}

#endif
