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

#ifndef NONNY_PUZZLE_HPP
#define NONNY_PUZZLE_HPP

#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include "color/color.hpp"
#include "color/color_palette.hpp"
#include "puzzle/puzzle_clue.hpp"
#include "puzzle/puzzle_grid.hpp"
#include "puzzle/puzzle_io.hpp"

/*
 * Class that represents a nonogram puzzle.
 */
class Puzzle {
  friend std::ostream& write_puzzle(std::ostream&, const Puzzle&);
  friend std::istream& read_puzzle(std::istream&, Puzzle&);

public:
  typedef std::map<std::string, std::string> Properties;
  typedef std::vector<std::vector<PuzzleClue>> ClueContainer;
  typedef ClueContainer::value_type ClueSequence;
  
  Puzzle() = default; // blank 0x0 puzzle
  Puzzle(const Puzzle&) = default;
  Puzzle(Puzzle&&) = default;

  // Creates a blank puzzle of the given size
  Puzzle(unsigned width, unsigned height);

  unsigned width() const { return m_grid.width(); }
  unsigned height() const { return m_grid.height(); }

  class PuzzleCol;
  PuzzleCol operator[](unsigned col) const;

  void mark_cell(unsigned col, unsigned row, const Color& color = Color());
  inline void clear_cell(unsigned col, unsigned row);
  inline void cross_out_cell(unsigned col, unsigned row);

  bool is_solved() const;
  bool is_row_solved(unsigned row) const;
  bool is_col_solved(unsigned col) const;

  // Get color palette associated with this puzzle
  const ColorPalette& palette() const { return m_palette; }

  // Return pointer to p's property value or nullptr if p is not found
  inline const std::string* find_property(const std::string& p) const;
  
  // Throw exception if p is not found in property map
  const std::string&
  property(const std::string& p) const { return m_properties.at(p); }

  const ClueContainer& row_clues() const { return m_row_clues; }
  const ClueContainer& col_clues() const { return m_col_clues; }
  inline const ClueSequence& row_clues(unsigned row) const;
  inline const ClueSequence& col_clues(unsigned col) const;
  
  Puzzle& operator=(const Puzzle&) & = default;
  Puzzle& operator=(Puzzle&&) & = default;

private:
  PuzzleGrid m_grid;
  ClueContainer m_row_clues;
  ClueContainer m_col_clues;
  ColorPalette m_palette;
  Properties m_properties;
};

// Reads and writes puzzles in the .non format
inline std::ostream& operator<<(std::ostream& os, const Puzzle& puzzle);
inline std::istream& operator>>(std::istream& is, Puzzle& puzzle);

// Represents a puzzle row, used to allow two-dimensional subscripting
class Puzzle::PuzzleCol {
public:
  PuzzleCol(const Puzzle& parent, unsigned col)
    : m_parent(parent), m_col(col) { }

  const PuzzleCell& operator[](unsigned row) const;
private:
  const Puzzle& m_parent;
  unsigned m_col;
};


/* implementation */

inline void Puzzle::clear_cell(unsigned col, unsigned row)
{
  m_grid.at(col, row).state = PuzzleCell::State::blank;
}

inline void Puzzle::cross_out_cell(unsigned col, unsigned row)
{
  m_grid.at(col, row).state = PuzzleCell::State::crossed_out;
}

inline const std::string* Puzzle::find_property(const std::string& p) const
{
  auto it = m_properties.find(p);
  return (it == m_properties.end()) ? nullptr : &it->second;
}

inline const Puzzle::ClueSequence& Puzzle::row_clues(unsigned row) const
{
  return m_row_clues.at(row);
}

inline const Puzzle::ClueSequence& Puzzle::col_clues(unsigned col) const
{
  return m_col_clues.at(col);
}

inline std::ostream& operator<<(std::ostream& os, const Puzzle& puzzle)
{
  return write_puzzle(os, puzzle);
}

inline std::istream& operator>>(std::istream& is, Puzzle& puzzle)
{
  return read_puzzle(is, puzzle);
}

#endif
