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

#include "view/puzzle_view.hpp"

#include <algorithm>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <experimental/filesystem>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "puzzle/puzzle_io.hpp"
#include "puzzle/puzzle_progress.hpp"
#include "settings/game_settings.hpp"
#include "ui/puzzle_info_panel.hpp"
#include "ui/puzzle_panel.hpp"
#include "ui/scrollbar.hpp"
#include "utility/utility.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"
#include "video/texture.hpp"
#include "video/video_system.hpp"
#include "view/view_manager.hpp"

namespace stdfs = std::experimental::filesystem;

constexpr unsigned info_pane_width = 256;
constexpr unsigned info_pane_slide_speed = 1000;
const Color info_pane_background_color(123, 175, 212);

PuzzleView::PuzzleView(ViewManager& vm, const std::string& filename)
  : View(vm), m_puzzle_filename(filename)
{
  load(filename);
}

PuzzleView::PuzzleView(ViewManager& vm, const std::string& filename,
                       unsigned width, unsigned height)
  : View(vm, width, height), m_puzzle_filename(filename)
{
  load(filename);
}

PuzzleView::PuzzleView(const PuzzleView& pv)
  : View(pv.m_mgr), m_puzzle(pv.m_puzzle)
{
  setup_panels();
}

PuzzleView::PuzzleView(PuzzleView&& pv)
  : View(pv.m_mgr), m_puzzle(std::move(pv.m_puzzle))
{
  setup_panels();
}

PuzzleView& PuzzleView::operator=(const PuzzleView& pv) &
{
  if (this != &pv) {
    if (&pv.m_mgr != &m_mgr)
      throw std::runtime_error("PuzzleView::operator=: "
                               "cannot assign a PuzzleView from a "
                               "different view manager");

    m_puzzle = pv.m_puzzle;
    setup_panels();
  }
  return *this;
}

PuzzleView& PuzzleView::operator=(PuzzleView&& pv) &
{
  if (this != &pv) {
    if (&pv.m_mgr != &m_mgr)
      throw std::runtime_error("PuzzleView::operator=: "
                               "cannot assign a PuzzleView from a "
                               "different view manager");

    m_puzzle = std::move(pv.m_puzzle);
    setup_panels();
  }
  return *this;
}

void PuzzleView::load(const std::string& filename)
{
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("PuzzleView::load: "
                             "could not open puzzle file " + filename);
  }

  auto pos = filename.rfind('.');
  std::string extension = "";
  if (pos != std::string::npos) {
    extension = filename.substr(pos + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   to_lower);
  }

  if (extension.empty() || extension == "non")
    file >> m_puzzle;
  else if (extension == "g")
    read_puzzle(file, m_puzzle, PuzzleFormat::g);
  else
    file >> m_puzzle;

  //load puzzle progress
  std::string id = puzzle_id();
  std::string collection = puzzle_collection();
  PuzzleProgress prog;
  m_mgr.save_manager().load_progress(prog, m_puzzle_filename,
                                     collection, id);
  prog.restore_progress(m_puzzle);

  setup_panels();

  //restore game time
  auto& ipanel
    = dynamic_cast<PuzzleInfoPanel&>(m_info_pane.main_panel());
  ipanel.time(prog.current_time());
}

std::string PuzzleView::puzzle_id() const
{
  const std::string* id = m_puzzle.find_property("id");
  if (!id)
    id = m_puzzle.find_property("title");
  if (!id)
    return stdfs::path(m_puzzle_filename).stem();
  else
    return *id;
}

std::string PuzzleView::puzzle_collection() const
{
  const std::string* col = m_puzzle.find_property("collection");
  if (col)
    return *col;
  else
    return "Default";
}

