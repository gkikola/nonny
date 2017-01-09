#ifndef NONNY_CONTROL_CONTAINER_H
#define NONNY_CONTROL_CONTAINER_H

#include <list>

class Control;

class ControlContainer {
 public:
  typedef std::list<Control*>::iterator iterator;
 private:
  typedef std::list<Control*> ControlList;

  ControlList m_controls;
};

#endif
