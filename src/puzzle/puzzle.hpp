/* Nonny -- free nonogram puzzles.
   Copyright (C) 2017 Gregory Kikola.

   This file is part of Nonny.

   Nonny is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Nonny is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Nonny.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Written by Gregory Kikola <gkikola@gmail.com>. */

#ifndef NONNY_PUZZLE_HPP
#define NONNY_PUZZLE_HPP

#include <iosfwd>
#include <string>
#include <vector>
#include "puzzle/puzzle_clue.hpp"
#include "puzzle/puzzle_grid.hpp"
#include "puzzle/puzzle_io.hpp"

/*
 * Puzzle metadata
 */
struct PuzzleInfo {
  std::string title;
  std::string author;
  std::string copyright;
};

/*
 * Class that represents a nonogram puzzle.
 */
class Puzzle {
  friend std::ostream& write_non(std::ostream&, const Puzzle&);
  friend std::istream& read_non(std::istream&, Puzzle&);
public:
private:
  PuzzleGrid m_grid;
  std::vector<std::vector<PuzzleClue>> m_row_clues;
  std::vector<std::vector<PuzzleClue>> m_col_clues;

  PuzzleInfo m_info;
};

// Reads and writes puzzles in the .non format
inline std::ostream& operator<<(std::ostream& os, const Puzzle& puzzle);
inline std::istream& operator>>(std::istream& is, Puzzle& puzzle);


/* implementation */

inline std::ostream& operator<<(std::ostream& os, const Puzzle& puzzle)
{
  return write_non(os, puzzle);
}

inline std::istream& operator>>(std::istream& is, Puzzle& puzzle)
{
  return read_non(is, puzzle);
}

#endif
