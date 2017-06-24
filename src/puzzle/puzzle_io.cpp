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
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include "color/color_palette.hpp"
#include "puzzle/puzzle.hpp"
#include "puzzle/puzzle_summary.hpp"
#include "utility/utility.hpp"

enum class ClueType { row, col };

/*
 * Structure holding the information needed to build a puzzle.
 */
struct PuzzleBlueprint {
  int width = 0;
  int height = 0;
  Puzzle::ClueContainer row_clues;
  Puzzle::ClueContainer col_clues;
  ColorPalette palette;
  Puzzle::Properties properties;
};

namespace non_format {
  std::ostream& write(std::ostream& os, const Puzzle& puzzle);
  std::istream& read(std::istream& is, PuzzleBlueprint& blueprint);
  std::istream& skim(std::istream& is, PuzzleSummary& summary);
}

namespace g_format {
  std::ostream& write(std::ostream& os, const Puzzle& puzzle);
  std::istream& read(std::istream& is, PuzzleBlueprint& blueprint);
  std::istream& skim(std::istream& is, PuzzleSummary& summary);
}

namespace mk_format {
  std::ostream& write(std::ostream& os, const Puzzle& puzzle);
  std::istream& read(std::istream& is, PuzzleBlueprint& blueprint);
  std::istream& skim(std::istream& is, PuzzleSummary& summary);
}

namespace nin_format {
  std::ostream& write(std::ostream& os, const Puzzle& puzzle);
  std::istream& read(std::istream& is, PuzzleBlueprint& blueprint);
  std::istream& skim(std::istream& is, PuzzleSummary& summary);
}

std::ostream& write_puzzle(std::ostream& os, Puzzle puzzle,
                           PuzzleFormat fmt)
{
  //just modifying local copy
  puzzle.purge_unused_colors();

  switch (fmt) {
  default:
  case PuzzleFormat::non:
    return non_format::write(os, puzzle);
  case PuzzleFormat::g:
    return g_format::write(os, puzzle);
  case PuzzleFormat::mk:
    return mk_format::write(os, puzzle);
  case PuzzleFormat::nin:
    return nin_format::write(os, puzzle);
  }
}

std::istream& read_puzzle(std::istream& is, Puzzle& puzzle,
                          PuzzleFormat fmt)
{
  PuzzleBlueprint blueprint;
  switch (fmt) {
  default:
  case PuzzleFormat::non:
    non_format::read(is, blueprint);
    break;
  case PuzzleFormat::g:
    g_format::read(is, blueprint);
    break;
  case PuzzleFormat::mk:
    mk_format::read(is, blueprint);
    break;
  case PuzzleFormat::nin:
    nin_format::read(is, blueprint);
    break;
  }

  puzzle = Puzzle();
  puzzle.m_grid = PuzzleGrid(blueprint.width, blueprint.height);
  puzzle.m_row_clues = std::move(blueprint.row_clues);
  puzzle.m_col_clues = std::move(blueprint.col_clues);
  puzzle.m_palette = std::move(blueprint.palette);
  puzzle.m_properties = std::move(blueprint.properties);

  puzzle.refresh_all_cells();
  return is;
}

std::istream& skim_puzzle(std::istream& is, PuzzleSummary& summary,
                          PuzzleFormat fmt)
{
  switch (fmt) {
  default:
  case PuzzleFormat::non:
    return non_format::skim(is, summary);
  case PuzzleFormat::g:
    return g_format::skim(is, summary);
  case PuzzleFormat::mk:
    return mk_format::skim(is, summary);
  case PuzzleFormat::nin:
    return nin_format::skim(is, summary);
  }
}


/* .non format */

namespace non_format {
  /* .non output */
  std::ostream& write_clues(std::ostream& os,
                            const Puzzle::ClueContainer& clues,
                            const ColorPalette& palette);
  std::ostream& write_colors(std::ostream& os, const ColorPalette& palette);

  /* .non input */
  std::istream&
  read_clues(std::istream& is, PuzzleBlueprint& blueprint, ClueType type);

  Puzzle::ClueSequence
  parse_clue_line(const std::string& line, const ColorPalette& palette);

  void parse_color(const std::string& args, ColorPalette& palette);
}

std::ostream&
non_format::write_clues(std::ostream& os, const Puzzle::ClueContainer& clues,
              const ColorPalette& palette)
{
  for (const auto& seq : clues) {
    Color cur_color;
    bool first = true;
    for (const auto& clue : seq) {
      if (!first)
        os << ", ";

      if (clue.color != cur_color) {
        cur_color = clue.color;
        auto color_entry = palette.find(clue.color);
        os << color_entry->name << ": ";
      }

      os << clue.value;
      first = false;
    }
    os << "\n";
  }
  return os;
}

