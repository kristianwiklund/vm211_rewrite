#include "screen.h"

bool Screen::isEnabled() { return isenabled; }

void Screen::setEnabled(bool x) {
  isenabled = x;
}
// draw the screen contents
bool Screen::draw() {
  // do something
  // return false if we are not supposed to draw this screen
  return isenabled;
};
