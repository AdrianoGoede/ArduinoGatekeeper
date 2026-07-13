#ifndef __ARDUINO_GATEKEEPER_RFID_CTRL__
#define __ARDUINO_GATEKEEPER_RFID_CTRL__

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <MFRC522.h>
#include <map>
#include "config.h"

enum RfidScanResult {
  Waiting = 0,
  Denied = ACCESS_DENIED_LED_PIN,
  Granted = ACCESS_GRANTED_LED_PIN
};

struct LogEntry {
  String uid;
  RfidScanResult result;
  time_t timestamp;
};

class RfidController {
  private:
    static MFRC522 _rfidReader;
    static SemaphoreHandle_t _authUsersMutex;
    static std::map<String, MFRC522::MIFARE_Key> _authorizedUsers;
    static QueueHandle_t _logs;
    static bool authenticateCard(const String& uid);
    static String getUidAsString(const byte* uid, size_t length);
    static void generateLogEntry(const String& uid, RfidScanResult result, time_t timestamp);
  public:
    static bool begin();
    static RfidScanResult handleScan();
    static String getNextLogEntry();
};

#endif