void PuzzleView::save() const
{
  bool just_completed = false;
  if (m_puzzle.is_solved())
    just_completed = true;
  
  //find collection and id
  std::string id = puzzle_id();
  std::string collection = puzzle_collection();

  //load previous progress
  PuzzleProgress prog;
  m_mgr.save_manager().load_progress(prog, m_puzzle_filename,
                                     collection, id);

  //store current progress
  auto& ip = dynamic_cast<const PuzzleInfoPanel&>(m_info_pane.main_panel());
  prog.store_progress(m_puzzle, ip.time(), just_completed);

  m_mgr.save_manager().save_progress(prog, m_puzzle_filename,
                                     collection, id);
}

void PuzzleView::restart()
{
  auto& ipanel = dynamic_cast<PuzzleInfoPanel&>(m_info_pane.main_panel());
  ipanel.time(0);
  m_puzzle.clear_all_cells();
}

void PuzzleView::setup_panels()
{
  const GameSettings& settings = m_mgr.game_settings();

  std::string font_file = settings.font_dir() + "FreeSans.ttf";
  std::string bold_font_file = settings.font_dir() + "FreeSansBold.ttf";
  std::string texture_file = settings.image_dir() + "cross.png";
  m_clue_font = m_mgr.video_system().new_font(font_file, 12);
  m_cell_texture = m_mgr.video_system().load_image(m_mgr.renderer(),
                                                   texture_file);
  
  auto ppanel = make_ui_panel<PuzzlePanel>(*m_clue_font, *m_cell_texture,
                                           m_puzzle);

  Rect win_region(0, 0, m_width, m_height);
  m_main_panel = ScrollingPanel(win_region, ppanel);

  m_title_font = m_mgr.video_system().new_font(bold_font_file, 32);
  m_info_font = m_mgr.video_system().new_font(font_file, 18);
  m_size_font = m_mgr.video_system().new_font(font_file, 24);

  texture_file = settings.image_dir() + "control.png";
  m_ctrl_texture = m_mgr.video_system().load_image(m_mgr.renderer(),
                                                   texture_file);

  auto ipanel = std::make_shared<PuzzleInfoPanel>(*m_title_font,
                                                  *m_info_font,
                                                  *m_size_font,
                                                  *m_ctrl_texture,
                                                  m_puzzle);
  ipanel->on_menu_open([this]() {
      m_mgr.schedule_action(ViewManager::Action::open_menu); });
  ipanel->on_zoom_in([]() { });
  ipanel->on_zoom_out([]() { });
  ipanel->on_hint_toggle([]() { });
  ipanel->start_slide();
  Rect info_region(0, 0, 0, m_height);
  m_info_pane = ScrollingPanel(info_region, ipanel);
}

void PuzzleView::update(unsigned ticks, InputHandler& input)
{
  m_main_panel.update(ticks, input);
  m_info_pane.update(ticks, input);

  unsigned cur_info_width = m_info_pane.boundary().width();
  if (cur_info_width < info_pane_width) {
    cur_info_width += info_pane_slide_speed * ticks / 1000;
    if (cur_info_width >= info_pane_width) {
      cur_info_width = info_pane_width;
      PuzzleInfoPanel* panel
        = dynamic_cast<PuzzleInfoPanel*>(&m_info_pane.main_panel());
      panel->stop_slide();
    }
    m_info_pane.resize(cur_info_width, m_height);
    m_info_pane.center_main_panel();
    resize(m_width, m_height);
  }

  if (m_puzzle.is_solved()) {
    save();
    m_mgr.schedule_action(ViewManager::Action::show_victory_screen);
  }
}

void PuzzleView::draw(Renderer& renderer)
{
  m_main_panel.draw(renderer);
  
  renderer.set_draw_color(info_pane_background_color);
  renderer.fill_rect(m_info_pane.boundary());
  m_info_pane.draw(renderer);
}

void PuzzleView::resize(unsigned width, unsigned height)
{
  View::resize(width, height);

  unsigned info_width = m_info_pane.boundary().width();
  m_info_pane.resize(info_width, height);
  if (width >= info_width) {
    m_main_panel.move(info_width, 0);
    m_main_panel.resize(width - info_width, height);
  } else {
    m_main_panel.move(width, 0);
    m_main_panel.resize(0, height);
  }
}
