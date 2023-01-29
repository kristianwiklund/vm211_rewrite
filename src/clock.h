#ifndef CLOCK_H
#define CLOCK_H
#include <time.h>
#include "screen.h"

time_t getNtpTime();
bool setup_clock();

class ClockScreen : public Screen {

public:
  ClockScreen();
  bool draw();
};



#endif
