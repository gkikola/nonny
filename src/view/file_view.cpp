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

#include "view/file_view.hpp"

#include <functional>
#include <vector>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "save/save_manager.hpp"
#include "settings/game_settings.hpp"
#include "video/renderer.hpp"
#include "view/view_manager.hpp"

namespace stdfs = std::experimental::filesystem;

const Color background_color(123, 175, 212);

constexpr unsigned path_spacing = 8;
constexpr unsigned button_spacing = 16;
constexpr unsigned panel_spacing = 32;

FileView::FileView(ViewManager& vm, Mode mode)
  : View(vm), m_mode(mode), m_cur_path(m_paths.end())
{
  load_resources();
  open_default_dir();
}

FileView::FileView(ViewManager& vm, Mode mode,
                   unsigned width, unsigned height)
  : View(vm, width, height), m_mode(mode), m_cur_path(m_paths.end())
{
  load_resources();
  open_default_dir();
  resize(width, height);
}

void FileView::update(unsigned ticks, InputHandler& input)
{
  if (m_cur_path != m_paths.end()) {
    if (input.was_mouse_button_pressed(Mouse::Button::left)
        || input.was_mouse_moved()) {
      int x = m_path_start.x(), y = m_path_start.y();
      unsigned width, height;
      unsigned index = 0;
      bool set_cursor = false;
      for (const auto& e : *m_cur_path) {
        if (index < m_path_collapse_start || index >= m_path_collapse_end) {
          m_filename_font->text_size(e, &width, &height);
          if (Rect(x, y, width, height)
              .contains_point(input.mouse_position())) {
            if (input.was_mouse_button_pressed(Mouse::Button::left))
              open_subdir(index);
            set_cursor = true;
          }
          x += width + path_spacing;
        } else if (index == m_path_collapse_start) {
          m_filename_font->text_size("...", &width, nullptr);
          x += width + path_spacing;
        }

        if (index <= m_path_collapse_start || index >= m_path_collapse_end) {
          m_filename_font->text_size(">", &width, nullptr);
          x += width + path_spacing;
        }
        ++index;
      }

      if (set_cursor && input.cursor() != Mouse::Cursor::hand)
        input.set_cursor(Mouse::Cursor::hand);
      if (!set_cursor && input.cursor() != Mouse::Cursor::arrow)
        input.reset_cursor();
    }
  }

  int mwheel_scroll = input.vert_mouse_wheel_scroll();
  if (mwheel_scroll < 0)
    m_file_selection.smooth_scroll_down();
  else if (mwheel_scroll > 0)
    m_file_selection.smooth_scroll_up();

  if (input.was_key_pressed(Keyboard::Key::tab)) {
    bool back = input.is_key_down(Keyboard::Key::lshift)
      || input.is_key_down(Keyboard::Key::rshift);
    switch_focus(!back);
  }

  m_menu_button->update(ticks, input);
  m_up_button->update(ticks, input);
  m_back_button->update(ticks, input);
  m_forward_button->update(ticks, input);
  m_open_button->update(ticks, input);
  m_filename_box->update(ticks, input);

  m_file_selection.update(ticks, input);

  if (m_need_path_change) {
    m_need_path_change = false;
    open_path(stdfs::path(m_selected_path));
    m_selected_path = "";
  }
}

void FileView::draw(Renderer& renderer)
{
  renderer.set_draw_color(background_color);
  renderer.fill_rect(Rect(0, 0, m_width, m_height));

  if (m_cur_path != m_paths.end()) {
    renderer.set_draw_color(default_colors::black);
    int x = m_path_start.x(), y = m_path_start.y();
    unsigned index = 0;
    for (auto& e : *m_cur_path) {
      Rect r;
      if (index != 0
          && (index <= m_path_collapse_start
              || index >= m_path_collapse_end)) {
        r = renderer.draw_text(Point(x, y), *m_filename_font, ">");
        x += r.width() + path_spacing;
      }
      if (index == m_path_collapse_start
          && index < m_path_collapse_end) { //collapsed - draw ellipsis
        r = renderer.draw_text(Point(x, y), *m_filename_font, "...");
        x += r.width() + path_spacing;
      } else if (index < m_path_collapse_start
                 || index >= m_path_collapse_end) { //not collapsed
        r = renderer.draw_text(Point(x, y), *m_filename_font, e);

        //draw underline
        renderer.draw_line(Point(x, y + r.height()),
                           Point(x + r.width(), y + r.height()));
        x += r.width() + path_spacing;
      }

      ++index;
    }
  }

  m_menu_button->draw(renderer);
  m_up_button->draw(renderer);
  m_back_button->draw(renderer);
  m_forward_button->draw(renderer);
  m_open_button->draw(renderer);
  m_filename_box->draw(renderer);

  renderer.set_draw_color(default_colors::white);
  renderer.fill_rect(m_file_selection.boundary());
  m_file_selection.draw(renderer);
}

