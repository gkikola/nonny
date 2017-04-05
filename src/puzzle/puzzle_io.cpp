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

enum class ClueType { row, col };

/*
 * Structure holding the information needed to build a puzzle.
 */
struct PuzzleBlueprint {
  unsigned width = 0;
  unsigned height = 0;
  Puzzle::ClueContainer row_clues;
  Puzzle::ClueContainer col_clues;
  ColorPalette palette;
  Puzzle::Properties properties;
};

namespace non_format {
  std::ostream& write(std::ostream& os, const Puzzle& puzzle);
  std::istream& read(std::istream& is, PuzzleBlueprint& blueprint);
}


std::ostream& write_puzzle(std::ostream& os, const Puzzle& puzzle)
{
  return non_format::write(os, puzzle);
}

std::istream& read_puzzle(std::istream& is, Puzzle& puzzle)
{
  PuzzleBlueprint blueprint;
  non_format::read(is, blueprint);
  
  puzzle = Puzzle();
  puzzle.m_grid = PuzzleGrid(blueprint.width, blueprint.height);
  puzzle.m_row_clues = std::move(blueprint.row_clues);
  puzzle.m_col_clues = std::move(blueprint.col_clues);
  puzzle.m_properties = std::move(blueprint.properties);
  return is;
}


/* .non format */

namespace non_format {

  /* .non output */
  std::ostream& write_clues(std::ostream& os, const Puzzle& puzzle,
                            const Puzzle::ClueContainer& clues);

  std::ostream&
  write_clues(std::ostream& os, const Puzzle::ClueContainer& clues)
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
    return os;
  }

  std::ostream& write(std::ostream& os, const Puzzle& puzzle)
  {
    //basic properties
    const std::string* val;

    if ( (val = puzzle.find_property("title")) )
      os << "title \"" << *val << "\"\n";
    if ( (val = puzzle.find_property("by")) )
      os << "by \"" << *val << "\"\n";
    if ( (val = puzzle.find_property("copyright")) )
      os << "copyright \"" << *val << "\"\n";
    if ( (val = puzzle.find_property("catalogue")) )
      os << "catalogue \"" << *val << "\"\n";

    os << "width " << puzzle.width() << "\n"
       << "height " << puzzle.height() << "\n";

    os << "\nrows\n";
    write_clues(os, puzzle.row_clues());
    os << "\ncolumns\n";
    write_clues(os, puzzle.col_clues());
    return os;
  }


  /* .non input */

  std::istream&
  read_clues(std::istream& is, PuzzleBlueprint& blueprint, ClueType type);

  Puzzle::ClueSequence
  parse_clue_line(const std::string& line, const ColorPalette& palette);

  /*
   * Read a series of lines containing clue numbers with color
   * specifiers.
   */
  std::istream&
  read_clues(std::istream& is, PuzzleBlueprint& blueprint, ClueType type)
  {
    Puzzle::ClueContainer* clues = nullptr;
    unsigned count = 0;
    if (type == ClueType::row) {
      clues = &blueprint.row_clues;
      count = blueprint.height;
    } else if (type == ClueType::col) {
      clues = &blueprint.col_clues;
      count = blueprint.width;
    }
  
    std::string line;
    while (count > 0) {
      if (!std::getline(is, line))
        throw InvalidPuzzleFile("number of clue lines does not match "
                                "puzzle dimensions");

      auto clue_seq
        = parse_clue_line(line, blueprint.palette);
    
      if (!clue_seq.empty()) {
        if (clues) clues->push_back(std::move(clue_seq));
        --count;
      }
    }
  
    return is;
  }

  /*
   * Interprets a line representing a set of clue numbers and their
   * associated colors in non format. The line should contain a
   * comma-separated list of clue numbers with optional color specifiers
   * (a color name followed by an optional colon). The palette parameter
   * should contain all the colors that have been read from the file so
   * far.
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
  Puzzle::ClueSequence
  parse_clue_line(const std::string& line, const ColorPalette& palette)
  {
    Puzzle::ClueSequence result;
    std::vector<std::string> clue_strs;
    split(line, std::back_inserter(clue_strs), ",: \t\v\f");

    Color color = palette["default"];
    PuzzleClue clue;
    for (auto& s : clue_strs) {
      if (!s.empty() && is_digit(s[0])) { //read clue number
        std::size_t chars_read = 0;
        clue.value = str_to_uint(s, &chars_read);
        clue.color = color;
        result.push_back(clue);
      
        s.erase(0, chars_read); //in case there's a color trailing the number
      }

      if (!s.empty() && is_alpha(s[0])) { //read color
        color = palette[s];
      }
    }

    return result;
  }

  std::istream&
  read(std::istream& is, PuzzleBlueprint& blueprint)
  {
    std::string line;
    while (std::getline(is, line)) {
      auto p = parse_property(line);
      std::string& property = p.first;
      std::string& argument = p.second;

      //"rows" or "columns" with an argument is an alias for width/height
      if (property == "rows" && !argument.empty())
        property = "width";
      else if (property == "columns" && !argument.empty())
        property = "height";

      //see if we're reading dimensions
      unsigned* dimension = nullptr;
      if (property == "width")
        dimension = &blueprint.width;
      else if (property == "height")
        dimension = &blueprint.height;

      if (dimension) {
        try {
          *dimension = str_to_uint(argument);
        } catch (const std::logic_error& e) {
          throw InvalidPuzzleFile("invalid puzzle " + property);
        }
      } else if (property == "rows" && argument.empty())
        read_clues(is, blueprint, ClueType::row);
      else if (property == "columns" && argument.empty())
        read_clues(is, blueprint, ClueType::col);
      else
        blueprint.properties[property] = argument;
    }
  
    return is;
  }

} //end namespace non_format
