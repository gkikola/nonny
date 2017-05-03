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

#include "video/sdl/sdl_font.hpp"

SDLFont::SDLFont(const std::string& filename, unsigned pt_size)
  : m_filename(filename), m_pt_size(pt_size)
{
  resize(pt_size);
}

SDLFont::~SDLFont()
{
  if (m_font)
    TTF_CloseFont(m_font);
}

void SDLFont::text_size(const std::string& text,
                        unsigned* width, unsigned* height) const
{
  int wd = 0, ht = 0;
  if (TTF_SizeUTF8(m_font, text.c_str(), &wd, &ht) == 0) {
    if (width)
      *width = wd;
    if (height)
      *height = ht;
  }
}

void SDLFont::resize(unsigned pt_size)
{
  if (m_font)
    TTF_CloseFont(m_font);
  m_pt_size = pt_size;
  m_font = TTF_OpenFont(m_filename.c_str(), m_pt_size);
}