void FileView::resize(unsigned width, unsigned height)
{
  View::resize(width, height);

  int x = width - panel_spacing - m_forward_button->boundary().width();
  int y = panel_spacing;
  m_forward_button->move(x, y);

  x -= button_spacing + m_back_button->boundary().width();
  m_back_button->move(x, y);

  x -= button_spacing + m_up_button->boundary().width();
  m_up_button->move(x, y);

  x -= button_spacing + m_menu_button->boundary().width();
  m_menu_button->move(x, y);

  //vertically center path text
  unsigned text_ht = 0;
  m_filename_font->text_size(">", nullptr, &text_ht);
  m_path_start = Point(panel_spacing, panel_spacing);
  m_path_start.y() += m_menu_button->boundary().height() / 2 - text_ht / 2;

  collapse_path();

  unsigned new_width = m_width - 2 * panel_spacing;
  unsigned new_height = m_height - 2 * panel_spacing - button_spacing
    - m_up_button->boundary().height()
    - m_open_button->boundary().height() - button_spacing;
  UIPanel& panel = m_file_selection.main_panel();
  panel.resize(new_width, panel.boundary().height());
  m_file_selection.move(panel_spacing,
                        panel_spacing + m_up_button->boundary().height()
                        + button_spacing);
  m_file_selection.resize(new_width, new_height);
  panel.move(panel.boundary().x(), m_file_selection.boundary().y());

  m_filename_box->move(panel_spacing,
                       m_height - m_open_button->boundary().height()
                       - panel_spacing);
  m_filename_box->resize(m_width - m_open_button->boundary().width()
                         - 2 * panel_spacing - button_spacing,
                         m_filename_box->boundary().height());
  m_open_button->move(m_width
                      - m_open_button->boundary().width()
                      - panel_spacing,
                      m_height - m_open_button->boundary().height()
                      - panel_spacing);
}

void FileView::load_resources()
{
  const GameSettings& settings = m_mgr.game_settings();
  VideoSystem& vs = m_mgr.video_system();

  std::string font_file = settings.font_dir() + "FreeSans.ttf";
  std::string nav_texture_file = settings.image_dir() + "nav.png";
  std::string icon_texture_file = settings.image_dir() + "file.png";
  m_filename_font = vs.new_font(font_file, 20);
  m_info_font = vs.new_font(font_file, 16);
  m_control_font = vs.new_font(font_file, 24);
  m_nav_texture = vs.load_image(m_mgr.renderer(), nav_texture_file);
  m_file_icons_texture = vs.load_image(m_mgr.renderer(), icon_texture_file);

  m_menu_button = std::make_shared<ImageButton>(*m_nav_texture, 0);
  m_up_button = std::make_shared<ImageButton>(*m_nav_texture, 1);
  m_back_button = std::make_shared<ImageButton>(*m_nav_texture, 2);
  m_forward_button = std::make_shared<ImageButton>(*m_nav_texture, 3);

  std::string open_label = m_mode == Mode::open ? "Load" : "Save";
  m_open_button = std::make_shared<Button>(*m_control_font, open_label);

  m_menu_button->register_callback([this]() {
      m_mgr.schedule_action(ViewManager::Action::close_menu); });
  m_up_button->register_callback(std::bind(&FileView::up, this));
  m_back_button->register_callback(std::bind(&FileView::back, this));
  m_forward_button->register_callback(std::bind(&FileView::forward, this));
  
  auto open = [this]() {
    FileSelectionPanel& panel
    = dynamic_cast<FileSelectionPanel&>(m_file_selection.main_panel());
    panel.open_selection();
  };
  m_open_button->register_callback(open);

  m_filename_box = std::make_shared<TextBox>(*m_control_font);

  auto fsv = std::make_shared<FileSelectionPanel>(m_mgr.save_manager(),
                                                  *m_filename_font,
                                                  *m_info_font,
                                                  *m_file_icons_texture);
  fsv->on_dir_change([this](const std::string& p)
                     { m_selected_path = p; m_need_path_change = true; });
  fsv->on_file_open([this](const std::string& f)
                    { m_mgr
                        .schedule_action(ViewManager::Action::load_puzzle,
                                         f); });
  fsv->on_file_select([this](const std::string& f)
                      { handle_selection_change(); });
  m_file_selection.attach_panel(fsv);
}

void FileView::open_path(const stdfs::path& p)
{
  if (m_cur_path == m_paths.end()) {
    m_paths.push_back(p);
    m_cur_path = m_paths.end() - 1;
  } else if (p != *m_cur_path) {
    m_paths.erase(m_cur_path + 1, m_paths.end());
    m_paths.push_back(p);
    m_cur_path = m_paths.end() - 1;
  }

  handle_directory_change();
}

void FileView::back()
{
  if (m_cur_path != m_paths.begin())
    --m_cur_path;
  handle_directory_change();
}

void FileView::forward()
{
  if (m_cur_path != m_paths.end()
      && m_cur_path + 1 != m_paths.end())
    ++m_cur_path;
  handle_directory_change();
}

