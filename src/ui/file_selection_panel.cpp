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

#include "ui/file_selection_panel.hpp"

#include <algorithm>
#include <fstream>
#include <experimental/filesystem>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "puzzle/puzzle_io.hpp"
#include "save/save_manager.hpp"
#include "utility/utility.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"
#include "video/texture.hpp"

namespace stdfs = std::experimental::filesystem;

const Color background_color = default_colors::white;
const Color foreground_color = default_colors::black;
const Color selection_color = default_colors::blue;
constexpr unsigned spacing = 4;
constexpr unsigned max_puzzles_open = 9999;

FileSelectionPanel::FileSelectionPanel(SaveManager& save_mgr,
                                       Font& filename_font, Font& info_font,
                                       Texture& icons,
                                       const std::string& path)
  : m_save_mgr(save_mgr),
    m_filename_font(filename_font),
    m_info_font(info_font),
    m_icon_texture(icons),
    m_path(path)
{
  load_file_list();
}

void FileSelectionPanel::open_path(const std::string& path)
{
  if (m_path != path) {
    m_path = path;
    if (m_dir_callback)
      m_dir_callback(path);
    load_file_list();
  }
}

void FileSelectionPanel::open_file(const std::string& file)
{
  if (m_file_open_callback)
    m_file_open_callback(file);
}

void FileSelectionPanel::open_selection()
{
  if (m_is_selected) {
    if (m_files[m_selection].type == FileInfo::Type::directory)
      open_path(m_files[m_selection].full_path);
    else
      open_file(m_files[m_selection].full_path);
  }
}

std::string FileSelectionPanel::selected_file() const
{
  if (m_is_selected)
    return m_files[m_selection].filename;
  else
    return "";
}

void FileSelectionPanel::on_file_select(Callback fn)
{
  m_file_sel_callback = fn;
}

void FileSelectionPanel::on_file_open(Callback fn)
{
  m_file_open_callback = fn;
}

void FileSelectionPanel::on_dir_change(Callback fn)
{
  m_dir_callback = fn;
}

void FileSelectionPanel::update(unsigned ticks, InputHandler& input,
                                const Rect& active_region)
{
  if (!m_files.empty()) {
    //see if we have new puzzle to read
    if (m_num_puzzles_loaded < m_files.size()
        && m_num_puzzles_loaded < max_puzzles_open)
      load_puzzle_info();
    
    //check for mouse click to select a file
    Point cursor = input.mouse_position();
    if ((input.was_mouse_button_pressed(Mouse::Button::left)
         || input.was_mouse_button_pressed(Mouse::Button::right))
        && active_region.contains_point(cursor)) {
      m_is_selected = false;
      unsigned index = (cursor.y() - m_boundary.y()) / entry_height();
      if (index < m_files.size()) {
        select(index);

        //handle double-clicks
        bool dbl_click
          = input.was_mouse_button_double_clicked(Mouse::Button::left)
          || input.was_mouse_button_double_clicked(Mouse::Button::right);
        if (dbl_click) {
          if (m_files[index].type == FileInfo::Type::directory)
            open_path(m_files[index].full_path);
          else
            open_file(m_files[index].full_path);
        }
      }
    }

    //change selection with arrow keys
    if (input.was_key_pressed(Keyboard::Key::down)
        || input.was_key_pressed(Keyboard::Key::kp_down)) {
      if (!m_is_selected) {
        select(0);
      } else {
        if (m_selection + 1 >= m_files.size())
          select(0);
        else
          select(m_selection + 1);
      }
    } else if (input.was_key_pressed(Keyboard::Key::up)
               || input.was_key_pressed(Keyboard::Key::kp_up)) {
      if (!m_is_selected)
        select(m_files.size() - 1);
      else {
        if (m_selection > 0)
          select(m_selection - 1);
        else
          select(m_files.size() - 1);
      }
    }

    //handle selection with enter key
    if (input.was_key_pressed(Keyboard::Key::enter)
        || input.was_key_pressed(Keyboard::Key::kp_enter)) {
      if (m_is_selected) {
        if (m_files[m_selection].type == FileInfo::Type::directory)
          open_path(m_files[m_selection].full_path);
        else
          open_file(m_files[m_selection].full_path);
      }
    }
  }
}

