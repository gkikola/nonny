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

namespace Keyboard {
  std::string key_name(Key key)
  {
    switch (key) {
    case escape:
      return "Escape";
    case f1:
      return "F1";
    case f2:
      return "F2";
    case f3:
      return "F3";
    case f4:
      return "F4";
    case f5:
      return "F5";
    case f6:
      return "F6";
    case f7:
      return "F7";
    case f8:
      return "F8";
    case f9:
      return "F9";
    case f10:
      return "F10";
    case f11:
      return "F11";
    case f12:
      return "F12";
    case digit_0:
      return "0";
    case digit_1:
      return "1";
    case digit_2:
      return "2";
    case digit_3:
      return "3";
    case digit_4:
      return "4";
    case digit_5:
      return "5";
    case digit_6:
      return "6";
    case digit_7:
      return "7";
    case digit_8:
      return "8";
    case digit_9:
      return "9";
    case minus:
      return "-";
    case equals:
      return "=";
    case back_quote:
      return "`";
    case letter_a:
      return "A";
    case letter_b:
      return "B";
    case letter_c:
      return "C";
    case letter_d:
      return "D";
    case letter_e:
      return "E";
    case letter_f:
      return "F";
    case letter_g:
      return "G";
    case letter_h:
      return "H";
    case letter_i:
      return "I";
    case letter_j:
      return "J";
    case letter_k:
      return "K";
    case letter_l:
      return "L";
    case letter_m:
      return "M";
    case letter_n:
      return "N";
    case letter_o:
      return "O";
    case letter_p:
      return "P";
    case letter_q:
      return "Q";
    case letter_r:
      return "R";
    case letter_s:
      return "S";
    case letter_t:
      return "T";
    case letter_u:
      return "U";
    case letter_v:
      return "V";
    case letter_w:
      return "W";
    case letter_x:
      return "X";
    case letter_y:
      return "Y";
    case letter_z:
      return "Z";
    case space:
      return "Space";
    case lbracket:
      return "[";
    case rbracket:
      return "]";
    case slash:
      return "/";
    case back_slash:
      return "\\";
    case period:
      return ".";
    case comma:
      return ",";
    case semicolon:
      return ";";
    case quote:
      return "'";
    case print_screen:
      return "Print Screen";
    case scroll_lock:
      return "Scroll Lock";
    case pause:
      return "Pause";
    case enter:
      return "Enter";
    case backspace:
      return "Backspace";
    case tab:
      return "Tab";
    case caps_lock:
      return "Caps Lock";
    case lshift:
      return "Left Shift";
    case rshift:
      return "Right Shift";
    case lctrl:
      return "Left Ctrl";
    case rctrl:
      return "Right Ctrl";
    case lalt:
      return "Left Alt";
    case ralt:
      return "Right Alt";
    case lsuper:
      return "Left Super";
    case rsuper:
      return "Right Super";
    case menu:
      return "Menu";
    case ins:
      return "Insert";
    case del:
      return "Delete";
    case home:
      return "Home";
    case end:
      return "End";
    case page_up:
      return "Page Up";
    case page_down:
      return "Page Down";
    case left:
      return "Left";
    case right:
      return "Right";
    case up:
      return "Up";
    case down:
      return "Down";
    case num_lock:
      return "Num Lock";
    case kp_divide:
      return "Keypad /";
    case kp_times:
      return "Keypad *";
    case kp_minus:
      return "Keypad -";
    case kp_plus:
      return "Keypad +";
    case kp_enter:
      return "Keypad Enter";
    case kp_home:
      return "Keypad 7";
    case kp_end:
      return "Keypad 1";
    case kp_page_up:
      return "Keypad 9";
    case kp_page_down:
      return "Keypad 3";
    case kp_5:
      return "Keypad 5";
    case kp_left:
      return "Keypad 4";
    case kp_right:
      return "Keypad 6";
    case kp_up:
      return "Keypad 8";
    case kp_down:
      return "Keypad 2";
    case kp_ins:
      return "Keypad 0";
    case kp_del:
      return "Keypad .";
    default:
      return "Unknown";
    }
  }
}

namespace Mouse {
  std::string button_name(Button button)
  {
    switch (button) {
    case left:
      return "Left Click";
    case right:
      return "Right Click";
    case middle:
      return "Middle Click";
    case back:
      return "Back";
    case forward:
      return "Forward";
    default:
      return "Unknown";
    }
  }
}
