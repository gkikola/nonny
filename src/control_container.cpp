#include <iostream>
#include "control.h"
#include "renderer.h"

#include "control_container.h"

ControlContainer::~ControlContainer() {
  clear();
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

void ControlContainer::clear() {
  for (Control* control : m_controls)
    delete control;

  m_controls.clear();
}

void ControlContainer::add_control(Control* control) {
  m_controls.push_back(control);
}

void ControlContainer::select_next(bool backwards) {
  if (!backwards) {
    bool found_selected = false;
    bool found_selectable = false;
  
    for (iterator it = begin(); it != end(); ++it) {
      if (found_selected) {
        if ((*it)->is_selectable()) {
          found_selectable = true;
          (*it)->select();
          break;
        }
      } else if ((*it)->is_selected()) {
        found_selected = true;
        (*it)->deselect();
      }
    }

    //if nothing found just pick the first one
    if (!found_selected || !found_selectable) {
      for (iterator it = begin(); it != end(); ++it) {
        if ((*it)->is_selectable()) {
          (*it)->select();
          break;
        }
      }
    }
  } else { //backwards
    iterator last_found = end();
    for (iterator it = begin(); it != end(); ++it) {
      if ((*it)->is_selected()) {
        (*it)->deselect();
        if (last_found != end()) {
          (*last_found)->select();
          break;
        }
      }
      if ((*it)->is_selectable())
        last_found = it;
    }

    if (last_found != end())
      (*last_found)->select();
  }
}

Control* ControlContainer::selection() {
  for (Control* control : *this) {
    if (control->is_selected())
      return control;
  }

  return nullptr;
}

void ControlContainer::clear_selection() {
  Control* control = selection();

  if (control)
    control->deselect();
}