void FileSelectionPanel::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_clip_rect(region);

  //figure out which file entries are on screen
  int min_y = region.y() - m_boundary.y();
  if (min_y < 0) min_y = 0;
  int max_y = min_y + region.height();
  unsigned min_index, max_index;
  if (m_files.empty()) {
    min_index = max_index = 0;
  } else {
    min_index = min_y / entry_height();
    if (min_index > 0)
      --min_index;
    if (min_index > m_files.size())
      min_index = m_files.size() - 1;

    max_index = max_y / entry_height() + 1;
    if (max_index > m_files.size())
      max_index = m_files.size();
  }

  //draw the file entries
  for (unsigned i = min_index; i < max_index; ++i) {
    //background
    if (m_is_selected && m_selection == i)
      renderer.set_draw_color(selection_color);
    else
      renderer.set_draw_color(background_color);
    renderer.fill_rect(Rect(m_boundary.x(),
                            m_boundary.y() + i * entry_height(),
                            m_boundary.width(),
                            entry_height()));

    int x = m_boundary.x() + spacing;
    int y = m_boundary.y() + i * entry_height() + spacing;

    //icon
    unsigned icon_width = m_icon_texture.width() / 2;
    unsigned icon_height = m_icon_texture.height();
    Rect src(0, 0, icon_width, icon_height);
    if (m_files[i].type == FileInfo::Type::file)
      src.x() += icon_width;
    Rect dest(x, y, icon_width, icon_height);
    if (m_files[i].type == FileInfo::Type::puzzle_file) {
      renderer.set_draw_color(background_color);
      renderer.fill_rect(dest);
      renderer.set_draw_color(foreground_color);
      renderer.draw_rect(dest);

      if (m_files[i].puzzle_progress
          && !m_files[i].puzzle_progress->is_complete()
          && m_files[i].puzzle_progress->current_time() == 0) {
        unsigned wd = 0, ht = 0;
        m_filename_font.text_size("?", &wd, &ht);
        Point qmark_loc(x + icon_width / 2 - wd / 2,
                        y + icon_height / 2 - ht/ 2);
        renderer.draw_text(qmark_loc, m_filename_font, "?");
      }
    } else {
      renderer.copy_texture(m_icon_texture, src, dest);
    }
    x += icon_width + spacing;

    Color color;
    if (m_is_selected && m_selection == i)
      color = background_color;
    else
      color = foreground_color;
    renderer.set_draw_color(color);

    //file/puzzle information
    Rect txt = renderer.draw_text(Point(x, y),
                                  m_filename_font, m_files[i].filename);
    y += txt.height();

    if (m_files[i].type == FileInfo::Type::puzzle_file) {
      if (m_files[i].puzzle_info) {
        std::string title;
        if (m_files[i].puzzle_info->title.empty())
          title = "Untitled";
        else
          title = m_files[i].puzzle_info->title;
        if (!m_files[i].puzzle_info->author.empty())
          title += " by " + m_files[i].puzzle_info->author;
        txt = renderer.draw_text(Point(x, y), m_info_font, title);
        y += txt.height();

        std::string size = std::to_string(m_files[i].puzzle_info->width);
        size += "\u00D7" + std::to_string(m_files[i].puzzle_info->height);
        if (m_files[i].puzzle_info->is_multicolor)
          size += " Multicolor";
        txt = renderer.draw_text(Point(x, y), m_info_font, size);
        x += txt.width();
        
        if (m_files[i].puzzle_progress) {
          txt = renderer.draw_text(Point(x, y),
                                   m_info_font, "    Completed:");
          x += txt.width();

          if (m_files[i].puzzle_progress->is_complete()) {
            txt = renderer.draw_text(Point(x, y), m_info_font, " Yes");
            x += txt.width();

            std::string time_str = "    Best time: ";
            time_str
              += time_to_string(m_files[i].puzzle_progress->best_time(),
                                true);
            renderer.draw_text(Point(x, y), m_info_font, time_str);
          } else {
            renderer.set_draw_color(default_colors::red);
            renderer.draw_text(Point(x, y), m_info_font, " No");
            renderer.set_draw_color(color);
          }
        }
      } else {
        renderer.draw_text(Point(x, y),
                           m_info_font, "Loading information...");
      }
    }
  }
  
  renderer.set_clip_rect();
}

void FileSelectionPanel::select(unsigned index)
{
  if (index < m_files.size()) {
    m_is_selected = true;
    m_selection = index;
    if (m_file_sel_callback)
      m_file_sel_callback(m_files[index].full_path);
  }
}

unsigned FileSelectionPanel::entry_height() const
{
  return m_icon_texture.height() + 2 * spacing;
}

void FileSelectionPanel::load_file_list()
{
  m_files.clear();
  m_selection = 0;
  m_is_selected = false;
  m_num_puzzles_loaded = 0;

  if (!m_path.empty()) {
    stdfs::path p(m_path);

    for (const auto& file : stdfs::directory_iterator(p)) {
      FileInfo info;
      info.full_path = file.path();
      info.filename = file.path().filename();
      if (stdfs::is_directory(file))
        info.type = FileInfo::Type::directory;
      else { //check for puzzle file
        if (file.path().extension() == ".non")
          info.type = FileInfo::Type::puzzle_file;
        //else if (file.path().extension() == ".g")
        //info.type = FileInfo::Type::puzzle_file;
        else
          info.type = FileInfo::Type::file;
      }
      m_files.push_back(info);
    }
  }

  resize(entry_height() * 3, entry_height() * m_files.size());

  sort_files();
}

bool
FileSelectionPanel::file_info_less_than(const FileInfo& l, const FileInfo& r)
{
  if ((l.type != FileInfo::Type::directory
       && r.type != FileInfo::Type::directory)
      || l.type == r.type)
    return l.filename < r.filename;
  else
    return l.type == FileInfo::Type::directory;
}

void FileSelectionPanel::sort_files()
{
  std::sort(m_files.begin(), m_files.end(), file_info_less_than);
}

void FileSelectionPanel::load_puzzle_info()
{
  unsigned index = m_num_puzzles_loaded;

  auto summary = std::make_shared<PuzzleSummary>();
  auto progress = std::make_shared<PuzzleProgress>();

  std::ifstream sfile(m_files[index].full_path);
  if (sfile.is_open()) {
    skim_puzzle(sfile, *summary);
  }
  sfile.close();
  m_files[index].puzzle_info = summary;
  
  std::string collection = summary->collection;
  std::string id = summary->id;
  if (collection.empty())
    collection = "Default";
  if (id.empty()) {
    if (summary->title.empty()) {
      id = stdfs::path(m_files[index].full_path).stem();
    } else
      id = summary->title;
  }

  m_save_mgr.load_progress(*progress,
                           m_files[index].full_path,
                           collection, id);
  m_files[index].puzzle_progress = progress;
  
  ++m_num_puzzles_loaded;
}
