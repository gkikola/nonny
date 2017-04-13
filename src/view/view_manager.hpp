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

#ifndef NONNY_VIEW_MANAGER_HPP
#define NONNY_VIEW_MANAGER_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include "view/view.hpp"

class InputHandler;
class Renderer;

class ViewManager {
public:
  ViewManager() { }
  ViewManager(unsigned width, unsigned height)
    : m_width(width), m_height(height) { }

  void push(std::shared_ptr<View> view) { m_views.push_back(view); }
  void pop();
  bool empty() const { return m_views.empty(); }
  std::size_t size() const { return m_views.size(); }
  
  void update(unsigned ticks, InputHandler& input);
  void draw(Renderer& renderer);

  inline void resize(unsigned width, unsigned height);
  unsigned width() const { return m_width; }
  unsigned height() const { return m_height; }
  
  void quit_game();
private:
  std::vector<std::shared_ptr<View>> m_views;
  unsigned m_width = 0;
  unsigned m_height = 0;
};


/* implementation */

inline void ViewManager::resize(unsigned width, unsigned height)
{
  m_width = width;
  m_height = height;
}

#endif
