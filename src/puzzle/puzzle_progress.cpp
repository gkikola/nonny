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

#include "puzzle/puzzle_progress.hpp"

#include <iomanip>
#include <iostream>
#include "utility/utility.hpp"

std::ostream& operator<<(std::ostream& os, const PuzzleProgress& prog)
{
  os << "file \"" << prog.m_filename << "\"\n"
     << "completed " << (prog.m_completed ? "yes" : "no") << "\n";
  
  os << "best_time \"";
  write_time(os, prog.m_best_time) << "\"\n";

  os << "time \"";
  write_time(os, prog.m_cur_time) << "\"\n";
  return os;
}

std::istream& operator>>(std::istream& is, PuzzleProgress& prog)
{
  std::string line;
  while (std::getline(is, line)) {
    auto p = parse_property(line);
    if (p.first == "file")
      prog.m_filename = p.second;
    else if (p.first == "completed") {
      if (p.second == "yes")
        prog.m_completed = true;
      else
        prog.m_completed = false;
    } else if (p.first == "best_time")
      prog.m_best_time = string_to_time(p.second);
    else if (p.first == "time")
      prog.m_cur_time = string_to_time(p.second);
  }
  return is;
}
