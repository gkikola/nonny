#include <cmath>
#include "renderer.h"

#include "scrollbar.h"

const int scroll_buffer = 4;

Scrollbar::Scrollbar(Game* game, bool vertical)
 : Control(game), m_thumb_pos{0}, m_thumb_size{1}, m_vertical{vertical} {
}

void Scrollbar::update_thumb(int visible_start, int visible_end,
                             int total_size) {
  int screen_size = visible_end - visible_start;

  int x, y;
  get_position(&x, &y);
  int bar_start = m_vertical ? y : x;

  int bar_width, bar_height;
  get_size(&bar_width, &bar_height);
  int bar_size = m_vertical ? bar_height : bar_width;

  double start_proportion = (double)visible_start / total_size;
  double end_proportion = (double)visible_end / total_size;

  int thumb_start = std::round(start_proportion * bar_size) + thumb_spacing();
  int thumb_end = std::round(end_proportion * bar_size) - thumb_spacing();
  
  m_thumb_pos = bar_start + thumb_start;
  m_thumb_size = thumb_end - thumb_start;
}

void Scrollbar::activate() {
}

void Scrollbar::draw(Renderer* renderer) const {
  renderer->render_control(this);
}

int Scrollbar::thumb_spacing() {
  return scroll_buffer;
}
