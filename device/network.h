#ifndef __ARDUINO_GATEKEEPER_NETWORK__
#define __ARDUINO_GATEKEEPER_NETWORK__

#include <WiFi.h>
#include <time.h>
#include <functional>
#include <WiFiClientSecure.h>
#include <ArduinoMqttClient.h>
#include "config.h"

using MqttMessageCallback = std::function<void(const String& payload)>;

class Network {
  private:
    static WiFiClientSecure _wifiClient;
    static MqttClient _mqttClient;
    static String _scanTopic;
    static String _deviceStatusStopic;
    static String _addUserTopic;
    static String _removeUserTopic;
    static MqttMessageCallback _addUserCallback, _removeUserCallback;
    static char _strBuffer[STRING_BUFFER_SIZE];
    static bool initialize(MqttMessageCallback addUserCallback, MqttMessageCallback removeUserCallback);
    static bool connectWiFi();
    static bool synchronizeClock();
    static bool connectToMqttBroker();
    static void publishMessage(const String& topic, const String& payload, bool retain = false);
    static void publishLastWill(const String& topic, const String& payload, bool retain = false);
    static String getJsonStatusMessage(bool online, bool addTimestamp);
    static void handleIncomingMessage(int messageSize);
  public:
    static bool begin(MqttMessageCallback addUserCallback, MqttMessageCallback removeUserCallback);
    static bool handleConnections();
    static void sendLogMessage(const String& payload);
};

#endif