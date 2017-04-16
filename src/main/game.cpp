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

#include "main/game.hpp"

#include "config.h"
#include "color/color.hpp"
#include "event/event_handler.hpp"
#include "input/input_handler.hpp"
#include "view/puzzle_view.hpp"

Game::Game(int argc, char* argv[])
{
  m_video = VideoSystem::create();

  WindowSettings ws;
  ws.title = NONNY_TITLE;
  m_window = m_video->new_window(ws);
  m_renderer = m_video->new_renderer(*m_window);

  m_view_mgr = std::make_unique<ViewManager>();

  //For now, start the game with a puzzle open
  auto view = std::make_shared<PuzzleView>("temp.non");
  m_view_mgr->push(view);
  m_view_mgr->resize(m_window->width(), m_window->height());
}

void Game::run()
{
  InputHandler input;
  std::unique_ptr<EventHandler> event = EventHandler::create();

  bool exit = false;
  while (!exit) {
    input.update(0);

    event->process(input, *m_view_mgr);
    exit = m_view_mgr->empty();

    m_renderer->set_draw_color(default_colors::white);
    m_renderer->clear();

    m_view_mgr->update(0, input);
    m_view_mgr->draw(*m_renderer);
    
    m_renderer->present();
  }
}
