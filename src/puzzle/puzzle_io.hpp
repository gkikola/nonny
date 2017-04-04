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

#ifndef NONNY_PUZZLE_IO_HPP
#define NONNY_PUZZLE_IO_HPP

#include <iosfwd>

class Puzzle;

// Read or write puzzles in the .non format
std::ostream& write_non(std::ostream& os, const Puzzle& puzzle);
std::istream& read_non(std::istream& is, Puzzle& puzzle);

#endif
