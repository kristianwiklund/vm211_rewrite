// draw a clock on the screen - requires that a real time clock chip is present. 
//
// (C) 2020 Kristian Wiklund
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//
#include <Arduino.h>
#include "config.h"
#include <math.h>
#include <TimeLib.h>
#include "clock.h"
#include "display.h"
#include "screen.h"
#include "log.h"
#include "network.h"

#include <Timezone.h>
TimeChangeRule myDST = CLOCK_DST_RULE;
TimeChangeRule mySTD = CLOCK_STD_RULE;
Timezone myTZ(myDST, mySTD);
TimeChangeRule *tcr;

#include <DS1307RTC.h>  

// graphics includes
//#include "src/TFTLCD-Library/Adafruit_TFTLCD.h"        	  // Hardware-specific library for TFT screen by Adafruit
#include <Fonts/FreeSans9pt7b.h>
#include <MCUFRIEND_kbv.h>


// --- begin code from timelib example ---

// this code print the current time to the serial port

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay(){
  // digital clock display of the time
  time_t utc = now();
  time_t local = myTZ.toLocal(utc, &tcr);
  Serial.print(hour(local));
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}


// --- end code from timelib example ---


// return TRUE if we are able to sync to at least one clock source
bool setup_clock() {

  time_t ntptime=0;
  
#ifdef WITH_ESP01
  if(!wifienabled)
    logger("Clock","No network, skipping NTP");
  else {
    logger("Clock","Getting NTP time"); 
    ntptime = getNtpTime();
    Serial.println(ntptime);
    Serial.println("Clock: NTP time response above this line");
    if (ntptime<1000) {
      ntptime=0;
      logger("Clock","NTP time response too low - setting to 0");
    }
  }
#endif

  // code partially from ds1307 lib example

  tmElements_t tm;


  // which source to use?
  // go for NTP, if available. Otherwise, use the RTC

  if(ntptime) {
    setSyncProvider(getNtpTime);
    if (timeStatus() != timeSet) { 
      logger("Clock","Unable to sync with NTP - trying RTC instead");
    }
    else {
      logger("Clock","NTP has set the system time");
      digitalClockDisplay();
      RTC.read(tm);
      if(RTC.chipPresent()) {
	logger("Clock","Updating RTC chip time to NTP time");
	RTC.set(ntptime);
	Serial.print("Clock: RTC seconds = ");
	Serial.print(RTC.get());
      } else {
	logger("Clock","RTC fail error");
      }
      return true;
    }

  }

  // if ntp failed, go for the ds1307

  logger("Clock","No NTP available, using DS1307 for time base");
  RTC.read(tm);
  if(!ntptime) {
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet) {
      logger("Clock","Unable to sync with the RTC - unable to keep real time!");

      if (RTC.read(tm)) {
	logger("Clock","RTC read okay");
	digitalClockDisplay();
      } else {
	if (RTC.chipPresent()) {
	  logger("Clock","The DS1307 is stopped.  Please run the TimeLib SetTime");
	  logger("Clock"," example to initialize the time and begin running.");
	  Serial.println();
	} else {
	  logger("Clock","DS1307 read error!  Please check the circuitry.");
	  Serial.println();
	}
      }
    }
    else {
      logger("Clock","RTC has set the system time");
      return true;
    }
  }

  return false;
}
  


ClockScreen::ClockScreen() {
  setEnabled(true);
  
}

bool ClockScreen::draw() {
  time_t utc = now();
  time_t t = myTZ.toLocal(utc, &tcr);
  static int oldsec=0, oldmin=0, oldhour=0;
  if(!isEnabled())
    return false;
  
  // this clears the screen in the caller
  // tft.fillRect(10,60,310,180,TFT_BLACK); 
  // which means that 10,60,10+310,60+180 is the area where we can play

  // draw a large circle with a small circle inside
  tft.drawCircle(CLOCK_X,CLOCK_Y,CLOCK_R, TFT_WHITE);
  tft.fillCircle(CLOCK_X, CLOCK_Y, CLOCK_CR, TFT_WHITE);

  // draw hour marks  
  // replace the trig calls with precalculated values for more efficiency, if needed

  for(int i=0;i<12;i++) {
    int x1,y1,x2,y2;
    
    x1 = sin(i*2*PI/12)*CLOCK_R+CLOCK_X;
    x2 = sin(i*2*PI/12)*(CLOCK_R-CLOCK_H_LEN)+CLOCK_X;
    y1 = cos(i*2*PI/12)*CLOCK_R+CLOCK_Y;
    y2 = cos(i*2*PI/12)*(CLOCK_R-CLOCK_H_LEN)+CLOCK_Y;

    tft.drawLine(x1,y1,x2,y2,TFT_WHITE);
  }
  

// cleanup by drawing black on top of the old lines

  if(oldsec!=second(t)) {
    tft.drawLine(CLOCK_X, CLOCK_Y, CLOCK_R*sin(oldsec*2*PI/60)+CLOCK_X,-CLOCK_R*cos(oldsec*2*PI/60)+CLOCK_Y, TFT_BLACK);
  }

if(oldmin!=minute(t)) {
    tft.fillTriangle(CLOCK_R*sin(oldmin*2*PI/60)+CLOCK_X,-CLOCK_R*cos(oldmin*2*PI/60)+CLOCK_Y,
       -CLOCK_H_LEN*cos(oldmin*2*PI/60)+CLOCK_X,-CLOCK_H_LEN*sin(oldmin*2*PI/60)+CLOCK_Y,  
       CLOCK_H_LEN*cos(oldmin*2*PI/60)+CLOCK_X,CLOCK_H_LEN*sin(oldmin*2*PI/60)+CLOCK_Y,
      TFT_BLACK);
  }


 if(oldhour!=hour(t) || oldmin!=minute(t)) {
    tft.fillTriangle(CLOCK_H_HAND*CLOCK_R*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,-CLOCK_H_HAND*CLOCK_R*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,
       -CLOCK_H_LEN*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,-CLOCK_H_LEN*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,  
       CLOCK_H_LEN*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,CLOCK_H_LEN*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,
      TFT_BLACK);
  }

// redraw by drawing white lines
  
 
  oldmin=minute(t);
  oldsec=second(t);
  oldhour=hour(t);

  tft.fillTriangle(CLOCK_H_HAND*CLOCK_R*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,-CLOCK_H_HAND*CLOCK_R*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,
  -CLOCK_H_LEN*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,-CLOCK_H_LEN*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,  
  CLOCK_H_LEN*cos((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_X,CLOCK_H_LEN*sin((oldhour+oldmin/60.0)*2*PI/12)+CLOCK_Y,
  TFT_LIGHTGREY);

  tft.fillTriangle(CLOCK_R*sin(oldmin*2*PI/60)+CLOCK_X,-CLOCK_R*cos(oldmin*2*PI/60)+CLOCK_Y,
  -CLOCK_H_LEN*cos(oldmin*2*PI/60)+CLOCK_X,-CLOCK_H_LEN*sin(oldmin*2*PI/60)+CLOCK_Y,  
  CLOCK_H_LEN*cos(oldmin*2*PI/60)+CLOCK_X,CLOCK_H_LEN*sin(oldmin*2*PI/60)+CLOCK_Y,
   TFT_LIGHTGREY);
   
 tft.drawLine(CLOCK_X, CLOCK_Y, CLOCK_R*sin(oldsec*2*PI/60)+CLOCK_X,-CLOCK_R*cos(oldsec*2*PI/60)+CLOCK_Y, TFT_WHITE);
  
 return true;
}
