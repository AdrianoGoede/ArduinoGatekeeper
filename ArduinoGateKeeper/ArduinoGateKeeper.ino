#include <Arduino_FreeRTOS.h>
#include "AccessControl.h"
#include "Network.h"
#include "config.h"

SemaphoreHandle_t semaphore;

void networkingTask(void* pvParameters) {
  String strBuffer;
  
  while (true) {
    if (Network::handleConnections()) {
      while (Network::hasAvailableMessages()) {
        MqttMessage message = Network::getNextMessage();
        if (message.topic == AUTHORIZED_USERS_TOPIC)
          AccessControl::handleUserListMessage(message.payload);
      }
      
      while (AccessControl::hasAvailableLogEntries())
        Network::reportActivity(AccessControl::getNextLogEntry());
    }
    
    vTaskDelay(pdMS_TO_TICKS(NETWORK_OPERATION_INTERVAL));
  }
}

void accessControlTask(void* pvParameters) {
  while (true) {
    time_t timestamp = Network::getCurrentTimestamp();
    
    AccessControlStatus status = AccessControl::handleAccessControl(timestamp);
    if (status != AccessControlStatus::Waiting) {
      digitalWrite((uint8_t)status, HIGH);
      vTaskDelay(pdMS_TO_TICKS(STATUS_LED_INTERVAL));
      digitalWrite((uint8_t)status, LOW);
    }

    vTaskDelay(pdMS_TO_TICKS(ACC_CONTROL_OPERATION_INTERVAL));
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) delay(1000);

  pinMode(ACCESS_DENIED_LED_PIN, OUTPUT);
  pinMode(ACCESS_GRANTED_LED_PIN, OUTPUT);

  semaphore = xSemaphoreCreateBinary();
  if (!semaphore) {
    Serial.println("Could not create semaphore, aborting...");
    while (true);
  }
  xSemaphoreGive(semaphore);
  Serial.println("Semaphore initialized");

  if (!Network::begin(semaphore)) {
    Serial.println("Could not initialize network module, aborting...");
    while (true);
  }

  if (!AccessControl::begin(semaphore)) {
    Serial.println("Could not initialize AccessControl module, aborting...");
    while (true);
  }

  xTaskCreate(
    networkingTask,
    "Networking Task",
    NETWORK_TASK_STACK_DEPTH,
    nullptr,
    NETWORK_TASK_PRIORITY,
    nullptr
  );

  xTaskCreate(
    accessControlTask,
    "Access Control Task",
    ACCESS_CONTROL_TASK_STACK_DEPTH,
    nullptr,
    ACCESS_CONTROL_TASK_PRIORITY,
    nullptr
  );

  Serial.println("All set! Starting...\n");
  vTaskStartScheduler();
}

void loop() {}