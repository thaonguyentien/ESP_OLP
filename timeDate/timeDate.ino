// Date and time functions using just software, based on millis() & timer

#include <Arduino.h>
#include <Wire.h>         // this #include still required because the RTClib depends on it
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_Millis rtc;

void setup () {
    Serial.begin(115200);
    rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
}

void loop () {
    DateTime now = rtc.now();
    
    Serial.print(" seconds since 1970: ");
    Serial.println(now.unixtime());
    
    Serial.println();
    delay(3000);
}
