#include <Wire.h> // needed for dependency matching, see https://github.com/adafruit/Adafruit-GFX-Library/issues/307#issuecomment-647414843
#include <SPI.h>
#include <Arduino.h>
#include "user.h"
#include "network.h"
#include "display.h"

void setup() {

  Serial.begin(9600);
  Serial.println("Starting configuration...");

  // display-related

  setup_display();
  
  // network-related
  
  setup_wifi();
  setup_mqtt();
  setup_ntp();
  
}

void loop() {

}
