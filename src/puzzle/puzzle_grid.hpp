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

#ifndef NONNY_PUZZLE_GRID_HPP
#define NONNY_PUZZLE_GRID_HPP

#include <iosfwd>
#include <stdexcept>
#include <vector>
#include "puzzle/puzzle_cell.hpp"

class ColorPalette;

/*
 * Represents a grid of puzzle cells.
 */
class PuzzleGrid {
  friend std::istream& read_grid(std::istream& is, PuzzleGrid& grid,
                                 const ColorPalette& palette);
public:
  PuzzleGrid() : m_width(0) { }
  PuzzleGrid(const PuzzleGrid&) = default;
  PuzzleGrid(PuzzleGrid&&) = default;

  PuzzleGrid(int width, int height)
    : m_grid(width * height), m_width(width) { }

  int width() const { return m_width; }
  inline int height() const;

  PuzzleCell& at(int x, int y);
  const PuzzleCell& at(int x, int y) const;

  PuzzleGrid& operator=(const PuzzleGrid&) & = default;
  PuzzleGrid& operator=(PuzzleGrid&&) & = default;
private:
  std::vector<PuzzleCell> m_grid;
  int m_width = 0;
};

std::ostream& operator<<(std::ostream& os, const PuzzleGrid& grid);
std::istream& operator>>(std::istream& is, PuzzleGrid& grid);

std::ostream& write_grid(std::ostream& os, const PuzzleGrid& grid,
                         const ColorPalette& palette);
std::istream& read_grid(std::istream& is, PuzzleGrid& grid,
                        const ColorPalette& palette);

/* implementation */

inline int PuzzleGrid::height() const
{
  if (m_width)
    return m_grid.size() / m_width;
  else
    return 0;
}

#endif
