#include "game.h"

Game::Game() : m_puzzle{nullptr} {
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
