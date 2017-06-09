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
#include "puzzle/compressed_state.hpp"
#include "puzzle/puzzle.hpp"
#include "puzzle/puzzle_cell.hpp"

/*
 * The state of the solver at a branch point.
 */
struct SolverState {
  CompressedState puzzle_state;
  int row = 0; //where the guess was made
  int col = 0;
};

/*
 * Takes a reference to a puzzle and, when invoked, solves that
 * puzzle.
 */
class Solver {
public:
  Solver(Puzzle& puzzle);

  // Execute one step in solving the puzzle, returns true if finished
  bool step();
  
  // Solve the whole puzzle, all at once
  void operator()();

  /*
   * Has the solver finished running? Even if the puzzle has been
   * solved the solver may continue to run in order to find additional
   * solutions
   */
  bool is_finished() const { return m_finished; }

  /*
   * If solver is finished, switch puzzle state to that of the
   * solution, or cycle to the next solution if more than one.
   */
  void cycle_solution();
  
  // How many solutions were found?
  int num_solutions() const { return m_solutions.size(); }

  // How many guesses did the solver have to make?
  int num_guesses() const { return m_num_guesses; }

  int search_depth() const { return m_max_depth; }
  
  // Can the puzzle be solved one line at a time?
  inline bool is_line_solvable() const;

  // Is there an inconsistency in the puzzle?
  bool was_contradiction_found() const { return m_inconsistent; }
  
private:
  // Choose a line to solve
  int select_row();
  int select_col();
  bool is_line_available();

  // Update solved line information
  void check_for_solved_lines();
  
  // Return to the most recent alternative guess state
  void backtrack();

  // Create a branch point and make a guess
  void guess();

  // Choose the best cell for guessing
  void choose_cell(int& x, int& y);

  /*
   * Find the score of the cell as a guess choice, lower is better.
   * Line score is slack plus twice the number of clues, cell score is
   * 3 * the smaller of row and column scores plus the larger of the
   * two scores, plus the number of unsolved neighbor cells.
   *
   * A score of -1 means the cell is invalid or already solved.
   */
  int cell_score(int x, int y);

  // Calculate slack in each line, used for guessing
  void calc_line_slack();
  
  // Returns false on contradiction
  bool solve_line(PuzzleLine& line, bool complete = false);

  // Store the solution in the solution list
  void record_solution();
  
  Puzzle& m_puzzle;
  std::vector<PuzzleCell> m_solved_line;

  // Solutions found and alternatives to consider
  std::vector<CompressedState> m_solutions;
  std::stack<SolverState> m_alternatives;
  std::vector<CompressedState>::iterator m_cur_solution;
  bool m_solution_selected = false;

  // Priority and solved lines
  std::vector<int> m_row_priority;
  std::vector<int> m_col_priority;
  std::set<int> m_rows_solved;
  std::set<int> m_cols_solved;

  // Slack in each line, used for choosing guesses
  std::vector<int> m_row_slack;
  std::vector<int> m_col_slack;

  // Used to guide the guesser
  int m_last_row_selected = 0;
  int m_last_col_selected = 0;

  bool m_finished = false; //are we done?
  int m_num_guesses = 0; //how many guesses have we made?
  int m_cur_depth = 0;
  int m_max_depth = 0;
  bool m_inconsistent = false; //is puzzle contradictory?
  bool m_use_complete = false; //use the complete rather than fast linesolver
  bool m_new_info_found = false; //did the line solver find new info?
};


/* implementation */

bool Solver::is_line_solvable() const
{
  return is_finished() && m_num_guesses == 0 && m_solutions.size() > 0;
}

#endif
