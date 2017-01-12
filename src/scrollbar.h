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

  static int thumb_spacing();
  
  virtual void activate();
  virtual bool is_selectable() const { return false; }

  virtual void draw(Renderer* renderer) const override;
 private:
  int m_thumb_pos;
  int m_thumb_size;
  bool m_vertical;
};

#endif
