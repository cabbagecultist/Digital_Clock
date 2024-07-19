#include "Clock_Screen.h"
#include <Adafruit_ImageReader.h>

class Cat_Screen : public Clock_Screen {
    Adafruit_ILI9341 *tft;
    Adafruit_ImageReader *reader;

    public:
        Cat_Screen(Adafruit_ILI9341 *tft, Adafruit_ImageReader *reader) {
            this->tft = tft;
            this->reader = reader;
        }

    void init() override {
        reader->drawBMP("test.bmp", *tft, 0, 0, true);
    }
};