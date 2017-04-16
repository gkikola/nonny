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
#include <utility>
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

PuzzleView::PuzzleView(const PuzzleView& pv)
  : m_puzzle(pv.m_puzzle)
{
  setup_panels();
}

PuzzleView::PuzzleView(PuzzleView&& pv)
  : m_puzzle(std::move(pv.m_puzzle))
{
  setup_panels();
}

PuzzleView& PuzzleView::operator=(const PuzzleView& pv) &
{
  if (this != &pv) {
    m_puzzle = pv.m_puzzle;
    setup_panels();
  }
  return *this;
}

PuzzleView& PuzzleView::operator=(PuzzleView&& pv) &
{
  if (this != &pv) {
    m_puzzle = std::move(pv.m_puzzle);
    setup_panels();
  }
  return *this;
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
  Rect puz_region(0, 0, 1600, 1200);
  auto ppanel = make_ui_panel<PuzzlePanel>(puz_region, m_puzzle);

  Rect win_region(0, 0, m_width, m_height);
  m_main_panel = ScrollingPanel(win_region, ppanel);
}

void PuzzleView::update(unsigned ticks, InputHandler& input)
{
  m_main_panel.update(ticks, input);
}

void PuzzleView::draw(Renderer& renderer)
{
  m_main_panel.draw(renderer);
}

void PuzzleView::resize(unsigned width, unsigned height)
{
  View::resize(width, height);
  m_main_panel.resize(width, height);
}
