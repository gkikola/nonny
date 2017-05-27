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

#include "video/font.hpp"

#include <cstddef>

void Font::text_size_wrapped(std::string text, int wrap_width,
                             int* width, int* height) const
{
  if (width)
    *width = 0;
  if (height)
    *height = 0;

  text += "\n";
  std::size_t pos = 0;
  std::string line, word;
  int line_wd, line_ht;
  while (pos != text.size()) {
    if (text[pos] != '\n' && text[pos] != ' ') {
      word += text[pos];
    } else {
      if (line.empty())
        text_size(word, &line_wd, &line_ht);
      else
        text_size(line + " " + word, &line_wd, &line_ht);

      if (line_wd > wrap_width) {
        text_size(line, &line_wd, &line_ht);
        if (width)
          *width = std::max(*width, line_wd);
        if (height)
          *height += line_ht;
        line = word;
        word = "";
      } else {
        if (!line.empty() && !word.empty())
          line += " ";
        line += word;
        word = "";
      }

      if (text[pos] == '\n') {
        if (word.empty())
          text_size(line, &line_wd, &line_ht);
        else if (line.empty())
          text_size(word, &line_wd, &line_ht);
        else
          text_size(line + " " + word, &line_wd, &line_ht);
        
        if (width)
          *width = std::max(*width, line_wd);
        if (height)
          *height += line_ht;
        line = "";
        word = "";
      }
    }

    ++pos;
  }
}
