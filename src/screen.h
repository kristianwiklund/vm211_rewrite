#ifndef _SCREEN_H
#define _SCREEN_H

class Screen {
  
  bool isenabled = false;

public:
  bool isEnabled();
  void setEnabled(bool);
  virtual bool draw();
};

#endif
