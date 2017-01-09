#include "control.h"
#include "renderer.h"

#include "control_container.h"

ControlContainer::~ControlContainer() {
  for (Control* control : m_controls)
    delete control;
}

ControlContainer::iterator ControlContainer::begin() {
  return m_controls.begin();
}

ControlContainer::iterator ControlContainer::end() {
  return m_controls.end();
}

void ControlContainer::draw(Renderer* renderer) const {
  for (auto control : m_controls)
    control->draw(renderer);
}

void ControlContainer::add_control(Control* control) {
  m_controls.push_back(control);
}
