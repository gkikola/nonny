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
#include "input/input_handler.hpp"
#include "puzzle/puzzle.hpp"
#include "ui/tooltip.hpp"
#include "utility/utility.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"
#include "video/texture.hpp"

constexpr int spacing = 8;
constexpr int preview_width = 196;

PuzzleInfoPanel::PuzzleInfoPanel(Font& title_font,
                                 Font& info_font,
                                 Font& size_font,
                                 Texture& ctrl_texture,
                                 Texture& arrow_texture,
                                 Texture& draw_texture,
                                 int max_width,
                                 bool edit_mode)
  : m_title_font(title_font),
    m_info_font(info_font),
    m_size_font(size_font),
    m_ctrl_texture(ctrl_texture),
    m_arrow_texture(arrow_texture),
    m_draw_texture(draw_texture),
    m_tool_selector(draw_texture),
    m_max_width(max_width),
    m_edit_mode(edit_mode)
{
  setup_buttons();
}

void PuzzleInfoPanel::attach_puzzle(Puzzle& puzzle)
{
  m_puzzle = &puzzle;
  m_preview.attach_puzzle(puzzle);
  m_color_selector.set_palette(puzzle.palette());
  retrieve_puzzle_info();
  calculate_bounds();
}

void PuzzleInfoPanel::setup_buttons()
{
  if (m_buttons.empty())
    m_buttons.insert(m_buttons.begin(), num_buttons, nullptr);

  if (!m_buttons[menu])
    m_buttons[menu] = std::make_shared<ImageButton>(m_ctrl_texture, 0);
  if (!m_buttons[zoom_in])
    m_buttons[zoom_in] = std::make_shared<ImageButton>(m_ctrl_texture, 1);
  if (!m_buttons[zoom_out])
    m_buttons[zoom_out] = std::make_shared<ImageButton>(m_ctrl_texture, 2);

  if (m_edit_mode)
    m_buttons[hint] = std::make_shared<ImageButton>(m_ctrl_texture, 4);
  else
    m_buttons[hint] = std::make_shared<ImageButton>(m_ctrl_texture, 3);

  if (!m_buttons[save])
    m_buttons[save] = std::make_shared<ImageButton>(m_ctrl_texture, 5);
  if (!m_buttons[undo])
    m_buttons[undo] = std::make_shared<ImageButton>(m_ctrl_texture, 6);
  if (!m_buttons[redo])
    m_buttons[redo] = std::make_shared<ImageButton>(m_ctrl_texture, 7);
  if (!m_buttons[analyze])
    m_buttons[analyze] = std::make_shared<ImageButton>(m_ctrl_texture, 8);

  if (m_edit_mode) {
    m_buttons[up] = std::make_shared<ImageButton>(m_arrow_texture, 3);
    m_buttons[left] = std::make_shared<ImageButton>(m_arrow_texture, 4);
    m_buttons[right] = std::make_shared<ImageButton>(m_arrow_texture, 5);
    m_buttons[down] = std::make_shared<ImageButton>(m_arrow_texture, 6);
  } else {
    m_buttons[up] = nullptr;
    m_buttons[left] = nullptr;
    m_buttons[right] = nullptr;
    m_buttons[down] = nullptr;
  }
}

void PuzzleInfoPanel::on_hint_toggle(Callback fn)
{
  m_hint_callback = fn;
  if (!m_edit_mode)
    m_buttons[hint]->register_callback(fn);
}

void PuzzleInfoPanel::on_clear_puzzle(Callback fn)
{
  m_clear_callback = fn;
  if (m_edit_mode)
    m_buttons[hint]->register_callback(fn);
}

void PuzzleInfoPanel::on_left(Callback fn)
{
  m_left_callback = fn;
  if (m_buttons[left])
    m_buttons[left]->register_callback(fn);
}

void PuzzleInfoPanel::on_right(Callback fn)
{
  m_right_callback = fn;
  if (m_buttons[right])
    m_buttons[right]->register_callback(fn);
}

void PuzzleInfoPanel::on_up(Callback fn)
{
  m_up_callback = fn;
  if (m_buttons[up])
    m_buttons[up]->register_callback(fn);
}

