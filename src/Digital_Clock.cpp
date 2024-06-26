#include <Arduino.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "RTClib.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>

void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info);
String httpGETRequest(const char* serverAddress);
JsonDocument deserialize(String json);

const String ssid = "test1234";
const String pass = "gaming1234";

int buttonPin = 21;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
RTC_DS3231 rtc;
LiquidCrystal_I2C screen(0x27, 16, 2);

const String serverAddress = "https://splatoon3.ink/data/schedules.json";
JsonDocument latestData;

enum Mode {
  TIME,
  STAGES
};

Mode currentMode = TIME;

const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFYjCCBEqgAwIBAgIQd70NbNs2+RrqIQ/E8FjTDTANBgkqhkiG9w0BAQsFADBX\n" \
"MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE\n" \
"CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIwMDYx\n" \
"OTAwMDA0MloXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT\n" \
"GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFIx\n" \
"MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAthECix7joXebO9y/lD63\n" \
"ladAPKH9gvl9MgaCcfb2jH/76Nu8ai6Xl6OMS/kr9rH5zoQdsfnFl97vufKj6bwS\n" \
"iV6nqlKr+CMny6SxnGPb15l+8Ape62im9MZaRw1NEDPjTrETo8gYbEvs/AmQ351k\n" \
"KSUjB6G00j0uYODP0gmHu81I8E3CwnqIiru6z1kZ1q+PsAewnjHxgsHA3y6mbWwZ\n" \
"DrXYfiYaRQM9sHmklCitD38m5agI/pboPGiUU+6DOogrFZYJsuB6jC511pzrp1Zk\n" \
"j5ZPaK49l8KEj8C8QMALXL32h7M1bKwYUH+E4EzNktMg6TO8UpmvMrUpsyUqtEj5\n" \
"cuHKZPfmghCN6J3Cioj6OGaK/GP5Afl4/Xtcd/p2h/rs37EOeZVXtL0m79YB0esW\n" \
"CruOC7XFxYpVq9Os6pFLKcwZpDIlTirxZUTQAs6qzkm06p98g7BAe+dDq6dso499\n" \
"iYH6TKX/1Y7DzkvgtdizjkXPdsDtQCv9Uw+wp9U7DbGKogPeMa3Md+pvez7W35Ei\n" \
"Eua++tgy/BBjFFFy3l3WFpO9KWgz7zpm7AeKJt8T11dleCfeXkkUAKIAf5qoIbap\n" \
"sZWwpbkNFhHax2xIPEDgfg1azVY80ZcFuctL7TlLnMQ/0lUTbiSw1nH69MG6zO0b\n" \
"9f6BQdgAmD06yK56mDcYBZUCAwEAAaOCATgwggE0MA4GA1UdDwEB/wQEAwIBhjAP\n" \
"BgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTkrysmcRorSCeFL1JmLO/wiRNxPjAf\n" \
"BgNVHSMEGDAWgBRge2YaRQ2XyolQL30EzTSo//z9SzBgBggrBgEFBQcBAQRUMFIw\n" \
"JQYIKwYBBQUHMAGGGWh0dHA6Ly9vY3NwLnBraS5nb29nL2dzcjEwKQYIKwYBBQUH\n" \
"MAKGHWh0dHA6Ly9wa2kuZ29vZy9nc3IxL2dzcjEuY3J0MDIGA1UdHwQrMCkwJ6Al\n" \
"oCOGIWh0dHA6Ly9jcmwucGtpLmdvb2cvZ3NyMS9nc3IxLmNybDA7BgNVHSAENDAy\n" \
"MAgGBmeBDAECATAIBgZngQwBAgIwDQYLKwYBBAHWeQIFAwIwDQYLKwYBBAHWeQIF\n" \
"AwMwDQYJKoZIhvcNAQELBQADggEBADSkHrEoo9C0dhemMXoh6dFSPsjbdBZBiLg9\n" \
"NR3t5P+T4Vxfq7vqfM/b5A3Ri1fyJm9bvhdGaJQ3b2t6yMAYN/olUazsaL+yyEn9\n" \
"WprKASOshIArAoyZl+tJaox118fessmXn1hIVw41oeQa1v1vg4Fv74zPl6/AhSrw\n" \
"9U5pCZEt4Wi4wStz6dTZ/CLANx8LZh1J7QJVj2fhMtfTJr9w4z30Z209fOU0iOMy\n" \
"+qduBmpvvYuR7hZL6Dupszfnw0Skfths18dG9ZKb59UhvmaSGZRVbNQpsg3BZlvi\n" \
"d0lIKO2d1xozclOzgjXPYovJJIultzkMu34qQb9Sz/yilrbCgj8=\n" \
"-----END CERTIFICATE-----\n";

bool connected = false;
bool WiFiTimeout = false;
uint64_t lastTime = 0;

void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println(WiFi.localIP());
  timeClient.begin();
  timeClient.setTimeOffset(-14400);
  connected = true;
}

JsonDocument deserialize(String json) {
  JsonDocument doc;
  deserializeJson(doc, json);
  return doc;
}

String httpsGETRequest(const String serverAddress) {
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

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  Wire.setPins(8, 9);
  Serial.begin(115200);
  while(!Serial);
  WiFi.disconnect(false, true);
  WiFi.onEvent(WiFiConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  screen.init();
  screen.backlight();
  screen.clear();

  //Maybe add an led for errors in the future
  while(!rtc.begin()) {
    Serial.println("RTC Module not found!");
    Serial.flush();
    delay(100);
  }
}

void loop() {
  DateTime now = rtc.now();

  //WiFi timeout triggered (30s)
  if(!connected && millis() >= 60000 && !WiFiTimeout) {
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

  if (connected) {
    // Make a GET request once, then repeat every 30m
    static bool firstRequest = false;
    if (!firstRequest) {
      String response = httpsGETRequest(serverAddress);
      latestData = deserialize(response);
      firstRequest = true;
    }

    if ((millis() - lastTime) > 18000000) {
      String response = httpsGETRequest(serverAddress);
      latestData = deserialize(response);
      lastTime = millis();
    }
  }

  if (!latestData.isNull()) {
    JsonArray data = latestData["data"]["bankaraSchedules"]["nodes"];
    String currentMode = data[0]["bankaraMatchSettings"][1]["vsRule"]["name"];
    Serial.println(currentMode);
  }
  
  //Switch menu on button press
  if (digitalRead(buttonPin) == LOW) {
    switch (currentMode) {
    case TIME:
      currentMode = STAGES;
      break;
    
    case STAGES:
      currentMode = TIME;
      break;
    }
  }

  switch (currentMode) {
  case TIME: {
    //Print time
    // screen.clear();
    screen.setCursor(0, 0);

    //There HAS to be a better way to do this
    String time = "";
    time.concat(now.hour());
    time.concat(":");
    time.concat(now.minute());
    time.concat(":");
    time.concat(now.second());

    screen.print(time);
    break;
  }
  case STAGES:
    //Print current stages
    // screen.clear();
    screen.setCursor(0, 0);
    screen.print("stages");
    break;
  }
  
  Serial.println(digitalRead(buttonPin));
  // screen.clear();
  // screen.setCursor(0, 0);
  // screen.print("test");

  // DateTime now = rtc.now();
  // Serial.println(now.year());
  // Serial.println(now.month());
  // Serial.println(now.day());
  // Serial.print(now.hour());
  // Serial.print(':');
  // Serial.print(now.minute());
  // Serial.print(':');
  // Serial.print(now.second());
  // Serial.println();
  // delay(1000);
}
