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

#ifndef NONNY_PUZZLE_PROGRESS
#define NONNY_PUZZLE_PROGRESS

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <vector>
#include "puzzle/puzzle_grid.hpp"

class Puzzle;

class PuzzleProgress {
  friend std::ostream& operator<<(std::ostream&, const PuzzleProgress&);
  friend std::istream& operator>>(std::istream&, PuzzleProgress&);
public:
  PuzzleProgress() = default;
  PuzzleProgress(const PuzzleProgress&) = default;
  PuzzleProgress(PuzzleProgress&&) = default;

  PuzzleProgress(const std::string& filename)
    : m_filename(filename) { }

  PuzzleProgress& operator=(const PuzzleProgress&) & = default;
  PuzzleProgress& operator=(PuzzleProgress&&) & = default;

  void store_progress(const Puzzle& puzzle, unsigned time,
                      bool update_solution = false);

  std::string filename() const { return m_filename; }
  bool is_complete() const { return m_completed; }
  unsigned best_time() const { return m_best_time; }
  unsigned current_time() const { return m_cur_time; }
  
  const PuzzleGrid& state() const { return m_progress; }
  const PuzzleGrid& solution() const { return m_solution; }
  inline const PuzzleCell& state(unsigned row, unsigned col) const;
  inline const PuzzleCell& solution(unsigned row, unsigned col) const;
private:
  std::string m_filename;
  bool m_completed = false;
  unsigned m_best_time = 0;
  unsigned m_cur_time = 0;
  PuzzleGrid m_solution;
  PuzzleGrid m_progress;
};

std::ostream& operator<<(std::ostream&, const PuzzleProgress&);
std::istream& operator>>(std::istream&, PuzzleProgress&);


/* implementation */

inline const PuzzleCell&
PuzzleProgress::state(unsigned row, unsigned col) const
{
  return m_progress.at(row, col);
}

inline const PuzzleCell&
PuzzleProgress::solution(unsigned row, unsigned col) const
{
  return m_solution.at(row, col);
}

#endif
