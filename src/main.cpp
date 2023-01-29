#include <Arduino.h>
#include "user.h"
#include "network.h"

void setup() {

  Serial.begin(9600);
  Serial.println("Starting configuration...");
  setup_wifi();
  
}

void loop() {

}
