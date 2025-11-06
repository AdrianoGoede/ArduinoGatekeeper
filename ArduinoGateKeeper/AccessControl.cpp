#include <cstdio>
#include <SPI.h>
#include <MFRC522.h>
#include "AccessControl.h"
#include "config.h"

MFRC522 AccessControl::_rfidReader(RFID_CS_PIN, RFID_RST_PIN);
SemaphoreHandle_t AccessControl::_semaphoreHandle;
std::map<String, MFRC522::MIFARE_Key> AccessControl::_authorizedUsers;
CircularBuffer<String> AccessControl::_logBuffer{ACTIVITY_LOG_BUFFER_SIZE};

bool AccessControl::begin(SemaphoreHandle_t semaphore) {
  _semaphoreHandle = semaphore;
  SPI.begin();
  _rfidReader.PCD_Init();
  return true;
}

AccessControlStatus AccessControl::handleAccessControl(time_t timestamp) {
  if (!_rfidReader.PICC_IsNewCardPresent() || !_rfidReader.PICC_ReadCardSerial())
    return AccessControlStatus::Waiting;

  String uid = getUidAsString(_rfidReader.uid.uidByte, _rfidReader.uid.size);

  AccessControlStatus status = (authenticateCard(uid) ? AccessControlStatus::Granted : AccessControlStatus::Denied);
  _rfidReader.PICC_HaltA();
  _rfidReader.PCD_StopCrypto1();

  generateLogEntry(uid, status, timestamp);

  return status;
}

bool AccessControl::authenticateCard(const String& uid) {
  auto userKey = _authorizedUsers.find(uid);
  if (userKey == _authorizedUsers.end()) return false;

  byte sector = 0, blockAddr = 0;
  return (_rfidReader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 0, &userKey->second, &(_rfidReader.uid)) == MFRC522::STATUS_OK);
}

String AccessControl::getUidAsString(const byte* uid, size_t length) {
  String result;
  char hexNum[3];
  for (uint8_t i = 0; i < length; i++) {
    snprintf(hexNum, 3, "%02x", uid[i]);
    result += hexNum;
  }
  return result;
}

bool AccessControl::generateLogEntry(const String& uid, AccessControlStatus status, time_t timestamp) {
  String entry;
  JsonDocument document;

  document["UID"] = uid.c_str();
  document["Granted"] = (status == AccessControlStatus::Granted);
  document["Timestamp"] = timestamp;

  serializeJson(document, entry);

  if (xSemaphoreTake(_semaphoreHandle, pdMS_TO_TICKS(ACC_CONTROL_OPERATION_INTERVAL)) == pdTRUE) {
    _logBuffer.push(entry);
    xSemaphoreGive(_semaphoreHandle);
  }
}

void AccessControl::handleUserListMessage(const String& jsonPayload) {
  JsonDocument document;
  deserializeJson(document, jsonPayload);
  if (!document.is<JsonArray>()) return;

  if (xSemaphoreTake(_semaphoreHandle, pdMS_TO_TICKS(ACC_CONTROL_OPERATION_INTERVAL)) == pdTRUE) {
    _authorizedUsers.clear();
    for (const JsonVariant& user : document.as<JsonArray>()) {
      JsonString uid = user["UID"];
      if (uid.isNull() || uid.size() == 0) continue;
      JsonArray keyArray = user["key"];
      if (keyArray.isNull() || keyArray.size() != MFRC522::MIFARE_Misc::MF_KEY_SIZE) continue;

      MFRC522::MIFARE_Key key;
      for (uint8_t i = 0; i < MFRC522::MIFARE_Misc::MF_KEY_SIZE; i++)
        key.keyByte[i] = keyArray[i];

      _authorizedUsers[uid.c_str()] = key;
    }
    xSemaphoreGive(_semaphoreHandle);
  }
}

bool AccessControl::hasAvailableLogEntries() { return !_logBuffer.isEmpty(); }

String AccessControl::getNextLogEntry() { return _logBuffer.pop(); }