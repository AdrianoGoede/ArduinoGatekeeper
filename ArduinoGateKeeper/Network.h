#ifndef __ARDUINO_GATEKEEPER_NETWORK__
#define __ARDUINO_GATEKEEPER_NETWORK__

#include <ArduinoMqttClient.h>
#include <Arduino_FreeRTOS.h>
#include <NTPClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <queue>
#include "config.h"

enum MqttMessageType {
  AuthorizedUserAdd = 0,
  AuthorizedUserRemove = 1
};

struct MqttMessage {
  MqttMessageType type;
  String payload;
};

class Network {
private:
  static char strBuffer[STRING_BUFFER_SIZE];
  static SemaphoreHandle_t semaphoreHandle;
  static std::queue<MqttMessage> messages;
  static WiFiUDP udpWifiClient;
  static NTPClient ntpClient;
  static WiFiClient wifiClient;
  static MqttClient mqttClient;
  static bool connectWiFi();
  static bool synchronizeClock();
  static bool connectMqttBroker();
  static void queueMessage(int msgSize);
public:
  static bool begin(SemaphoreHandle_t semaphore);
  static bool handleConnections();
  static time_t getCurrentTimestamp();
  static bool hasAvailableMessages();
  static MqttMessage getNextMessage();
  static void reportActivity(const String& payload);
};

#endif