#include <iostream>

#include "puzzle.h"

#include "game.h"

const int cell_size_step = 8;
const int cell_age_rate = 50;
const int default_info_pane_width = 256;

Game::Game() : m_puzzle{nullptr}, m_x{0}, m_y{0}, m_cell_size{32},
               m_recalc_size{true}, m_row_rule_width{0}, m_col_rule_height{0},
               m_info_pane_width{default_info_pane_width},
               m_info_pane_visible{true} {
  m_state = GameState::puzzle;

  m_x += m_info_pane_width;
}

Game::~Game() {
  if (m_puzzle) delete m_puzzle;
}

void Game::update(int elapsed_time) {
}

void Game::load_puzzle(const std::string& filename) {
  if (m_puzzle) delete m_puzzle;

  m_puzzle = new Puzzle(filename);
}

void Game::set_rule_dimensions(int row_rule_width, int col_rule_height) {
  //put puzzle grid back to default spot
  m_x -= m_row_rule_width;
  m_y -= m_col_rule_height;
  
  m_recalc_size = false;
  m_row_rule_width = row_rule_width;
  m_col_rule_height = col_rule_height;

  //reposition puzzle grid
  m_x += m_row_rule_width;
  m_y += m_col_rule_height;
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
