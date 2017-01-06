#include "game.h"

const int cell_size_step = 8;
const int cell_age_rate = 50;

Game::Game() : m_puzzle{nullptr}, m_x{0}, m_y{0}, m_cell_size{16} {
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
