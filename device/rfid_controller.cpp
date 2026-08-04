#include "MFRC522.h"
#include "rfid_controller.h"
#include <ArduinoJson.h>

MFRC522 RfidController::_rfidReader(RFID_CS_PIN, RFID_RST_PIN);
SemaphoreHandle_t RfidController::_authUsersMutex;
std::map<String, MFRC522::MIFARE_Key> RfidController::_authorizedUsers;
QueueHandle_t RfidController::_logs;

bool RfidController::begin() {
  _authUsersMutex = xSemaphoreCreateMutex();
  _logs = xQueueCreate(RFID_LOG_QUEUE_MAX_SIZE, sizeof(LogEntry));
  SPI.begin();
  _rfidReader.PCD_Init();
  return true;
}

RfidScanResult RfidController::handleScan() {
  if (!_rfidReader.PICC_IsNewCardPresent() || !_rfidReader.PICC_ReadCardSerial())
    return RfidScanResult::Waiting;

  time_t timestamp;
  time(&timestamp);
  String uid = getUidAsString(_rfidReader.uid.uidByte, _rfidReader.uid.size);

  RfidScanResult result = (authenticateCard(uid) ? RfidScanResult::Granted : RfidScanResult::Denied);
  generateLogEntry(uid, result, timestamp);

  return result;
}

String RfidController::getNextLogEntry() { 
  LogEntry entry;
  if (!xQueueReceive(_logs, &entry, 0))
    return "";

  JsonDocument doc;
  doc["CardId"] = entry.uid;
  doc["DeviceId"] = MQTT_DEVICE_ID;
  doc["Granted"] = (entry.result == RfidScanResult::Granted);
  doc["Timestamp"] = entry.timestamp;

  char buffer[STRING_BUFFER_SIZE];
  serializeJson(doc, buffer, STRING_BUFFER_SIZE);

  return String(buffer);
}

bool RfidController::authenticateCard(const String& uid) {
  bool result = false;
  xSemaphoreTake(_authUsersMutex, portMAX_DELAY);
  auto userKey = _authorizedUsers.find(uid);
  
  if (userKey != _authorizedUsers.end()) {
    result = (_rfidReader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 0, &userKey->second, &(_rfidReader.uid)) == MFRC522::STATUS_OK);
    _rfidReader.PICC_HaltA();
    _rfidReader.PCD_StopCrypto1();
  }

  xSemaphoreGive(_authUsersMutex);
  return result;
}

String RfidController::getUidAsString(const byte* uid, size_t length) {
  String result;
  char hexNum[3];
  for (uint8_t i = 0; i < length; i++) {
    snprintf(hexNum, 3, "%02x", uid[i]);
    result += hexNum;
  }
  return result;
}

void RfidController::generateLogEntry(const String& uid, RfidScanResult result, time_t timestamp) {
  LogEntry entry{ uid, result, timestamp };
  xQueueSend(_logs, &entry, portMAX_DELAY);
}

void RfidController::AddUser(const String& payload) {
  JsonDocument document;
  deserializeJson(document, payload);
  if (!document.is<JsonObject>()) return;
  
  String cardId = document["Uid"];
  if (cardId.isEmpty()) return;
  JsonArray keyArray = document["Key"];
  if (keyArray.isNull() || keyArray.size() != MFRC522::MIFARE_Misc::MF_KEY_SIZE) return;

  MFRC522::MIFARE_Key key;
  for (uint8_t i = 0; i < MFRC522::MIFARE_Misc::MF_KEY_SIZE; i++)
    key.keyByte[i] = keyArray[i];

  xSemaphoreTake(_authUsersMutex, portMAX_DELAY);
  _authorizedUsers[cardId] = key;
  xSemaphoreGive(_authUsersMutex);
}

void RfidController::RemoveUser(const String& payload) {
  JsonDocument document;
  deserializeJson(document, payload);
  if (!document.is<JsonObject>()) return;
  
  String cardId = document["Uid"];
  if (cardId.isEmpty()) return;

  xSemaphoreTake(_authUsersMutex, portMAX_DELAY);
  _authorizedUsers.erase(cardId);
  xSemaphoreGive(_authUsersMutex);
}