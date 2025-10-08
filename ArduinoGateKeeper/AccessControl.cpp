#include <cstdio>
#include <SPI.h>
#include <MFRC522.h>
#include "AccessControl.h"
#include "config.h"

MFRC522 AccessControl::rfidReader(RFID_CS_PIN, RFID_RST_PIN);
SemaphoreHandle_t AccessControl::semaphoreHandle;
std::map<String, MFRC522::MIFARE_Key> AccessControl::authorizedUsers;
CircularBuffer<String> AccessControl::logBuffer{ACTIVITY_LOG_BUFFER_SIZE};

bool AccessControl::begin(SemaphoreHandle_t semaphore) {
  semaphoreHandle = semaphore;
  SPI.begin();
  rfidReader.PCD_Init();
  return true;
}

AccessControlStatus AccessControl::handleAccessControl(time_t timestamp) {
  if (!rfidReader.PICC_IsNewCardPresent() || !rfidReader.PICC_ReadCardSerial())
    return AccessControlStatus::Waiting;

  String uid = getUidAsString(rfidReader.uid.uidByte, rfidReader.uid.size);

  AccessControlStatus status = (authenticateCard(uid) ? AccessControlStatus::Granted : AccessControlStatus::Denied);
  rfidReader.PICC_HaltA();
  rfidReader.PCD_StopCrypto1();

  generateLogEntry(uid, status, timestamp);

  return status;
}

bool AccessControl::authenticateCard(const String& uid) {
  auto userKey = authorizedUsers.find(uid);
  if (userKey == authorizedUsers.end()) return false;

  byte sector = 0, blockAddr = 0;
  return (rfidReader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 0, &userKey->second, &(rfidReader.uid)) == MFRC522::STATUS_OK);
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

  if (xSemaphoreTake(semaphoreHandle, pdMS_TO_TICKS(ACC_CONTROL_OPERATION_INTERVAL)) == pdTRUE) {
    logBuffer.push(entry);
    xSemaphoreGive(semaphoreHandle);
  }
}

void AccessControl::addAuthorizedUser(const String& jsonPayload) {
  JsonDocument document;
  if (deserializeJson(document, jsonPayload) != DeserializationError::Ok) return;

  JsonString uid = document["UID"];
  if (uid.isNull() || uid.size() == 0) return;
  JsonArray keyArray = document["key"];
  if (keyArray.isNull() || keyArray.size() != MFRC522::MIFARE_Misc::MF_KEY_SIZE) return;

  MFRC522::MIFARE_Key key;
  for (uint8_t i = 0; i < MFRC522::MIFARE_Misc::MF_KEY_SIZE; i++)
    key.keyByte[i] = keyArray[i];

  if (xSemaphoreTake(semaphoreHandle, pdMS_TO_TICKS(ACC_CONTROL_OPERATION_INTERVAL)) == pdTRUE) {
    authorizedUsers[uid.c_str()] = key;
    xSemaphoreGive(semaphoreHandle);
  }
}

void AccessControl::removeAuthorizedUser(const String& jsonPayload) {
  JsonDocument document;
  deserializeJson(document, jsonPayload);

  JsonString uid = document["UID"];
  if (uid.isNull() || uid.size() == 0) return;

  if (xSemaphoreTake(semaphoreHandle, pdMS_TO_TICKS(ACC_CONTROL_OPERATION_INTERVAL)) == pdTRUE) {
    authorizedUsers.erase(uid.c_str());
    xSemaphoreGive(semaphoreHandle);
  }
}

bool AccessControl::hasAvailableLogEntries() { return !logBuffer.isEmpty(); }

String AccessControl::getNextLogEntry() { return logBuffer.pop(); }