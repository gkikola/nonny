#include "game.h"

#include "button.h"

Button::Button(Game* game) : Control{game}, m_callback_func{nullptr} {
}

void Button::set_label(const std::string& label) {
  m_label = label;
}

void Button::set_data(const std::string& data) {
  m_data = data;
}

void Button::register_callback(callback function) {
  m_callback_func = function;
}

void Button::activate() {
  if (m_callback_func)
    (*m_callback_func)(m_game, m_data);
}

void Button::draw(Renderer* renderer) const {
  renderer->render_control(this);
}
