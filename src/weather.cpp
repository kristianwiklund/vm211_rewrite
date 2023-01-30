#include <Arduino.h>
#include <ArduinoJson.h>
#include "network.h"
#include "log.h"
#include <ArduinoHttpClient.h>
#include "display.h"
#include "meteocons20pt7b.h"
#include <TimeLib.h>
#include "user.h"

// connect to a weather url, deserialize, and return something...
// obvs, change lat lon + start and end dates to whatever today is

// for the weather close in time, we use this:
// https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m,weathercode&start_date=2023-01-29&end_date=2023-01-29


// for daily weather (which I'm leaning towards), we use this:
// https://api.open-meteo.com/v1/forecast?latitude=59.33&longitude=18.07&daily=weathercode,temperature_2m_max,temperature_2m_min,precipitation_sum&timezone=auto&start_date=2023-01-29&end_date=2023-02-02

// weather codes are found here:
// https://artefacts.ceda.ac.uk/badc_datadocs/surface/code.html

const char *codes =
  "AH)3EEEEEFMMMMQQR0FYQQWXXRXXMPFFFFFFWWWWMMMMMMMMMMQQQQQQQQQQQQQQQQQQQQVVVVWWXXXXQRRRRVWXXXZZZZZZZZZZ";

// Stream& input;

#define server "api.open-meteo.com"
HttpClient http = HttpClient(wificlient, server, 80);


void draw_symbol(const char *day, int symbol, float min, float max, float prec, int offset) {

  int x = offset*64;

  tft.fillRect(x,240-64,x+63,240, TFT_BLACK);
  tft.drawRect(x,240-64,x+63,240, TFT_WHITE);

  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(x+3,240-58);
  tft.print(day+2);
  
  tft.setFont(&meteocons20pt7b);

  tft.setCursor(x+10,240-15);
  tft.print(codes[symbol]);

  tft.setFont(); // set default

  
}

void update_weather() {

  StaticJsonDocument<768> doc;
  char buf[256];
  time_t later=previousMidnight(now())+5*SECS_PER_DAY; // 4 days in the future
    
  snprintf(buf,256,"/v1/forecast?latitude=" LAT "&longitude=" LON "&daily=weathercode,temperature_2m_max,temperature_2m_min,precipitation_sum&timezone=auto&start_date=%d-%02d-%02d&end_date=%d-%02d-%02d",
	   year(),month(),day(),year(later),month(later),day(later));
  logger("Weather","Updating");
  logger("Weather",buf);
  Serial.println(now());
  Serial.println(later);
  http.get(buf);

  // read the status code and body of the response
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();

  //Serial.print("Status code: ");
  //Serial.println(statusCode);
  //Serial.print("Response: ");
  //Serial.println(response);

  DeserializationError error = deserializeJson(doc, response);
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonObject daily = doc["daily"];

  JsonArray daily_time = daily["time"];
  JsonArray daily_weathercode = daily["weathercode"];
  JsonArray daily_temperature_2m_max = daily["temperature_2m_max"];
  JsonArray daily_temperature_2m_min = daily["temperature_2m_min"];
  JsonArray daily_precipitation_sum = daily["precipitation_sum"];


  // now draw this on the screen

    
  for (int i=0;i<5;i++) {
    draw_symbol(daily_time[i], daily_weathercode[i], daily_temperature_2m_min[i], daily_temperature_2m_max[i], daily_precipitation_sum[i], i);
  }

  logger("Weather","Done");
  
}
