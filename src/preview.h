#ifndef NONNY_PREVIEW_H
#define NONNY_PREVIEW_H

#include "control.h"

class Renderer;

class Preview : public Control {
 public:
  explicit Preview(Game* game);
  
  int pixel_size() const { return m_pixel_size; }
  void update_pixel_size();

  virtual void activate() override {}
  virtual void draw(Renderer* renderer) const override;

  virtual bool is_selectable() const override { return false; }
 private:
  int m_pixel_size;
};

#endif
