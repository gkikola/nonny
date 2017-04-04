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

#include "puzzle/puzzle_grid.hpp"

PuzzleCell& PuzzleGrid::at(unsigned x, unsigned y)
{
  //call const version and convert back
  return const_cast<PuzzleCell&>
    (const_cast<const PuzzleGrid*>(this)->at(x, y));
}

const PuzzleCell& PuzzleGrid::at(unsigned x, unsigned y) const
{
  decltype(m_grid.size()) pos = y * m_width + x;
  
  if (pos > m_grid.size())
    throw std::out_of_range("attempted to access invalid puzzle cell");

  return m_grid[pos];
}
