#pragma once
#include "Adafruit_ILI9341.h"

class Clock_Screen {
    public:
        virtual void init();
        virtual void draw();
};