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

#include "view/puzzle_view.hpp"

#include <fstream>
#include <memory>
#include "input/input_handler.hpp"
#include "ui/scrollbar.hpp"
#include "video/renderer.hpp"

PuzzleView::PuzzleView(ViewManager& view_mgr, std::string filename)
  : View(view_mgr)
{
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("PuzzleView::PuzzleView: "
                             "could not open puzzle file " + filename);
  }

  file >> m_puzzle;

  setup_panels();
}

void PuzzleView::setup_panels()
{
}

void PuzzleView::update(unsigned ticks, InputHandler& input)
{
  m_panel.update(ticks, input);
}

void PuzzleView::draw(Renderer& renderer)
{
  m_panel.draw(renderer);
}
