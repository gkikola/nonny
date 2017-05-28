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
#include <string>
#include <vector>
#include "save/save_manager.hpp"
#include "view/message_box_view.hpp"
#include "view/view.hpp"
#include "video/font.hpp"
#include "video/video_system.hpp"

class GameSettings;
class InputHandler;
class Renderer;
class SaveManager;

/*
 * Manages the different Views that make up the game. Responsible for
 * updating and drawing the current View and for handling changes to
 * the game state.
 */
class ViewManager {
public:
  ViewManager(VideoSystem& vs, Renderer& renderer, GameSettings& settings);
  ViewManager(VideoSystem& vs, Renderer& renderer, GameSettings& settings,
              int width, int height);
  
  enum class Action { no_action, quit_game, force_quit, save_and_quit,
      choose_puzzle, create_puzzle,
      open_menu, close_menu, load_puzzle, save_puzzle, save_puzzle_as,
      analyze_puzzle, quit_puzzle, save_and_quit_puzzle, force_quit_puzzle,
      save_game, restart, show_victory_screen, edit_puzzle_data,
      message_box, close_message_box };
  inline void schedule_action(Action action, std::string argument = "");

  typedef std::function<void()> Callback;
  void message_box(const std::string& message,
                   MessageBoxView::Type type,
                   Callback on_yes, Callback on_no, Callback on_cancel);
  
  void push(std::shared_ptr<View> view) { m_views.push_back(view); }
  void pop();
  bool empty() const { return m_views.empty(); }
  std::size_t size() const { return m_views.size(); }
  
  void update(unsigned ticks, InputHandler& input);
  void draw(Renderer& renderer);

  void refresh();
  void resize(int width, int height);
  int width() const { return m_width; }
  int height() const { return m_height; }

  VideoSystem& video_system() { return m_video; }
  const VideoSystem& video_system() const { return m_video; }

  Renderer& renderer() { return m_renderer; }
  const Renderer& renderer() const { return m_renderer; }
  
  GameSettings& game_settings() { return m_settings; }
  const GameSettings& game_settings() const { return m_settings; }

  SaveManager& save_manager() { return m_save_mgr; }
  const SaveManager& save_manager() const { return m_save_mgr; }

private:
  bool is_save_needed() const;
  bool save(); //returns true if save was successful or unneeded
  
  std::vector<std::shared_ptr<View>> m_views;
  VideoSystem& m_video;
  Renderer& m_renderer;
  GameSettings& m_settings;
  SaveManager m_save_mgr;
  int m_width = 0;
  int m_height = 0;
  enum PuzzleStatus { no_puzzle, puzzle_play, puzzle_edit };
  PuzzleStatus m_puzzle_status = no_puzzle;

  Action m_action = Action::no_action;
  std::string m_action_arg;

  //Message box callbacks
  MessageBoxView::Type m_mbox_type;
  Callback m_mbox_yes;
  Callback m_mbox_no;
  Callback m_mbox_cancel;
  bool m_mbox_open = false;
};


/* implementation */

inline void ViewManager::schedule_action(Action action, std::string argument)
{
  m_action = action;
  m_action_arg = argument;
}

#endif
