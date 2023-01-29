#include "display.h"
#include <LibPrintf.h>
#include <Arduino.h>

void logger(char *what, char *s) {

  char buf[256];
  snprintf(buf, sizeof(buf), "%s:%s", what,s);
  Serial.println(buf);
  display_log(buf);

}
