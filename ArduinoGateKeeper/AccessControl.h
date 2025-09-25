#ifndef __ARDUINO_GATEKEEPER_ACC_CTRL__
#define __ARDUINO_GATEKEEPER_ACC_CTRL__

#include <map>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include <Arduino_FreeRTOS.h>
#include "CircularBuffer.h"
#include "config.h"

enum AccessControlStatus {
  Waiting = 0,
  Denied = ACCESS_DENIED_LED_PIN,
  Granted = ACCESS_GRANTED_LED_PIN
};

struct AccessControlLogEntry {
  String Uid;
  AccessControlStatus status;
  time_t timestamp;
};

class AccessControl {
private:
  static MFRC522 rfidReader;
  static SemaphoreHandle_t semaphoreHandle;
  static std::map<String, MFRC522::MIFARE_Key> authorizedUsers;
  static CircularBuffer<String> logBuffer;
  static bool authenticateCard(const String& uid);
  static String getUidAsString(const byte* uid, size_t length);
  static bool generateLogEntry(const String& uid, AccessControlStatus status, time_t timestamp);
public:
  static bool begin(SemaphoreHandle_t semaphore);
  static AccessControlStatus handleAccessControl(time_t timestamp);
  static void addAuthorizedUser(const String& jsonPayload);
  static void removeAuthorizedUser(const String& jsonPayload);
  static bool hasAvailableLogEntries();
  static String getNextLogEntry();
};

#endif