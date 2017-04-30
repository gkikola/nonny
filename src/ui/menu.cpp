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

#include "ui/menu.hpp"

#include <algorithm>

constexpr unsigned spacing = 16;

void Menu::position_controls()
{
  calc_size();
  int x = m_boundary.x();
  int y = m_boundary.y() + spacing;

  for (auto& c : m_controls) {
    c->resize(m_boundary.width(), c->boundary().height());
    c->move(x + m_boundary.width() / 2 - c->boundary().width() / 2, y);
    y += c->boundary().height() + spacing;
  }
}

void Menu::calc_size()
{
  unsigned width = 0;
  unsigned height = spacing;
  for (const auto& c : m_controls) {
    width = std::max(width, c->boundary().width());
    height += c->boundary().height() + spacing;
  }

  resize(width, height);
}