std::ostream&
non_format::write_colors(std::ostream& os, const ColorPalette& palette)
{
  for (const auto& color_entry : palette) {
    if (color_entry.name != "background"
        || color_entry.color != default_colors::white) {
      os << "color " << color_entry.name << " "
         << color_entry.color;
      if (color_entry.symbol)
        os << " " << color_entry.symbol;
      os << "\n";
    }
  }
  return os;
}

std::ostream&
non_format::write(std::ostream& os, const Puzzle& puzzle)
{
  //basic properties
  const std::string* val;

  if ( (val = puzzle.find_property("title")) )
    os << "title \"" << *val << "\"\n";
  if ( (val = puzzle.find_property("by")) )
    os << "by \"" << *val << "\"\n";
  if ( (val = puzzle.find_property("collection")) )
    os << "collection \"" << *val << "\"\n";
  if ( (val = puzzle.find_property("id")) )
    os << "id \"" << *val << "\"\n";
  if ( (val = puzzle.find_property("copyright")) )
    os << "copyright \"" << *val << "\"\n";
  if ( (val = puzzle.find_property("catalogue")) )
    os << "catalogue \"" << *val << "\"\n";

  os << "width " << puzzle.width() << "\n"
     << "height " << puzzle.height() << "\n";

  //only write colors if they're different from defaults
  if (puzzle.palette() != ColorPalette())
    write_colors(os << "\n", puzzle.palette());

  os << "\nrows\n";
  write_clues(os, puzzle.row_clues(), puzzle.palette());
  os << "\ncolumns\n";
  write_clues(os, puzzle.col_clues(), puzzle.palette());
  return os;
}



/*
 * Read a series of lines containing clue numbers with color
 * specifiers.
 */
std::istream&
non_format::read_clues(std::istream& is,
                       PuzzleBlueprint& blueprint, ClueType type)
{
  Puzzle::ClueContainer* clues = nullptr;
  int count = 0;
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
      throw InvalidPuzzleFile("non_format::read_clues: "
                              "number of clue lines "
                              "does not match puzzle dimensions");

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
non_format::parse_clue_line(const std::string& line,
                            const ColorPalette& palette)
{
  Puzzle::ClueSequence result;
  std::vector<std::string> clue_strs;
  split(line, std::back_inserter(clue_strs), ",: \t\v\f");

  Color color = palette["black"];
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

/*
 * Reads a color entry from a string and stores it in the provided
 * palette. The format is
 *
 *   color_name hex_value [symbol]
 *
 * where symbol is an optional character to be used in reading or
 * generating puzzle solutions.
 */
void
non_format::parse_color(const std::string& args, ColorPalette& palette)
{
  std::string name;
  Color value;
  char symbol;
  std::istringstream ss(args);

  ss >> name >> value >> symbol;
  palette.add(value, name, symbol);
}

std::istream&
non_format::read(std::istream& is, PuzzleBlueprint& blueprint)
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
    int* dimension = nullptr;
    if (property == "width")
      dimension = &blueprint.width;
    else if (property == "height")
      dimension = &blueprint.height;

    if (dimension) {
      try {
        *dimension = str_to_uint(argument);
      } catch (const std::logic_error&) {
        throw InvalidPuzzleFile("non_format::read: invalid puzzle "
                                + property);
      }
    } else if (property == "rows" && argument.empty())
      read_clues(is, blueprint, ClueType::row);
    else if (property == "columns" && argument.empty())
      read_clues(is, blueprint, ClueType::col);
    else if (property == "color")
      parse_color(argument, blueprint.palette);
    else
      blueprint.properties[property] = argument;
  }

  return is;
}

std::istream&
non_format::skim(std::istream& is, PuzzleSummary& summary)
{
  try {
    std::string line;
    while (std::getline(is, line)) {
      auto p = parse_property(line);
      if (p.first == "title")
        summary.title = p.second;
      else if (p.first == "by")
        summary.author = p.second;
      else if (p.first == "width")
        summary.width = str_to_uint(p.second);
      else if (p.first == "height")
        summary.height = str_to_uint(p.second);
      else if (p.first == "collection")
        summary.collection = p.second;
      else if (p.first == "id")
        summary.id = p.second;
      else if (p.first == "rows" && !p.second.empty())
        summary.width = str_to_uint(p.second);
      else if (p.first == "columns" && !p.second.empty())
        summary.height = str_to_uint(p.second);
    }
  } catch (const std::exception&) { } //ignore file errors

  return is;
}


