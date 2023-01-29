// draw a clock on the screen - requires that a real time clock chip is present.
//
// (C) 2020 Kristian Wiklund
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "user.h"
#include "log.h"

#ifdef WITH_ESP01

#ifndef NTP_HOST
#error "Configure NTP_HOST in user.h"
#endif
  

#include <Arduino.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <time.h>
#include <WiFiEspAT.h>
#include <sys/types.h>

// globals

WiFiUDP ntpUDP;
WiFiClient wificlient;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, NTP_HOST, 0, 60000);

// mqtt connection
PubSubClient client(wificlient);

// flag to know if we have wifi or not

boolean wifienabled = false;

// code

// calling this reboots the arduino
void (*resetFunc)(void) = 0;

void mqtt_callback(char *topic, unsigned char *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // remove this - it is not good. needed for testing though...
  if (!strncmp("reboot", (char *)payload, 6)) {
    Serial.println("Attempting restart");
    resetFunc();
  }
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  logger("MQTT","Attempting connection...");
  // Attempt to connect
  if (client.connect("arduinoClient")) {
    logger("MQTT","connected");
    // Once connected, publish an announcement...
    client.publish("vm211/status", "online");
    // ... and resubscribe
    client.subscribe("inTopic");
  } else {
    Serial.print("= MQTT: failed, rc=");
    Serial.print(client.state());
    logger("MQTT","try again in 5 seconds (that code is missing?)");
    // Wait 5 seconds before retrying
  }
}

// convenience function
void f2mqtt(const char *t, float v) {
  char s[50];

  if (wifienabled && client.connected()) {
    dtostrf(v, 3, 1, s);
    client.publish(t, s);
  }
}

void i2mqtt(const char *t, int v) {
  char s[50];

  if (wifienabled && client.connected()) {
    client.publish(t, itoa(v, s, 10));
  }
}

void setup_wifi() {

  logger("WIFI","Starting config");
  // Start by connecting to the ESP module

  Serial3.begin(115200);
  WiFi.init(Serial3);

  if (WiFi.status() == WL_NO_MODULE) {
    logger("WIFI","Communication with WiFi module failed!");
    logger("WIFI","If you have a WiFi module - are you sure that it is "
                   "powered correctly?");
    // don't continue
    wifienabled = false;
    return;
  }

  // waiting for connection to Wifi network set with the SetupWiFiConnection
  // sketch
  logger("WIFI","Waiting for connection to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print('.');
  }
  wifienabled = true;
  //#ifdef WEATHER_FORECAST
  //Serial.println("SMHI - enabled");
  //forecastscreen.setEnabled(true);
  //#endif

  Serial.println();

  IPAddress ip = WiFi.localIP();
  Serial.println();
  Serial.println("= WIFI: Connected to WiFi network.");
  Serial.print("= WIFI: Setup done. IP: ");
  Serial.println(ip);
}

void setup_mqtt() {

  if(!wifienabled) {
    Serial.println("= MQTT: No Network");
    return;
  }
  
  // once wifi is done, connect to mqtt

  client.setServer(MQTT_HOST, 1883);
  client.setCallback(mqtt_callback);

  // and connect to mqtt

  mqtt_reconnect();
}

void setup_ntp() {
  // finally, set up NTP

  if(!wifienabled) {
    Serial.println("= NTP: No Network");
    return;
  }
  
  Serial.println("= NTP: Attempting NTP Sync");
  timeClient.begin();
  timeClient.update();

  Serial.print("= NTP: time is ");
  Serial.println(timeClient.getFormattedTime());
}

time_t getNtpTime() {
  timeClient.update();
  return timeClient.getEpochTime();
}

// endif WITH_ESP01

#endif
