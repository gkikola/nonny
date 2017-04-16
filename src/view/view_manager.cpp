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

#include "view/view_manager.hpp"

#include <stdexcept>
#include <SDL2/SDL.h>
#include "input/input_handler.hpp"
#include "video/renderer.hpp"

void ViewManager::pop()
{
  if (m_views.empty())
    throw std::out_of_range("ViewManager::pop: no game view found");
  m_views.pop_back();
}

void ViewManager::update(unsigned ticks, InputHandler& input)
{
  if (!m_views.empty())
    m_views.back()->update(ticks, input);
}

void ViewManager::draw(Renderer& renderer)
{
  if (!m_views.empty())
    m_views.back()->draw(renderer);
}

void ViewManager::resize(unsigned width, unsigned height)
{
  m_width = width;
  m_height = height;

  for (auto view : m_views)
    view->resize(width, height);
}

void ViewManager::quit_game()
{
  while (!m_views.empty())
    m_views.pop_back();
}
