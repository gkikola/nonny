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

#include "nonny/utility.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>

char escape(char c)
{
  switch (c) {
  case 'b':
    return '\b';
  case 'f':
    return '\f';
  case 'n':
    return '\n';
  case 'r':
    return '\r';
  case 't':
    return '\t';
  case 'v':
    return '\v';
  default:
    return c;
  }
}

std::string escape(const std::string& s)
{
  auto beg = s.begin();
  auto end = s.end();

  //ignore outer quotation marks
  if (beg != end && *beg == '"')
    ++beg;
  if (beg != end && *(end - 1) == '"')
    --end;
  
  std::string retvalue;
  bool reading_escape_seq = false;
  for (auto it = beg; it != end; ++it) {    
    if (reading_escape_seq) {
      reading_escape_seq = false;
      retvalue.push_back(escape(*it));
    } else {
      if (*it == '\\')
        reading_escape_seq = true;
      else
        retvalue.push_back(*it);
    }
  }

  return retvalue;
}

std::string trim(std::string s)
{
  auto front = std::find_if_not(s.begin(), s.end(), is_space);
  s.erase(s.begin(), front);

  auto back = std::find_if_not(s.rbegin(), s.rend(), is_space).base();
  s.erase(back, s.end());

  return s;
}

std::pair<std::string, std::string>
parse_property(const std::string& line)
{
  std::string trimmed_line = trim(line);
  auto end_of_name = std::find_if(trimmed_line.begin(),
                                  trimmed_line.end(),
                                  is_space);

  std::string property;
  std::transform(trimmed_line.begin(), end_of_name,
                 std::back_inserter(property), to_lower);
  
  std::string argument
    = escape(trim(std::string(end_of_name, trimmed_line.end())));
  
  return std::make_pair(std::move(property), std::move(argument));
}

unsigned str_to_uint(const std::string& s, std::size_t* pos, int base)
{
  unsigned long lresult = std::stoul(s, pos, base);
  unsigned result = static_cast<unsigned>(lresult);
  if (result != lresult)
    throw std::out_of_range("str_to_uint");
  return result;
}
