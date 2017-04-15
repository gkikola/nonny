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

PuzzleView::PuzzleView(const std::string& filename)
{
  load(filename);
}

PuzzleView::PuzzleView(const std::string& filename,
                       unsigned width, unsigned height)
  : View(width, height)
{
  load(filename);
}

void PuzzleView::load(const std::string& filename)
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
  Rect region = { 0, 0,
                  static_cast<int>(m_width) * 2,
                  static_cast<int>(m_height) * 2 };
  auto ppanel = make_ui_panel<PuzzlePanel>(region, m_puzzle);
  m_panel.attach_panel(ppanel);
  m_panel.move(0, 0);
  m_panel.resize(m_width, m_height);
  m_panel.set_visible_region(Rect{0, 0,
        static_cast<int>(m_width), static_cast<int>(m_height)});
}

void PuzzleView::update(unsigned ticks, InputHandler& input)
{
  m_panel.update(ticks, input);
}

void PuzzleView::draw(Renderer& renderer)
{
  m_panel.draw(renderer);
}

void PuzzleView::resize(unsigned width, unsigned height)
{
  View::resize(width, height);
  setup_panels();
}
