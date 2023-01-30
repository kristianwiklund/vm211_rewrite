#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>

#define USE_ADAFRUIT_SHIELD_PINOUT

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

#include "user.h"
#include "log.h"

// this is way more complicated in the original code - there is no reasonable reason why
// we should test for all kinds of screens when we know which we have


MCUFRIEND_kbv tft;

boolean log_to_screen=false;
void display_log(char *msg) {

  if (!log_to_screen)
    return;
  
  tft.println(msg);
  
}

void enable_screen_log() {
  tft.setTextSize(1);
  tft.setTextColor(TFT_BLACK);
  log_to_screen=true;
}

void disable_screen_log() {
  log_to_screen=false;
}

void setup_display() {

  int id;
  
  logger("DISPLAY","Setting up");
  tft.reset();

  id = tft.readID();
  
  Serial.print("DISPLAY: type (should end with 9341) ");
  Serial.println(id, HEX);

  
  tft.begin(id);
  
  // green is good
  tft.fillScreen(TFT_GREEN);
  tft.setRotation(1);

  Serial.print("DISPLAY: width =");
  Serial.println(tft.width());

  Serial.print("DISPLAY: height =");
  Serial.println(tft.height());

  
  // set up the display to work as console during the rest of the boot process

  enable_screen_log();
  
  display_log("Booting device...");
  
  delay(0);
}
