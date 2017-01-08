#include "game.h"

#include "preview.h"

Preview::Preview(Game* game) : Control{game}, m_pixel_size{0} {
}

void Preview::resize(int width, int height) {
  int pixel_width = width / m_game->puzzle().width();
  int pixel_height = height / m_game->puzzle().height();

  m_pixel_size = (pixel_width < pixel_height) ? pixel_width : pixel_height;

  Control::resize(m_game->puzzle().width() * m_pixel_size,
                  m_game->puzzle().height() * m_pixel_size);
}
