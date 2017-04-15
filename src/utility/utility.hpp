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

#ifndef NONNY_UTILITY_HPP
#define NONNY_UTILITY_HPP

#include <cctype>
#include <cstddef>
#include <string>
#include <utility>

// Represents a point in the window
struct Point {
  int x = 0;
  int y = 0;
};

// Converts an escape sequence code to its corresponding character
char escape(char c);

/*
 * Converts all escape sequences in the given string and removes an outer
 * pair of quotation marks if present
 */
std::string escape(const std::string& s);

// Trims whitespace from the beginning and end of a string
std::string trim(std::string s);

/*
 * Splits a string across a set of delimiters and stores each substring
 * in the output range starting at dest. dest should be an iterator into a
 * container of strings.
 */
template <typename OutputIt>
void split(const std::string& s, OutputIt dest,
           const std::string& delims = ",");

/*
 * Converts a string to an unsigned integer or throws an out_of_range
 * or invalid_argument exception if it can't be done.
 *
 * If the pos parameter is nonzero, it specifies the address of a variable 
 * in which to store the number of characters that were read.
 */
unsigned
str_to_uint(const std::string& s, std::size_t* pos = 0, int base = 10);

/*
 * Wrappers for <cctype> functions to handle signed chars gracefully
 */
inline bool is_alpha(char c);
inline bool is_digit(char c);
inline bool is_space(char c);
inline char to_lower(char c);


/*
 * Reads a property-value pair from a string. The string should have the
 * format
 *
 *   property_name "argument"
 *
 * where the quotation marks are optional and extra whitespace is ignored. The
 * string should not contain newline characters.
 *
 * Returns a pair of strings, the first of which is the property name 
 * converted to lowercase and the second of which is the argument or 
 * arguments (if any). A blank property name indicates a blank line.
 */
std::pair<std::string, std::string>
parse_property(const std::string& line);


/* implementation */

template <typename Out>
void split(const std::string& s, Out dest, const std::string& delims)
{
  std::string::size_type pos = 0, next_pos = 0;
  while (pos < s.size()) {
    pos = s.find_first_not_of(delims, pos);
    if (pos == std::string::npos)
      break;

    next_pos = s.find_first_of(delims, pos);
    if (next_pos == std::string::npos)
      next_pos = s.size();

    *dest++ = s.substr(pos, next_pos - pos);

    pos = next_pos;
  }
}

inline bool is_alpha(char c)
{
  return c >= 0 && std::isalpha(static_cast<unsigned char>(c));
}

inline bool is_digit(char c)
{
  return c >= 0 && std::isdigit(static_cast<unsigned char>(c));
}

inline bool is_space(char c)
{
  return c >= 0 && std::isspace(static_cast<unsigned char>(c));
}

inline char to_lower(char c)
{
  if (c >= 0)
    return std::tolower(static_cast<unsigned char>(c));
  else
    return c;
}

#endif
