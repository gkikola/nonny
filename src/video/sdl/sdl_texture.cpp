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

#include "video/sdl/sdl_texture.hpp"

SDLTexture::SDLTexture(SDL_Renderer* renderer,
                       unsigned width, unsigned height)
{
  m_texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET,
                                width, height);
  m_width = width;
  m_height = height;
}

SDLTexture::SDLTexture(SDL_Renderer* renderer, SDL_Surface* surface)
{
  m_texture = SDL_CreateTextureFromSurface(renderer, surface);

  Uint32 fmt;
  int access, wd, ht;
  if (SDL_QueryTexture(m_texture, &fmt, &access, &wd, &ht) == 0) {
    m_width = wd;
    m_height = ht;
  }
}

SDLTexture::~SDLTexture()
{
  if (m_texture)
    SDL_DestroyTexture(m_texture);
}
