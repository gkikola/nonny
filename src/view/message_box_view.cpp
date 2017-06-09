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

#include "view/message_box_view.hpp"

#include "input/input_handler.hpp"
#include "settings/game_settings.hpp"
#include "ui/button.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"
#include "view/view_manager.hpp"

constexpr int button_width = 150;
constexpr int mbox_width = 256;

MessageBoxView::MessageBoxView(ViewManager& vm,
                               const std::string& message,
                               Type type, Callback on_yes, Callback on_no,
                               Callback on_cancel)
  : View(vm)
{
  load_resources();
  m_mbox = MessageBox(*m_control_font, message, mbox_width);
  switch (type) {
  case Type::okay:
    {
      auto button = std::make_shared<Button>(*m_control_font, "Okay");
      button->register_callback(on_yes);
      button->resize(button_width, button->boundary().height());
      m_mbox.add_control(button);
      m_esc_callback = on_yes;
    }
    break;
  case Type::yes_no:
    {
      auto button = std::make_shared<Button>(*m_control_font, "Yes");
      button->register_callback(on_yes);
      button->resize(button_width, button->boundary().height());
      m_mbox.add_control(button);

      button = std::make_shared<Button>(*m_control_font, "No");
      button->register_callback(on_no);
      button->resize(button_width, button->boundary().height());
      m_mbox.add_control(button);

      m_esc_callback = on_no;
    }
    break;
  case Type::yes_no_cancel:
    {   
      auto button = std::make_shared<Button>(*m_control_font, "Yes");
      button->register_callback(on_yes);
      button->resize(button_width, button->boundary().height());
      m_mbox.add_control(button);

      button = std::make_shared<Button>(*m_control_font, "No");
      button->register_callback(on_no);
      button->resize(button_width, button->boundary().height());
      m_mbox.add_control(button);

      button = std::make_shared<Button>(*m_control_font, "Cancel");
      button->register_callback(on_cancel);
      button->resize(button_width, button->boundary().height());
      m_mbox.add_control(button);

      m_esc_callback = on_cancel;
      break;
    }
  }
  resize(m_width, m_height);
}

void MessageBoxView::update(unsigned ticks, InputHandler& input)
{
  if (input.was_key_pressed(Keyboard::Key::escape)) {
    if (m_esc_callback)
      m_esc_callback();
  }    

  m_mbox.update(ticks, input);
}

void MessageBoxView::draw(Renderer& renderer)
{
  m_mbox.draw(renderer);
}

void MessageBoxView::resize(int width, int height)
{
  View::resize(width, height);

  m_mbox.recalculate_size();
  m_mbox.move(m_width / 2 - m_mbox.boundary().width() / 2,
              m_height / 2 - m_mbox.boundary().height() / 2);
  m_mbox.position_controls();
}

void MessageBoxView::load_resources()
{
  std::string file = m_mgr.game_settings().font_dir() + "FreeSans.ttf";
  m_text_font = m_mgr.video_system().new_font(file, 18);
  m_control_font = m_mgr.video_system().new_font(file, 24);
}
