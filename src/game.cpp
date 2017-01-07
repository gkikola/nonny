#include <iostream>

#include "puzzle.h"

#include "game.h"

const int cell_size_step = 8;
const int cell_age_rate = 50;
const int default_info_pane_width = 256;
const double default_screen_coverage = 0.80;

Game::Game()
  : m_puzzle{nullptr}, m_x{0}, m_y{0}, m_cell_size{32}, m_recalc_size{true},
    m_row_rule_width{0}, m_col_rule_height{0},
    m_info_pane_width{default_info_pane_width}, m_info_pane_visible{true},
    m_screen_width{0}, m_screen_height{0} {
  m_state = GameState::puzzle;
}

Game::~Game() {
  if (m_puzzle) delete m_puzzle;
}

void Game::update(int elapsed_time) {
}

void Game::load_puzzle(const std::string& filename) {
  if (m_puzzle) delete m_puzzle;

  m_puzzle = new Puzzle(filename);

  default_zoom();
}

void Game::set_rule_dimensions(int row_rule_width, int col_rule_height) {
  m_recalc_size = false;
  m_row_rule_width = row_rule_width;
  m_col_rule_height = col_rule_height;
}

void Game::update_screen_size(int width, int height) {
  //recenter the puzzle
  m_x += (width - m_screen_width) / 2;
  m_y += (height - m_screen_height) / 2;

  //update the dimensions
  m_screen_width = width;
  m_screen_height = height;
}

void Game::get_puzzle_coords(int* x, int* y) const {
  *x = m_x;
  *y = m_y;
}

void Game::age_cells(int max_age) {
  for (int x = 0; x < m_puzzle->width(); ++x)
    for (int y = 0; y < m_puzzle->height(); ++y)
      m_puzzle->age_cell(x, y, max_age);
}

void Game::screen_coords_to_cell_coords(int screen_x, int screen_y,
                                        int* x, int* y) {
  *x = (screen_x - m_x - 1) / (m_cell_size + 1);
  *y = (screen_y - m_y - 1) / (m_cell_size + 1);
}

void Game::cell_coords_to_screen_coords(int x, int y,
                                        int* screen_x, int* screen_y) {
  *screen_x = m_x + 1 + x * (m_cell_size + 1);
  *screen_y = m_y + 1 + y * (m_cell_size + 1);
}

int Game::cell_grid_width() {
  return m_puzzle->width() * (m_cell_size + 1) + 1;
}

int Game::cell_grid_height() {
  return m_puzzle->height() * (m_cell_size + 1) + 1;
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

  int max_cell_width = ((m_screen_width - m_info_pane_width - width)
                        * default_screen_coverage / width);
  int max_cell_height = ((m_screen_height - height)
                         * default_screen_coverage / height);

  m_cell_size = max_cell_width;
  if (m_cell_size > max_cell_height)
    m_cell_size = max_cell_height;

  //make sure cell_size is a multiple of cell_size_step
  m_cell_size -= m_cell_size % cell_size_step;

  m_recalc_size = true;

  m_x = m_info_pane_width
    + (m_screen_width - m_info_pane_width) / 2 - cell_grid_width() / 2;
  m_y = m_screen_height / 2 - cell_grid_height() / 2;
}
