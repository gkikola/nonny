#include "game.h"

#include "menu.h"

Menu::Menu(Game* game) : m_game{game} {
}

void Menu::add_control(Control* control) {
  ControlContainer::add_control(control);
}
