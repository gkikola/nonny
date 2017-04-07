#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "color/color.hpp"
#include "puzzle/puzzle.hpp"

#include "collection_index.h"
#include "info_pane.h"
#include "menu.h"

#include "game.h"

const int cell_size_step = 8;
const int move_speed = 1024;
const int zoom_speed = 32;
const int max_cell_size = 112;
const int cell_age_rate = 50;
const int default_info_pane_width = 256;
const int scrollbar_size = 16;
const int button_width = 192;
const int button_height = 36;
const double default_screen_coverage = 0.80;

const std::string program_name = "Nonny";
const std::string program_version = "0.1";
const std::string copyright_str = "Copyright © 2017 Gregory Kikola";
const std::string license_str = "License GPLv3+: GNU GPL version 3 or later";

const std::string puzzle_dir = "puzzles";
const std::string index_file = "index.nin";

void do_play(Game* game, const std::string& data);
void do_resume(Game* game, const std::string& data);
void do_create(Game* game, const std::string& data);
void do_main_menu(Game* game, const std::string& data);
void do_options(Game* game, const std::string& data);
void do_about(Game* game, const std::string& data);
void do_quit(Game* game, const std::string& data);
void select_collection(Game* game, const std::string& col_path);
void select_puzzle(Game* game, const std::string& puzzle_path);

Game::Game(const std::string& data_dir, const std::string& save_dir,
           char filesystem_separator)
  : m_puzzle_loaded{false},
    m_collection_index{nullptr}, m_collection{nullptr},
    m_grid_x{0}, m_grid_y{0}, m_cell_size{32},
    m_target_grid_x{0}, m_target_grid_y{0},
    m_grid_x_delta{0}, m_grid_y_delta{0},
    m_target_cell_size{32}, m_target_x{0}, m_target_y{0}, m_zoom_delta{0},
    m_selected{false}, m_selection_x{0}, m_selection_y{0},
    m_recalc_size{true}, m_row_rule_width{0}, m_col_rule_height{0},
    m_info_pane{nullptr}, m_main_menu{nullptr}, m_collection_menu{nullptr},
    m_puzzle_menu{nullptr}, m_screen_width{0}, m_screen_height{0},
    m_data_dir{data_dir}, m_save_dir{save_dir},
    m_filesystem_separator{filesystem_separator},
    m_running{true} {
  m_state = m_next_state = GameState::main_menu;

  m_main_menu = new Menu(this);
  setup_main_menu(true);

  m_about_menu = new Menu(this);
  setup_about_menu();
  
  m_info_pane = new InfoPane(this);
  m_vscroll = new Scrollbar(this, true);
  m_hscroll = new Scrollbar(this, false);

  std::string puzzles = m_data_dir + puzzle_dir;
  m_collection_index = new CollectionIndex(puzzles, index_file);

  m_collection_menu = new Menu(this);
  setup_collection_menu();
}

Game::~Game() {
  if (m_puzzle_menu) delete m_puzzle_menu;
  if (m_collection) delete m_collection;
  if (m_collection_index) delete m_collection_index;
  if (m_collection_menu) delete m_collection_menu;
  if (m_about_menu) delete m_about_menu;
  if (m_main_menu) delete m_main_menu;
  if (m_info_pane) delete m_info_pane;
  if (m_vscroll) delete m_vscroll;
  if (m_hscroll) delete m_hscroll;
}

void Game::set_state(GameState state) {
  m_next_state = state;

  switch (state) {
  case GameState::puzzle:
    m_state = state;
    m_info_pane->slide_pane(m_screen_width, default_info_pane_width);
    update_screen_size(m_screen_width, m_screen_height);
    break;
  default:
  case GameState::collection_selection:
  case GameState::puzzle_selection:
    m_state = state;
    break;
  case GameState::main_menu:
    if (m_state == GameState::puzzle)
      m_info_pane->slide_pane(default_info_pane_width, m_screen_width);
    else
      m_state = state;
    setup_main_menu(true);
    break;
  case GameState::about:
    m_state = state;
    break;
  }
}

