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

#include "ui/puzzle_info_panel.hpp"

#include <algorithm>
#include <string>
#include "puzzle/puzzle.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

constexpr unsigned spacing = 8;
constexpr unsigned preview_size = 196;

PuzzleInfoPanel::PuzzleInfoPanel(Font& title_font, Font& info_font,
                                 Font& button_font)
  : m_title_font(title_font),
    m_info_font(info_font),
    m_button_font(button_font)
{
}

PuzzleInfoPanel::PuzzleInfoPanel(Font& title_font, Font& info_font,
                                 Font& button_font, Puzzle& puzzle)
  : m_title_font(title_font),
    m_info_font(info_font),
    m_button_font(button_font)
{
  attach_puzzle(puzzle);
}

void PuzzleInfoPanel::attach_puzzle(Puzzle& puzzle)
{
  m_puzzle = &puzzle;
  m_preview.attach_puzzle(puzzle);
  retrieve_puzzle_info();
  calculate_bounds();
}

void PuzzleInfoPanel::update(unsigned ticks, InputHandler& input,
                             const Rect& active_region)
{
  m_preview.update(ticks, input, active_region);
}

void PuzzleInfoPanel::draw(Renderer& renderer, const Rect& region) const
{
  if (!m_sliding) {
    renderer.set_clip_rect(region);
    renderer.set_draw_color(default_colors::black);

    Point pos(m_boundary.x(), m_boundary.y() + spacing);

    //draw title
    unsigned text_width, text_height;
    m_title_font.text_size(m_puzzle_title, &text_width, &text_height);
    Point text_pos(pos.x() + m_boundary.width() / 2 - text_width / 2, pos.y());
    renderer.draw_text(text_pos, m_title_font, m_puzzle_title);
    pos.y() += text_height + spacing;

    //draw puzzle size
    m_info_font.text_size(m_puzzle_size, &text_width, &text_height);
    text_pos.x() = pos.x() + m_boundary.width() / 2 - text_width / 2;
    text_pos.y() = pos.y();
    renderer.draw_text(text_pos, m_button_font, m_puzzle_size);
    pos.y() += text_height + spacing;

    //leave room for preview
    pos.y() += m_preview.boundary().height() + spacing;

    //draw author
    if (!m_puzzle_author.empty()) {
      m_info_font.text_size(m_puzzle_author, &text_width, &text_height);
      text_pos.x() = pos.x() + m_boundary.width() / 2 - text_width / 2;
      text_pos.y() = pos.y();
      renderer.draw_text(text_pos, m_info_font, m_puzzle_author);
      pos.y() += text_height + spacing;
    }

    //draw preview
    m_preview.draw(renderer, region);
    
    renderer.set_clip_rect();
  }
}

void PuzzleInfoPanel::move(int x, int y)
{
  int old_x = m_boundary.x(), old_y = m_boundary.y();
  UIPanel::move(x, y);
  m_preview.scroll(x - old_x, y - old_y);
}

void PuzzleInfoPanel::retrieve_puzzle_info()
{
  const std::string* property;
  property = m_puzzle->find_property("title");
  if (property)
    m_puzzle_title = *property;
  else
    m_puzzle_title = "Untitled";
  property = m_puzzle->find_property("by");
  if (property)
    m_puzzle_author = "Author: " + *property;

  m_puzzle_size = std::to_string(m_puzzle->width())
    + "\u00D7" + std::to_string(m_puzzle->height());
}

void PuzzleInfoPanel::calculate_bounds()
{
  unsigned width = 0, height = spacing;

  unsigned text_wd, text_ht;
  m_title_font.text_size(m_puzzle_title, &text_wd, &text_ht);
  width = std::max(width, text_wd + 2 * spacing);
  height += text_ht + spacing;

  m_button_font.text_size(m_puzzle_size, &text_wd, &text_ht);
  width = std::max(width, text_wd + 2 * spacing);
  height += text_ht + spacing;

  unsigned preview_pos = m_boundary.y() + height;
  width = std::max(width, preview_size + 2 * spacing);
  height += preview_size + spacing;

  if (!m_puzzle_author.empty()) {
    m_info_font.text_size(m_puzzle_author, &text_wd, &text_ht);
    width = std::max(width, text_wd + 2 * spacing);
    height += text_ht + spacing;
  }

  m_preview.move(m_boundary.x() + width / 2 - preview_size / 2, preview_pos);
  m_preview.resize(preview_size, preview_size);

  resize(width, height);
}
