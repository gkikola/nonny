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
#include "view/victory_view.hpp"

ViewManager::ViewManager(VideoSystem& vs, Renderer& renderer,
                         GameSettings& settings)
  : m_video(vs),
    m_renderer(renderer),
    m_settings(settings),
    m_save_mgr(settings)
{
}

ViewManager::ViewManager(VideoSystem& vs, Renderer& renderer,
                         GameSettings& settings,
                         int width, int height)
  : m_video(vs),
    m_renderer(renderer),
    m_settings(settings),
    m_save_mgr(settings),
    m_width(width),
    m_height(height)
{
}

void ViewManager::message_box(const std::string& message,
                              MessageBoxView::Type type,
                              Callback on_yes,
                              Callback on_no,
                              Callback on_cancel)
{
  m_mbox_yes = on_yes;
  m_mbox_no = on_no;
  m_mbox_cancel = on_cancel;
  m_mbox_type = type;
  m_action_arg = message;
  m_action = Action::message_box;
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
    if (m_mbox_open && m_action != Action::message_box) {
      m_mbox_open = false;
      pop();
    }

    Action action = m_action;
    m_action = Action::no_action;
    switch (action) {
    default:
    case Action::no_action:
      break;
    case Action::quit_game:
      {
        if (is_save_needed()) {
          std::string message;
          if (m_puzzle_status == puzzle_play)
            message = "Do you want to save your progress before exiting?";
          else if (m_puzzle_status == puzzle_edit)
            message = "Do you want to save your puzzle before exiting?";
          message_box(message, MessageBoxView::Type::yes_no_cancel,
                      std::bind(&ViewManager::schedule_action,
                                this, Action::save_and_quit, ""),
                      std::bind(&ViewManager::schedule_action,
                                this, Action::force_quit, ""),
                      std::bind(&ViewManager::schedule_action,
                                this, Action::close_message_box, ""));
        } else {
          m_views.clear();
        }
      }
      break;
    case Action::force_quit:
      m_views.clear();
      break;
    case Action::save_and_quit:
      if (save())
        m_views.clear();
      break;
    case Action::choose_puzzle:
      push(std::make_shared<FileView>(*this, FileView::Mode::open));
      break;
    case Action::create_puzzle:
      push(std::make_shared<PuzzleView>(*this));
      m_puzzle_status = puzzle_edit;
      break;
    case Action::open_menu:
      if (!m_views.empty()
          && typeid(*m_views.back()) == typeid(FileView))
        pop();
      if (m_views.empty()
          || typeid(*m_views.back()) != typeid(MenuView)) {
        if (m_puzzle_status == puzzle_play)
          push(std::make_shared<MenuView>(*this,
                                          MenuView::MenuType::in_game_menu));
        else if (m_puzzle_status == puzzle_edit)
          push(std::make_shared<MenuView>(*this,
                                          MenuView::MenuType::edit_menu));
        else
          push(std::make_shared<MenuView>(*this));
      }
      break;
    case Action::close_menu:
      pop();
      break;
    case Action::load_puzzle:
      pop(); //close file selector
      push(std::make_shared<PuzzleView>(*this, m_action_arg));
      m_puzzle_status = puzzle_play;
      break;
    case Action::save_puzzle_as:
      if (!m_views.empty() && typeid(*m_views.back()) != typeid(FileView))
        push(std::make_shared<FileView>(*this, FileView::Mode::save));
      break;
    case Action::save_puzzle:
      {
        if (typeid(*m_views.back()) == typeid(FileView))
          pop();
        if (typeid(*m_views.back()) == typeid(MenuView))
          pop();
        auto view = std::dynamic_pointer_cast<PuzzleView>(m_views.back());
        if (view)
          view->save_puzzle(m_action_arg);
      }
      break;
    case Action::analyze_puzzle:
      break;
    case Action::quit_puzzle:
      if (!m_views.empty()
          && typeid(*m_views.back()) != typeid(VictoryView)
          && is_save_needed()) {
        std::string message;
        if (m_puzzle_status == puzzle_play)
          message = "Do you want to save your progress before exiting?";
        else if (m_puzzle_status == puzzle_edit)
          message = "Do you want to save your puzzle before exiting?";
        message_box(message, MessageBoxView::Type::yes_no_cancel,
                    std::bind(&ViewManager::schedule_action,
                              this, Action::save_and_quit_puzzle, ""),
                    std::bind(&ViewManager::schedule_action,
                              this, Action::force_quit_puzzle, ""),
                    std::bind(&ViewManager::schedule_action,
                              this, Action::close_message_box, ""));
      } else {
        schedule_action(Action::force_quit_puzzle);
      }
      break;
    case Action::save_and_quit_puzzle:
      if (save())
        schedule_action(Action::force_quit_puzzle);
      break;
    case Action::force_quit_puzzle:
      pop(); //close the menu/victory screen
      pop(); //close the puzzle
      m_puzzle_status = no_puzzle;
      break;
    case Action::save_game:
      pop(); //close the menu
      if (!m_views.empty()) {
        auto pv = std::dynamic_pointer_cast<PuzzleView>(m_views.back());
        if (pv)
          pv->save();
      }
      break;
    case Action::restart:
      pop(); //close menu
      if (!m_views.empty()) {
        auto pv = std::dynamic_pointer_cast<PuzzleView>(m_views.back());
        if (pv)
          pv->restart();
      }
      break;
    case Action::show_victory_screen:
      if (!m_views.empty()) {
        auto pv = std::dynamic_pointer_cast<PuzzleView>(m_views.back());
        if (pv)
          push(std::make_shared<VictoryView>(*this, pv->puzzle(),
                                             pv->time(), pv->best_time()));
        m_puzzle_status = no_puzzle;
      }
      break;
    case Action::edit_puzzle_data:
      break;
    case Action::message_box:
      push(std::make_shared<MessageBoxView>(*this, m_action_arg,
                                            m_mbox_type, m_mbox_yes,
                                            m_mbox_no, m_mbox_cancel));
      m_mbox_open = true;
      break;
    case Action::close_message_box:
      //don't need to do anything here, was closed before the switch
      break;
    }
    input.release_mouse();
    input.reset_cursor();
    resize(m_width, m_height);
  }
  
  if (!m_views.empty())
    m_views.back()->update(ticks, input);
}

