#pragma once
#include "Screen.h"
#include <Adafruit_ImageReader.h>
#include <Adafruit_ILI9341.h>

class Cat_Screen : public Screen {
    Adafruit_ILI9341 *tft;
    Adafruit_ImageReader *reader;

    public:
    Cat_Screen(Adafruit_ILI9341 *tft, Adafruit_ImageReader *reader) {
        this->tft = tft;
        this->reader = reader;
    };

    void init();
    void draw();
};