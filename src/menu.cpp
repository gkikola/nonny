#include "game.h"

#include "menu.h"

const int menu_spacing = 16;

Menu::Menu(Game* game) : m_game{game} {
}

void Menu::add_control(Control* control) {
  ControlContainer::add_control(control);
}

void Menu::arrange_controls(int screen_width, int screen_height) {
  int total_height = menu_spacing;
  for (Control* control : *this) {
    int width, height;
    control->get_size(&width, &height);
    total_height += menu_spacing + height;
  }

  int y = screen_height / 2 - total_height / 2;
  
  for (Control* control : *this) {
    int width, height;
    control->get_size(&width, &height);
    control->move(screen_width / 2 - width / 2, y);
    y += height;
    y += menu_spacing;
  }
}
