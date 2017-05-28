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

#ifndef NONNY_MESSAGE_BOX_VIEW_HPP
#define NONNY_MESSAGE_BOX_VIEW_HPP

#include <functional>
#include <memory>
#include "ui/message_box.hpp"
#include "view/view.hpp"

class Font;

class MessageBoxView : public View {
public:
  enum class Type { okay, yes_no, yes_no_cancel };
  typedef std::function<void()> Callback;
  MessageBoxView(ViewManager& vm, const std::string& message,
                 Type type, Callback on_yes,
                 Callback on_no, Callback on_cancel);

  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(int width, int height) override;

  bool is_transparent() const override { return true; }

private:
  void load_resources();

  MessageBox m_mbox;
  std::unique_ptr<Font> m_text_font;
  std::unique_ptr<Font> m_control_font;
};

#endif