void Game::update(int elapsed_time) {
  m_info_pane->update(elapsed_time);
  
  if (m_state != m_next_state && !m_info_pane->is_sliding())
    m_state = m_next_state;

  if (m_state == GameState::puzzle) {
    m_vscroll->update(elapsed_time);
    m_hscroll->update(elapsed_time);
    
    if (m_cell_size != m_target_cell_size) {
      int multiplier = (1 + std::abs(m_cell_size - m_target_cell_size)
                        / cell_size_step);
      m_zoom_delta += multiplier * zoom_speed * (elapsed_time / 1000.0);
      int amount = std::floor(m_zoom_delta);
      if (amount > 0) {
        m_zoom_delta = 0;

        if (m_cell_size > m_target_cell_size)
          amount = -amount;
    
        m_grid_x -= amount * (m_target_x - m_grid_x) / (m_cell_size + 1);
        m_grid_y -= amount * (m_target_y - m_grid_y) / (m_cell_size + 1);
        m_target_grid_x = m_grid_x;
        m_target_grid_y = m_grid_y;

        m_cell_size += amount;
        m_recalc_size = true;
        move_puzzle_in_bounds();
      }
    }

    if (m_target_grid_x != m_grid_x) {
      m_grid_x_delta += move_speed * (elapsed_time / 1000.0);
      int amount = std::floor(m_grid_x_delta);
      if (amount > 0) {
        m_grid_x_delta = 0;

        if (m_grid_x > m_target_grid_x)
          amount = -amount;
        
        m_grid_x += amount;

        if ((amount > 0 && m_grid_x > m_target_grid_x)
            || (amount < 0 && m_grid_x < m_target_grid_x))
          m_grid_x = m_target_grid_x;
        
        move_puzzle_in_bounds();
      }
    }

    if (m_target_grid_y != m_grid_y) {
      m_grid_y_delta += move_speed * (elapsed_time / 1000.0);
      int amount = std::floor(m_grid_y_delta);
      if (amount > 0) {
        m_grid_y_delta = 0;

        if (m_grid_y > m_target_grid_y)
          amount = -amount;
        
        m_grid_y += amount;

        if ((amount > 0 && m_grid_y > m_target_grid_y)
            || (amount < 0 && m_grid_y < m_target_grid_y))
          m_grid_y = m_target_grid_y;
        
        move_puzzle_in_bounds();
      }
    }
  } //if game in puzzle state
}

void Game::load_puzzle(const std::string& filename) {
  std::ifstream in(filename);

  if (in.is_open()) {
    in >> m_puzzle;
    m_puzzle_loaded = true;

    set_state(GameState::puzzle);

    update_screen_size(m_screen_width, m_screen_height);
    default_zoom();
  }
}

void Game::open_puzzle_collection(const std::string& path) {
  if (m_collection)
    delete m_collection;

  m_collection = m_collection_index->open_collection(path);

  set_state(GameState::puzzle_selection);

  if (m_puzzle_menu)
    delete m_puzzle_menu;

  m_puzzle_menu = new Menu(this);  
  setup_puzzle_menu();
}

void Game::set_rule_dimensions(int row_rule_width, int col_rule_height) {
  m_recalc_size = false;
  m_row_rule_width = row_rule_width;
  m_col_rule_height = col_rule_height;
}

void Game::update_screen_size(int width, int height) {
  int prev_width = m_screen_width;
  int prev_height = m_screen_height;
  m_screen_width = width;
  m_screen_height = height;
  
  //recenter main menu
  setup_main_menu(false);

  //recenter about menu
  setup_about_menu();

  //recenter collection menu
  setup_collection_menu();

  //recenter puzzle menu
  setup_puzzle_menu();

  //recenter the puzzle
  move_puzzle((width - prev_width) / 2, (height - prev_height) / 2, true);

  //recenter info pane
  m_info_pane->setup_controls(default_info_pane_width);

  //update scrollbars
  m_vscroll->move(m_screen_width - scrollbar_size, 0);
  m_vscroll->resize(scrollbar_size, m_screen_height);
  m_hscroll->move(m_info_pane->target_width(),
                  m_screen_height - scrollbar_size);
  m_hscroll->resize(m_screen_width - scrollbar_size
                    - m_info_pane->target_width(), scrollbar_size);
  update_scrollbars();
}

void Game::get_puzzle_coords(int* x, int* y) const {
  *x = m_grid_x;
  *y = m_grid_y;
}

void Game::move_puzzle(int relx, int rely, bool instant) {
  if (instant) {
    m_grid_x += relx;
    m_grid_y += rely;
    m_target_grid_x = m_grid_x;
    m_target_grid_y = m_grid_y;

    //make sure puzzle is not completely off screen
    move_puzzle_in_bounds();
  } else {
    m_target_grid_x += relx;
    m_target_grid_y += rely;
  }

  m_grid_x_delta = m_grid_y_delta = 0;
}

