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
#include "puzzle/puzzle.hpp"
#include "puzzle/puzzle_grid.hpp"
#include "utility/utility.hpp"

void PuzzleProgress::store_progress(const Puzzle& puzzle, unsigned time,
                                    bool update_solution)
{
  if (update_solution) {
    m_completed = true;
    m_cur_time = 0;
    if (m_best_time == 0 || time < m_best_time)
      m_best_time = time;
  } else
    m_cur_time = time;

  //reset progress state and then store progress or solution as needed
  m_progress = PuzzleGrid();
  PuzzleGrid* grid = update_solution ? &m_solution : &m_progress;
  *grid = PuzzleGrid(puzzle.width(), puzzle.height());
  for (int y = 0; y != puzzle.height(); ++y) {
    for (int x = 0; x != puzzle.width(); ++x) {
      grid->at(x, y) = puzzle[x][y];
    }
  }
}

void PuzzleProgress::restore_progress(Puzzle& puzzle) const
{
  restore(puzzle, m_progress);
}

void PuzzleProgress::restore_solution(Puzzle& puzzle) const
{
  restore(puzzle, m_solution);
}

void PuzzleProgress::restore(Puzzle& puzzle, const PuzzleGrid& grid) const
{
  for (int y = 0; y != grid.height(); ++y) {
    for (int x = 0; x != grid.width(); ++x) {
      const PuzzleCell& cell = grid.at(x, y);
      if (cell.state == PuzzleCell::State::filled)
        puzzle.mark_cell(x, y, cell.color);
      else if (cell.state == PuzzleCell::State::crossed_out)
        puzzle.cross_out_cell(x, y);
      else
        puzzle.clear_cell(x, y);
    }
  }
}

std::ostream& operator<<(std::ostream& os, const PuzzleProgress& prog)
{
  os << "file \"" << unescape(prog.m_filename) << "\"\n"
     << "completed " << (prog.m_completed ? "yes" : "no") << "\n";
  
  os << "best_time \"";
  write_time(os, prog.m_best_time, true) << "\"\n";
  
  os << "time \"";
  write_time(os, prog.m_cur_time, true) << "\"\n";

  if (prog.m_completed)
    os << "solution\n" << prog.solution() << "\n";

  os << "progress\n" << prog.state() << "\n";
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
    else if (p.first == "solution")
      is >> prog.m_solution;
    else if (p.first == "progress")
      is >> prog.m_progress;
  }
  return is;
}
