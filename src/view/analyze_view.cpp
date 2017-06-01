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

#include "view/analyze_view.hpp"

#include "puzzle/puzzle.hpp"
#include "settings/game_settings.hpp"
#include "view/view_manager.hpp"

AnalyzeView::AnalyzeView(ViewManager& vm, Puzzle& puzzle)
  : View(vm), m_puzzle(puzzle)
{
  load_resources();
  m_panel = std::make_shared<AnalysisPanel>(*m_font, m_puzzle);
  m_panel->on_close([this]() {
      m_mgr.schedule_action(ViewManager::Action::close_menu);
    });
}

void AnalyzeView::update(unsigned ticks, InputHandler& input)
{
  m_panel->update(ticks, input);
}

void AnalyzeView::draw(Renderer& renderer)
{
  m_panel->draw(renderer);
}

void AnalyzeView::resize(int width, int height)
{
  int pwidth = m_panel->boundary().width();
  int pheight = m_panel->boundary().height();
  m_panel->move(width / 2 - pwidth / 2, height / 2 - pheight / 2);
}

void AnalyzeView::load_resources()
{
  std::string font_file = m_mgr.game_settings().font_dir() + "FreeSans.ttf";
  m_font = m_mgr.video_system().new_font(font_file, 24);
}
