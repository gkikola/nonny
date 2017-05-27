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

#ifndef NONNY_SDL_FONT_HPP
#define NONNY_SDL_FONT_HPP

#include <string>
#include <SDL2/SDL_ttf.h>
#include "video/font.hpp"

class SDLFont : public Font {
public:
  SDLFont(const std::string& filename, int pt_size = 12);
  ~SDLFont();

  void text_size(const std::string& text,
                 int* width, int* height) const override;
  void resize(int pt_size) override;

  TTF_Font* get_sdl_handle() const { return m_font; }

private:
  TTF_Font* m_font = nullptr;
  std::string m_filename;
  int m_pt_size;
};

#endif
