#include <stdio.h>
#include "network.h"

char Network::_strBuffer[STRING_BUFFER_SIZE];

bool Network::begin() {
  return (connectWiFi() && synchronizeClock());
}

bool Network::connectWiFi() {
  snprintf(_strBuffer, STRING_BUFFER_SIZE, "Attempting to connect to network %s...", WIFI_SSID);
  Serial.print(_strBuffer);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  snprintf(_strBuffer, STRING_BUFFER_SIZE, " Success! IP Address: %s", WiFi.localIP().toString().c_str());
  Serial.println(_strBuffer);
  return true;
}

bool Network::synchronizeClock() {
  Serial.print("Attempting to synchronize clock...");
  configTime(0, 0, NTP_SERVER_PRIMARY, NTP_SERVER_SECONDARY);

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    delay(1000);
    Serial.print(".");
  }

  time_t now;
  time(&now);
  snprintf(_strBuffer, STRING_BUFFER_SIZE, " Success! Current Unix timestamp: %ld", now);
  Serial.println(_strBuffer);
  return true;
}