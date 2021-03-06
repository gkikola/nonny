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

#ifndef NONNY_SDL_TEXTURE_HPP
#define NONNY_SDL_TEXTURE_HPP

#include "SDL.h"
#include "video/texture.hpp"

class SDLTexture : public Texture {
public:
  SDLTexture() { } //null texture
  SDLTexture(SDL_Renderer* renderer, int width, int height);
  SDLTexture(SDL_Renderer* renderer, SDL_Surface* surface);

  SDLTexture(const SDLTexture&) = delete;
  SDLTexture& operator=(const SDLTexture&) = delete;

  int width() const override { return m_width; }
  int height() const override { return m_height; }

  SDL_Texture* get_sdl_handle() const { return m_texture; }

  ~SDLTexture();
private:
  SDL_Texture* m_texture = nullptr;
  int m_width = 0, m_height = 0;
};

#endif
