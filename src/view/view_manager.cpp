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
#include <string>
#include <SDL2/SDL.h>
#include "input/input_handler.hpp"
#include "settings/game_settings.hpp"
#include "video/renderer.hpp"
#include "view/file_view.hpp"
#include "view/menu_view.hpp"
#include "view/puzzle_view.hpp"

ViewManager::ViewManager(VideoSystem& vs, Renderer& renderer,
                         GameSettings& settings)
  : m_video(vs),
    m_renderer(renderer),
    m_settings(settings)
{
}

ViewManager::ViewManager(VideoSystem& vs, Renderer& renderer,
                         GameSettings& settings,
                         unsigned width, unsigned height)
  : m_video(vs),
    m_renderer(renderer),
    m_settings(settings),
    m_width(width),
    m_height(height)
{
}

void ViewManager::pop()
{
  if (m_views.empty())
    throw std::out_of_range("ViewManager::pop: no game view found");
  m_views.pop_back();
}

void ViewManager::update(unsigned ticks, InputHandler& input)
{
  if (m_action != Action::no_action) {
    switch (m_action) {
    default:
    case Action::no_action:
      break;
    case Action::quit_game:
      m_views.clear();
      break;
    case Action::choose_puzzle:
      push(std::make_shared<FileView>(*this, FileView::Mode::open));
      break;
    case Action::open_menu:
      push(std::make_shared<MenuView>(*this));
      break;
    case Action::load_puzzle:
      push(std::make_shared<PuzzleView>(*this, m_action_arg));
      break;
    }
    resize(m_width, m_height);
    m_action = Action::no_action;
  }
  
  if (!m_views.empty())
    m_views.back()->update(ticks, input);
}

void ViewManager::draw(Renderer& renderer)
{
  if (!m_views.empty())
    m_views.back()->draw(renderer);
}

void ViewManager::refresh()
{
  resize(m_width, m_height);
}

void ViewManager::resize(unsigned width, unsigned height)
{
  m_width = width;
  m_height = height;

  for (auto view : m_views)
    view->resize(width, height);
}
