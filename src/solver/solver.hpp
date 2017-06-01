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

#ifndef NONNY_SOLVER_HPP
#define NONNY_SOLVER_HPP

#include <set>
#include <stack>
#include <vector>
#include "puzzle/puzzle.hpp"
#include "puzzle/puzzle_cell.hpp"

/*
 * Takes a reference to a puzzle and, when invoked, solves that
 * puzzle.
 */
class Solver {
public:
  Solver(Puzzle& puzzle);

  // Execute one step in solving the puzzle, returns true if solved
  bool step();
  
  // Solve the puzzle
  void operator()();

  inline bool is_finished() const;
  bool was_contradiction_found() const { return m_inconsistent; }
  
private:
  // Returns false on contradiction
  bool solve_line(PuzzleLine& line, bool complete = false);
  Puzzle& m_puzzle;
  std::vector<PuzzleCell> m_solved_line;

  std::set<int> m_rows_to_check;
  std::set<int> m_cols_to_check;
  std::set<int> m_rows_solved;
  std::set<int> m_cols_solved;
  bool m_need_guess = false;
  bool m_inconsistent = false;
  bool m_use_complete = false;
};


/* implementation */

inline bool Solver::is_finished() const
{
  return static_cast<int>(m_rows_solved.size()) == m_puzzle.height()
    && static_cast<int>(m_cols_solved.size()) == m_puzzle.width();
}

#endif
