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

#include "view/menu_view.hpp"

#include "config.h"
#include "color/color.hpp"
#include "settings/game_settings.hpp"
#include "ui/button.hpp"
#include "ui/menu.hpp"
#include "ui/static_image.hpp"
#include "ui/static_text.hpp"
#include "video/renderer.hpp"
#include "view/puzzle_view.hpp"
#include "view/view_manager.hpp"

constexpr unsigned menu_button_width = 150;
constexpr unsigned menu_slide_speed = 2000;
const Color menu_background_color(123, 175, 212);

MenuView::MenuView(ViewManager& vm, unsigned width, unsigned height,
                   MenuType menu)
  : View(vm, width, height)
{
  load_resources();
  open_menu(menu);
  start_slide();
}

void MenuView::update(unsigned ticks, InputHandler& input)
{
  if (m_sliding) {
    m_slide_offset += menu_slide_speed * ticks / 1000;
    if (m_slide_offset >= 0) {
      m_slide_offset = 0;
      m_sliding = false;
    }
  }
  
  if (m_action != MenuAction::no_action) {
    switch (m_action) {
    default:
    case MenuAction::no_action:
      break;
    case MenuAction::load_main:
      open_menu(MenuType::main_menu);
      break;
    case MenuAction::load_about:
      open_menu(MenuType::about_menu);
      break;
    }

    m_action = MenuAction::no_action;
  }
    
  m_main_panel.update(ticks, input);
}

void MenuView::draw(Renderer& renderer)
{
  renderer.set_draw_color(menu_background_color);
  renderer.fill_rect(Rect(0, 0, m_width, m_height));

  if (m_sliding)
    renderer.set_viewport(Rect(0, m_slide_offset, m_width, m_height));
  m_main_panel.draw(renderer);
  if (m_sliding)
    renderer.set_viewport();
}

void MenuView::resize(unsigned width, unsigned height)
{
  View::resize(width, height);
  m_main_panel.resize(width, height);
}

void MenuView::load_resources()
{
  std::string file = m_mgr.game_settings().image_dir() + "nonny.png";
  m_logo_texture = m_mgr.video_system().load_image(m_mgr.renderer(), file);

  file = m_mgr.game_settings().font_dir() + "FreeSansBold.ttf";
  m_title_font = m_mgr.video_system().new_font(file, 56);

  file = m_mgr.game_settings().font_dir() + "FreeSans.ttf";
  m_about_font = m_mgr.video_system().new_font(file, 18);
  m_control_font = m_mgr.video_system().new_font(file, 24);
}

void MenuView::start_slide()
{
  m_sliding = true;
  m_slide_offset = -m_main_panel.main_panel().boundary().height();
}

void MenuView::open_menu(MenuType menu)
{
  switch (menu) {
  default:
  case MenuType::main_menu:
    main_menu();
    break;
  case MenuType::in_game_menu:
    in_game_menu();
    break;
  case MenuType::about_menu:
    about_menu();
    break;
  };
}

void MenuView::main_menu()
{
  auto menu = std::make_shared<Menu>();

  auto title = std::make_shared<StaticText>(*m_title_font, NONNY_TITLE);
  menu->add_control(title);

  auto logo = std::make_shared<StaticImage>(*m_logo_texture);
  menu->add_control(logo);
  
  auto button = std::make_shared<Button>(*m_control_font, "Play");
  button->register_callback([this]()
                            {
                              m_mgr.schedule_action
                                (ViewManager::Action::choose_puzzle);
                            });
  button->resize(menu_button_width, button->boundary().height());
  menu->add_control(button);

  button = std::make_shared<Button>(*m_control_font, "Create");
  button->resize(menu_button_width, button->boundary().height());
  menu->add_control(button);

  button = std::make_shared<Button>(*m_control_font, "About");
  button->register_callback([this]()
                            {
                              m_action = MenuAction::load_about;
                            });
  button->resize(menu_button_width, button->boundary().height());
  menu->add_control(button);

  menu->position_controls();
  m_main_panel.attach_panel(menu);
  resize(m_width, m_height);
}

void MenuView::in_game_menu()
{
  auto menu = std::make_shared<Menu>();

  auto title = std::make_shared<StaticText>(*m_title_font, NONNY_TITLE);
  menu->add_control(title);

  auto logo = std::make_shared<StaticImage>(*m_logo_texture);
  menu->add_control(logo);

  auto button = std::make_shared<Button>(*m_control_font, "Return");
  button->register_callback([this]() {
      m_mgr.schedule_action(ViewManager::Action::close_menu); });
  button->resize(menu_button_width, button->boundary().height());
  menu->add_control(button);

  button = std::make_shared<Button>(*m_control_font, "Save Game");
  button->register_callback([this]() {
      m_mgr.schedule_action(ViewManager::Action::save_game); });
  button->resize(menu_button_width, button->boundary().height());
  menu->add_control(button);

  button = std::make_shared<Button>(*m_control_font, "Start Over");
  button->register_callback([this]() {
      m_mgr.schedule_action(ViewManager::Action::restart); });
  button->resize(menu_button_width, button->boundary().height());
  menu->add_control(button);
  
  button = std::make_shared<Button>(*m_control_font, "Quit Puzzle");
  button->register_callback([this]() {
      m_mgr.schedule_action(ViewManager::Action::quit_puzzle); });
  button->resize(menu_button_width, button->boundary().height());
  menu->add_control(button);

  menu->position_controls();
  m_main_panel.attach_panel(menu);
  resize(m_width, m_height);
}

void MenuView::about_menu()
{
  auto about = std::make_shared<Menu>();

  auto title = std::make_shared<StaticText>(*m_title_font, NONNY_TITLE);
  about->add_control(title);

  auto logo = std::make_shared<StaticImage>(*m_logo_texture);
  about->add_control(logo);

  auto text = std::make_shared<StaticText>(*m_about_font,
                                           std::string(NONNY_TITLE)
                                           + " v" + NONNY_VERSION);
  about->add_control(text);

  text = std::make_shared<StaticText>(*m_about_font,
                                      "Copyright © 2017 Gregory Kikola");
  about->add_control(text);

  text = std::make_shared<StaticText>(*m_about_font,
                                      "License GPLv3+: "
                                      "GNU GPL version 3 or later");
  about->add_control(text);

  auto button = std::make_shared<Button>(*m_control_font, "Back");
  button->register_callback([this]()
                            {
                              m_action = MenuAction::load_main;
                            });
  button->resize(menu_button_width, button->boundary().height());
  about->add_control(button);

  about->position_controls();
  m_main_panel.attach_panel(about);
  resize(m_width, m_height);
}
