#include "game.h"

#include "control.h"

Control::Control(Game* game) : m_game{game}, m_x{0}, m_y{0},
                               m_width{0}, m_height{0},
                               m_hover{false}, m_selected{false},
                               m_depressed{false} {
}

void Control::get_position(int* x, int* y) const {
  if (x) *x = m_x;
  if (y) *y = m_y;
}

void Control::get_size(int* width, int* height) const {
  if (width) *width = m_width;
  if (height) *height = m_height;
}

void Control::hover_mouse(bool hovering) {
  m_hover = hovering;
}

void Control::select() {
  m_selected = true;
}

void Control::deselect() {
  m_selected = false;
  m_depressed = false;
}

void Control::depress() {
  m_depressed = true;
}

void Control::unpress() {
  m_depressed = false;
}

void Control::move(int x, int y) {
  m_x = x;
  m_y = y;
}

void Control::resize(int width, int height) {
  m_width = width;
  m_height = height;
}

void Control::key_press(KeyAction action, bool down) {
  switch (action) {
  case KeyAction::activate:
    if (down && is_selected())
      activate();
    break;
  case KeyAction::depress:
    if (down && is_selected()) {
      depress();
    } else if (!down && is_selected() && is_depressed()) {
      activate();
      unpress();
    }
    break;
  }
}

void Control::mouse_press(MouseAction action, int x, int y, bool down) {
  bool mouse_over = false;
  if (x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height)
    mouse_over = true;

  if (down) {
    if (!mouse_over) {
      deselect();
      unpress();
    } else {
      if (action == MouseAction::left)
        select();
        depress();
    }
  } else { //not down
    if (is_depressed() && mouse_over)
      activate();

    unpress();
  }
}

void Control::mouse_move(int x, int y) {
  if (x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height)
    hover_mouse(true);
  else
    hover_mouse(false);
}