void Game::move_puzzle_in_bounds() {
  if (m_puzzle_loaded) {
    int prev_x = m_grid_x;
    int prev_y = m_grid_y;
  
    if (m_grid_x >= m_screen_width - scrollbar_size)
      m_grid_x = m_screen_width - scrollbar_size - 1;
    else if (m_grid_x + cell_grid_width() <= m_info_pane->target_width())
      m_grid_x = m_info_pane->target_width() - cell_grid_width() + 1;
    if (m_grid_y >= m_screen_height - scrollbar_size)
      m_grid_y = m_screen_height - scrollbar_size - 1;
    else if (m_grid_y + cell_grid_height() <= 0)
      m_grid_y = -cell_grid_height() + 1;

    if (m_grid_x != prev_x)
      m_target_grid_x = m_grid_x;
    if (m_grid_y != prev_y)
      m_target_grid_y = m_grid_y;

    update_scrollbars();
  }
}

void Game::make_selected_cell_visible() {
  int x, y;
  cell_coords_to_screen_coords(m_selection_x, m_selection_y, &x, &y);

  if (x < m_info_pane->width())
    m_grid_x -= x - m_info_pane->width();
  else if (x + m_cell_size > m_screen_width)
    m_grid_x -= x + m_cell_size - m_screen_width;
  if (y < 0)
    m_grid_y -= y;
  else if (y + m_cell_size > m_screen_height)
    m_grid_y -= y + m_cell_size - m_screen_height;

  update_scrollbars();
}

int Game::vert_scrollbar_width() const {
  int w, h;
  m_vscroll->get_size(&w, &h);
  return w;
}

int Game::horiz_scrollbar_height() const {
  int w, h;
  m_hscroll->get_size(&w, &h);
  return h;
}

void Game::get_play_area(int* x, int* y, int* width, int* height) const {
  *x = m_info_pane->target_width();
  *y = 0;
  *width = m_screen_width - vert_scrollbar_width()
    - m_info_pane->target_width();
  *height = m_screen_height - horiz_scrollbar_height();
}

void Game::age_cells(int max_age) {
  /*  if (m_puzzle_loaded) {
    for (int x = 0; x < m_puzzle->width(); ++x)
      for (int y = 0; y < m_puzzle->height(); ++y)
        m_puzzle->age_cell(x, y, max_age);
        }*/
}

void Game::screen_coords_to_cell_coords(int screen_x, int screen_y,
                                        int* x, int* y) {
  *x = (screen_x - m_grid_x - 1) / (m_cell_size + 1);
  *y = (screen_y - m_grid_y - 1) / (m_cell_size + 1);
}

void Game::cell_coords_to_screen_coords(int x, int y,
                                        int* screen_x, int* screen_y) {
  *screen_x = m_grid_x + 1 + x * (m_cell_size + 1);
  *screen_y = m_grid_y + 1 + y * (m_cell_size + 1);
}

int Game::cell_grid_width() {
  return m_puzzle.width() * (m_cell_size + 1) + 1;
}

int Game::cell_grid_height() {
  return m_puzzle.height() * (m_cell_size + 1) + 1;
}

void Game::setup_main_menu(bool full_reset) {
  if (full_reset || m_main_menu->size() == 0) {
    m_main_menu->clear();
    
    StaticText* logo = new StaticText(this);
    logo->resize(150, 150);
    logo->set_string("NONNY_LOGO");
    logo->set_type(StaticText::Type::huge);
    m_main_menu->add_control(logo);

    StaticText* title = new StaticText(this);
    title->resize(button_width * 2, 24);
    title->set_string(program_name);
    title->set_type(StaticText::Type::heading);
    m_main_menu->add_control(title);

    Button* resume;
    if (is_puzzle_loaded()) {
      resume = new Button(this);
      resume->resize(button_width, button_height);
      resume->set_label("Resume Game");
      resume->set_help_text("Continue your current game");
      resume->register_callback(do_resume);
      m_main_menu->add_control(resume);
    }
    
    Button* play = new Button(this);
    play->resize(button_width, button_height);
    play->set_label("Play");
    play->set_help_text("Open a puzzle to play");
    play->register_callback(do_play);
    m_main_menu->add_control(play);

    if (is_puzzle_loaded())
      resume->select();
    else
      play->select();
    
    Button* create = new Button(this);
    create->resize(button_width, button_height);
    create->set_label("Create");
    create->set_help_text("Create your own puzzle");
    create->register_callback(do_create);
    m_main_menu->add_control(create);
    
    Button* options = new Button(this);
    options->resize(button_width, button_height);
    options->set_label("Options");
    options->set_help_text("Change game settings");
    options->register_callback(do_options);
    m_main_menu->add_control(options);

    Button* about = new Button(this);
    about->resize(button_width, button_height);
    about->set_label("About");
    about->set_help_text("About Nonny");
    about->register_callback(do_about);
    m_main_menu->add_control(about);

    Button* quit = new Button(this);
    quit->resize(button_width, button_height);
    quit->set_label("Quit");
    quit->set_help_text("Exit the game");
    quit->register_callback(do_quit);
    m_main_menu->add_control(quit);
  }
  
  m_main_menu->arrange_controls(m_screen_width, m_screen_height);
}

