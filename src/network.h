#ifndef _WIFI_H
#define _WIFI_H

void setup_wifi();
void setup_mqtt();
void setup_ntp();
void mqtt_reconnect();

#include <PubSubClient.h>
#include <WiFiEspAT.h>

extern PubSubClient client;
extern boolean wifienabled;
extern WiFiClient wificlient;
#endif