void PuzzleInfoPanel::on_down(Callback fn)
{
  m_down_callback = fn;
  if (m_buttons[down])
    m_buttons[down]->register_callback(fn);
}

void PuzzleInfoPanel::on_data_edit_request(Callback fn)
{
  m_data_edit_callback = fn;
}

void PuzzleInfoPanel::set_edit_mode(bool edit_mode)
{
  if (m_edit_mode != edit_mode) {
    m_edit_mode = edit_mode;

    setup_buttons();
    calculate_bounds();
    
    //switch between hint and clear buttons
    if (edit_mode) {
      m_buttons[hint]->register_callback(m_clear_callback);
      m_buttons[up]->register_callback(m_up_callback);
      m_buttons[down]->register_callback(m_down_callback);
      m_buttons[left]->register_callback(m_left_callback);
      m_buttons[right]->register_callback(m_right_callback);
    } else {
      m_buttons[hint]->register_callback(m_hint_callback);
    }
  }
}

Color PuzzleInfoPanel::active_color() const
{
  return m_color_selector.selected_color();
}

PuzzlePanel::DrawTool PuzzleInfoPanel::active_draw_tool() const
{
  return m_tool_selector.selected_tool();
}

void PuzzleInfoPanel::refresh_puzzle_properties()
{
  retrieve_puzzle_info();
  calculate_bounds();
}

void PuzzleInfoPanel::update(unsigned ticks, InputHandler& input,
                             const Rect& active_region)
{
  m_time += ticks;
  m_preview.update(ticks, input, active_region);
  m_color_selector.update(ticks, input, active_region);
  m_tool_selector.update(ticks, input, active_region);
  for (auto& button : m_buttons) {
    if (button)
      button->update(ticks, input, active_region);
  }

  if (m_edit_mode && input.was_mouse_button_pressed(Mouse::Button::left)) {
    Rect area = m_boundary;
    area.height() = (m_preview.boundary().y() - area.y()) - spacing;
    if (area.contains_point(input.mouse_position())
        && m_data_edit_callback)
      m_data_edit_callback();
  }
}

void PuzzleInfoPanel::draw(Renderer& renderer, const Rect& region) const
{
  if (!m_sliding) {
    renderer.set_clip_rect(region);
    renderer.set_draw_color(default_colors::black);

    Point pos(m_boundary.x(), m_boundary.y() + spacing);

    //draw title
    int text_width, text_height;
    m_title_font.text_size_wrapped(m_puzzle_title, m_boundary.width(),
                                   &text_width, &text_height);
    Point text_pos(pos.x(), pos.y());

    if (m_edit_mode || m_show_title
        || m_puzzle_title == "Untitled") {
      renderer.draw_text_wrapped(text_pos, m_title_font, m_puzzle_title,
                                 m_boundary.width(), true);
    } else {
      renderer.draw_text_wrapped(text_pos, m_title_font, "???",
                         m_boundary.width(), true);
    }

    pos.y() += text_height + spacing;

    //draw author
    if (!m_puzzle_author.empty()) {
      m_size_font.text_size_wrapped(m_puzzle_author, m_boundary.width(),
                                    &text_width, &text_height);
      text_pos.x() = pos.x();
      text_pos.y() = pos.y();
      renderer.draw_text_wrapped(text_pos, m_size_font, m_puzzle_author,
                                 m_boundary.width(), true);
      pos.y() += text_height + spacing;
    }

    //draw puzzle size
    m_size_font.text_size(m_puzzle_size, &text_width, &text_height);
    text_pos.x() = pos.x() + m_boundary.width() / 2 - text_width / 2;
    text_pos.y() = pos.y();
    renderer.draw_text(text_pos, m_size_font, m_puzzle_size);
    pos.y() += text_height + spacing;

    //draw time
    if (!m_edit_mode) {
      std::string time_str = time_to_string(m_time);
      m_info_font.text_size(time_str, &text_width, &text_height);
      text_pos.x() = pos.x() + m_boundary.width() / 2 - text_width / 2;
      text_pos.y() = pos.y();
      renderer.draw_text(text_pos, m_info_font, time_str);
      pos.y() += text_height + spacing;
    } else { //edit message
      std::string edit_str = "(Click to edit)";
      m_info_font.text_size(edit_str, &text_width, &text_height);
      text_pos.x() = pos.x() + m_boundary.width() / 2 - text_width / 2;
      text_pos.y() = pos.y();
      renderer.draw_text(text_pos, m_info_font, edit_str);
      pos.y() += text_height + spacing;
    }

    //draw preview
    m_preview.draw(renderer, region);
    pos.y() += m_preview.boundary().height() + spacing;

    //draw color/tool selectors
    if (m_edit_mode || m_puzzle->is_multicolor()) {
      m_color_selector.draw(renderer, region);
      pos.y() += m_color_selector.boundary().height() + spacing;
    }
    if (m_edit_mode) {
      m_tool_selector.draw(renderer, region);
      pos.y() += m_tool_selector.boundary().height() + spacing;
    }

    //draw buttons
    for (auto& button : m_buttons) {
      if (button)
        button->draw(renderer, region);
    }
    pos.y() += 2 * m_buttons[0]->boundary().height() + 2 * spacing;

    renderer.set_clip_rect();
  }
}

