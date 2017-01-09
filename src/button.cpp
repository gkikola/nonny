#include "game.h"

#include "button.h"

Button::Button(Game* game) : Control{game} {
}

void Button::set_label(const std::string& label) {
  m_label = label;
}

void Button::register_callback(callback function) {
  m_callback_func = function;
}

void Button::activate() {
  (*m_callback_func)(m_game);
}

void Button::draw(Renderer* renderer) const {
  renderer->render_control(this);
}
