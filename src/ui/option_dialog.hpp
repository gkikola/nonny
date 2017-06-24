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

#ifndef NONNY_OPTION_DIALOG_HPP
#define NONNY_OPTION_DIALOG_HPP

#include <functional>
#include <map>
#include <vector>
#include "ui/dialog.hpp"

class Font;

class OptionDialog : public Dialog {
public:
  OptionDialog(Font& label_font, Font& ctrl_font);

  void position_controls() override;

  void add_control(ControlPtr control) override;
  void add_text_box(const std::string& label, const std::string& id,
                    const std::string& default_text = "");
  void add_size_text_box(const std::string& label,
                         const std::string& cross_label,
                         const std::string& wd_id,
                         const std::string& ht_id,
                         int default_wd = 0, int default_ht = 0);

  typedef std::function<void()> Callback;
  void add_buttons(const std::string& save_label,
                   const std::string& cancel_label,
                   Callback on_save, Callback on_cancel);

  std::string text_box_text(const std::string& id);
  int size_text_box_value(const std::string& id);

  void move(int x, int y) override;

  void draw(Renderer& renderer) const override;

protected:
  void calc_size();

  Font& m_label_font;
  Font& m_control_font;

  std::map<std::string, ControlPtr> m_id_map;
  std::vector<int> m_num_controls;
  int m_label_width = 0;
  int m_line_height = 0;
};

#endif
