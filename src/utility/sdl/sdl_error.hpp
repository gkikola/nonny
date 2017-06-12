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

#ifndef NONNY_SDL_ERROR_HPP
#define NONNY_SDL_ERROR_HPP

#include <stdexcept>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

class SDLError : public std::runtime_error {
public:
  SDLError(const std::string& what_arg)
    : std::runtime_error(what_arg + ": " + SDL_GetError()) { }
};

class IMGError : public std::runtime_error {
public:
  IMGError(const std::string& what_arg)
    : std::runtime_error(what_arg + ": " + IMG_GetError()) { }
};

class TTFError : public std::runtime_error {
public:
  TTFError(const std::string& what_arg)
    : std::runtime_error(what_arg + ": " + TTF_GetError()) { }
};

#endif