namespace g_format {
  /* .g output */
  void generate_short_color_names(const ColorPalette& palette,
                                  std::map<std::string, char>& short_names);
  std::ostream& write_colors(std::ostream& os, const ColorPalette& palette,
                             const std::map<std::string, char>& short_names);
  std::ostream& write_clues(std::ostream& os,
                            const Puzzle::ClueContainer& clues,
                            const ColorPalette& palette,
                            const std::map<std::string, char>& short_names);

  /* .g input */
  std::istream& read_front_matter(std::istream& is,
                                  PuzzleBlueprint& blueprint);
  std::istream& read_front_matter(std::istream& is,
                                  PuzzleSummary& summary);
  std::istream& read_colors(std::istream& is,
                            PuzzleBlueprint& blueprint,
                            std::map<char, std::string>& color_names);
  std::istream& read_clues(std::istream& is,
                           PuzzleBlueprint& blueprint,
                           const std::map<char, std::string>& color_names,
                           ClueType type);

}

std::ostream&
g_format::write(std::ostream& os, const Puzzle& puzzle)
{
  const std::string* cat = puzzle.find_property("catalogue");
  const std::string* title = puzzle.find_property("title");
  if (cat && title)
    os << *cat << ": " << *title << "\n";
  else if (cat)
    os << *cat << "\n";
  else if (title)
    os << *title << "\n";

  const std::string* author = puzzle.find_property("by");
  if (author)
    os << "by " << *author << "\n";

  const std::string* copy = puzzle.find_property("copyright");
  if (copy)
    os << *copy << "\n";

  std::map<std::string, char> short_names;
  generate_short_color_names(puzzle.palette(), short_names);

  os << "#d\n";
  write_colors(os, puzzle.palette(), short_names);

  os << ": rows\n";
  write_clues(os, puzzle.row_clues(), puzzle.palette(), short_names);
  os << ": columns\n";
  write_clues(os, puzzle.col_clues(), puzzle.palette(), short_names);

  return os;
}

void
g_format::generate_short_color_names(const ColorPalette& palette,
                                     std::map<std::string, char>& short_names)
{
  std::string used = "";
  char next = 'a';
  for (const auto& c : palette) {
    const std::string& name = c.name;
    auto pos = name.find_first_not_of(used);
    char sn = next;
    if (pos != std::string::npos)
      sn = name[pos];
    else {
      while (used.find(next) != std::string::npos && next < 'z')
        ++next;
      sn = next;
    }

    short_names[name] = sn;
    used.push_back(sn);
  }
}

std::ostream&
g_format::write_colors(std::ostream& os, const ColorPalette& palette,
                       const std::map<std::string, char>& short_names)
{
  os << "   0:   #" << palette["background"] << "   "
     << "background\n";
  for (auto& c : palette) {
    if (c.name != "background") {
      os << "   " << short_names.at(c.name) << ":" << c.symbol
         << "  #" << c.color << "   " << c.name << "\n";
    }
  }
  return os;
}

std::ostream&
g_format::write_clues(std::ostream& os,
                      const Puzzle::ClueContainer& clues,
                      const ColorPalette& palette,
                      const std::map<std::string, char>& short_names)
{
  for (auto& cseq : clues) {
    bool first = true;
    for (auto& clue : cseq) {
      if (clue.value != 0) {
        if (!first)
          os << " ";
        os << clue.value;
        std::string name = palette.find(clue.color)->name;
        os << short_names.at(name);
        first = false;
      }
    }
    os << "\n";
  }
  return os;
}

std::istream&
g_format::read(std::istream& is, PuzzleBlueprint& blueprint)
{
  read_front_matter(is, blueprint);

  //look for color declaration
  std::map<char, std::string> color_names;
  std::string line;
  std::getline(is, line);
  if (line.size() > 1 && line[0] == '#' && to_lower(line[1]) == 'd')
    read_colors(is, blueprint, color_names);

  read_clues(is, blueprint, color_names, ClueType::row);
  read_clues(is, blueprint, color_names, ClueType::col);
  return is;
}

std::istream&
g_format::read_front_matter(std::istream& is,
                            PuzzleBlueprint& blueprint)
{
  std::string line;

  //first line should be title and catalogue
  if (is.peek() != ':' && is.peek() != '#') {
    std::getline(is, line);
    auto colon_pos = line.find(':');
    if (colon_pos != std::string::npos) {
      blueprint.properties["catalogue"] = trim(line.substr(0, colon_pos));
      blueprint.properties["title"] = trim(line.substr(colon_pos + 1));
    } else {
      blueprint.properties["title"] = trim(line);
    }

    //ignore rest of comment block
    while (is.peek() != ':' && is.peek() != '#'
           && std::getline(is, line)) { }
  }

  return is;
}

