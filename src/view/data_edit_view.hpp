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

#ifndef NONNY_DATA_EDIT_VIEW_HPP
#define NONNY_DATA_EDIT_VIEW_HPP

#include <memory>
#include "ui/option_dialog.hpp"
#include "video/font.hpp"
#include "view/view.hpp"

class Puzzle;
class ViewManager;

class DataEditView : public View {
public:
  DataEditView(ViewManager& vm, Puzzle& puzzle);

  void save_properties();
  
  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(int width, int height) override;

  bool is_transparent() const override { return true; }
  
private:
  void load_resources();
  
  Puzzle& m_puzzle;
  std::unique_ptr<OptionDialog> m_dialog;
  std::unique_ptr<Font> m_control_font;
};

#endif
