#include <cmath>

#include "info_pane.h"
#include "menu.h"
#include "puzzle.h"

#include "game.h"

const int cell_size_step = 8;
const int zoom_speed = 32;
const int max_cell_size = 112;
const int cell_age_rate = 50;
const int default_info_pane_width = 256;
const int menu_spacing = 16;
const int button_width = 192;
const int button_height = 36;
const double default_screen_coverage = 0.80;

Game::Game()
  : m_puzzle{nullptr}, m_grid_x{0}, m_grid_y{0}, m_cell_size{32},
    m_target_cell_size{32}, m_target_x{0}, m_target_y{0},
    m_selected{false}, m_selection_x{0}, m_selection_y{0},
    m_recalc_size{true}, m_row_rule_width{0}, m_col_rule_height{0},
    m_info_pane{nullptr}, m_main_menu{nullptr},
    m_screen_width{0}, m_screen_height{0} {
  m_state = GameState::main_menu;

  m_main_menu = new Menu(this);
  setup_main_menu();
  
  m_info_pane = new InfoPane(this);
  m_info_pane->set_width(default_info_pane_width);
}

Game::~Game() {
  if (m_main_menu) delete m_main_menu;
  if (m_info_pane) delete m_info_pane;
  if (m_puzzle) delete m_puzzle;
}

void Game::update(int elapsed_time) {
  if (m_cell_size != m_target_cell_size) {
    int multiplier = (1 + std::abs(m_cell_size - m_target_cell_size)
                      / cell_size_step);
    m_zoom_duration += multiplier * zoom_speed * (elapsed_time / 1000.0);
    int amount = round(m_zoom_duration);
    if (amount > 0) {
      m_zoom_duration = 0;

      if (m_cell_size > m_target_cell_size)
        amount = -amount;
    
      m_grid_x -= amount * (m_target_x - m_grid_x) / (m_cell_size + 1);
      m_grid_y -= amount * (m_target_y - m_grid_y) / (m_cell_size + 1);

      m_cell_size += amount;
      m_recalc_size = true;
    }
  }
}

void Game::load_puzzle(const std::string& filename) {
  if (m_puzzle) delete m_puzzle;

  m_puzzle = new Puzzle(filename);

  default_zoom();

  m_info_pane->setup_controls();
}

void Game::set_rule_dimensions(int row_rule_width, int col_rule_height) {
  m_recalc_size = false;
  m_row_rule_width = row_rule_width;
  m_col_rule_height = col_rule_height;
}

void Game::update_screen_size(int width, int height) {
  //recenter the puzzle
  m_grid_x += (width - m_screen_width) / 2;
  m_grid_y += (height - m_screen_height) / 2;

  //update the dimensions
  m_screen_width = width;
  m_screen_height = height;

  if (m_state == GameState::main_menu)
    setup_main_menu();
}

void Game::get_puzzle_coords(int* x, int* y) const {
  *x = m_grid_x;
  *y = m_grid_y;
}

void Game::move_puzzle(int relx, int rely) {
  m_grid_x += relx;
  m_grid_y += rely;
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
}

void Game::age_cells(int max_age) {
  for (int x = 0; x < m_puzzle->width(); ++x)
    for (int y = 0; y < m_puzzle->height(); ++y)
      m_puzzle->age_cell(x, y, max_age);
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
  return m_puzzle->width() * (m_cell_size + 1) + 1;
}

int Game::cell_grid_height() {
  return m_puzzle->height() * (m_cell_size + 1) + 1;
}

void Game::setup_main_menu() {
  if (m_main_menu->size() == 0) {
    StaticText* title = new StaticText(this);
    title->resize(button_width * 2, 92);
    title->set_string("Nonny");
    title->set_type(StaticText::Type::huge);
    m_main_menu->add_control(title);

    Button* play = new Button(this);
    play->resize(button_width, button_height);
    play->set_label("Play");
    m_main_menu->add_control(play);

    Button* create = new Button(this);
    create->resize(button_width, button_height);
    create->set_label("Create");
    m_main_menu->add_control(create);
    
    Button* options = new Button(this);
    options->resize(button_width, button_height);
    options->set_label("Options");
    m_main_menu->add_control(options);

    Button* about = new Button(this);
    about->resize(button_width, button_height);
    about->set_label("About");
    m_main_menu->add_control(about);
  }

  int y = menu_spacing;
  
  for (Control* control : *m_main_menu) {
    int width, height;
    control->get_size(&width, &height);
    control->move(m_screen_width / 2 - width / 2, y);
    y += height;
    y += menu_spacing;
  }
}

void Game::default_zoom() {
  //add rule numbers to grid size
  int max_row_rule_width = 0, max_col_rule_height = 0;
  for (int i = 0; i < m_puzzle->width(); ++i) {
    int col_rule_height = m_puzzle->get_col_rule(i).size();
    
    if (col_rule_height > max_col_rule_height)
      max_col_rule_height = col_rule_height;
  }

  for (int j = 0; j < m_puzzle->height(); ++j) {
    int row_rule_width = m_puzzle->get_row_rule(j).size();

    if (row_rule_width > max_row_rule_width)
      max_row_rule_width = row_rule_width;
  }

  int width = m_puzzle->width() + max_row_rule_width;
  int height = m_puzzle->height() + max_col_rule_height;

  int max_cell_width = ((m_screen_width - m_info_pane->width() - width)
                        * default_screen_coverage / width);
  int max_cell_height = ((m_screen_height - height)
                         * default_screen_coverage / height);

  m_cell_size = max_cell_width;
  if (m_cell_size > max_cell_height)
    m_cell_size = max_cell_height;

  //make sure cell_size is a multiple of cell_size_step
  m_cell_size -= m_cell_size % cell_size_step;

  m_recalc_size = true;

  m_grid_x = m_info_pane->width()
    + (m_screen_width - m_info_pane->width()) / 2 - cell_grid_width() / 2;
  m_grid_y = m_screen_height / 2 - cell_grid_height() / 2;
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
  m_puzzle->set_cell(x, y, state);
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
    //m_grid_x -= amount * (x - m_grid_x) / (m_cell_size + 1);
    //m_grid_y -= amount * (y - m_grid_y) / (m_cell_size + 1);

    m_target_x = x;
    m_target_y = y;
    m_target_cell_size += amount;
    m_recalc_size = true;
    m_zoom_duration = 0.0;
  }
}

