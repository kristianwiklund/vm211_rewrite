// figure out when postnord delivers mail...

#include "user.h"


#ifdef ZIPCODE

#include <Arduino.h>
#include <ArduinoJson.h>
#include "network.h"
#include "log.h"
#include <ArduinoHttpClient.h>
#include "display.h"
#include <TimeLib.h>
#include "user.h"

// end ifdef ZIPCODE

#define POSTNORD "portal.postnord.com"
HttpClient posthttp = HttpClient(wificlient, POSTNORD, 80);

void update_postnord() {

  // API: https://portal.postnord.com/api/sendoutarrival/closest?postalCode=ZIPCODE
  // returns:
  // {"postalCode":41452,"city":"Göteborg","delivery":"31 januari, 2023","upcoming":"2 februari, 2023"}

  StaticJsonDocument<128> doc;

  posthttp.get("/api/sendoutarrival/closest?postalCode="ZIPCODE);

  // read the status code and body of the response
  int statusCode = posthttp.responseStatusCode();
  String response = posthttp.responseBody();
  Serial.println(response);
  
  DeserializationError error = deserializeJson(doc, response );

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  

  long postalCode = doc["postalCode"]; // 41452
  const char* city = doc["city"]; // "Göteborg"
  const char* delivery = doc["delivery"]; // "31 januari, 2023"
  const char* upcoming = doc["upcoming"]; // "2 februari, 2023"
  
  
}

#endif