void ViewManager::draw(Renderer& renderer)
{
  if (!m_views.empty()) {
    auto cur = m_views.end() - 1;
    while (true) {
      (*cur)->draw(renderer);
      if (!(*cur)->is_transparent() || cur == m_views.begin())
        break;
      
      --cur;
    }
    m_views.back()->draw(renderer);
  }
}

void ViewManager::refresh()
{
  resize(m_width, m_height);
}

void ViewManager::resize(int width, int height)
{
  m_width = width;
  m_height = height;

  for (auto view : m_views)
    view->resize(width, height);
}

bool ViewManager::is_save_needed() const
{
  for (auto it = m_views.rbegin(); it != m_views.rend(); ++it) {
    if (typeid(**it) == typeid(PuzzleView)) {
      auto pv = std::dynamic_pointer_cast<PuzzleView>(*it);
      return pv->is_save_needed();
    }
  }
  return false;
}

bool ViewManager::save()
{
  for (auto it = m_views.rbegin(); it != m_views.rend(); ++it) {
    if (typeid(**it) == typeid(PuzzleView)) {
      auto pv = std::dynamic_pointer_cast<PuzzleView>(*it);
      if (m_puzzle_status == puzzle_play)
        pv->save();
      else if (m_puzzle_status == puzzle_edit)
        pv->save_puzzle();
      return !pv->is_save_needed();
    }
  }
  return true;
}
