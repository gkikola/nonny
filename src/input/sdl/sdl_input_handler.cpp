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

#include "input/sdl/sdl_input_handler.hpp"

#include <SDL2/SDL.h>

SDLInputHandler::~SDLInputHandler()
{
  if (m_cursor)
    SDL_FreeCursor(m_cursor);
}

void SDLInputHandler::capture_mouse()
{
  SDL_CaptureMouse(SDL_TRUE);
}

void SDLInputHandler::release_mouse()
{
  SDL_CaptureMouse(SDL_FALSE);
}

void SDLInputHandler::set_cursor(Mouse::Cursor cursor)
{
  if (m_cursor)
    SDL_FreeCursor(m_cursor);

  SDL_SystemCursor type;
  switch (cursor) {
  default:
  case Mouse::Cursor::unknown:
  case Mouse::Cursor::arrow:
    type = SDL_SYSTEM_CURSOR_ARROW;
    break;
  case Mouse::Cursor::ibeam:
    type = SDL_SYSTEM_CURSOR_IBEAM;
    break;
  case Mouse::Cursor::crosshair:
    type = SDL_SYSTEM_CURSOR_CROSSHAIR;
    break;
  case Mouse::Cursor::size_we:
    type = SDL_SYSTEM_CURSOR_SIZEWE;
    break;
  case Mouse::Cursor::size_ns:
    type = SDL_SYSTEM_CURSOR_SIZENS;
    break;
  case Mouse::Cursor::no:
    type = SDL_SYSTEM_CURSOR_NO;
    break;
  case Mouse::Cursor::hand:
    type = SDL_SYSTEM_CURSOR_HAND;
    break;
  }
  m_cursor = SDL_CreateSystemCursor(type);
  SDL_SetCursor(m_cursor);
  m_cursor_type = cursor;
}

void SDLInputHandler::reset_cursor()
{
  SDL_FreeCursor(m_cursor);
  m_cursor = nullptr;
  m_cursor_type = Mouse::Cursor::arrow;
}