void PuzzleInfoPanel::draw_tooltips(Renderer& renderer,
                                    int screen_width,
                                    int screen_height) const
{
  const int tt_spacing = 2;
  std::string tooltip;
  Rect bound;
  if (m_buttons[menu] && m_buttons[menu]->is_mouse_over()) {
    tooltip = "Open menu";
    bound = m_buttons[menu]->boundary();
  } else if (m_buttons[zoom_in] && m_buttons[zoom_in]->is_mouse_over()) {
    tooltip = "Zoom in";
    bound = m_buttons[zoom_in]->boundary();
  } else if (m_buttons[zoom_out] && m_buttons[zoom_out]->is_mouse_over()) {
    tooltip = "Zoom out";
    bound = m_buttons[zoom_out]->boundary();
  } else if (m_buttons[hint] && m_buttons[hint]->is_mouse_over()
             && m_edit_mode) {
    tooltip = "Clear puzzle";
    bound = m_buttons[hint]->boundary();
  } else if (m_buttons[hint] && m_buttons[hint]->is_mouse_over()
             && !m_edit_mode) {
    tooltip = "Show hints";
    bound = m_buttons[hint]->boundary();
  } else if (m_buttons[save] && m_buttons[save]->is_mouse_over()
             && m_edit_mode) {
    tooltip = "Save puzzle";
    bound = m_buttons[save]->boundary();
  } else if (m_buttons[save] && m_buttons[save]->is_mouse_over()
             && !m_edit_mode) {
    tooltip = "Save progress";
    bound = m_buttons[save]->boundary();
  } else if (m_buttons[undo] && m_buttons[undo]->is_mouse_over()) {
    tooltip = "Undo";
    bound = m_buttons[undo]->boundary();
  } else if (m_buttons[redo] && m_buttons[redo]->is_mouse_over()) {
    tooltip = "Redo";
    bound = m_buttons[redo]->boundary();
  } else if (m_buttons[analyze] && m_buttons[analyze]->is_mouse_over()) {
    tooltip = "Solve and analyze";
    bound = m_buttons[analyze]->boundary();
  } else if (m_buttons[left] && m_buttons[left]->is_mouse_over()) {
    tooltip = "Shift puzzle left";
    bound = m_buttons[left]->boundary();
  } else if (m_buttons[right] && m_buttons[right]->is_mouse_over()) {
    tooltip = "Shift puzzle right";
    bound = m_buttons[right]->boundary();
  } else if (m_buttons[up] && m_buttons[up]->is_mouse_over()) {
    tooltip = "Shift puzzle up";
    bound = m_buttons[up]->boundary();
  } else if (m_buttons[down] && m_buttons[down]->is_mouse_over()) {
    tooltip = "Shift puzzle down";
    bound = m_buttons[down]->boundary();
  }

  if (!tooltip.empty()) {
    int text_ht;
    m_info_font.text_size(tooltip, nullptr, &text_ht);
    Point pt(bound.x(), bound.y() + bound.height() + tt_spacing);

    if (pt.y() + text_ht >= screen_height)
      pt.y() = bound.y() - text_ht - tt_spacing * 3;

    draw_tooltip(renderer, pt, m_info_font, tooltip);
  }
}

