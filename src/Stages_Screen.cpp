#include "Stages_Screen.h"

void Stages_Screen::init() {
    tft->fillScreen(ILI9341_BLACK);
    yield();
    tft->fillRect(0, 0, 320, 20, ILI9341_WHITE);
    yield();
    tft->setTextColor(ILI9341_BLACK, ILI9341_WHITE);
    tft->setCursor(15, 0);
    tft->setTextSize(2);
    tft->print("Current Anarchy Rotation");
    yield();

    //Stupid hack
    sprintf(formattedTime, "Next at %.2d:%.2d:%.2d", endTimeCorrected.hour(), endTimeCorrected.minute(), endTimeCorrected.second());
    tft->setCursor(15, 50);
    tft->setTextSize(3);
    tft->print(formattedTime);
    yield();
    tft->setCursor(15, 74);
    tft->print(gameMode);
    yield();
}

void Stages_Screen::draw() {
    static char lastFormattedTime[] = "Next at 00:00:00";
    sprintf(formattedTime, "Next at %.2d:%.2d:%.2d", endTimeCorrected.hour(), endTimeCorrected.minute(), endTimeCorrected.second());
    if (strcmp(formattedTime, lastFormattedTime) != 0) {
        tft->setCursor(15, 50);
        tft->setTextSize(3);
        tft->print(formattedTime);
        yield();
        tft->setCursor(15, 74);
        tft->print(gameMode);
        yield();
        strcpy(lastFormattedTime, formattedTime);
    }
}

void Stages_Screen::update() {
    static uint64_t lastTime = 0;
    int year, month, day, hour, minute, second;

    if (WiFi.status() == WL_CONNECTED){
    // Make a GET request once, then repeat every 30m
        static bool firstRequest = false;
        if (!firstRequest) {
            String response = httpsGETRequest(serverAddress);
            deserializeJson(latestData, response);
            firstRequest = true;
            if (!latestData.isNull()) {
                char endDatetime[] = "0000-00-00T00:00:00Z";
                JsonArray data = latestData["data"]["bankaraSchedules"]["nodes"];
                strcpy(gameMode, data[0]["bankaraMatchSettings"][1]["vsRule"]["name"]);
                strcpy(endDatetime, data[0]["endTime"]);

                sscanf(endDatetime, "%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &minute, &second);
                DateTime endTime = DateTime(year, month, day, hour, minute, second);
                endTimeCorrected = DateTime(endTime.unixtime() - 14400);
            }
        }

        if ((millis() - lastTime) >= 18000000) {
        String response = httpsGETRequest(serverAddress);
        deserializeJson(latestData, response);
        if (!latestData.isNull()) {
            sscanf(latestData["data"]["bankaraSchedules"]["nodes"][0]["endTime"], "%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &minute, &second);
            DateTime endTime = DateTime(year, month, day, hour, minute, second);
            endTimeCorrected = DateTime(endTime.unixtime() - 14400);
        }
        lastTime = millis();
        }
  }
}

String Stages_Screen::httpsGETRequest(const char* serverAddress) {
  WiFiClientSecure client;
  HTTPClient https;
  // client.setInsecure();
  client.setCACert(root_ca);
  https.begin(client, serverAddress);
  int httpResponseCode = https.GET();
  String response = "{}";

  if (httpResponseCode > 0)
  {
    response = https.getString();
  }
  https.end();
  return response;
}