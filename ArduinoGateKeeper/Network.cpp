#include <queue>
#include "MqttClient.h"
#include "Network.h"

char Network::_strBuffer[STRING_BUFFER_SIZE];
SemaphoreHandle_t Network::_semaphoreHandle;
std::queue<MqttMessage> Network::_messages;
WiFiUDP Network::_udpWifiClient;
NTPClient Network::_ntpClient(Network::_udpWifiClient, NTP_SERVER_URL, TIME_OFFSET, TIME_RESYNC_INTERVAL);
WiFiClient Network::_wifiClient;
MqttClient Network::_mqttClient(Network::_wifiClient);

bool Network::begin(SemaphoreHandle_t semaphore) {
  _semaphoreHandle = semaphore;
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
    _strBuffer,
    STRING_BUFFER_SIZE,
    " Success! IP address: %s",
    WiFi.localIP().toString().c_str()
  );
  Serial.println(_strBuffer);
  return true;
}

bool Network::synchronizeClock() {
  snprintf(
    _strBuffer,
    STRING_BUFFER_SIZE,
    "Attempting to synchronize clock with %s...",
    NTP_SERVER_URL
  );
  Serial.print(_strBuffer);
  
  _ntpClient.begin();
  if (_ntpClient.update() && _ntpClient.isTimeSet()) {
    snprintf(
      _strBuffer,
      STRING_BUFFER_SIZE,
      " Success! Current time: %s",
      _ntpClient.getFormattedTime().c_str()
    );
    Serial.println(_strBuffer);
    return true;
  }
  else {
    Serial.println(" Error!");
    return false;
  }
}

bool Network::connectMqttBroker() {
  snprintf(
    _strBuffer,
    STRING_BUFFER_SIZE,
    "Attempting to connect to MQTT broker at %s:%i... ",
    MQTT_BROKER_ADDRESS,
    MQTT_BROKER_PORT
  );
  Serial.print(_strBuffer);

  _mqttClient.setId(MQTT_DEVICE_ID);
  snprintf(_strBuffer, STRING_BUFFER_SIZE, DEVICE_STATUS_TOPIC, MQTT_DEVICE_ID);
  publishLastWill(_strBuffer, "offline", true);

  if (!_mqttClient.connect(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT)) {
    snprintf(
      _strBuffer,
      STRING_BUFFER_SIZE,
      "Failed with code %i",
      _mqttClient.connectError()
    );
    Serial.println(_strBuffer);
    return false;
  }

  _mqttClient.onMessage(Network::queueMessage);
  _mqttClient.subscribe(AUTHORIZED_USERS_TOPIC, MQTT_QOS_LEVEL);

  publishMessage(_strBuffer, "online", true);

  Serial.println("Success!");
  return true;
}

void Network::queueMessage(int msgSize) {
  if (msgSize == 0) return;

  if (xSemaphoreTake(_semaphoreHandle, pdMS_TO_TICKS(NETWORK_OPERATION_INTERVAL)) == pdTRUE) {
    if (_mqttClient.messageTopic() == AUTHORIZED_USERS_TOPIC)
      _messages.push(MqttMessage{ _mqttClient.messageTopic(), String() });
    else {
      xSemaphoreGive(_semaphoreHandle);
      return;
    };

    while (_mqttClient.available())
      _messages.back().payload += (char)_mqttClient.read();
    
    xSemaphoreGive(_semaphoreHandle);
  }
}

void Network::publishMessage(const char* topic, const char* payload, bool retain) {
  _mqttClient.beginMessage(topic, retain, MQTT_QOS_LEVEL);
  _mqttClient.print(payload);
  _mqttClient.endMessage();
}

void Network::publishLastWill(const char* topic, const char* payload, bool retain) {
  _mqttClient.beginWill(topic, retain, MQTT_QOS_LEVEL);
  _mqttClient.print(payload);
  _mqttClient.endWill();
}

bool Network::handleConnections() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost, attempting to reestablish it...");
    if (connectWiFi()) return false;
  }

  if (!_mqttClient.connected()) {
    Serial.println("Connection to MQTT broker lost, attempting to reestablish it...");
    if (connectMqttBroker()) return false;
  }

  _mqttClient.poll();
  return true;
}

time_t Network::getCurrentTimestamp() {
  if (xSemaphoreTake(_semaphoreHandle, pdMS_TO_TICKS(NETWORK_OPERATION_INTERVAL)) != pdTRUE) return 0;
  time_t ret = _ntpClient.getEpochTime();
  xSemaphoreGive(_semaphoreHandle);
  return ret;
}

bool Network::hasAvailableMessages() { return !_messages.empty(); }

MqttMessage Network::getNextMessage() {
  if (xSemaphoreTake(_semaphoreHandle, pdMS_TO_TICKS(NETWORK_OPERATION_INTERVAL)) != pdTRUE) return MqttMessage{};
  MqttMessage message = _messages.front();
  _messages.pop();
  xSemaphoreGive(_semaphoreHandle);
  return message;
}

void Network::reportActivity(const String& payload) {
  snprintf(_strBuffer, STRING_BUFFER_SIZE, ACTIVITY_LOG_TOPIC, MQTT_DEVICE_ID);
  publishMessage(_strBuffer, payload.c_str());
}