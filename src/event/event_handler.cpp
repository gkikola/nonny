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

#include "event/event_handler.hpp"

#include <stdexcept>
#include "config.h"

#ifdef NONNY_INPUT_SDL
#include "event/sdl/sdl_event_handler.hpp"
#endif

std::unique_ptr<EventHandler> EventHandler::create()
{
  #ifdef NONNY_INPUT_SDL
    return std::make_unique<SDLEventHandler>();
  #else
    throw std::runtime_error("EventHandler::create: "
                             "event handler not implemented");
  #endif
}
