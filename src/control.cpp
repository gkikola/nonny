#include "game.h"

#include "control.h"

Control::Control(Game* game) : m_game{game}, m_x{0}, m_y{0},
                               m_width{0}, m_height{0},
                               m_hover{false}, m_selected{false} {
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
