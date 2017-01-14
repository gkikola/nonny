#include <cmath>
#include "game.h"
#include "renderer.h"

#include "scrollbar.h"

const int scroll_buffer = 4;
const int move_step = 192;
const int move_delay = 100;

Scrollbar::Scrollbar(Game* game, bool vertical)
 : Control(game), m_thumb_pos{0}, m_thumb_size{1}, m_vertical{vertical},
   m_dragging_left{false}, m_dragging_right{false}, m_holding_right{false},
   m_holding_time{0}, m_holding_x{0}, m_holding_y{0}, m_down_pos{0} {
}

void Scrollbar::update(int elapsed_time) {
  if (m_holding_right) {
    m_holding_time += elapsed_time;
    if (m_holding_time >= move_delay) {
      m_holding_time %= move_delay;
      mouse_move(m_holding_x, m_holding_y);
    }
  }
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

bool Scrollbar::is_thumb_being_dragged() const {
  return m_dragging_left || m_dragging_right;
}

void Scrollbar::activate() {
}

void Scrollbar::draw(Renderer* renderer) const {
  renderer->render_control(this);
}

int Scrollbar::thumb_spacing() {
  return scroll_buffer;
}

void Scrollbar::key_press(KeyAction action, bool down) {
}

void Scrollbar::mouse_press(MouseAction action, int x, int y, bool down) {
  Control::mouse_press(action, x, y, down);

  int* pos = m_vertical ? &y : &x;

  if (!down) {
    if (action == MouseAction::left) {
      m_dragging_left = false;
    } else if (action == MouseAction::right) {
      //stop scrolling
      if (m_holding_right)
        m_game->move_puzzle(0, 0, true);
      
      m_dragging_right = false;
      m_holding_right = false;
    }

    deselect();
    unpress();
  }

  if (is_selected() && action == MouseAction::left) {
    m_dragging_left = true;
    set_thumb_position(*pos - m_thumb_size / 2);
  } else if (is_selected() && action == MouseAction::right) {
    if (*pos >= m_thumb_pos && *pos <= m_thumb_pos + m_thumb_size) {
      m_down_pos = *pos - m_thumb_pos;
      m_dragging_right = true;
    } else {
      m_holding_right = true;
      m_holding_x = x;
      m_holding_y = y;
      m_holding_time = 0;
      
      int move_x = 0, move_y = 0;
      int* coord = m_vertical ? &move_y : &move_x;

      if (*pos < m_thumb_pos)
        *coord = move_step;
      else if (*pos > m_thumb_pos + m_thumb_size)
        *coord = -move_step;

      m_game->move_puzzle(move_x, move_y, false);

      deselect();
    }
  }
}

void Scrollbar::mouse_move(int x, int y) {
  Control::mouse_move(x, y);

  int* pos = m_vertical ? &y : &x;

  if (*pos < m_thumb_pos || *pos > m_thumb_pos + m_thumb_size)
    hover_mouse(false);
  
  if (m_dragging_left)
    set_thumb_position(*pos - m_thumb_size / 2);

  if (m_dragging_right)
    set_thumb_position(*pos - m_down_pos);

  if (m_holding_right) {
    int delta_x = 0, delta_y = 0;
    int* delta = m_vertical ? &delta_y : &delta_x;  
    
    if (*pos < m_thumb_pos)
      *delta = move_step;
    else if (*pos > m_thumb_pos + m_thumb_size)
      *delta = -move_step;

    m_game->move_puzzle(delta_x, delta_y, false);

    m_holding_x = x;
    m_holding_y = y;
  }
}

void Scrollbar::set_thumb_position(int pos) {
  int x, y;
  get_position(&x, &y);
  int width, height;
  get_size(&width, &height);
  int sb_pos = m_vertical ? y : x;
  int sb_size = m_vertical ? height : width;
  
  m_thumb_pos = pos;
  if (m_thumb_pos < sb_pos + thumb_spacing())
    m_thumb_pos = sb_pos + thumb_spacing();
  else if (m_thumb_pos + m_thumb_size >= sb_pos + sb_size - thumb_spacing())
    m_thumb_pos = sb_pos + sb_size - thumb_spacing() - m_thumb_size + 1;
  
  move_puzzle_to_scroll_position();
}

void Scrollbar::move_puzzle_to_scroll_position() {
  int x, y;
  get_position(&x, &y);
  int bar_pos = m_vertical ? y : x;

  int width, height;
  get_size(&width, &height);
  int bar_size = m_vertical ? height : width;
  
  double proportion = (double)(m_thumb_pos - thumb_spacing() - bar_pos)
    / (bar_size - 2 * thumb_spacing());

  int screen_size = m_vertical
    ? m_game->screen_height() - m_game->horiz_scrollbar_height()
    : m_game->screen_width() - m_game->vert_scrollbar_width();
  int grid_size = m_vertical
    ? m_game->cell_grid_height() : m_game->cell_grid_width();

  int screen_start = std::round((-2 * (screen_size + thumb_spacing())
                                 - grid_size)
                                * proportion + screen_size - 1);
  
  int puzzle_x, puzzle_y;
  m_game->get_puzzle_coords(&puzzle_x, &puzzle_y);
  if (m_vertical)
    m_game->move_puzzle(0, screen_start - puzzle_y, true);
  else
    m_game->move_puzzle(screen_start - puzzle_x, 0, true);
}