std::istream&
g_format::read_front_matter(std::istream& is,
                            PuzzleSummary& summary)
{
  std::string line;

  //first line should be title and catalogue
  if (is.peek() != ':' && is.peek() != '#') {
    std::getline(is, line);
    auto colon_pos = line.find(':');
    if (colon_pos != std::string::npos)
      summary.title = trim(line.substr(colon_pos + 1));
    else
      summary.title = trim(line);

    //ignore rest of comment block
    while (is.peek() != ':' && is.peek() != '#'
           && std::getline(is, line)) { }
  }

  return is;
}

std::istream&
g_format::read_colors(std::istream& is,
                      PuzzleBlueprint& blueprint,
                      std::map<char, std::string>& color_names)
{
  std::string line;
  while (std::getline(is, line)) {
    if (!line.empty() && line[0] == ':')
      break;

    std::istringstream ss(trim(line));
    char inchar, outchar;
    ss >> inchar;
    ss.get(); //eat the colon
    ss >> outchar;
    std::string color_str, name;
    ss >> color_str;
    std::getline(ss, name);
    name = trim(name);

    Color color;
    if (!color_str.empty() && color_str[0] == '#') {
      std::istringstream css(color_str.substr(1, 6));
      css >> color;
    } else {
      if (color_str == "black")
        color = default_colors::black;
      else if (color_str == "red")
        color = default_colors::red;
      else if (color_str == "green")
        color = default_colors::dark_green;
      else if (color_str == "blue")
        color = default_colors::blue;
      else if (color_str == "yellow")
        color = default_colors::yellow;
      else
        color = default_colors::black;
    }

    if (inchar == '0')
      name = "background";
    else
      color_names[inchar] = name;

    blueprint.palette.add(color, name, outchar);
  }
  return is;
}

std::istream&
g_format::read_clues(std::istream& is,
                     PuzzleBlueprint& blueprint,
                     const std::map<char, std::string>& color_names,
                     ClueType type)
{
  Puzzle::ClueContainer* clues = nullptr;
  int* size = nullptr;
  if (type == ClueType::row) {
    clues = &blueprint.row_clues;
    size = &blueprint.height;
  } else {
    clues = &blueprint.col_clues;
    size = &blueprint.width;
  }

  std::string line;
  while (std::getline(is, line)) {
    if (!line.empty() && line[0] == ':')
      break;

    std::istringstream ss(line);
    int value;
    Puzzle::ClueSequence cseq;
    while (ss >> value) {
      PuzzleClue clue;
      clue.value = value;
      clue.color = default_colors::black;

      if (ss.good()) {
        char c = ss.get();
        if (!is_space(c))
          clue.color = blueprint.palette[color_names.at(c)];
        else if (color_names.find('1') != color_names.end())
          clue.color = blueprint.palette[color_names.at('1')];
      }

      cseq.push_back(clue);
    }

    //handle empty lines
    if (cseq.empty()) {
      PuzzleClue zero;
      zero.value = 0;
      cseq.push_back(zero);
    }

    clues->push_back(std::move(cseq));
  }

  *size = clues->size();
  return is;
}

std::istream&
g_format::skim(std::istream& is, PuzzleSummary& summary)
{
  read_front_matter(is, summary);

  std::string line;
  std::getline(is, line);

  //read colors
  summary.is_multicolor = false;
  if (!line.empty() && line[0] == '#' && to_lower(line[1]) == 'd') {
    int num_colors = 1; //just background color
    while (std::getline(is, line)) {
      if (!line.empty() && line[0] == ':')
        break;
      //ignore whitespace
      unsigned i = 0;
      while (i < line.size() && is_space(line[i]))
        ++i;
      //make sure this isn't the background color
      if (i < line.size() && line[i] != '0')
        ++num_colors;
    }

    if (num_colors > 2)
      summary.is_multicolor = true;
  }

  //determine dimensions
  summary.height = 0;
  while (std::getline(is, line)) {
    if (!line.empty() && line[0] == ':')
      break;
    ++summary.height;
  }

  summary.width = 0;
  while (std::getline(is, line)) {
    if (!line.empty() && line[0] == ':')
      break;
    ++summary.width;
  }
  return is;
}


