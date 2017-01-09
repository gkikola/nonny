#include <string>

#include "renderer.h"

#include "static_text.h"

StaticText::StaticText(Game* game)
  : Control{game}, m_type{Type::standard} {
}

void StaticText::draw(Renderer* renderer) const {
  renderer->render_control(this);
}

void StaticText::set_string(const std::string& str) {
  m_string = str;
}

void StaticText::get_string(std::string* str) const {
  *str = m_string;
}
