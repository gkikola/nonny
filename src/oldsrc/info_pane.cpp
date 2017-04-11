#include <cmath>
#include "button.h"
#include "game.h"
#include "preview.h"
#include "static_text.h"

#include "info_pane.h"

const int default_spacing = 20;
const int text_height_std = 18;
const int text_height_heading = 32;
const int text_height_small = 14;
const int button_size = 48;
const int space_between_buttons = 12;
const int slide_speed = 2048;

void open_menu(Game* game, const std::string& data);
void toggle_hints(Game* game, const std::string& data);
void toggle_errors(Game* game, const std::string& data);

InfoPane::InfoPane(Game* game) : m_game{game}, m_width{0}, m_target_width{0},
                                 m_slide_delta{0.0}, m_sliding{false} {
  add_control(m_preview = new Preview(game));
  add_control(m_title = new StaticText(game));
  add_control(m_size = new StaticText(game));
  add_control(m_author = new StaticText(game));
  add_control(m_menu = new Button(game));
  add_control(m_hints = new Button(game));
  add_control(m_errors = new Button(game));
}

void InfoPane::get_preview_position(int* x, int* y) const {
  m_preview->get_position(x, y);
}

void InfoPane::get_preview_size(int* width, int* height) const {
  m_preview->get_size(width, height);
}

void InfoPane::update(int elapsed_time) {
  if (m_sliding) {
    if (m_width != m_target_width) {
      int multiplier = 1 + m_game->screen_width() / 805;
      m_slide_delta += multiplier * slide_speed * (elapsed_time / 1000.0);
      int amount = floor(m_slide_delta);
      if (amount > 0) {
        m_slide_delta = 0;
        
        if (m_width > m_target_width)
          amount = -amount;

        m_width += amount;
        if ((amount > 0 && m_width >= m_target_width)
            || (amount < 0 && m_width <= m_target_width)) {
          m_sliding = false;
          m_width = m_target_width;
        } //if m_width overtook m_target_width
      } //if amount > 0
    } //if m_width != m_target_width
  } //if m_sliding
}

void InfoPane::draw(Renderer* renderer) const {
  if (!m_sliding)
    ControlContainer::draw(renderer);
}

void InfoPane::slide_pane(int starting_width, int ending_width) {
  m_target_width = ending_width;
  m_width = starting_width;
  m_slide_delta = 0;
  m_sliding = true;
}

void InfoPane::setup_controls(int pane_width) {
  int y = default_spacing;

  std::string title = "";
  if (m_game->is_puzzle_loaded()) {
    const std::string *ptitle = m_game->puzzle().find_property("title");
    if (ptitle) title = *ptitle;
  }
  
  if (title.length() > 0) {  
    m_title->move(default_spacing, y);
    m_title->resize(pane_width - 2 * default_spacing, text_height_heading);
    m_title->set_string(title);
    m_title->set_help_text(title);
    m_title->set_type(StaticText::Type::heading);
  
    y += text_height_heading;
    y += default_spacing;
  }

  std::string size_str = "";
  int puzzle_width = 0, puzzle_height = 0;
  if (m_game->is_puzzle_loaded()) {
    puzzle_width = m_game->puzzle().width();
    puzzle_height = m_game->puzzle().height();
    size_str = std::to_string(puzzle_width);
    size_str += " × ";
    size_str += std::to_string(puzzle_height);
  }
  
  m_size->move(default_spacing, y);
  m_size->resize(pane_width - 2 * default_spacing, text_height_std);
  m_size->set_string(size_str);
  m_size->set_help_text("Puzzle size");
  m_size->set_type(StaticText::Type::standard);

  y += text_height_std;
  y += default_spacing;
  
  m_preview->resize(pane_width - 2 * default_spacing,
                    pane_width - 2 * default_spacing);
  m_preview->update_pixel_size();

  int pixel_size = m_preview->pixel_size();
  int width = pixel_size * puzzle_width;
  int height = pixel_size * puzzle_height;
  
  m_preview->resize(width, height);
  m_preview->move(pane_width / 2 - width / 2, y);

  y += height;
  y += default_spacing;

  std::string author = "";
  if (m_game->is_puzzle_loaded()) {
    const std::string* pauthor = m_game->puzzle().find_property("author");
    if (pauthor) author = *pauthor;
  }

  if (author.length() > 0) {
    m_author->move(default_spacing, y);
    m_author->resize(pane_width - 2 * default_spacing, text_height_small);
    m_author->set_string("Author: " + author);
    m_author->set_help_text("Puzzle creator");
    m_author->set_type(StaticText::Type::small);

    y += text_height_small;
    y += default_spacing;
  }

  m_menu->move(pane_width / 2 - 3 * button_size / 2 - space_between_buttons, y);
  m_menu->resize(button_size, text_height_std + default_spacing);
  m_menu->set_label("MENU_SYMBOL");
  m_menu->set_help_text("Open menu");
  m_menu->register_callback(open_menu);

  m_hints->move(pane_width / 2 - button_size / 2, y);
  m_hints->resize(48, text_height_std + default_spacing);
  m_hints->set_label("?");
  m_hints->set_help_text("Toggle blue hint numbers");
  m_hints->register_callback(toggle_hints);

  m_errors->move(pane_width / 2 + button_size / 2 + space_between_buttons, y);
  m_errors->resize(button_size, text_height_std + default_spacing);
  m_errors->set_label("X");
  m_errors->set_help_text("Toggle error visibility");
  m_errors->register_callback(toggle_errors);

  y += text_height_std;
  y += default_spacing;

  //figure out how much space is left over
  int diff = (m_game->screen_height() - y) / 2;

  for (Control* control : *this) {
    int x, y;
    control->get_position(&x, &y);
    control->move(x, y + diff);
  }
}

void open_menu(Game* game, const std::string& data) {
  game->set_state(GameState::main_menu);
}

void toggle_hints(Game* game, const std::string& data) {
}

void toggle_errors(Game* game, const std::string& data) {
}