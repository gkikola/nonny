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
#include "view/view_manager.hpp"

void SDLEventHandler::process(InputHandler& input, ViewManager& view_mgr)
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      view_mgr.schedule_action(ViewManager::Action::quit_game);
      break;
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
        view_mgr.resize(event.window.data1, event.window.data2);
        break;
      }
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      {
        Keyboard::Key key = convert_keycode(event.key.keysym.scancode);
        bool down = (event.type == SDL_KEYDOWN);
        input.process_key_event(key, down);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      {
        Mouse::Button button = convert_mouse_button(event.button.button);
        bool down = (event.type == SDL_MOUSEBUTTONDOWN);
        unsigned num_clicks = event.button.clicks;
        input.process_mouse_button_event(button, down, num_clicks);
      }
      break;
    case SDL_MOUSEMOTION:
      input.process_mouse_move_event(event.motion.x, event.motion.y);
      break;
    case SDL_MOUSEWHEEL:
      {
        int vert = event.wheel.y;
        int horiz = event.wheel.x;
        if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
          vert = -vert;
          horiz = -horiz;
        }
        input.process_mouse_wheel_event(vert, horiz);
      }
      break;
    }
  }
}

Keyboard::Key SDLEventHandler::convert_keycode(SDL_Scancode key)
{
  switch (key) {
  case SDL_SCANCODE_0:
    return Keyboard::digit_0;
  case SDL_SCANCODE_1:
    return Keyboard::digit_1;
  case SDL_SCANCODE_2:
    return Keyboard::digit_2;
  case SDL_SCANCODE_3:
    return Keyboard::digit_3;
  case SDL_SCANCODE_4:
    return Keyboard::digit_4;
  case SDL_SCANCODE_5:
    return Keyboard::digit_5;
  case SDL_SCANCODE_6:
    return Keyboard::digit_6;
  case SDL_SCANCODE_7:
    return Keyboard::digit_7;
  case SDL_SCANCODE_8:
    return Keyboard::digit_8;
  case SDL_SCANCODE_9:
    return Keyboard::digit_9;
  case SDL_SCANCODE_A:
    return Keyboard::letter_a;
  case SDL_SCANCODE_B:
    return Keyboard::letter_b;
  case SDL_SCANCODE_C:
    return Keyboard::letter_c;
  case SDL_SCANCODE_D:
    return Keyboard::letter_d;
  case SDL_SCANCODE_E:
    return Keyboard::letter_e;
  case SDL_SCANCODE_F:
    return Keyboard::letter_f;
  case SDL_SCANCODE_G:
    return Keyboard::letter_g;
  case SDL_SCANCODE_H:
    return Keyboard::letter_h;
  case SDL_SCANCODE_I:
    return Keyboard::letter_i;
  case SDL_SCANCODE_J:
    return Keyboard::letter_j;
  case SDL_SCANCODE_K:
    return Keyboard::letter_k;
  case SDL_SCANCODE_L:
    return Keyboard::letter_l;
  case SDL_SCANCODE_M:
    return Keyboard::letter_m;
  case SDL_SCANCODE_N:
    return Keyboard::letter_n;
  case SDL_SCANCODE_O:
    return Keyboard::letter_o;
  case SDL_SCANCODE_P:
    return Keyboard::letter_p;
  case SDL_SCANCODE_Q:
    return Keyboard::letter_q;
  case SDL_SCANCODE_R:
    return Keyboard::letter_r;
  case SDL_SCANCODE_S:
    return Keyboard::letter_s;
  case SDL_SCANCODE_T:
    return Keyboard::letter_t;
  case SDL_SCANCODE_U:
    return Keyboard::letter_u;
  case SDL_SCANCODE_V:
    return Keyboard::letter_v;
  case SDL_SCANCODE_W:
    return Keyboard::letter_w;
  case SDL_SCANCODE_X:
    return Keyboard::letter_x;
  case SDL_SCANCODE_Y:
    return Keyboard::letter_y;
  case SDL_SCANCODE_Z:
    return Keyboard::letter_z;
  case SDL_SCANCODE_APOSTROPHE:
    return Keyboard::quote;
  case SDL_SCANCODE_BACKSLASH:
    return Keyboard::back_slash;
  case SDL_SCANCODE_BACKSPACE:
    return Keyboard::backspace;
  case SDL_SCANCODE_CAPSLOCK:
    return Keyboard::caps_lock;
  case SDL_SCANCODE_COMMA:
    return Keyboard::comma;
  case SDL_SCANCODE_DELETE:
    return Keyboard::del;
  case SDL_SCANCODE_DOWN:
    return Keyboard::down;
  case SDL_SCANCODE_END:
    return Keyboard::end;
  case SDL_SCANCODE_EQUALS:
    return Keyboard::equals;
  case SDL_SCANCODE_ESCAPE:
    return Keyboard::escape;
  case SDL_SCANCODE_F1:
    return Keyboard::f1;
  case SDL_SCANCODE_F2:
    return Keyboard::f2;
  case SDL_SCANCODE_F3:
    return Keyboard::f3;
  case SDL_SCANCODE_F4:
    return Keyboard::f4;
  case SDL_SCANCODE_F5:
    return Keyboard::f5;
  case SDL_SCANCODE_F6:
    return Keyboard::f6;
  case SDL_SCANCODE_F7:
    return Keyboard::f7;
  case SDL_SCANCODE_F8:
    return Keyboard::f8;
  case SDL_SCANCODE_F9:
    return Keyboard::f9;
  case SDL_SCANCODE_F10:
    return Keyboard::f10;
  case SDL_SCANCODE_F11:
    return Keyboard::f11;
  case SDL_SCANCODE_F12:
    return Keyboard::f12;
  case SDL_SCANCODE_GRAVE:
    return Keyboard::back_quote;
  case SDL_SCANCODE_HOME:
    return Keyboard::home;
  case SDL_SCANCODE_INSERT:
    return Keyboard::ins;
  case SDL_SCANCODE_KP_0:
    return Keyboard::kp_ins;
  case SDL_SCANCODE_KP_1:
    return Keyboard::kp_end;
  case SDL_SCANCODE_KP_2:
    return Keyboard::kp_down;
  case SDL_SCANCODE_KP_3:
    return Keyboard::kp_page_down;
  case SDL_SCANCODE_KP_4:
    return Keyboard::kp_left;
  case SDL_SCANCODE_KP_5:
    return Keyboard::kp_5;
  case SDL_SCANCODE_KP_6:
    return Keyboard::kp_right;
  case SDL_SCANCODE_KP_7:
    return Keyboard::kp_home;
  case SDL_SCANCODE_KP_8:
    return Keyboard::kp_up;
  case SDL_SCANCODE_KP_9:
    return Keyboard::kp_page_up;
  case SDL_SCANCODE_KP_DIVIDE:
    return Keyboard::kp_divide;
  case SDL_SCANCODE_KP_ENTER:
    return Keyboard::kp_enter;
  case SDL_SCANCODE_KP_MINUS:
    return Keyboard::kp_minus;
  case SDL_SCANCODE_KP_MULTIPLY:
    return Keyboard::kp_times;
  case SDL_SCANCODE_KP_PERIOD:
    return Keyboard::kp_del;
  case SDL_SCANCODE_KP_PLUS:
    return Keyboard::kp_plus;
  case SDL_SCANCODE_LALT:
    return Keyboard::lalt;
  case SDL_SCANCODE_LCTRL:
    return Keyboard::lctrl;
  case SDL_SCANCODE_LEFT:
    return Keyboard::left;
  case SDL_SCANCODE_LEFTBRACKET:
    return Keyboard::lbracket;
  case SDL_SCANCODE_LGUI:
    return Keyboard::lsuper;
  case SDL_SCANCODE_LSHIFT:
    return Keyboard::lshift;
  case SDL_SCANCODE_MENU:
    return Keyboard::menu;
  case SDL_SCANCODE_MINUS:
    return Keyboard::minus;
  case SDL_SCANCODE_NUMLOCKCLEAR:
    return Keyboard::num_lock;
  case SDL_SCANCODE_PAGEDOWN:
    return Keyboard::page_down;
  case SDL_SCANCODE_PAGEUP:
    return Keyboard::page_up;
  case SDL_SCANCODE_PAUSE:
    return Keyboard::pause;
  case SDL_SCANCODE_PERIOD:
    return Keyboard::period;
  case SDL_SCANCODE_PRINTSCREEN:
    return Keyboard::print_screen;
  case SDL_SCANCODE_RALT:
    return Keyboard::ralt;
  case SDL_SCANCODE_RCTRL:
    return Keyboard::rctrl;
  case SDL_SCANCODE_RETURN:
    return Keyboard::enter;
  case SDL_SCANCODE_RGUI:
    return Keyboard::rsuper;
  case SDL_SCANCODE_RIGHT:
    return Keyboard::right;
  case SDL_SCANCODE_RIGHTBRACKET:
    return Keyboard::rbracket;
  case SDL_SCANCODE_RSHIFT:
    return Keyboard::rshift;
  case SDL_SCANCODE_SCROLLLOCK:
    return Keyboard::scroll_lock;
  case SDL_SCANCODE_SEMICOLON:
    return Keyboard::semicolon;
  case SDL_SCANCODE_SLASH:
    return Keyboard::slash;
  case SDL_SCANCODE_SPACE:
    return Keyboard::space;
  case SDL_SCANCODE_TAB:
    return Keyboard::tab;
  case SDL_SCANCODE_UP:
    return Keyboard::up;
  default:
    return Keyboard::unknown;
  }
}

Mouse::Button SDLEventHandler::convert_mouse_button(Uint8 button)
{
  switch (button) {
  case SDL_BUTTON_LEFT:
    return Mouse::left;
  case SDL_BUTTON_MIDDLE:
    return Mouse::middle;
  case SDL_BUTTON_RIGHT:
    return Mouse::right;
  case SDL_BUTTON_X1:
    return Mouse::back;
  case SDL_BUTTON_X2:
    return Mouse::forward;
  default:
    return Mouse::unknown;
  }
}
