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

#ifndef NONNY_SDL_INPUT_HANDLER_HPP
#define NONNY_SDL_INPUT_HANDLER_HPP

#include "SDL.h"
#include "input/input_handler.hpp"

class SDLInputHandler : public InputHandler {
public:
  SDLInputHandler() { }
  ~SDLInputHandler();

  SDLInputHandler(const SDLInputHandler&) = delete;
  SDLInputHandler& operator=(const SDLInputHandler&) = delete;

  void capture_mouse() override;
  void release_mouse() override;

  void set_cursor(Mouse::Cursor cursor) override;
  void reset_cursor() override;
  Mouse::Cursor cursor() const override { return m_cursor_type; }

private:
  Mouse::Cursor m_cursor_type = Mouse::Cursor::arrow;
  SDL_Cursor* m_cursor = nullptr;
};

#endif
