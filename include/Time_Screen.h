#pragma once
#include "Screen.h"
#include <Adafruit_ImageReader.h>
#include <Adafruit_ILI9341.h>
#include "RTClib.h"

class Time_Screen : public Screen {
    Adafruit_ILI9341 *tft;
    Adafruit_ImageReader *reader;
    RTC_DS3231 *rtc;

    char lastFormattedTime[9] = "00:00:00";
    char formattedTime[9] = "00:00:00";

    public:
    Time_Screen(Adafruit_ILI9341 *tft, Adafruit_ImageReader *reader, RTC_DS3231 *rtc) {
        this->tft = tft;
        this->reader = reader;
        this->rtc = rtc;
    };

    void init();
    void draw();
};