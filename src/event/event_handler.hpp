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

#ifndef NONNY_EVENT_HANDLER_HPP
#define NONNY_EVENT_HANDLER_HPP

#include <cstddef>
#include <memory>

class InputHandler;
class ViewManager;

/*
 * Processes window events and passes input to input handler
 */
class EventHandler {
public:
  EventHandler() { }
  virtual ~EventHandler() { }

  static std::unique_ptr<EventHandler> create();

  //get time in milliseconds since program started
  virtual std::size_t get_ticks() const = 0;

  virtual void process(InputHandler&, ViewManager&) = 0;
};

#endif
