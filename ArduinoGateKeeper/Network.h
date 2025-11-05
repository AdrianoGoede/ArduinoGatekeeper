#ifndef __ARDUINO_GATEKEEPER_NETWORK__
#define __ARDUINO_GATEKEEPER_NETWORK__

#include <ArduinoMqttClient.h>
#include <Arduino_FreeRTOS.h>
#include <NTPClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <queue>
#include "config.h"

struct MqttMessage {
  String topic, payload;
};

class Network {
private:
  static char _strBuffer[STRING_BUFFER_SIZE];
  static SemaphoreHandle_t _semaphoreHandle;
  static std::queue<MqttMessage> _messages;
  static WiFiUDP _udpWifiClient;
  static NTPClient _ntpClient;
  static WiFiClient _wifiClient;
  static MqttClient _mqttClient;
  static bool connectWiFi();
  static bool synchronizeClock();
  static bool connectMqttBroker();
  static void queueMessage(int msgSize);
  static void publishMessage(const char* topic, const char* payload, bool retain = false);
  static void publishLastWill(const char* topic, const char* payload, bool retain = false);
public:
  static bool begin(SemaphoreHandle_t semaphore);
  static bool handleConnections();
  static time_t getCurrentTimestamp();
  static bool hasAvailableMessages();
  static MqttMessage getNextMessage();
  static void reportActivity(const String& payload);
};

#endif