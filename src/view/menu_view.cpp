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

const Color menu_background_color(123, 175, 212);

void MenuView::update(unsigned ticks, InputHandler& input)
{
  m_main_panel.update(ticks, input);
}

void MenuView::draw(Renderer& renderer)
{
  renderer.set_draw_color(menu_background_color);
  renderer.fill_rect(Rect(0, 0, m_width, m_height));
  m_main_panel.draw(renderer);
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
  m_control_font = m_mgr.video_system().new_font(file, 24);
}

void MenuView::main_menu()
{
  auto menu = std::make_shared<Menu>();

  auto title = std::make_shared<StaticText>(*m_title_font, NONNY_TITLE);
  menu->add_control(title);

  auto logo = std::make_shared<StaticImage>(*m_logo_texture);
  menu->add_control(logo);
  
  auto button = std::make_shared<Button>(*m_control_font, "Play");
  ViewManager& mgr = m_mgr;
  button->register_callback([&mgr]()
                            {
                              auto pview = std::make_shared<PuzzleView>
                                (mgr,
                                 mgr.game_settings().puzzle_dir()
                                 + "easy/test.non");
                              mgr.push(pview);
                              mgr.refresh();
                            });
  menu->add_control(button);

  button = std::make_shared<Button>(*m_control_font, "Create");
  menu->add_control(button);

  button = std::make_shared<Button>(*m_control_font, "About");
  menu->add_control(button);

  menu->position_controls();

  m_main_panel.attach_panel(menu);
  resize(m_width, m_height);
}
