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

#include "input/key.hpp"

std::string key_name(Key key)
{
  switch (key) {
  case Key::escape:
    return "Escape";
  case Key::f1:
    return "F1";
  case Key::f2:
    return "F2";
  case Key::f3:
    return "F3";
  case Key::f4:
    return "F4";
  case Key::f5:
    return "F5";
  case Key::f6:
    return "F6";
  case Key::f7:
    return "F7";
  case Key::f8:
    return "F8";
  case Key::f9:
    return "F9";
  case Key::f10:
    return "F10";
  case Key::f11:
    return "F11";
  case Key::f12:
    return "F12";
  case Key::digit_0:
    return "0";
  case Key::digit_1:
    return "1";
  case Key::digit_2:
    return "2";
  case Key::digit_3:
    return "3";
  case Key::digit_4:
    return "4";
  case Key::digit_5:
    return "5";
  case Key::digit_6:
    return "6";
  case Key::digit_7:
    return "7";
  case Key::digit_8:
    return "8";
  case Key::digit_9:
    return "9";
  case Key::minus:
    return "-";
  case Key::equals:
    return "=";
  case Key::back_quote:
    return "`";
  case Key::letter_a:
    return "A";
  case Key::letter_b:
    return "B";
  case Key::letter_c:
    return "C";
  case Key::letter_d:
    return "D";
  case Key::letter_e:
    return "E";
  case Key::letter_f:
    return "F";
  case Key::letter_g:
    return "G";
  case Key::letter_h:
    return "H";
  case Key::letter_i:
    return "I";
  case Key::letter_j:
    return "J";
  case Key::letter_k:
    return "K";
  case Key::letter_l:
    return "L";
  case Key::letter_m:
    return "M";
  case Key::letter_n:
    return "N";
  case Key::letter_o:
    return "O";
  case Key::letter_p:
    return "P";
  case Key::letter_q:
    return "Q";
  case Key::letter_r:
    return "R";
  case Key::letter_s:
    return "S";
  case Key::letter_t:
    return "T";
  case Key::letter_u:
    return "U";
  case Key::letter_v:
    return "V";
  case Key::letter_w:
    return "W";
  case Key::letter_x:
    return "X";
  case Key::letter_y:
    return "Y";
  case Key::letter_z:
    return "Z";
  case Key::space:
    return "Space";
  case Key::lbracket:
    return "[";
  case Key::rbracket:
    return "]";
  case Key::slash:
    return "/";
  case Key::back_slash:
    return "\\";
  case Key::period:
    return ".";
  case Key::comma:
    return ",";
  case Key::semicolon:
    return ";";
  case Key::quote:
    return "'";
  case Key::print_screen:
    return "Print Screen";
  case Key::scroll_lock:
    return "Scroll Lock";
  case Key::pause:
    return "Pause";
  case Key::enter:
    return "Enter";
  case Key::backspace:
    return "Backspace";
  case Key::tab:
    return "Tab";
  case Key::caps_lock:
    return "Caps Lock";
  case Key::lshift:
    return "Left Shift";
  case Key::rshift:
    return "Right Shift";
  case Key::lctrl:
    return "Left Ctrl";
  case Key::rctrl:
    return "Right Ctrl";
  case Key::lalt:
    return "Left Alt";
  case Key::ralt:
    return "Right Alt";
  case Key::lsuper:
    return "Left Super";
  case Key::rsuper:
    return "Right Super";
  case Key::menu:
    return "Menu";
  case Key::ins:
    return "Insert";
  case Key::del:
    return "Delete";
  case Key::home:
    return "Home";
  case Key::end:
    return "End";
  case Key::page_up:
    return "Page Up";
  case Key::page_down:
    return "Page Down";
  case Key::left:
    return "Left";
  case Key::right:
    return "Right";
  case Key::up:
    return "Up";
  case Key::down:
    return "Down";
  case Key::num_lock:
    return "Num Lock";
  case Key::kp_divide:
    return "Keypad /";
  case Key::kp_times:
    return "Keypad *";
  case Key::kp_minus:
    return "Keypad -";
  case Key::kp_plus:
    return "Keypad +";
  case Key::kp_enter:
    return "Keypad Enter";
  case Key::kp_home:
    return "Keypad 7";
  case Key::kp_end:
    return "Keypad 1";
  case Key::kp_page_up:
    return "Keypad 9";
  case Key::kp_page_down:
    return "Keypad 3";
  case Key::kp_5:
    return "Keypad 5";
  case Key::kp_left:
    return "Keypad 4";
  case Key::kp_right:
    return "Keypad 6";
  case Key::kp_up:
    return "Keypad 8";
  case Key::kp_down:
    return "Keypad 2";
  case Key::kp_ins:
    return "Keypad 0";
  case Key::kp_del:
    return "Keypad .";
  default:
    return "Unknown";
  }
}
