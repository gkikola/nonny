#ifndef NONNY_SCROLLBAR_H
#define NONNY_SCROLLBAR_H

#include "control.h"

class Scrollbar : public Control {
 public:
  Scrollbar(Game* game, bool vertical);

  int thumb_position() const { return m_thumb_pos; }
  int thumb_size() const { return m_thumb_size; }

  bool is_vertical() const { return m_vertical; }

  void update_thumb(int visible_start, int visible_end, int total_size);
  bool is_thumb_being_dragged() const;

  void update(int elapsed_time);

  static int thumb_spacing();
  
  virtual void activate();
  virtual bool is_selectable() const { return false; }

  virtual void draw(Renderer* renderer) const override;

  virtual void key_press(KeyAction action, bool down) override;
  virtual void mouse_press(MouseAction action,
                           int x, int y, bool down) override;
  virtual void mouse_move(int x, int y) override;
 private:
  void set_thumb_position(int pos);
  void move_puzzle_to_scroll_position();
  
  int m_thumb_pos;
  int m_thumb_size;
  bool m_vertical;

  bool m_dragging_left;
  bool m_dragging_right;
  bool m_holding_right;
  int m_holding_time;
  int m_holding_x, m_holding_y;
  int m_down_pos;
};

#endif
