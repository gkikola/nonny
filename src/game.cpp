#include "game.h"

const int cell_size_step = 8;
const int cell_age_rate = 50;

Game::Game() : m_puzzle{nullptr}, m_x{0}, m_y{0}, m_cell_size{16},
               m_recalc_size{true}, m_row_rule_width{0}, m_col_rule_height{0} {
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
}

void Game::set_rule_dimensions(int row_rule_width, int col_rule_height) {
  m_recalc_size = false;
  m_row_rule_width = row_rule_width;
  m_col_rule_height = col_rule_height;
}