namespace mk_format {
  /* .mk output */
  std::ostream&
  write_clues(std::ostream& os, const Puzzle::ClueContainer& clues);

  /* .mk input */
  std::istream&
  read_clues(std::istream& is, PuzzleBlueprint& blueprint, ClueType type);
}

std::ostream&
mk_format::write(std::ostream& os, const Puzzle& puzzle)
{
  if (puzzle.is_multicolor())
    throw UnsupportedFeature("mk_format::write: multicolor puzzles are "
                             "not supported by .mk format");

  //write dimensions (rows first)
  os << puzzle.height() << " " << puzzle.width() << "\n";

  write_clues(os, puzzle.row_clues());
  os << "#\n";
  write_clues(os, puzzle.col_clues());

  return os;
}

std::ostream&
mk_format::write_clues(std::ostream& os, const Puzzle::ClueContainer& clues)
{
  for (const auto& clue_seq : clues) {
    bool first = true;
    for (const auto& clue : clue_seq) {
      if (!first)
        os << " ";
      else
        first = false;
      os << clue.value;
    }
    os << "\n";
  }
  return os;
}

std::istream&
mk_format::read(std::istream& is, PuzzleBlueprint& blueprint)
{
  //read dimensions
  std::string line;
  std::getline(is, line);
  std::istringstream ss(line);
  ss >> blueprint.height; //num rows
  ss >> blueprint.width; //num columns
  if (blueprint.width < 0)
    blueprint.width = 0;
  if (blueprint.height < 0)
    blueprint.height = 0;

  //read clues
  read_clues(is, blueprint, ClueType::row);

  if (is.peek() == '#') {
    std::getline(is, line);
  }

  read_clues(is, blueprint, ClueType::col);
  return is;
}

std::istream&
mk_format::read_clues(std::istream& is, PuzzleBlueprint& blueprint,
                      ClueType type)
{
  Puzzle::ClueContainer* clues = nullptr;
  int count = 0;
  if (type == ClueType::row) {
    clues = &blueprint.row_clues;
    count = blueprint.height;
  } else if (type == ClueType::col) {
    clues = &blueprint.col_clues;
    count = blueprint.width;
  }

  std::string line;
  while (count > 0) {
    if (!std::getline(is, line)
        || (!line.empty() && line[0] == '#'))
      throw InvalidPuzzleFile("mk_format::read_clues: "
                              "number of clue lines does not match "
                              "puzzle dimensions");

    std::istringstream ss(line);
    Puzzle::ClueSequence clue_seq;
    int clue_val;
    while (ss >> clue_val) {
      PuzzleClue clue;
      clue.value = clue_val;
      clue_seq.push_back(clue);
    }

    //blank lines are supposed to be indicated by 0,
    //but let's check just in case
    if (clue_seq.empty()) {
      PuzzleClue zero;
      zero.value = 0;
      clue_seq.push_back(zero);
    }

    if (clues) clues->push_back(std::move(clue_seq));
    --count;
  }
  return is;
}

std::istream&
mk_format::skim(std::istream& is, PuzzleSummary& summary)
{
  summary.is_multicolor = false;
  std::string line;
  std::getline(is, line);
  std::istringstream ss(line);

  ss >> summary.height;
  ss >> summary.width;

  if (summary.width < 0)
    summary.width = 0;
  if (summary.height < 0)
    summary.height = 0;
  return is;
}


std::ostream&
nin_format::write(std::ostream& os, const Puzzle& puzzle)
{
  //format is same as .mk except for the first line
  os << puzzle.width() << " " << puzzle.height() << "\n";
  mk_format::write_clues(os, puzzle.row_clues());
  mk_format::write_clues(os, puzzle.col_clues());
  return os;
}

std::istream&
nin_format::read(std::istream& is, PuzzleBlueprint& blueprint)
{
  std::string line;
  std::getline(is, line);
  std::istringstream ss(line);

  ss >> blueprint.width;
  ss >> blueprint.height;
  if (blueprint.width < 0)
    blueprint.width = 0;
  if (blueprint.height < 0)
    blueprint.height = 0;

  //clue format is the same as .mk
  mk_format::read_clues(is, blueprint, ClueType::row);
  mk_format::read_clues(is, blueprint, ClueType::col);
  return is;
}

std::istream&
nin_format::skim(std::istream& is, PuzzleSummary& summary)
{
  std::string line;
  std::getline(is, line);
  std::istringstream ss(line);

  ss >> summary.width;
  ss >> summary.height;
  if (summary.width < 0)
    summary.width = 0;
  if (summary.height < 0)
    summary.height = 0;

  return is;
}
