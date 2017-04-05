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

#include "puzzle/puzzle_io.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include "color/color_palette.hpp"
#include "nonny/utility.hpp"
#include "puzzle/puzzle.hpp"

// Write a puzzle's row rules or column rules to a stream
std::ostream&
write_clues(std::ostream& os,
            const std::vector<std::vector<PuzzleClue>>& clues)
{
  for (const auto& seq : clues) {
    bool first = true;
    for (const auto& clue : seq) {
      if (!first)
        os << ", ";
      os << clue.value;
      first = false;
    }
    os << "\n";
  }
}

std::ostream& write_non(std::ostream& os, const Puzzle& puzzle)
{
  if (!puzzle.m_info.title.empty())
    os << "title \"" << puzzle.m_info.title << "\"\n";
  if (!puzzle.m_info.author.empty())
    os << "author \"" << puzzle.m_info.author << "\"\n";
  if (!puzzle.m_info.copyright.empty())
    os << "copyright \"" << puzzle.m_info.copyright << "\"\n";

  os << "width " << puzzle.m_grid.width() << "\n"
     << "height " << puzzle.m_grid.height() << "\n";

  os << "rows\n";
  write_clues(os, puzzle.m_row_clues) << "\n";
  os << "columns\n";
  write_clues(os, puzzle.m_col_clues);
  return os;
}

/*
 * Interprets a line representing a set of clue numbers and their
 * associated colors. The line should contain a comma-separated list
 * of clue numbers with optional color specifiers (a color name
 * followed by an optional colon). The palette parameter should
 * contain all the colors that have been read from the file so far.
 *
 * An typical line of clues might look like
 * 
 *   blue: 3, 1, 2  red: 4  blue: 1, 1  black: 3
 *
 * A color specifier affects all remaining clues up until the next
 * specifier or until the end of the line. The default color at the
 * start of each line is "default" which is usually an alias for
 * black.
 */
std::vector<PuzzleClue>
parse_clue_line(const std::string& line, const ColorPalette& palette)
{
  std::vector<PuzzleClue> result;
  std::vector<std::string> clue_strs;
  split(line, std::back_inserter(clue_strs), ",: \t\v\f");

  Color color = palette["default"];
  PuzzleClue clue;
  for (auto& s : clue_strs) {
    if (!s.empty() && is_digit(s[0])) { //read clue number
      std::size_t chars_read = 0;
      clue.value = str_to_uint(s, chars_read);
      clue.color = color;
      result.push_back(clue);
      
      s.erase(0, chars_read); //in case there's a color trailing the number
    }

    if (!s.empty() && is_alpha(s[0])) { //read color
      color = palette[s];
    }
  }
}

std::istream& read_non(std::istream& is, Puzzle& puzzle)
{
  enum class ReadType { property, row, col, solution, colorized_solution };

  //read into temporary and then do a move if no errors occurred
  Puzzle result;
  unsigned width = 0, height = 0;
  ColorPalette palette;
  ReadType type = ReadType::property;
  std::string line;
  
  while (std::getline(is, line)) {
    auto prop_arg_pair = parse_property(line);
    const std::string& property = prop_arg_pair.first;
    const std::string& argument = prop_arg_pair.second;

    try {
      if (property == "width")
        width = str_to_uint(argument);
      else if (property == "height")
        height = str_to_uint(argument);
      else if (property == "rows")
        type = ReadType::row;
      else if (property == "columns")
        type = ReadType::col;
      else if (property == "title")
        result.m_info.title = argument;
      else if (property == "author" || property == "by")
        result.m_info.author = argument;
      else if (property == "copyright")
        result.m_info.copyright = argument;
    } catch (const std::logic_error&) { } //ignore formatting errors
  }

  result.m_grid = PuzzleGrid(width, height);
  
  puzzle = std::move(result);
  return is;
}
