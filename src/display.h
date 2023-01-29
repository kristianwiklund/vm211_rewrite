#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <MCUFRIEND_kbv.h>

void setup_display();
void enable_screen_log();
void disable_screen_log();
void display_log(char *);

extern MCUFRIEND_kbv tft;

#endif
