#include <Arduino.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "RTClib.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_SPIFlash.h"
#include "Adafruit_ImageReader.h"
#include "SdFat.h"
#include "Screen.h"
#include "Cat_Screen.h"
#include "Time_Screen.h"
#include "Stages_Screen.h"


void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
RTC_DS3231 rtc;
SdFat SD;
Adafruit_ImageReader reader(SD);
// Adafruit_ILI9341 tft = Adafruit_ILI9341(7, 21, 6, 4, 20, 5);
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, 21, 7, 20);
// enum Mode {
//   TIME,
//   STAGES,
//   IMAGE
// };

Cat_Screen catScreen(&tft, &reader);
Time_Screen timeScreen(&tft, &reader, &rtc);
Stages_Screen stagesScreen(&tft, &reader);
// Screen modes[3] = {timeScreen, catScreen, stagesScreen};
std::vector<Screen*> modes = {&timeScreen, &stagesScreen, &catScreen};
Screen currentMode;
int modeIndex = 0;

// Mode currentMode = TIME;
Screen* screen = &timeScreen;

const char ssid[] = "test1234";
const char pass[] = "gaming1234";
const int buttonPin = 10;

int previousButtonState = HIGH;

void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println(WiFi.localIP());
  timeClient.begin();
  timeClient.setTimeOffset(-14400);
}

void WiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  timeClient.end();
  WiFi.disconnect(true, true);
}

void setup() {
  Wire.setPins(8, 9);
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, 320, 20, ILI9341_WHITE);

  WiFi.disconnect(false, true);
  WiFi.onEvent(WiFiConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  SD.begin(0, SD_SCK_MHZ(25));
  pinMode(buttonPin, INPUT_PULLUP);

  //Maybe add an led for errors in the future
  while(!rtc.begin()) {
    delay(100);
  }

  screen->init();
}

void loop() {
  //WiFi timeout triggered (30s)
  if(WiFi.status() != WL_CONNECTED && millis() >= 60000) {
    static bool WiFiTimeout = false;
    if (!WiFiTimeout) {
      Serial.println("WiFi timeout triggered!");
      WiFi.disconnect(true, true);
      WiFiTimeout = true;
    }
  }

  if(WiFi.status() == WL_CONNECTED && rtc.lostPower()) {
    static bool timeSet = false;
    if(!timeSet) {
      Serial.println("RTC lost power!");
      timeClient.update();
      uint64_t timestamp = timeClient.getEpochTime();
      rtc.adjust(DateTime(timestamp));
      timeSet = true;
    }
  }
  
  //Switch menu on button press
  int currentButtonState = digitalRead(buttonPin);
  if (currentButtonState == LOW && previousButtonState == HIGH) {
    if (modeIndex >= modes.size() - 1){
      modeIndex = 0;
    } else {
      modeIndex++;
    } 
    screen = modes[modeIndex];
    screen->init();
    // currentMode = modes[modeIndex];
    // screen = currentMode;
    // screen->init();
    // switch (currentMode) {
    // case TIME:
    //   screen = &stagesScreen;
    //   screen->init(); 
    //   currentMode = STAGES;
    //   break;
    
    // case STAGES:
    //   screen = &catScreen;
    //   screen->init();
    //   currentMode = IMAGE;
    //   break;

    // case IMAGE:
    //   screen = &timeScreen;
    //   screen->init();
    //   currentMode = TIME;
    //   break;
    // }
  }
  previousButtonState = currentButtonState;

  screen->draw();
  stagesScreen.update();
}
