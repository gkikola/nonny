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

#include <stdexcept>
#include <vector>
#include "puzzle/puzzle_cell.hpp"

/*
 * Represents a grid of puzzle cells.
 */
class PuzzleGrid {
public:
  PuzzleGrid() : m_width(0) { }
  PuzzleGrid(const PuzzleGrid&) = default;
  PuzzleGrid(PuzzleGrid&&) = default;
  
  PuzzleGrid(unsigned width, unsigned height)
    : m_grid(width * height), m_width(width) { }

  unsigned width() const { return m_width; }
  unsigned height() const { return m_grid.size() / m_width; }

  PuzzleCell& at(unsigned x, unsigned y);
  const PuzzleCell& at(unsigned x, unsigned y) const;

  PuzzleGrid& operator=(const PuzzleGrid&) & = default;
  PuzzleGrid& operator=(PuzzleGrid&&) & = default;
private:
  std::vector<PuzzleCell> m_grid;
  unsigned m_width;
};

#endif
