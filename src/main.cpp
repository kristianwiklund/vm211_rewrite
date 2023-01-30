#include <Wire.h> // needed for dependency matching, see https://github.com/adafruit/Adafruit-GFX-Library/issues/307#issuecomment-647414843
#include <SPI.h>
#include <Arduino.h>
#include "user.h"
#include "network.h"
#include "display.h"
#include "clock.h"
#include "log.h"
#include "weather.h"
#include "postnord.h"

void setup() {

  Serial.begin(9600);
  Serial.println("Starting configuration...");

  // display-related

  setup_display();
  
  // network-related
  
  setup_wifi();
  setup_mqtt();

  // time
  setup_ntp();
  setup_clock();
  
  // sensors

  logger("main","Configuration done, yielding to loop");
  disable_screen_log();
  
  tft.fillScreen(TFT_BLACK);

  update_weather();

#ifdef ZIPCODE
  update_postnord();
#endif
}

ClockScreen clockscreen;

void loop() {

#ifdef WITH_ESP01
  // mqtt
  if (wifienabled) {
    client.loop();
    if (!client.connected())
      mqtt_reconnect();
  }
#endif

  // MVP: show only the clock screen because that is the only thing that we have implemented...

  clockscreen.draw();
  

  
  
}
