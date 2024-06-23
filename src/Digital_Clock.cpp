#include <Arduino.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "RTClib.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info);

String httpGETRequest(const char* serverAddress);

const String ssid = "test1234";
const String pass = "gaming1234";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
RTC_DS3231 rtc;

const String serverAddress = "https://splatoon3.ink/data/schedules.json";

// const char* root_ca = \ 
// "-----BEGIN CERTIFICATE-----\n" \
// "MIIFcDCCBFigAwIBAgIRAPsU9FnGLEVVDXsSKUzwsuYwDQYJKoZIhvcNAQELBQAw\n" \
// "RjELMAkGA1UEBhMCVVMxIjAgBgNVBAoTGUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBM\n" \
// "TEMxEzARBgNVBAMTCkdUUyBDQSAxUDUwHhcNMjQwNTI5MTIxOTIwWhcNMjQwODI3\n" \
// "MTIxOTE5WjAYMRYwFAYDVQQDEw1zcGxhdG9vbjMuaW5rMIIBIjANBgkqhkiG9w0B\n" \
// "AQEFAAOCAQ8AMIIBCgKCAQEA6KHnTRdQFx6RdgKxjgF8lHLluGm4gscGqr325wFy\n" \
// "zXHfVYOxnbmvq0b3LFoJ0sCCbhXwg84gSwctkaYsmsGcXpdqfIOMjGdHMrzmh+fo\n" \
// "o0o419ImNe44hoDdv4ORHQ0rkSpa7+KMu41StKWoxR+t9gyeLAGFWpAaWTYl47dD\n" \
// "BmXc8Q+/5maEsRgLxLGT+LU6nL1tReyhH/9/KKymXKEOg081BhnVHl4qFa+3XYLn\n" \
// "wwtTru3lrQhuUe3SrzlNoexMQQ2x62OYZprG0fH4FRlBvs7mrsoTF1sLEmqwejkU\n" \
// "yJRQTOdEb0k+40XjMEdP9WGhxcCxJe9EEOLdkYTs8iyODwIDAQABo4IChTCCAoEw\n" \
// "DgYDVR0PAQH/BAQDAgWgMBMGA1UdJQQMMAoGCCsGAQUFBwMBMAwGA1UdEwEB/wQC\n" \
// "MAAwHQYDVR0OBBYEFEiWme0RrWYnS/uuZvhKQZKXsKkmMB8GA1UdIwQYMBaAFNX8\n" \
// "ng3fHsrdCJeXbivFX8Ur9ey4MHgGCCsGAQUFBwEBBGwwajA1BggrBgEFBQcwAYYp\n" \
// "aHR0cDovL29jc3AucGtpLmdvb2cvcy9ndHMxcDUvMExLOEpXZHp1YTQwMQYIKwYB\n" \
// "BQUHMAKGJWh0dHA6Ly9wa2kuZ29vZy9yZXBvL2NlcnRzL2d0czFwNS5kZXIwKQYD\n" \
// "VR0RBCIwIIINc3BsYXRvb24zLmlua4IPKi5zcGxhdG9vbjMuaW5rMCEGA1UdIAQa\n" \
// "MBgwCAYGZ4EMAQIBMAwGCisGAQQB1nkCBQMwPAYDVR0fBDUwMzAxoC+gLYYraHR0\n" \
// "cDovL2NybHMucGtpLmdvb2cvZ3RzMXA1LzU4S293N2w1QVBrLmNybDCCAQQGCisG\n" \
// "AQQB1nkCBAIEgfUEgfIA8AB1AO7N0GTV2xrOxVy3nbTNE6Iyh0Z8vOzew1FIWUZx\n" \
// "H7WbAAABj8SBVdkAAAQDAEYwRAIgbmIeyfTyLEHhPgFJb6ZDafkoUzmqL1t01MXW\n" \
// "27dAT58CIGzDsP0jwWknpkeeuLDxPS4PAIb7YB7VC+5KB0PnBeG0AHcASLDja9qm\n" \
// "RzQP5WoC+p0w6xxSActW3SyB2bu/qznYhHMAAAGPxIFV/QAABAMASDBGAiEAvux1\n" \
// "a3kFogFpooXoGwK0TZbfHEYNZ2r1xGkbsEtx7U0CIQDx6fIg+OhO+NVwwd2LrddD\n" \
// "hIj3z1TdxPQTFnrFXTZjgTANBgkqhkiG9w0BAQsFAAOCAQEARXWZzfdRz4CLCpxc\n" \
// "9ymVNKXUW+puaKMzP+Vi+lAw3Sa+o4nnSq4VL3qMQOA5mPR+hFCLvrAf5YLBjxoE\n" \
// "Z5vT1/WXhswlzxu8hkD9jT1hUtDwdWv1KhRPm3YZRZ9TUKLO1xG2D3vEAqbtIWmJ\n" \
// "/ZjXKQpoqbttuEJN8+2ERXFci2we9nOqGQImctVgoa3oNTyg1asWGQ7Y+JWt6A/P\n" \
// "KAUPVb+pl3fK9jpjjd55FqfwAZOOVWlkDeLDB6Gpg3ZOZ1A82cHb2uTQoR7ha4oy\n" \
// "wMniwTQIGuBOZ+DDvgQ/tWaIy1f7482cqUun4lVgmiZS2172nNvnciKi3pE3FAfz\n" \
// "f3Ew7Q==\n" \
// "-----END CERTIFICATE-----\n";

bool connected = false;
bool WiFiTimeout = false;
uint64_t lastTime = 0;

void WiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println(WiFi.localIP());
  timeClient.begin();
  timeClient.setTimeOffset(-14400);
  connected = true;
}

String httpGETRequest(const String serverAddress) {
  WiFiClientSecure client;
  HTTPClient https;
  client.setInsecure();
  // client.setCACert(root_ca);
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

  //Code requiring an internet connection
  if (connected) {
    // Make a GET request every hour
    if ((millis() - lastTime) > 10000) {
      String response = httpGETRequest(serverAddress);
      Serial.print(response);
      lastTime = millis();
    }
  }
  
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
  delay(1000);
}
