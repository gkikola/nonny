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
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <experimental/filesystem>
#include "SDL.h"
#include "SDL_image.h"
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "puzzle/puzzle_io.hpp"
#include "puzzle/puzzle_progress.hpp"
#include "settings/game_settings.hpp"
#include "solver/solver.hpp"
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

constexpr int info_pane_width = 256;
constexpr int info_pane_slide_speed = 1000;
const Color info_pane_background_color(123, 175, 212);

PuzzleView::PuzzleView(ViewManager& vm)
  : View(vm)
{
  new_puzzle();
}

PuzzleView::PuzzleView(ViewManager& vm, int width, int height)
  : View(vm, width, height)
{
  new_puzzle();
}

PuzzleView::PuzzleView(ViewManager& vm, const std::string& filename)
  : View(vm)
{
  load(filename);
}

PuzzleView::PuzzleView(ViewManager& vm, const std::string& filename,
                       int width, int height)
  : View(vm, width, height)
{
  load(filename);
}

PuzzleView::PuzzleView(const PuzzleView& pv)
  : View(pv.m_mgr),
    m_puzzle(pv.m_puzzle),
    m_puzzle_filename(pv.m_puzzle_filename),
    m_edit_mode(pv.m_edit_mode),
    m_best_time(pv.m_best_time)
{
  setup_panels();
}

PuzzleView::PuzzleView(PuzzleView&& pv)
  : View(pv.m_mgr),
    m_puzzle(std::move(pv.m_puzzle)),
    m_puzzle_filename(std::move(pv.m_puzzle_filename)),
    m_edit_mode(pv.m_edit_mode),
    m_best_time(pv.m_best_time)
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
    m_puzzle_filename = pv.m_puzzle_filename;
    m_edit_mode = pv.m_edit_mode;
    m_best_time = pv.m_best_time;
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
    m_puzzle_filename = std::move(pv.m_puzzle_filename);
    m_edit_mode = pv.m_edit_mode;
    m_best_time = pv.m_best_time;
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

  m_puzzle_filename = filename;

  read_puzzle(file, m_puzzle, file_type(filename));

  //load puzzle progress
  std::string id = puzzle_id();
  std::string collection = puzzle_collection();
  PuzzleProgress prog;
  m_mgr.save_manager().load_progress(prog, m_puzzle_filename,
                                     collection, id);
  m_best_time = prog.best_time();
  prog.restore_progress(m_puzzle);

  if (m_puzzle.width() == 0 || m_puzzle.height() == 0)
    throw InvalidPuzzleFile("PuzzleView::load: puzzle has a "
                            "size of 0");

  setup_panels();

  //restore game time
  auto& ipanel
    = dynamic_cast<PuzzleInfoPanel&>(m_info_pane.main_panel());
  ipanel.time(prog.current_time());

  if (prog.is_complete())
    ipanel.show_puzzle_title();
  else
    ipanel.hide_puzzle_title();

  handle_color_change();
}

PuzzleFormat PuzzleView::file_type(const std::string& filename) const
{
  auto pos = filename.rfind('.');
  std::string extension = "";
  if (pos != std::string::npos) {
    extension = filename.substr(pos);
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   to_lower);
  }

  if (extension.empty() || extension == ".non")
    return PuzzleFormat::non;
  else if (extension == ".g")
    return PuzzleFormat::g;
  else if (extension == ".mk")
    return PuzzleFormat::mk;
  else if (extension == ".nin")
    return PuzzleFormat::nin;
  else if (extension == ".png")
    return PuzzleFormat::png;
  else
    return PuzzleFormat::non;
}

void PuzzleView::new_puzzle()
{
  m_puzzle = Puzzle(10, 10, std::move(ColorPalette::default_palette()));
  m_edit_mode = true;
  setup_panels();
  handle_color_change();
}

