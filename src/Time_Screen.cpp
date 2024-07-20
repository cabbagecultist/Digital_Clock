#include "Time_Screen.h"

void Time_Screen::init() {
    tft->fillScreen(ILI9341_BLACK);
    yield();
    tft->fillRect(0, 0, 320, 20, ILI9341_WHITE);
    yield();
    tft->setCursor(0, 0);
    tft->setTextColor(ILI9341_BLACK, ILI9341_WHITE);
    tft->setCursor(130, 0);
    tft->setTextSize(2);
    tft->print("Time");
    yield();
    draw();
}

void Time_Screen::draw() {
    tft->setTextColor(ILI9341_BLACK, ILI9341_WHITE);
    tft->setCursor(17, 90);
    tft->setTextSize(6);

    DateTime now = rtc->now();
    sprintf(formattedTime, "%.2d:%.2d:%.2d", now.hour(), now.minute(), now.second());
    if (strcmp(formattedTime, lastFormattedTime) != 0) {
      tft->println(formattedTime);
      strcpy(lastFormattedTime, formattedTime);
    }

}