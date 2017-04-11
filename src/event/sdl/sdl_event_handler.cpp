/* Nonny -- Play and create nonogram puzzles.
 * Copyright (C) 2017 Gregory Kikola.
 *
 * This file is part of Nonny.
 *
 * Nonny is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nonny is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nonny.  If not, see <http://www.gnu.org/licenses/>.
 */
/* Written by Gregory Kikola <gkikola@gmail.com>. */

#include "event/sdl/sdl_event_handler.hpp"

#include "input/input_handler.hpp"

bool SDLEventHandler::process(InputHandler& input)
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      return false;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      {
        Key key = convert_keycode(event.key.keysym.scancode);
        bool down = (event.type == SDL_KEYDOWN);
        input.process_key_event(key, down);
      }
    case SDL_MOUSEBUTTONDOWN:
      break;
    }
  }
  
  return true;
}

Key SDLEventHandler::convert_keycode(SDL_Scancode key)
{
  switch (key) {
  case SDL_SCANCODE_0:
    return Key::digit_0;
  case SDL_SCANCODE_1:
    return Key::digit_1;
  case SDL_SCANCODE_2:
    return Key::digit_2;
  case SDL_SCANCODE_3:
    return Key::digit_3;
  case SDL_SCANCODE_4:
    return Key::digit_4;
  case SDL_SCANCODE_5:
    return Key::digit_5;
  case SDL_SCANCODE_6:
    return Key::digit_6;
  case SDL_SCANCODE_7:
    return Key::digit_7;
  case SDL_SCANCODE_8:
    return Key::digit_8;
  case SDL_SCANCODE_9:
    return Key::digit_9;
  case SDL_SCANCODE_A:
    return Key::letter_a;
  case SDL_SCANCODE_B:
    return Key::letter_b;
  case SDL_SCANCODE_C:
    return Key::letter_c;
  case SDL_SCANCODE_D:
    return Key::letter_d;
  case SDL_SCANCODE_E:
    return Key::letter_e;
  case SDL_SCANCODE_F:
    return Key::letter_f;
  case SDL_SCANCODE_G:
    return Key::letter_g;
  case SDL_SCANCODE_H:
    return Key::letter_h;
  case SDL_SCANCODE_I:
    return Key::letter_i;
  case SDL_SCANCODE_J:
    return Key::letter_j;
  case SDL_SCANCODE_K:
    return Key::letter_k;
  case SDL_SCANCODE_L:
    return Key::letter_l;
  case SDL_SCANCODE_M:
    return Key::letter_m;
  case SDL_SCANCODE_N:
    return Key::letter_n;
  case SDL_SCANCODE_O:
    return Key::letter_o;
  case SDL_SCANCODE_P:
    return Key::letter_p;
  case SDL_SCANCODE_Q:
    return Key::letter_q;
  case SDL_SCANCODE_R:
    return Key::letter_r;
  case SDL_SCANCODE_S:
    return Key::letter_s;
  case SDL_SCANCODE_T:
    return Key::letter_t;
  case SDL_SCANCODE_U:
    return Key::letter_u;
  case SDL_SCANCODE_V:
    return Key::letter_v;
  case SDL_SCANCODE_W:
    return Key::letter_w;
  case SDL_SCANCODE_X:
    return Key::letter_x;
  case SDL_SCANCODE_Y:
    return Key::letter_y;
  case SDL_SCANCODE_Z:
    return Key::letter_z;
  case SDL_SCANCODE_APOSTROPHE:
    return Key::quote;
  case SDL_SCANCODE_BACKSLASH:
    return Key::back_slash;
  case SDL_SCANCODE_BACKSPACE:
    return Key::backspace;
  case SDL_SCANCODE_CAPSLOCK:
    return Key::caps_lock;
  case SDL_SCANCODE_COMMA:
    return Key::comma;
  case SDL_SCANCODE_DELETE:
    return Key::del;
  case SDL_SCANCODE_DOWN:
    return Key::down;
  case SDL_SCANCODE_END:
    return Key::end;
  case SDL_SCANCODE_EQUALS:
    return Key::equals;
  case SDL_SCANCODE_ESCAPE:
    return Key::escape;
  case SDL_SCANCODE_F1:
    return Key::f1;
  case SDL_SCANCODE_F2:
    return Key::f2;
  case SDL_SCANCODE_F3:
    return Key::f3;
  case SDL_SCANCODE_F4:
    return Key::f4;
  case SDL_SCANCODE_F5:
    return Key::f5;
  case SDL_SCANCODE_F6:
    return Key::f6;
  case SDL_SCANCODE_F7:
    return Key::f7;
  case SDL_SCANCODE_F8:
    return Key::f8;
  case SDL_SCANCODE_F9:
    return Key::f9;
  case SDL_SCANCODE_F10:
    return Key::f10;
  case SDL_SCANCODE_F11:
    return Key::f11;
  case SDL_SCANCODE_F12:
    return Key::f12;
  case SDL_SCANCODE_GRAVE:
    return Key::back_quote;
  case SDL_SCANCODE_HOME:
    return Key::home;
  case SDL_SCANCODE_INSERT:
    return Key::ins;
  case SDL_SCANCODE_KP_0:
    return Key::kp_ins;
  case SDL_SCANCODE_KP_1:
    return Key::kp_end;
  case SDL_SCANCODE_KP_2:
    return Key::kp_down;
  case SDL_SCANCODE_KP_3:
    return Key::kp_page_down;
  case SDL_SCANCODE_KP_4:
    return Key::kp_left;
  case SDL_SCANCODE_KP_5:
    return Key::kp_5;
  case SDL_SCANCODE_KP_6:
    return Key::kp_right;
  case SDL_SCANCODE_KP_7:
    return Key::kp_home;
  case SDL_SCANCODE_KP_8:
    return Key::kp_up;
  case SDL_SCANCODE_KP_9:
    return Key::kp_page_up;
  case SDL_SCANCODE_KP_DIVIDE:
    return Key::kp_divide;
  case SDL_SCANCODE_KP_ENTER:
    return Key::kp_enter;
  case SDL_SCANCODE_KP_MINUS:
    return Key::kp_minus;
  case SDL_SCANCODE_KP_MULTIPLY:
    return Key::kp_times;
  case SDL_SCANCODE_KP_PERIOD:
    return Key::kp_del;
  case SDL_SCANCODE_KP_PLUS:
    return Key::kp_plus;
  case SDL_SCANCODE_LALT:
    return Key::lalt;
  case SDL_SCANCODE_LCTRL:
    return Key::lctrl;
  case SDL_SCANCODE_LEFT:
    return Key::left;
  case SDL_SCANCODE_LEFTBRACKET:
    return Key::lbracket;
  case SDL_SCANCODE_LGUI:
    return Key::lsuper;
  case SDL_SCANCODE_LSHIFT:
    return Key::lshift;
  case SDL_SCANCODE_MENU:
    return Key::menu;
  case SDL_SCANCODE_MINUS:
    return Key::minus;
  case SDL_SCANCODE_NUMLOCKCLEAR:
    return Key::num_lock;
  case SDL_SCANCODE_PAGEDOWN:
    return Key::page_down;
  case SDL_SCANCODE_PAGEUP:
    return Key::page_up;
  case SDL_SCANCODE_PAUSE:
    return Key::pause;
  case SDL_SCANCODE_PERIOD:
    return Key::period;
  case SDL_SCANCODE_PRINTSCREEN:
    return Key::print_screen;
  case SDL_SCANCODE_RALT:
    return Key::ralt;
  case SDL_SCANCODE_RCTRL:
    return Key::rctrl;
  case SDL_SCANCODE_RETURN:
    return Key::enter;
  case SDL_SCANCODE_RGUI:
    return Key::rsuper;
  case SDL_SCANCODE_RIGHT:
    return Key::right;
  case SDL_SCANCODE_RIGHTBRACKET:
    return Key::rbracket;
  case SDL_SCANCODE_RSHIFT:
    return Key::rshift;
  case SDL_SCANCODE_SCROLLLOCK:
    return Key::scroll_lock;
  case SDL_SCANCODE_SEMICOLON:
    return Key::semicolon;
  case SDL_SCANCODE_SLASH:
    return Key::slash;
  case SDL_SCANCODE_SPACE:
    return Key::space;
  case SDL_SCANCODE_TAB:
    return Key::tab;
  case SDL_SCANCODE_UP:
    return Key::up;
  default:
    return Key::unknown;
  }
}