void FileView::up()
{
  if (m_cur_path != m_paths.end() && m_cur_path->has_parent_path())
    open_path(m_cur_path->parent_path());
}

void FileView::switch_focus(bool fwd) {
  static std::vector<ControlPtr> v = { m_menu_button,
                                       m_up_button,
                                       m_back_button,
                                       m_forward_button,
                                       m_filename_box,
                                       m_open_button };
  bool focus_changed = false;
  for (unsigned i = 0; i < v.size(); ++i) {
    if (v[i]->has_focus()) {
      v[i]->remove_focus();
      unsigned next;
      if (i == 0 && !fwd)
        next = v.size() - 1;
      else if (i == v.size() - 1 && fwd)
        next = 0;
      else
        next = fwd ? i + 1 : i - 1;
      v[next]->give_focus();
      focus_changed = true;
      break;
    }
  }

  if (!focus_changed) {
    if (fwd)
      m_menu_button->give_focus();
    else
      m_filename_box->give_focus();
  }
}

void FileView::open_default_dir()
{
  if (m_mode == Mode::open)
    open_puzzle_dir();
  else if (m_mode == Mode::save)
    open_save_dir();
}

void FileView::open_puzzle_dir()
{
  open_path(stdfs::canonical(stdfs::path(m_mgr.game_settings().puzzle_dir())));
}

void FileView::open_save_dir()
{
  open_path(stdfs::canonical(stdfs::path(m_mgr.game_settings().save_dir())));
}

unsigned FileView::path_name_width() const
{
  unsigned total = 0;
  
  if (m_cur_path != m_paths.end()) {
    unsigned index = 0;
    for (const auto& e : *m_cur_path) {
      unsigned width = 0;
      if (index == m_path_collapse_start
          && index < m_path_collapse_end)
        m_filename_font->text_size("...", &width, nullptr);
      else if (index < m_path_collapse_start
               || index >= m_path_collapse_end)
        m_filename_font->text_size(e.filename(), &width, nullptr);
      if (width > 0) {
        total += width + path_spacing;
        if (index != 0) {
          m_filename_font->text_size(">", &width, nullptr);
          total += width + path_spacing;
        }
      }
      ++index;
    }
  }

  return total;
}

unsigned FileView::path_subdir_count() const
{
  unsigned count = 0;
  if (m_cur_path != m_paths.end()) {
    for (auto it = m_cur_path->begin(); it != m_cur_path->end(); ++it) {
      ++count;
    }
  }
  return count;
}

void FileView::handle_directory_change()
{
  if (m_cur_path != m_paths.end()) {
    //update selection view
    FileSelectionPanel& file_panel
      = dynamic_cast<FileSelectionPanel&>(m_file_selection.main_panel());
    //if path hasn't already been changed, change it
    if (*m_cur_path != file_panel.path())
      file_panel.open_path(*m_cur_path);
    //resize and reposition panels
    unsigned panel_width = m_file_selection.boundary().width();
    unsigned panel_height = m_file_selection.boundary().height();
    file_panel.resize(panel_width, file_panel.boundary().height());
    m_file_selection.resize(panel_width, panel_height); //refresh scrollbars
    file_panel.move(file_panel.boundary().x(),
                    m_file_selection.boundary().y());
  }

  collapse_path();
}

void FileView::handle_selection_change()
{
  FileSelectionPanel& panel
    = dynamic_cast<FileSelectionPanel&>(m_file_selection.main_panel());
  m_filename_box->set_text(panel.selected_file());
  m_filename_box->remove_focus();
}

void FileView::collapse_path()
{    
  if (m_cur_path != m_paths.end()) {
    //max allowed width is screen width minus the four nav buttons
    unsigned max_width = 2 * panel_spacing
      + 4 * (m_up_button->boundary().width() + button_spacing);
    if (max_width < m_width)
      max_width = m_width - max_width;
    else
      max_width = 0;

    //move collapse points until width is within bounds
    unsigned count = path_subdir_count();
    m_path_collapse_start = count / 2;
    m_path_collapse_end = m_path_collapse_start;

    if (max_width == 0) {
      m_path_collapse_start = 0;
      m_path_collapse_end = count;
      return;
    }

    while (path_name_width() > max_width
           && m_path_collapse_end - m_path_collapse_start < count) {
      if (m_path_collapse_end + 1 >= count && m_path_collapse_start > 0)
        --m_path_collapse_start;
      else if (m_path_collapse_start <= 1 && m_path_collapse_end < count)
        ++m_path_collapse_end;
      else {
        if (m_path_collapse_start % 2 == 0 && m_path_collapse_start > 0)
          --m_path_collapse_start;
        else
          ++m_path_collapse_end;
      }
    }
  }
}

void FileView::open_subdir(unsigned index)
{
  if (m_cur_path != m_paths.end()) {
    unsigned count = path_subdir_count();
    stdfs::path p = *m_cur_path;

    while (index + 1 < count) {
      p = p.parent_path();
      ++index;
    }

    open_path(p);
  }
}
