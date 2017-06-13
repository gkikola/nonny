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

#include "utility/utility.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <experimental/filesystem>
#include "config.h"

#ifdef NONNY_INPUT_SDL
#include "sdl/sdl_paths.hpp"
#endif

namespace stdfs = std::experimental::filesystem;

std::string base_path()
{
#ifdef NONNY_INPUT_SDL
  std::string result = sdl_base_path();
#else
  throw std::runtime_error("::base_path: base path not retrievable");
#endif

  stdfs::path p(result);
  if (!stdfs::exists(p))
    stdfs::create_directories(p);

  return stdfs::canonical(p).string();
}

std::string save_path()
{
#ifdef NONNY_INPUT_SDL
  std::string result = sdl_save_path();
#else
  throw std::runtime_error("::save_path: save path not retrievable");
#endif

  stdfs::path p(result);
  if (!stdfs::exists(p))
    stdfs::create_directories(p);

  return stdfs::canonical(p).string();
}

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

std::string unescape(const std::string& s)
{
  std::string result;

  for (char c : s) {
    switch (c) {
    default:
      result.push_back(c);
      break;
    case '\\':
      result += "\\\\";
      break;
    case '\b':
      result += "\\b";
      break;
    case '\f':
      result += "\\f";
      break;
    case '\n':
      result += "\\n";
      break;
    case '\r':
      result += "\\r";
      break;
    case '\t':
      result += "\\t";
      break;
    case '\v':
      result += "\\v";
      break;
    }
  }
  
  return result;
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
    throw std::out_of_range("::str_to_uint: out of range");
  return result;
}

std::string time_to_string(unsigned time, bool show_fractional)
{
  time /= 100;
  unsigned tenths = time % 10;
  time /= 10;
  unsigned seconds = time % 60;
  time /= 60;
  unsigned minutes = time % 60;
  time /= 60;
  unsigned hours = time;

  std::string result;
  if (hours) {
    if (hours < 10)
      result += '0';
    result += std::to_string(hours) + ":";
  }

  if (minutes < 10)
    result += '0';
  result += std::to_string(minutes) + ":";

  if (seconds < 10)
    result += '0';
  result += std::to_string(seconds);

  if (show_fractional)
    result += "." + std::to_string(tenths);

  return result;
}

std::ostream& write_time(std::ostream& os, unsigned time,
                         bool show_fractional)
{
  return os << time_to_string(time, show_fractional);
}

unsigned string_to_time(const std::string& time_str)
{
  unsigned result;
  std::istringstream ss(time_str);
  read_time(ss, result);
  return result;
}

std::istream& read_time(std::istream& is, unsigned& time)
{
  unsigned result = 0;
  bool has_tenths = false;
  while (is && is_digit(is.peek())) {
    unsigned component;
    is >> component;
    result += component;

    if (is.peek() == ':') {
      result *= 60;
      is.get();
    } else if (!has_tenths && is.peek() == '.') {
      result *= 10;
      is.get();
      has_tenths = true;
    }
  }

  if (has_tenths)
    result *= 100;
  else
    result *= 1000;

  time = result;
  return is;
}
