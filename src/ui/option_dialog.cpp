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

#include "ui/option_dialog.hpp"

#include <algorithm>
#include <string>
#include "color/color.hpp"
#include "ui/button.hpp"
#include "ui/static_text.hpp"
#include "ui/text_box.hpp"
#include "video/renderer.hpp"

constexpr int spacing = 12;
constexpr int text_box_width = 384;
constexpr int size_text_box_width = 64;
constexpr int button_width = 150;
const Color background_color(192, 192, 192);

OptionDialog::OptionDialog(Font& label_font, Font& ctrl_font)
  : m_label_font(label_font), m_control_font(ctrl_font)
{
}

void OptionDialog::position_controls()
{
  m_need_reposition = false;
  calc_size();

  int num_lines = m_num_controls.size();
  unsigned pos = 0;
  int x = spacing, y = spacing;
  for (int i = 0; i < num_lines; ++i) {
    x = spacing;
    int line_height = 0;
    for (int j = 0; j < m_num_controls[i]; ++j, ++pos) {
      if (pos < m_controls.size()) {
        //control label should be right-aligned
        if (j == 0 && typeid(*m_controls[pos]) == typeid(StaticText))
          x = m_label_width - m_controls[pos]->boundary().width() - spacing;
        else if (j == 1) //second control should be left-aligned
          x = m_label_width;

        //position and vertically center control
        int yoffset = m_line_height / 2
          - m_controls[pos]->boundary().height() / 2;
        m_controls[pos]->move(x, y + yoffset);
        x += m_controls[pos]->boundary().width() + spacing;
        line_height = std::max(line_height,
                               m_controls[pos]->boundary().height());
      }
    }

    y += line_height + spacing;
  }

  //buttons on last line should be centered
  if (!m_num_controls.empty()) {
    int count = m_num_controls[m_num_controls.size() - 1];
    if (count > 0
        && typeid(*m_controls[m_controls.size() - 1])
        == typeid(Button)) {
      int x = m_boundary.width() / 2
        - (count * (button_width + spacing) - spacing) / 2;
      for (int i = m_controls.size() - count;
           i < static_cast<int>(m_controls.size());
           ++i) {
        m_controls[i]->move(x, m_controls[i]->boundary().y());
        x += m_controls[i]->boundary().width() + spacing;
      }
    }
  }
}

void OptionDialog::add_control(ControlPtr control)
{
  Dialog::add_control(control);
  m_num_controls.push_back(1);
}

void OptionDialog::add_text_box(const std::string& label,
                                const std::string& id,
                                const std::string& default_text)
{
  ControlPtr label_ctrl = make_control<StaticText>(m_label_font, label);
  auto text_box = std::make_shared<TextBox>(m_control_font);

  text_box->resize(text_box_width, text_box->boundary().height());
  text_box->set_text(default_text);

  m_controls.push_back(label_ctrl);
  m_controls.push_back(text_box);
  m_focused = m_controls.end();
  m_need_reposition = true;

  m_num_controls.push_back(2);
  m_id_map[id] = text_box;
}

void OptionDialog::add_size_text_box(const std::string& label,
                                     const std::string& cross_label,
                                     const std::string& wd_id,
                                     const std::string& ht_id,
                                     int default_wd, int default_ht)
{
  //width box
  ControlPtr label_ctrl = make_control<StaticText>(m_label_font, label);
  auto text_box = std::make_shared<TextBox>(m_control_font);
  text_box->resize(size_text_box_width, text_box->boundary().height());
  text_box->set_text(std::to_string(default_wd));
  m_controls.push_back(label_ctrl);
  m_controls.push_back(text_box);
  m_id_map[wd_id] = text_box;

  //height box
  label_ctrl = make_control<StaticText>(m_label_font, cross_label);
  text_box = std::make_shared<TextBox>(m_control_font);
  text_box->resize(size_text_box_width, text_box->boundary().height());
  text_box->set_text(std::to_string(default_ht));
  m_controls.push_back(label_ctrl);
  m_controls.push_back(text_box);
  m_id_map[ht_id] = text_box;
  m_focused = m_controls.end();
  m_need_reposition = true;

  m_num_controls.push_back(4);
}

void OptionDialog::add_buttons(const std::string& save_label,
                               const std::string& cancel_label,
                               Callback on_save, Callback on_cancel)
{
  auto button = std::make_shared<Button>(m_control_font, save_label);
  button->register_callback(on_save);
  button->resize(button_width, button->boundary().height());
  m_controls.push_back(button);

  button = std::make_shared<Button>(m_control_font, cancel_label);
  button->register_callback(on_cancel);
  button->resize(button_width, button->boundary().height());
  m_controls.push_back(button);
  m_focused = m_controls.end();
  m_need_reposition = true;

  m_num_controls.push_back(2);
}

std::string OptionDialog::text_box_text(const std::string& id)
{
  auto control = std::dynamic_pointer_cast<TextBox>(m_id_map.at(id));
  if (control)
    return control->get_text();
  else
    return "";
}

int OptionDialog::size_text_box_value(const std::string& id)
{
  auto control = std::dynamic_pointer_cast<TextBox>(m_id_map.at(id));
  try {
    if (control)
      return std::stoi(control->get_text());
    else
      return 1;
  } catch (std::invalid_argument) {
    return 1;
  }
}

void OptionDialog::move(int x, int y)
{
  int old_x = m_boundary.x();
  int old_y = m_boundary.y();
  UIPanel::move(x, y);
  for (auto ctrl : m_controls)
    ctrl->scroll(x - old_x, y - old_y);
}

void OptionDialog::draw(Renderer& renderer) const
{
  renderer.set_draw_color(background_color);
  renderer.fill_rect(m_boundary);
  renderer.set_draw_color(default_colors::black);
  renderer.draw_rect(m_boundary);
  Dialog::draw(renderer);
}

void OptionDialog::calc_size()
{
  m_label_width = spacing;
  m_line_height = 0;
  int width = 0, height = spacing;
  int lines = m_num_controls.size();
  unsigned ctrl_pos = 0;
  for (int i = 0; i < lines; ++i) {
    int line_width = spacing;
    int line_height = 0;
    for (int j = 0; j < m_num_controls[i]; ++j, ++ctrl_pos) {
      if (ctrl_pos < m_controls.size()) {
        line_width += m_controls[ctrl_pos]->boundary().width() + spacing;
        line_height = std::max(line_height,
                               m_controls[ctrl_pos]->boundary().height());

        if (j == 0 && m_num_controls[i] > 1
            && typeid(*m_controls[ctrl_pos]) != typeid(Button))
          m_label_width = std::max(m_label_width, line_width);
      }
    }

    width = std::max(width, line_width);
    height += line_height + spacing;
    m_line_height = std::max(m_line_height, line_height);
  }

  resize(width, height);
}
