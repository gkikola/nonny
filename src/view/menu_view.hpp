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

#ifndef NONNY_MENU_VIEW_HPP
#define NONNY_MENU_VIEW_HPP

#include <memory>
#include "ui/scrolling_panel.hpp"
#include "video/font.hpp"
#include "video/texture.hpp"
#include "view/view.hpp"

/* 
 * Menu screen. Presents a menu with options that the user may select.
 */
class MenuView : public View {
public:
  enum class MenuType { main_menu, in_game_menu, edit_menu, about_menu };

  MenuView(ViewManager& vm, MenuType menu = MenuType::main_menu)
    : View(vm) { load_resources(); open_menu(menu); start_slide(); }
  MenuView(ViewManager& vm, int width, int height,
           MenuType menu = MenuType::main_menu);

  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(int width, int height) override;

private:
  void load_resources();
  void start_slide();
  void open_menu(MenuType menu);
  void main_menu();
  void in_game_menu();
  void edit_menu();
  void about_menu();

  enum class MenuAction { no_action, load_main, load_about };

  MenuType m_cur_menu;
  ScrollingPanel m_main_panel;
  MenuAction m_action = MenuAction::no_action;

  std::unique_ptr<Font> m_title_font;
  std::unique_ptr<Font> m_control_font;
  std::unique_ptr<Font> m_about_font;
  std::unique_ptr<Texture> m_logo_texture;

  bool m_sliding = false;
  int m_slide_offset = 0;
};

#endif