std::string PuzzleView::puzzle_id() const
{
  const std::string* id = m_puzzle.find_property("id");
  if (!id)
    id = m_puzzle.find_property("title");
  if (!id)
    return stdfs::path(m_puzzle_filename).stem().string();
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

void PuzzleView::save_progress()
{
  bool just_completed = false;
  if (m_edit_mode || m_puzzle.is_solved())
    just_completed = true;

  //find collection and id
  std::string id = puzzle_id();
  std::string collection = puzzle_collection();

  //load previous progress
  PuzzleProgress prog;
  m_mgr.save_manager().load_progress(prog, m_puzzle_filename,
                                     collection, id);

  //store current progress
  unsigned time = 0;
  if (!m_edit_mode) {
    auto& ip = dynamic_cast<const PuzzleInfoPanel&>(m_info_pane.main_panel());
    time = ip.time();
  }
  prog.store_progress(m_puzzle, time, just_completed);

  m_mgr.save_manager().save_progress(prog, m_puzzle_filename,
                                     collection, id);

  //clear save flag
  auto& pp = dynamic_cast<PuzzlePanel&>(m_main_panel.main_panel());
  pp.clear_save_flag();
}

void PuzzleView::restart()
{
  auto& ipanel = dynamic_cast<PuzzleInfoPanel&>(m_info_pane.main_panel());
  ipanel.time(0);

  auto& ppanel = dynamic_cast<PuzzlePanel&>(m_main_panel.main_panel());
  ppanel.clear_puzzle();
}

void PuzzleView::save_puzzle(std::string filename)
{
  if (filename.empty())
    filename = m_puzzle_filename;
  else if (filename != m_puzzle_filename)
    m_ask_before_save = false;

  if (filename.empty()) {
    m_mgr.schedule_action(ViewManager::Action::save_puzzle_as);
  } else {
    if (m_ask_before_save) {
      std::string fname = stdfs::path(filename).filename().string();
      auto do_save = [this]() {
        m_ask_before_save = false;
        m_mgr.schedule_action(ViewManager::Action::save_puzzle); };
      auto cancel = [this]() {
        m_mgr.schedule_action(ViewManager::Action::close_message_box); };
      m_mgr.message_box("Are you sure you want to overwrite the file "
                        "\"" + fname + "\"?",
                        MessageBoxView::Type::yes_no,
                        do_save, cancel, cancel);
    } else {
      PuzzleFormat type = file_type(filename);

      if (type == PuzzleFormat::png) {
        write_puzzle_png(filename, m_puzzle);
      } else {
        std::ofstream file(filename);

        if (file.is_open())
          m_puzzle_filename = filename;

        try {
          write_puzzle(file, m_puzzle, file_type(filename));

          //wipe previous puzzle progress and store solution
          save_progress();
        } catch (const std::exception& e) {
          std::string err_msg = "Error saving puzzle:\n\n";
          err_msg += e.what();

          //show error message
          auto close = [this]() {
            m_mgr.schedule_action(ViewManager::Action::close_message_box); };
          m_mgr.message_box(err_msg, MessageBoxView::Type::okay,
                            close, []() { }, []() { });
        }
      }
    }
  }
}

void PuzzleView::update_properties()
{
  auto ipanel = dynamic_cast<PuzzleInfoPanel*>(&m_info_pane.main_panel());
  if (ipanel)
    ipanel->refresh_puzzle_properties();
}

void PuzzleView::set_edit_mode()
{
  //find collection and id
  std::string id = puzzle_id();
  std::string collection = puzzle_collection();

  //load progress
  PuzzleProgress prog;
  m_mgr.save_manager().load_progress(prog, m_puzzle_filename,
                                     collection, id);

  if (!prog.is_complete()) {
    auto solve = [this]() {
      m_mgr.schedule_action(ViewManager::Action::solve_and_edit); };
    auto close = [this]() {
      m_mgr.schedule_action(ViewManager::Action::close_message_box); };
    m_mgr.message_box("The puzzle must be solved before editing. "
                      "Do you want to run the solver now?",
                      MessageBoxView::Type::yes_no,
                      solve, close, close);
  } else {
    prog.restore_solution(m_puzzle);
    enable_editing();
  }
}

void PuzzleView::solve_and_edit()
{
  m_puzzle.clear_all_cells();
  Solver solver(m_puzzle);
  solver();

  enable_editing();
}

bool PuzzleView::is_save_needed() const
{
  auto ppanel = dynamic_cast<const PuzzlePanel*>(&m_main_panel.main_panel());
  if (ppanel)
    return ppanel->is_save_needed();
  else
    return false;
}

void PuzzleView::setup_panels()
{
  const GameSettings& settings = m_mgr.game_settings();

  std::string font_file = settings.font_dir()
    + settings.filesystem_separator() + "FreeSans.ttf";
  std::string bold_font_file = settings.font_dir()
    + settings.filesystem_separator() + "FreeSansBold.ttf";
  std::string texture_file = settings.image_dir()
    + settings.filesystem_separator() + "puzzle.png";
  m_clue_font = m_mgr.video_system().new_font(font_file, 12);
  m_cell_texture = m_mgr.video_system().load_image(m_mgr.renderer(),
                                                   texture_file);

  auto ppanel = std::make_shared<PuzzlePanel>(*m_clue_font, *m_cell_texture,
                                              m_puzzle);
  ppanel->set_edit_mode(m_edit_mode);
  ppanel->clear_save_flag();

  Rect win_region(0, 0, m_width, m_height);
  m_main_panel = ScrollingPanel(win_region, ppanel);

  m_title_font = m_mgr.video_system().new_font(bold_font_file, 32);
  m_info_font = m_mgr.video_system().new_font(font_file, 18);
  m_size_font = m_mgr.video_system().new_font(font_file, 24);

  texture_file = settings.image_dir()
    + settings.filesystem_separator() + "control.png";
  m_ctrl_texture = m_mgr.video_system().load_image(m_mgr.renderer(),
                                                   texture_file);

  texture_file = settings.image_dir()
    + settings.filesystem_separator() + "nav.png";
  m_arrow_texture = m_mgr.video_system().load_image(m_mgr.renderer(),
                                                    texture_file);

  texture_file = settings.image_dir()
    + settings.filesystem_separator() + "draw.png";
  m_draw_texture = m_mgr.video_system().load_image(m_mgr.renderer(),
                                                   texture_file);

  auto ipanel = std::make_shared<PuzzleInfoPanel>(*m_title_font,
                                                  *m_info_font,
                                                  *m_size_font,
                                                  *m_ctrl_texture,
                                                  *m_arrow_texture,
                                                  *m_draw_texture,
                                                  info_pane_width,
                                                  m_edit_mode);
  ipanel->attach_puzzle(m_puzzle);
  ipanel->on_menu_open([this]() {
      m_mgr.schedule_action(ViewManager::Action::open_menu); });
  ipanel->on_zoom_in([ppanel]() { ppanel->zoom(1); });
  ipanel->on_zoom_out([ppanel]() { ppanel->zoom(-1); });
  ipanel->on_hint_toggle([ppanel]() { ppanel->toggle_hints(); });
  ipanel->on_clear_puzzle([ppanel]() { ppanel->clear_puzzle(); });
  ipanel->on_save(std::bind(&PuzzleView::save, this));
  ipanel->on_undo([ppanel]() { ppanel->undo(); });
  ipanel->on_redo([ppanel]() { ppanel->redo(); });
  ipanel->on_analyze([this]() {
      m_draw_tooltips = false;
      m_mgr.schedule_action(ViewManager::Action::analyze_puzzle); });
  ipanel->on_left([ppanel]() { ppanel->shift_left(); });
  ipanel->on_right([ppanel]() { ppanel->shift_right(); });
  ipanel->on_up([ppanel]() { ppanel->shift_up(); });
  ipanel->on_down([ppanel]() { ppanel->shift_down(); });
  ipanel->on_data_edit_request([this]() {
      m_mgr.schedule_action(ViewManager::Action::edit_puzzle_data); });
  ipanel->on_color_change(std::bind(&PuzzleView::handle_color_change, this));
  ipanel->on_tool_change(std::bind(&PuzzleView::handle_tool_change, this));
  ipanel->start_slide();
  Rect info_region(0, 0, 0, m_height);
  m_info_pane = ScrollingPanel(info_region, ipanel);

  ppanel->on_resize([this, ipanel]() {
      m_main_panel.center_main_panel();
      ipanel->refresh_puzzle_properties();
    });
}

void PuzzleView::handle_color_change()
{
  auto* ipanel = dynamic_cast<PuzzleInfoPanel*>(&m_info_pane.main_panel());
  auto* ppanel = dynamic_cast<PuzzlePanel*>(&m_main_panel.main_panel());
  if (ipanel && ppanel) {
    ppanel->set_active_color(ipanel->active_color());
  }
}

void PuzzleView::handle_tool_change()
{
  auto* ipanel = dynamic_cast<PuzzleInfoPanel*>(&m_info_pane.main_panel());
  auto* ppanel = dynamic_cast<PuzzlePanel*>(&m_main_panel.main_panel());
  if (ipanel && ppanel) {
    ppanel->set_draw_tool(ipanel->active_draw_tool());
  }
}

void PuzzleView::save()
{
  if (m_edit_mode)
    save_puzzle();
  else
    save_progress();
}

void PuzzleView::enable_editing()
{
  m_edit_mode = true;
  auto ppanel = dynamic_cast<PuzzlePanel*>(&m_main_panel.main_panel());
  auto ipanel = dynamic_cast<PuzzleInfoPanel*>(&m_info_pane.main_panel());
  ppanel->set_edit_mode(true);
  ipanel->set_edit_mode(true);

  //get rid of crosses
  for (int y = 0; y < m_puzzle.height(); ++y) {
    for (int x = 0; x < m_puzzle.width(); ++x) {
      if (m_puzzle[x][y].state == PuzzleCell::State::crossed_out)
        m_puzzle.clear_cell(x, y);
    }
  }

  //restore default palette
  m_puzzle.reset_palette();
  update_properties();

  m_ask_before_save = true;
}

void PuzzleView::update(unsigned ticks, InputHandler& input)
{
  m_main_panel.update(ticks, input);
  m_info_pane.update(ticks, input);

  int cur_info_width = m_info_pane.boundary().width();
  if (cur_info_width < info_pane_width) {
    cur_info_width += info_pane_slide_speed * ticks / 1000;

    if (cur_info_width >= info_pane_width) {
      cur_info_width = info_pane_width;

      PuzzleInfoPanel* ipanel
        = dynamic_cast<PuzzleInfoPanel*>(&m_info_pane.main_panel());
      ipanel->stop_slide();
    }
    m_info_pane.resize(cur_info_width, m_height);
    m_info_pane.center_main_panel();
    resize(m_width, m_height);
    m_main_panel.center_main_panel();
  }

  //see if mouse is over info pane
  //if so we need to allow it to draw tooltips
  if (input.was_mouse_moved()) {
    m_draw_tooltips = false;
    if (m_info_pane.boundary().contains_point(input.mouse_position())) {
      m_draw_tooltips = true;
    }
  }

  if (!m_edit_mode && m_puzzle.is_solved()) {
    save_progress();
    m_mgr.schedule_action(ViewManager::Action::show_victory_screen);
  }

  if (input.was_key_pressed(Keyboard::Key::escape))
    m_mgr.schedule_action(ViewManager::Action::open_menu);
}

void PuzzleView::draw(Renderer& renderer)
{
  m_main_panel.draw(renderer);

  renderer.set_draw_color(info_pane_background_color);
  renderer.fill_rect(m_info_pane.boundary());
  m_info_pane.draw(renderer);

  if (m_draw_tooltips) {
    auto& ipanel = dynamic_cast<PuzzleInfoPanel&>(m_info_pane.main_panel());
    ipanel.draw_tooltips(renderer, m_width, m_height);
  }
}

void PuzzleView::resize(int width, int height)
{
  View::resize(width, height);

  int info_width = m_info_pane.boundary().width();
  m_info_pane.resize(info_width, height);
  if (width >= info_width) {
    m_main_panel.move(info_width, 0);
    m_main_panel.resize(width - info_width, height);
  } else {
    m_main_panel.move(width, 0);
    m_main_panel.resize(0, height);
  }
}

unsigned PuzzleView::time() const
{
  auto& ipanel
    = dynamic_cast<const PuzzleInfoPanel&>(m_info_pane.main_panel());
  return ipanel.time();
}

void read_puzzle_png(const std::string& filename, Puzzle& puzzle)
{
}

void write_puzzle_png(const std::string& filename, Puzzle& puzzle)
{
  Uint32* data = new Uint32[puzzle.width() * puzzle.height()];
  auto* fmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);

  for (int y = 0; y < puzzle.height(); ++y) {
    for (int x = 0; x < puzzle.width(); ++x) {
      Uint32 value;
      if (puzzle[x][y].state == PuzzleCell::State::filled) {
        Color color = puzzle[x][y].color;
        value = static_cast<Uint32>(SDL_MapRGBA(fmt,
                                                color.red(),
                                                color.green(),
                                                color.blue(),
                                                255));
      } else {
        value = static_cast<Uint32>(SDL_MapRGBA(fmt, 0, 0, 0, 0));
      }

      data[x + y * puzzle.width()] = value;
    }
  }

  Uint32 rmask = 0x000000ff,
         gmask = 0x0000ff00,
         bmask = 0x00ff0000,
         amask = 0xff000000;
  SDL_FreeFormat(fmt);
  SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(static_cast<void*>(data),
                                                  puzzle.width(),
                                                  puzzle.height(),
                                                  32, 4 * puzzle.width(),
                                                  rmask, gmask, bmask, amask);

  IMG_SavePNG(surface, filename.c_str());

  SDL_FreeSurface(surface);
  delete[] data;
}
