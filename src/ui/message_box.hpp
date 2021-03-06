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

#ifndef NONNY_MESSAGE_BOX_HPP
#define NONNY_MESSAGE_BOX_HPP

#include <string>
#include "ui/dialog.hpp"

class Font;

/*
 * Represents a dialog box with some control buttons that displays a
 * message.
 */
class MessageBox : public Dialog {
public:
  MessageBox() = default;
  MessageBox(const Font& text_font, const std::string& text,
             int width);

  void position_controls() override;
  void recalculate_size();

  using Dialog::draw;
  void draw(Renderer& renderer, const Rect& region) const override;

protected:
  bool allow_arrow_focus() const override { return true; }

private:
  const Font* m_text_font;
  std::string m_text;
};

#endif
