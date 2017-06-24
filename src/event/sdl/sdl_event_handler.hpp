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

#ifndef NONNY_SDL_EVENT_HANDLER_HPP
#define NONNY_SDL_EVENT_HANDLER_HPP

#include "SDL.h"
#include "event/event_handler.hpp"
#include "input/key.hpp"

class ViewManager;

class SDLEventHandler : public EventHandler {
public:
  SDLEventHandler() { }
  ~SDLEventHandler() { }

  SDLEventHandler(const SDLEventHandler&) = delete;
  SDLEventHandler& operator=(const SDLEventHandler&) = delete;

  std::size_t get_ticks() const { return SDL_GetTicks(); }

  void process(InputHandler& input, ViewManager& view_mgr) override;

private:
  static Keyboard::Key convert_keycode(SDL_Scancode key);
  static Mouse::Button convert_mouse_button(Uint8 button);
};

#endif
