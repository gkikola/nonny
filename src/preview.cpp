#include "game.h"
#include "renderer.h"

#include "preview.h"

Preview::Preview(Game* game) : Control{game}, m_pixel_size{0} {
}

void Preview::update_pixel_size() {
  int width, height;
  get_size(&width, &height);
  
  int pixel_width = 1, pixel_height = 1;
  pixel_width = width / m_game->puzzle().width();
  pixel_height = height / m_game->puzzle().height();
  
  m_pixel_size = (pixel_width < pixel_height) ? pixel_width : pixel_height;
}

void Preview::update() {
}

void Preview::draw(Renderer* renderer) const {
  renderer->render_control(this);
}
