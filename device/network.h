#ifndef __ARDUINO_GATEKEEPER_NETWORK__
#define __ARDUINO_GATEKEEPER_NETWORK__

#include <WiFi.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include <ArduinoMqttClient.h>
#include "config.h"

class Network {
  private:
    static WiFiClientSecure _wifiClient;
    static MqttClient _mqttClient;
    static String _scanTopic;
    static String _deviceStatusStopic;
    static char _strBuffer[STRING_BUFFER_SIZE];
    static bool initialize();
    static bool connectWiFi();
    static bool synchronizeClock();
    static bool connectToMqttBroker();
    static void publishMessage(const String& topic, const String& payload, bool retain = false);
    static void publishLastWill(const String& topic, const String& payload, bool retain = false);
    static String getJsonStatusMessage(bool online, bool addTimestamp);
  public:
    static bool begin();
    static bool handleConnections();
};

#endif