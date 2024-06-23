#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "RTClib.h"

const String ssid = "test1234";
const String pass = "gaming1234";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
RTC_DS3231 rtc;

bool connected = false;
bool WiFiTimeout = false;

void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println(WiFi.localIP());
  timeClient.begin();
  timeClient.setTimeOffset(-14400);
  connected = true;
}

void setup() {
  Wire.setPins(8, 9);
  Serial.begin(115200);
  while(!Serial);
  WiFi.disconnect(false, true);
  WiFi.onEvent(WiFiConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  //Maybe add an led for errors in the future
  while(!rtc.begin()) {
    Serial.println("RTC Module not found!");
    Serial.flush();
    delay(100);
  }
}

void loop() {
  //WiFi timeout triggered (30s)
  if(!connected && millis() >= 30000 && !WiFiTimeout) {
    Serial.println("WiFi timeout triggered!");
    WiFi.disconnect(true, true);
    WiFiTimeout = true;
  }

  if(connected && rtc.lostPower()) {
    static bool timeSet = false;
    if(!timeSet) {
      Serial.println("RTC lost power!");
      timeClient.update();
      uint64_t timestamp = timeClient.getEpochTime();
      rtc.adjust(DateTime(timestamp));
      timeSet = true;
    }
  }

  DateTime now = rtc.now();
  Serial.println(now.year());
  Serial.println(now.month());
  Serial.println(now.day());
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.print(now.second());
  Serial.println();
  delay(1000);
}
