#include <queue>
#include "MqttClient.h"
#include "Network.h"

char Network::strBuffer[STRING_BUFFER_SIZE];
SemaphoreHandle_t Network::semaphoreHandle;
std::queue<MqttMessage> Network::messages;
WiFiUDP Network::udpWifiClient;
NTPClient Network::ntpClient(Network::udpWifiClient, NTP_SERVER_URL, TIME_OFFSET, TIME_RESYNC_INTERVAL);
WiFiClient Network::wifiClient;
MqttClient Network::mqttClient(Network::wifiClient);

bool Network::begin(SemaphoreHandle_t semaphore) {
  semaphoreHandle = semaphore;
  return (connectWiFi() && synchronizeClock() && connectMqttBroker());
}

bool Network::connectWiFi() {
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    return false;
  }

  Serial.print("Attempting to connect to WiFi network");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(5000);
  }
  
  snprintf(
    strBuffer,
    STRING_BUFFER_SIZE,
    " Success! IP address: %s",
    WiFi.localIP().toString().c_str()
  );
  Serial.println(strBuffer);
  return true;
}

bool Network::synchronizeClock() {
  snprintf(
    strBuffer,
    STRING_BUFFER_SIZE,
    "Attempting to synchronize clock with %s...",
    NTP_SERVER_URL
  );
  Serial.print(strBuffer);
  
  ntpClient.begin();
  if (ntpClient.update() && ntpClient.isTimeSet()) {
    snprintf(
      strBuffer,
      STRING_BUFFER_SIZE,
      " Success! Current time: %s",
      ntpClient.getFormattedTime().c_str()
    );
    Serial.println(strBuffer);
    return true;
  }
  else {
    Serial.println(" Error!");
    return false;
  }
}

bool Network::connectMqttBroker() {
  snprintf(
    strBuffer,
    STRING_BUFFER_SIZE,
    "Attempting to connect to MQTT broker at %s:%i... ",
    MQTT_BROKER_ADDRESS,
    MQTT_BROKER_PORT
  );
  Serial.print(strBuffer);

  mqttClient.setId(MQTT_DEVICE_ID);
  snprintf(strBuffer, STRING_BUFFER_SIZE, DEVICE_STATUS_TOPIC, MQTT_DEVICE_ID);
  publishLastWill(strBuffer, "offline", true);

  if (!mqttClient.connect(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT)) {
    snprintf(
      strBuffer,
      STRING_BUFFER_SIZE,
      "Failed with code %i",
      mqttClient.connectError()
    );
    Serial.println(strBuffer);
    return false;
  }

  mqttClient.onMessage(Network::queueMessage);
  mqttClient.subscribe(AUTHORIZED_USERS_ADD_TOPIC, MQTT_QOS_LEVEL);
  mqttClient.subscribe(AUTHORIZED_USERS_REMOVE_TOPIC, MQTT_QOS_LEVEL);

  publishMessage(strBuffer, "online", true);

  Serial.println("Success!");
  return true;
}

void Network::queueMessage(int msgSize) {
  if (msgSize == 0) return;

  if (xSemaphoreTake(semaphoreHandle, pdMS_TO_TICKS(NETWORK_OPERATION_INTERVAL)) == pdTRUE) {
    if (mqttClient.messageTopic() == AUTHORIZED_USERS_ADD_TOPIC)
      messages.push(MqttMessage{ MqttMessageType::AuthorizedUserAdd, String() });
    else if (mqttClient.messageTopic() == AUTHORIZED_USERS_REMOVE_TOPIC)
      messages.push(MqttMessage{ MqttMessageType::AuthorizedUserRemove, String() });
    else {
      xSemaphoreGive(semaphoreHandle);
      return;
    };

    while (mqttClient.available())
      messages.back().payload += (char)mqttClient.read();
    
    xSemaphoreGive(semaphoreHandle);
  }
}

void Network::publishMessage(const char* topic, const char* payload, bool retain) {
  mqttClient.beginMessage(topic, retain, MQTT_QOS_LEVEL);
  mqttClient.print(payload);
  mqttClient.endMessage();
}

void Network::publishLastWill(const char* topic, const char* payload, bool retain) {
  mqttClient.beginWill(topic, retain, MQTT_QOS_LEVEL);
  mqttClient.print(payload);
  mqttClient.endWill();
}

bool Network::handleConnections() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost, attempting to reestablish it...");
    if (connectWiFi()) return false;
  }

  if (!mqttClient.connected()) {
    Serial.println("Connection to MQTT broker lost, attempting to reestablish it...");
    if (connectMqttBroker()) return false;
  }

  mqttClient.poll();
  return true;
}

time_t Network::getCurrentTimestamp() {
  if (xSemaphoreTake(semaphoreHandle, pdMS_TO_TICKS(NETWORK_OPERATION_INTERVAL)) != pdTRUE) return 0;
  time_t ret = ntpClient.getEpochTime();
  xSemaphoreGive(semaphoreHandle);
  return ret;
}

bool Network::hasAvailableMessages() { return !messages.empty(); }

MqttMessage Network::getNextMessage() {
  if (xSemaphoreTake(semaphoreHandle, pdMS_TO_TICKS(NETWORK_OPERATION_INTERVAL)) != pdTRUE) return MqttMessage{};
  MqttMessage message = messages.front();
  messages.pop();
  xSemaphoreGive(semaphoreHandle);
  return message;
}

void Network::reportActivity(const String& payload) {
  snprintf(strBuffer, STRING_BUFFER_SIZE, ACTIVITY_LOG_TOPIC, MQTT_DEVICE_ID);
  publishMessage(strBuffer, payload.c_str());
}