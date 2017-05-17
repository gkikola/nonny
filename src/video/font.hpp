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

#ifndef NONNY_FONT_HPP
#define NONNY_FONT_HPP

#include <string>

class Font {
public:
  Font() { }
  virtual ~Font() { }

  Font(const Font&) = delete;
  Font& operator=(const Font&) = delete;

  virtual void text_size(const std::string& text,
                         unsigned* width, unsigned* height) const = 0;
  virtual void text_size_wrapped(std::string text, unsigned wrap_width,
                                 unsigned* width, unsigned* height) const;
  virtual void resize(unsigned pt_size) = 0;
};

#endif
