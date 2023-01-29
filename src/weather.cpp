#include <ArduinoJson.h>

// connect to a weather url, deserialize, and return something...
// obvs, change lat lon + start and end dates to whatever today is

// for the weather close in time, we use this:
// https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m,weathercode&start_date=2023-01-29&end_date=2023-01-29


// for daily weather (which I'm leaning towards), we use this:
// https://api.open-meteo.com/v1/forecast?latitude=59.33&longitude=18.07&daily=weathercode,temperature_2m_max,temperature_2m_min,precipitation_sum&timezone=auto&start_date=2023-01-29&end_date=2023-02-02

// weather codes are found here:
// https://artefacts.ceda.ac.uk/badc_datadocs/surface/code.html


