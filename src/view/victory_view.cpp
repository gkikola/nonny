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

#include "view/victory_view.hpp"

#include <sstream>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "puzzle/puzzle.hpp"
#include "settings/game_settings.hpp"
#include "utility/utility.hpp"
#include "video/rect.hpp"
#include "video/renderer.hpp"
#include "view/view_manager.hpp"

const Color background_color(123, 175, 212);
constexpr unsigned spacing = 8;

VictoryView::VictoryView(ViewManager& vm, Puzzle& puzzle,
                         unsigned clear_time, unsigned best_time)
  : View(vm),
    m_puzzle(puzzle),
    m_clear_time(clear_time),
    m_best_time(best_time)
{
  load_resources();
}

VictoryView::VictoryView(ViewManager& vm, Puzzle& puzzle,
                         unsigned clear_time, unsigned best_time,
                         unsigned width, unsigned height)
  : View(vm, width, height),
    m_puzzle(puzzle),
    m_clear_time(clear_time),
    m_best_time(best_time)
{
  load_resources();
}

void VictoryView::update(unsigned ticks, InputHandler& input)
{
  m_preview.update(ticks, input);
  m_back_button->update(ticks, input);
}

void VictoryView::draw(Renderer& renderer)
{
  Point pt(0, spacing);

  renderer.set_draw_color(background_color);
  renderer.fill_rect(Rect(0, 0, m_width, m_height));

  renderer.set_draw_color(default_colors::black);
  
  unsigned text_width, text_height;
  m_title_font->text_size(m_puzzle_title, &text_width, &text_height);
  pt.x() = m_width / 2 - text_width / 2;
  renderer.draw_text(pt, *m_title_font, m_puzzle_title);
  pt.y() += text_height + spacing;

  m_info_font->text_size(m_puzzle_author, &text_width, &text_height);
  pt.x() = m_width / 2 - text_width / 2;
  renderer.draw_text(pt, *m_info_font, m_puzzle_author);
  pt.y() += text_height + spacing;
  
  m_preview.draw(renderer);
  pt.y() += m_preview.boundary().height() + spacing;

  renderer.set_draw_color(default_colors::black);
  renderer.draw_rect(m_preview.boundary());

  m_info_font->text_size(m_times, &text_width, &text_height);
  pt.x() = m_width / 2 - text_width / 2;
  renderer.draw_text(pt, *m_info_font, m_times);
  pt.y() += text_height + spacing;

  m_back_button->draw(renderer);
}

void VictoryView::resize(unsigned width, unsigned height)
{
  View::resize(width, height);

  Point pos(0, spacing);
  
  unsigned text_width, text_height;
  m_title_font->text_size(m_puzzle_title, &text_width, &text_height);
  pos.y() += text_height + spacing;
  m_info_font->text_size(m_puzzle_author, &text_width, &text_height);
  pos.y() += text_height + spacing;

  m_info_font->text_size(m_times, &text_width, &text_height);
  unsigned space = pos.y() + spacing
    + m_back_button->boundary().height() + spacing + text_height + spacing;
  unsigned preview_width = m_width;
  unsigned preview_height = m_height >= space ? m_height - space : 0;
  if (preview_width / m_puzzle.width() > preview_height / m_puzzle.height())
    preview_width = preview_height * m_puzzle.width() / m_puzzle.height();
  else
    preview_height = preview_width * m_puzzle.height() / m_puzzle.width();

  pos.x() = m_width / 2 - preview_width / 2;
  
  m_preview.move(pos.x(), pos.y());
  m_preview.resize(preview_width, preview_height);

  pos.y() += preview_height + spacing;
  pos.y() += text_height + spacing;
  pos.x() = m_width / 2 - m_back_button->boundary().width() / 2;

  m_back_button->move(pos.x(), pos.y());
}

void VictoryView::load_resources()
{
  m_preview.attach_puzzle(m_puzzle);

  const std::string* property = m_puzzle.find_property("title");
  if (property)
    m_puzzle_title = *property;
  else
    m_puzzle_title = "Untitled";
  property = m_puzzle.find_property("by");
  if (property && !property->empty())
    m_puzzle_author = "by " + *property;

  std::ostringstream oss;
  oss << "Clear time: ";
  write_time(oss, m_clear_time, true) << "    ";
  oss << "Best time: ";
  write_time(oss, m_best_time, true);
  if (m_clear_time < m_best_time)
    oss << " (New record!)";
  m_times = oss.str();

  GameSettings& settings = m_mgr.game_settings();
  std::string font_file = settings.font_dir() + "FreeSans.ttf";
  std::string bold_font_file = settings.font_dir() + "FreeSansBold.ttf";
  m_title_font = m_mgr.video_system().new_font(bold_font_file, 32);
  m_info_font = m_mgr.video_system().new_font(font_file, 24);

  m_back_button = std::make_shared<Button>(*m_info_font, "Back");
  m_back_button->register_callback([this]() {
      m_mgr.schedule_action(ViewManager::Action::quit_puzzle); });
}
