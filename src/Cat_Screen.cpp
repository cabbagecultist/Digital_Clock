#include "Cat_Screen.h"

void Cat_Screen::init() {
    reader->drawBMP("test.bmp", *tft, 0, 0, true);
}

void Cat_Screen::draw() {}