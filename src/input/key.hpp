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

#ifndef NONNY_KEY_HPP
#define NONNY_KEY_HPP

#include <string>

namespace Keyboard {

  enum Key {
    unknown, escape,
    f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
    digit_0, digit_1, digit_2, digit_3, digit_4, digit_5, digit_6,
    digit_7, digit_8, digit_9, minus, equals, back_quote,
    letter_a, letter_b, letter_c, letter_d, letter_e, letter_f, letter_g,
    letter_h, letter_i, letter_j, letter_k, letter_l, letter_m, letter_n,
    letter_o, letter_p, letter_q, letter_r, letter_s, letter_t, letter_u,
    letter_v, letter_w, letter_x, letter_y, letter_z, space,
    lbracket, rbracket, slash, back_slash, period, comma,
    semicolon, quote,
    print_screen, scroll_lock, pause,
    enter, backspace, tab, caps_lock,
    lshift, rshift, lctrl, rctrl, lalt, ralt, lsuper, rsuper, menu,
    ins, del, home, end, page_up, page_down,
    left, right, up, down,
    num_lock, kp_divide, kp_times, kp_minus, kp_plus, kp_enter,
    kp_home, kp_end, kp_page_up, kp_page_down, kp_5,
    kp_left, kp_right, kp_up, kp_down, kp_ins, kp_del
  };

  constexpr int num_keys = kp_del + 1;

  std::string key_name(Key key);
}

namespace Mouse {

  enum Button {
    unknown, left, right, middle, back, forward
  };

  constexpr int num_buttons = forward + 1;

  enum class Cursor {
    unknown, arrow, ibeam, wait, crosshair, size_we, size_ns, no, hand
  };

  std::string button_name(Button button);
}

#endif
