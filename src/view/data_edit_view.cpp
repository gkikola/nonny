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

#include "view/data_edit_view.hpp"

#include "input/input_handler.hpp"
#include "puzzle/puzzle.hpp"
#include "settings/game_settings.hpp"
#include "video/font.hpp"
#include "view/view_manager.hpp"

DataEditView::DataEditView(ViewManager& vm, Puzzle& puzzle)
  : View(vm), m_puzzle(puzzle)
{
  load_resources();
}

void DataEditView::save_properties()
{
  std::string title = m_dialog->text_box_text("title");
  std::string author = m_dialog->text_box_text("author");
  std::string copyright = m_dialog->text_box_text("copyright");
  std::string collection = m_dialog->text_box_text("collection");

  if (!title.empty())
    m_puzzle.set_property("title", title);
  if (!author.empty())
    m_puzzle.set_property("by", author);
  if (!copyright.empty())
    m_puzzle.set_property("copyright", copyright);
  if (!collection.empty())
    m_puzzle.set_property("collection", collection);

  int new_width = m_dialog->size_text_box_value("width");
  int new_height = m_dialog->size_text_box_value("height");
  if (new_width != m_puzzle.width()
      || new_height != m_puzzle.height())
    m_puzzle.resize(new_width, new_height);
}

void DataEditView::update(unsigned ticks, InputHandler& input)
{
  if (input.was_key_pressed(Keyboard::Key::escape))
    m_mgr.schedule_action(ViewManager::Action::close_menu);
  
  m_dialog->update(ticks, input);
}

void DataEditView::draw(Renderer& renderer)
{
  m_dialog->draw(renderer);
}

void DataEditView::resize(int width, int height)
{
  m_dialog->move(width / 2 - m_dialog->boundary().width() / 2,
                 height / 2 - m_dialog->boundary().height() / 2);
}

void DataEditView::load_resources()
{
  std::string file = m_mgr.game_settings().font_dir()
    + m_mgr.game_settings().filesystem_separator() + "FreeSans.ttf";
  m_control_font = m_mgr.video_system().new_font(file, 24);

  m_dialog = std::make_unique<OptionDialog>(*m_control_font,
                                            *m_control_font);

  std::string title, author, copyright, collection;
  const std::string* prop = m_puzzle.find_property("title");
  if (prop) title = *prop;
  prop = m_puzzle.find_property("by");
  if (prop) author = *prop;
  prop = m_puzzle.find_property("copyright");
  if (prop) copyright = *prop;
  prop = m_puzzle.find_property("collection");
  if (prop) collection = *prop;
  
  m_dialog->add_text_box("Title:", "title", title);
  m_dialog->add_text_box("Author:", "author", author);
  m_dialog->add_size_text_box("Size:", u8"\u00D7", "width", "height",
                              m_puzzle.width(), m_puzzle.height());
  m_dialog->add_text_box("Copyright:", "copyright", copyright);
  m_dialog->add_text_box("Collection:", "collection", collection);
  m_dialog
    ->add_buttons("Save", "Cancel",
                  [this]() {
                    m_mgr.schedule_action(ViewManager
                                          ::Action::save_properties); },
                  [this]() {
                    m_mgr.schedule_action(ViewManager::Action::close_menu); });
  m_dialog->position_controls();
}
