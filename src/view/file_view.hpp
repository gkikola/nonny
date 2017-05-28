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

#ifndef NONNY_FILE_VIEW_HPP
#define NONNY_FILE_VIEW_HPP

#include <deque>
#include <list>
#include <memory>
#include <string>
#include <experimental/filesystem>
#include "ui/file_selection_panel.hpp"
#include "ui/image_button.hpp"
#include "ui/scrolling_panel.hpp"
#include "ui/text_box.hpp"
#include "video/font.hpp"
#include "video/rect.hpp"
#include "video/texture.hpp"
#include "view/view.hpp"

/*
 * File selection screen. In this View, the user can select files or
 * directories on their storage device. The user can then open or save
 * a puzzle in a particular location.
 */
class FileView : public View {
public:
  enum class Mode { open, save };
  
  FileView(ViewManager& vm, Mode mode = Mode::open);
  FileView(ViewManager& vm, Mode mode, int width, int height);

  FileView(const FileView&) = delete;
  FileView(FileView&&) = default;
  FileView& operator=(const FileView&) = delete;
  FileView& operator=(FileView&&) & = default;

  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(int width, int height) override;

private:
  void load_resources();

  void open_path(const std::experimental::filesystem::path& p);
  void back();
  void forward();
  void up();

  void switch_focus(bool fwd = true);
  void clear_focus();
  
  void open_default_dir();
  void open_puzzle_dir();
  void open_save_dir();

  void open_file(const std::string& filename = "");

  int path_name_width() const; //get display width of cur path name
  int path_subdir_count() const; //get num subdirs in cur path
  void handle_directory_change(); //update controls for new directory
  void handle_selection_change(); //update text box with selection
  void collapse_path(); //calculate start and end of ellipses in path display
  void open_subdir(int index); //open subdir at index in *m_cur_path
  
  Mode m_mode;

  typedef std::deque<std::experimental::filesystem::path> PathContainer;
  PathContainer m_paths;
  PathContainer::iterator m_cur_path;
  int m_path_collapse_start;
  int m_path_collapse_end;
  Point m_path_start;

  std::shared_ptr<ImageButton> m_menu_button;
  std::shared_ptr<ImageButton> m_up_button;
  std::shared_ptr<ImageButton> m_back_button;
  std::shared_ptr<ImageButton> m_forward_button;
  std::shared_ptr<Button> m_open_button;
  std::shared_ptr<TextBox> m_filename_box;

  std::vector<ControlPtr> m_controls;

  ScrollingPanel m_file_selection;

  std::unique_ptr<Font> m_filename_font;
  std::unique_ptr<Font> m_info_font;
  std::unique_ptr<Font> m_control_font;
  std::unique_ptr<Texture> m_nav_texture;
  std::unique_ptr<Texture> m_file_icons_texture;

  std::string m_selected_path;
  bool m_need_path_change = false;
};

#endif