void Game::setup_about_menu() {
  if (m_about_menu->size() == 0) {
    StaticText* title = new StaticText(this);
    title->resize(button_width * 2, 32);
    title->set_string(program_name);
    title->set_type(StaticText::Type::heading);
    m_about_menu->add_control(title);

    StaticText* version = new StaticText(this);
    version->resize(button_width * 2, 24);
    version->set_string("Version " + program_version);
    version->set_type(StaticText::Type::standard);
    m_about_menu->add_control(version);

    StaticText* copyright = new StaticText(this);
    copyright->resize(button_width * 4, 18);
    copyright->set_string(copyright_str + ".");
    copyright->set_type(StaticText::Type::small);
    m_about_menu->add_control(copyright);

    StaticText* license = new StaticText(this);
    license->resize(button_width * 4, 18);
    license->set_string(license_str + ".");
    license->set_type(StaticText::Type::small);
    m_about_menu->add_control(license);

    StaticText* free_software = new StaticText(this);
    free_software->resize(button_width * 4, 18);
    free_software->set_string(program_name + " is free software: "
                              "you are free to change and redistribute it.");
    free_software->set_type(StaticText::Type::small);
    m_about_menu->add_control(free_software);

    StaticText* no_warranty = new StaticText(this);
    no_warranty->resize(button_width * 4, 18);
    no_warranty->set_string("There is NO WARRANTY, to the extent permitted "
                            "by law.");
    no_warranty->set_type(StaticText::Type::small);
    m_about_menu->add_control(no_warranty);

    Button* done_button = new Button(this);
    done_button->resize(button_width, button_height);
    done_button->set_label("Done");
    done_button->set_help_text("Go back to the main menu");
    done_button->register_callback(do_main_menu);
    done_button->select();
    m_about_menu->add_control(done_button);
  }

  m_about_menu->arrange_controls(m_screen_width, m_screen_height);
}

void Game::setup_collection_menu() {
  if (m_collection_menu->size() == 0) {
    for (auto col_entry : *m_collection_index) {
      Button* entry_button = new Button(this);
      entry_button->resize(button_width, button_height);
      entry_button->set_label(col_entry->title);
      entry_button->set_data(col_entry->path);
      entry_button->register_callback(select_collection);
      m_collection_menu->add_control(entry_button);
    }
  }

  m_collection_menu->arrange_controls(m_screen_width, m_screen_height, 3);
}

void Game::setup_puzzle_menu() {
  if (m_collection && m_puzzle_menu) {
    if (m_puzzle_menu->size() == 0) {
      for (PuzzleEntry* puzzle_entry : *m_collection) {
        Button* puzzle_button = new Button(this);
        puzzle_button->resize(button_width, button_height);
        puzzle_button->set_label(puzzle_entry->title);
        puzzle_button->set_data(puzzle_entry->path);
        puzzle_button->register_callback(select_puzzle);
        m_puzzle_menu->add_control(puzzle_button);
      }
    }
    
    m_puzzle_menu->arrange_controls(m_screen_width, m_screen_height, 3);
  }
}

