#ifndef NONNY_PREVIEW_H
#define NONNY_PREVIEW_H

#include "control.h"

class Preview : public Control {
 public:
  Preview(Game* game);
  
  void resize(int width, int height);

  int pixel_size() const { return m_pixel_size; }
 private:
  int m_pixel_size;
};

#endif