void PuzzleInfoPanel::move(int x, int y)
{
  int old_x = m_boundary.x(), old_y = m_boundary.y();
  UIPanel::move(x, y);
  m_preview.scroll(x - old_x, y - old_y);
  m_color_selector.scroll(x - old_x, y - old_y);
  m_tool_selector.scroll(x - old_x, y - old_y);
  for (auto& button : m_buttons) {
    if (button)
      button->scroll(x - old_x, y - old_y);
  }
}

void PuzzleInfoPanel::retrieve_puzzle_info()
{
  const std::string* property;
  property = m_puzzle->find_property("title");
  if (property) {
    m_puzzle_title = *property;
  } else {
    m_puzzle_title = "Untitled";
  }
  m_puzzle_author = "";
  property = m_puzzle->find_property("by");
  if (property)
    m_puzzle_author = "by " + *property;

  m_puzzle_size = std::to_string(m_puzzle->width())
    + "\u00D7" + std::to_string(m_puzzle->height());
  m_time = 0;
}

void PuzzleInfoPanel::calculate_bounds()
{
  if (m_puzzle) {
    int width = 0, height = spacing;

    int text_wd, text_ht;
    m_title_font.text_size_wrapped(m_puzzle_title, m_max_width,
                                   &text_wd, &text_ht);
    width = std::max(width, text_wd + 2 * spacing);
    height += text_ht + spacing;

    if (!m_puzzle_author.empty()) {
      m_size_font.text_size_wrapped(m_puzzle_author, m_max_width,
                                    &text_wd, &text_ht);
      width = std::max(width, text_wd + 2 * spacing);
      height += text_ht + spacing;
    }

    m_size_font.text_size(m_puzzle_size, &text_wd, &text_ht);
    width = std::max(width, text_wd + 2 * spacing);
    height += text_ht + spacing;

    if (!m_edit_mode) {
      m_info_font.text_size("00:00.0", &text_wd, &text_ht);
      width = std::max(width, text_wd + 2 * spacing);
      height += text_ht + spacing;
    } else {
      m_info_font.text_size("(Click to edit)", &text_wd, &text_ht);
      width = std::max(width, text_wd + 2 * spacing);
      height += text_ht + spacing;
    }

    int preview_pos = m_boundary.y() + height;
    int preview_height = preview_width
      * m_puzzle->height() / m_puzzle->width();
    if (preview_height > preview_width)
      preview_height = preview_width;
    width = std::max(width, preview_width + 2 * spacing);
    height += preview_height + spacing;

    int color_sel_pos = m_boundary.y() + height;
    if (m_edit_mode || m_puzzle->is_multicolor()) {
      m_color_selector.move(m_boundary.x(), color_sel_pos);
      m_color_selector.set_width(width);
      height += m_color_selector.boundary().height() + spacing;
    }

    if (m_edit_mode) {
      int tool_sel_pos = m_boundary.y() + height;
      int tool_sel_width = m_tool_selector.boundary().width();
      m_tool_selector.move(m_boundary.x() + width / 2
                           - tool_sel_width / 2, tool_sel_pos);
      height += m_tool_selector.boundary().height() + spacing;
    }

    Point button_pos(m_boundary.x(), m_boundary.y() + height);
    int button_width = m_buttons[menu]->boundary().width();
    int button_height = m_buttons[menu]->boundary().height();
    int button_group_width = 4 * button_width + 3 * spacing;
    int buttons_per_line = 4;
    height += (button_height + spacing)
      * (m_buttons.size() / buttons_per_line - 1);
    if (m_edit_mode)
      height += button_height + spacing;

    button_pos.x() += width / 2 - button_group_width / 2;
    for (int i = 0; i < static_cast<int>(m_buttons.size()); ++i) {
      if (m_buttons[i]) {
        m_buttons[i]->move(button_pos.x() + (button_width + spacing)
                           * (i % buttons_per_line),
                           button_pos.y() + (button_height + spacing)
                           * (i / buttons_per_line));
      }
    }

    m_preview.move(m_boundary.x() + width / 2 - preview_width / 2,
                   preview_pos);
    m_preview.resize(preview_width, preview_height);

    resize(width, height);
  }
}
