
# Adding networking to the VM211


## Preparing the ESP-01 device


Unless already flashed with the right firmware, you need to flash your ESP-01 to use espressif firmware 1.7.3, which is found here: https://www.espressif.com/en/support/download/at

Instructions for flashing is available in the WiFiEspAT README. The reason is that the WiFiEspAT library need 1.7.x to provide a stable networking experience. 

The Wifi credentials are not set in the VM211 sketch, use the tools/setuppersistentwificonnection example from wifiespat to do that.

## Connecting the ESP-01 to the arduino


* Connect the ESP01 to the Arduino through a level converter. 
* You also need a 3.3V regulator connected to the input power rail on the arduino, the 3.3V arduino power is not good enough. 5V from USB is not sufficient, you need a power supply to power the device. 
* Connect the ESP01 to serial3 (which is the only easily accessible port in the VM211 design). 

## Clock

Using the clock feature requires either a DS1307 RTC (set to the correct time, and mine drifts...), a network card for NTP access (the above mentioned ESP-01) or both. The networking solution is recommended, if network is present, it will use any DS1307 that are present for backup timekeeping.

![ESP Connection](/pics/esp.png)

# What is the hardware configuration of a VM211?

The information below is aggregated from the [Velleman manual](https://manuals.whadda.com/category.php?id=378)

## Main board

This is an Arduino compatible ATmega2560 ("Arduino mega") board.

## Touch screen

As far as I can figure out, this is a [VMA412](https://www.velleman.eu/products/view/?id=435582) which is functionally equivalent 
to an [Adafruit 2.8" shield](https://learn.adafruit.com/adafruit-2-8-tft-touch-shield-v2). The touch screen shield also carry the sd reader.

## Lightning sensor

The included VMA343 lightning sensor is based on an AMS3935 and is connected via I2C on address 0x03 as well as interrupt on pin 18.
The [capacitance setting](https://forum.whadda.com/t/vma343-lightning-sensor-missing-product-info-and-datasheet/31395) is 72 pF.

## Air quality sensor

The included VMA342 contains two devices, both connected through I2C, one CCS811 (0x5B) and one BME280 (0x77)

## Real time clock

* If a ds1307 has been added, it is accessed through I2C, address 0x68. 
* It seems like Velleman ("Whadda" nowadays) are selling ds1302:s to add to the device, I do not have any of those and my fork does not support them. It shouldn't be too complicated, have a look at [Bert Lindeman's pull request to the main repo](https://github.com/Velleman/VM211_Firmware/pull/7/commits/f1e41ab82534fed6aa6ba8c7d96e238750d0af6c) for details.

## Buzzer

Piezo buzzer connected to A10.






