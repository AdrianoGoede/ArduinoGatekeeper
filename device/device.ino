#include <freertos/FreeRTOS.h>
#include "rfid_controller.h"
#include "network.h"
#include "config.h"

TaskHandle_t networkTaskHandle;
TaskHandle_t rfidTaskHandle;

void networkTask(void* params);
void rfidTask(void* params);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) {}

  pinMode(ACCESS_DENIED_LED_PIN, OUTPUT);
  pinMode(ACCESS_GRANTED_LED_PIN, OUTPUT);

  if (!Network::begin()) {
    Serial.println("Network module setup failed! aborting...");
    while (true) {}
  }

  if (!RfidController::begin()) {
    Serial.println("Access control module failed! aborting...");
    while (true) {}
  }

  xTaskCreatePinnedToCore(
    networkTask,
    "Network",
    NETWORK_TASK_STACK_DEPTH,
    nullptr,
    NETWORK_TASK_PRIORITY,
    &networkTaskHandle,
    0
  );

  xTaskCreatePinnedToCore(
    rfidTask,
    "RFID",
    RFID_TASK_STACK_DEPTH,
    nullptr,
    RFID_TASK_PRIORITY,
    &rfidTaskHandle,
    1
  );
}

void loop() { vTaskDelete(nullptr); }

void networkTask(void* params) {
  while (true) {
    if (Network::handleConnections()) {
      while (true) {
        String log = RfidController::getNextLogEntry();
        if (log.isEmpty()) break;
        Network::sendLogMessage(log);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(NETWORK_TASK_DELAY));
  }
}

void rfidTask(void* params) {
  while (true) {
    RfidScanResult result = RfidController::handleScan();
    if (result != RfidScanResult::Waiting) {
      digitalWrite((uint8_t)result, HIGH);
      vTaskDelay(pdMS_TO_TICKS(STATUS_LED_INTERVAL));
      digitalWrite((uint8_t)result, LOW);
    }

    vTaskDelay(pdMS_TO_TICKS(RFID_TASK_DELAY));
  }
}