void Game::default_zoom() {
  //add rule numbers to grid size
  int max_row_rule_width = 0, max_col_rule_height = 0;
  for (int i = 0; i < m_puzzle.width(); ++i) {
    int col_rule_height = m_puzzle.col_clues(i).size();
    
    if (col_rule_height > max_col_rule_height)
      max_col_rule_height = col_rule_height;
  }

  for (int j = 0; j < m_puzzle.height(); ++j) {
    int row_rule_width = m_puzzle.row_clues(j).size();

    if (row_rule_width > max_row_rule_width)
      max_row_rule_width = row_rule_width;
  }

  int width = m_puzzle.width() + max_row_rule_width;
  int height = m_puzzle.height() + max_col_rule_height;

  int max_cell_width = ((m_screen_width - default_info_pane_width - width)
                        * default_screen_coverage / width);
  int max_cell_height = ((m_screen_height - height)
                         * default_screen_coverage / height);

  m_cell_size = max_cell_width;
  if (m_cell_size > max_cell_height)
    m_cell_size = max_cell_height;

  //make sure cell_size is a multiple of cell_size_step
  m_cell_size -= m_cell_size % cell_size_step;
  m_target_cell_size = m_cell_size;
  
  m_recalc_size = true;

  m_grid_x = default_info_pane_width
    + (m_screen_width - default_info_pane_width - scrollbar_size)
    / 2 - cell_grid_width() / 2;
  m_grid_y = (m_screen_height - scrollbar_size) / 2 - cell_grid_height() / 2;
  m_target_grid_x = m_grid_x;
  m_target_grid_y = m_grid_y;

  update_scrollbars();
}

void Game::select_cell(int x, int y) {
  m_selection_x = x;
  m_selection_y = y;
  m_selected = true;
}

void Game::get_selected_cell(int* x, int* y) const {
  if (x) *x = m_selection_x;
  if (y) *y = m_selection_y;
}

void Game::clear_selection() {
  m_selected = false;
}

void Game::set_cell(int x, int y, CellState state) {
  switch(state) {
  case CellState::blank:
    m_puzzle.clear_cell(x, y);
    break;
  case CellState::marked:
    m_puzzle.mark_cell(x, y, Color(0, 0, 0));
    break;
  case CellState::exedout:
    m_puzzle.cross_out_cell(x, y);
    break;
  }
}

void Game::zoom_in(int x, int y) {
  zoom(cell_size_step, x, y);
}

void Game::zoom_out(int x, int y) {
  zoom(-cell_size_step, x, y);
}

void Game::zoom(int amount, int x, int y) {
  if ((amount < 0 && m_target_cell_size > cell_size_step)
      || (amount > 0 && m_target_cell_size < max_cell_size)) {
    m_target_x = x;
    m_target_y = y;
    m_target_cell_size += amount;
    m_recalc_size = true;
    m_zoom_delta = 0.0;
  }
}

void Game::update_scrollbars() {  
  if (m_puzzle_loaded) {
    m_vscroll->update_thumb(m_screen_height - horiz_scrollbar_height() - 1
                            - m_grid_y,
                            2 * (m_screen_height - 1) - m_grid_y,
                            cell_grid_height() + 2 * m_screen_height - 2);
    m_hscroll->update_thumb(m_screen_width - vert_scrollbar_width() - 1
                            - m_grid_x,
                            2 * (m_screen_width - 1)
                            + m_info_pane->target_width() - m_grid_x,
                            cell_grid_width() + 2 * m_screen_width - 2);
  }
}

void Game::escape_to_menu() {
  switch (m_state) {
  default:
  case GameState::puzzle:
    set_state(GameState::main_menu);
    break;
  case GameState::main_menu:
    quit();
    break;
  case GameState::about:
    set_state(GameState::main_menu);
    break;
  case GameState::collection_selection:
    set_state(GameState::main_menu);
    break;
  case GameState::puzzle_selection:
    set_state(GameState::collection_selection);
    break;
  }
}

void Game::quit() {
  m_running = false;
}

void do_play(Game* game, const std::string& data) {
  game->set_state(GameState::collection_selection);
}

void do_resume(Game* game, const std::string& data) {
  game->set_state(GameState::puzzle);
}

void do_create(Game* game, const std::string& data) {
}

void do_main_menu(Game* game, const std::string& data) {
  game->set_state(GameState::main_menu);
}

void do_options(Game* game, const std::string& data) {
}

void do_about(Game* game, const std::string& data) {
  game->set_state(GameState::about);
}

void do_quit(Game* game, const std::string& data) {
  game->quit();
}

void select_collection(Game* game, const std::string& col_path) {
  game->open_puzzle_collection(col_path);
}

void select_puzzle(Game* game, const std::string& puzzle_path) {
  game->load_puzzle(puzzle_path);
}
