#include "game.h"
#include "preview.h"

#include "info_pane.h"

const int default_spacing = 32;

InfoPane::InfoPane(Game* game) : m_game{game}, m_width{0} {
  m_preview = new Preview(game);
  m_controls.push_back(m_preview);
}

InfoPane::~InfoPane() {
  for (auto control : m_controls)
    delete control;
}

void InfoPane::update() {
  for (auto control : m_controls)
    control->update();
}

void InfoPane::position_controls() {
  m_preview->resize(m_width - 2 * default_spacing,
                    m_width - 2 * default_spacing);
  m_preview->update_pixel_size();
  
  int pixel_size = m_preview->pixel_size();
  int width = pixel_size * m_game->puzzle().width();
  int height = pixel_size * m_game->puzzle().height();
  
  m_preview->resize(width, height);
  m_preview->move(m_width / 2 - width / 2, default_spacing);
}
