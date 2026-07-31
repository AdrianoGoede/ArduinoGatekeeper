#include "network.h"
#include <ArduinoJson.h>

WiFiClientSecure Network::_wifiClient;
MqttClient Network::_mqttClient(_wifiClient);
String Network::_scanTopic;
String Network::_deviceStatusStopic;
String Network::_addUserTopic;
String Network::_removeUserTopic;
MqttMessageCallback Network::_addUserCallback;
MqttMessageCallback Network::_removeUserCallback;
char Network::_strBuffer[STRING_BUFFER_SIZE];

bool Network::begin(MqttMessageCallback addUserCallback, MqttMessageCallback removeUserCallback) {
  return (initialize(addUserCallback, removeUserCallback) && connectWiFi() && synchronizeClock() && connectToMqttBroker());
}

bool Network::initialize(MqttMessageCallback addUserCallback, MqttMessageCallback removeUserCallback) {
  Serial.print("Initializing...");

  _addUserCallback = addUserCallback;
  _removeUserCallback = removeUserCallback;
  
  _scanTopic = String(SCAN_TOPIC);
  _scanTopic.replace("+", MQTT_DEVICE_ID);
  _deviceStatusStopic = String(DEVICE_STATUS_TOPIC);
  _deviceStatusStopic.replace("+", MQTT_DEVICE_ID);
  _addUserTopic = String(ADD_USER_TOPIC);
  _addUserTopic.replace("+", MQTT_DEVICE_ID);
  _removeUserTopic = String(REMOVE_USER_TOPIC);
  _removeUserTopic.replace("+", MQTT_DEVICE_ID);

  _wifiClient.setCACert(CA_CERT);
  _wifiClient.setCertificate(CLIENT_CERT);
  _wifiClient.setPrivateKey(CLIENT_KEY);

  Serial.println(" Done!");
  return true;
}

bool Network::connectWiFi() {
  snprintf(_strBuffer, STRING_BUFFER_SIZE, "Attempting to connect to network %s...", WIFI_SSID);
  Serial.print(_strBuffer);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  snprintf(_strBuffer, STRING_BUFFER_SIZE, " Success! IP Address: %s", WiFi.localIP().toString().c_str());
  Serial.println(_strBuffer);
  return true;
}

bool Network::synchronizeClock() {
  Serial.print("Attempting to synchronize clock...");
  configTime(0, 0, NTP_SERVER_PRIMARY, NTP_SERVER_SECONDARY);

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    delay(1000);
    Serial.print(".");
  }

  time_t now;
  time(&now);
  snprintf(_strBuffer, STRING_BUFFER_SIZE, " Success! Current Unix timestamp: %ld", now);
  Serial.println(_strBuffer);
  return true;
}

bool Network::connectToMqttBroker() {
  snprintf(
    _strBuffer,
    STRING_BUFFER_SIZE,
    "Attempting to connect to MQTT broker at %s:%i... ",
    MQTT_BROKER_ADDRESS,
    MQTT_BROKER_PORT
  );
  Serial.print(_strBuffer);

  _mqttClient.setId(MQTT_DEVICE_ID);
  _mqttClient.setKeepAliveInterval(MQTT_KEEP_ALIVE_INTERVAL);
  publishLastWill(_deviceStatusStopic, getJsonStatusMessage(false, false));

  _mqttClient.onMessage(Network::handleIncomingMessage);

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

  _mqttClient.subscribe(_addUserTopic, MQTT_QOS_LEVEL);
  _mqttClient.subscribe(_removeUserTopic, MQTT_QOS_LEVEL);
  publishMessage(_deviceStatusStopic, getJsonStatusMessage(true, true));

  Serial.println("Success!");
  return true;
}

void Network::publishMessage(const String& topic, const String& payload, bool retain) {
  _mqttClient.beginMessage(topic, retain, MQTT_QOS_LEVEL);
  _mqttClient.print(payload);
  _mqttClient.endMessage();
}

void Network::publishLastWill(const String& topic, const String& payload, bool retain) {
  _mqttClient.beginWill(topic, retain, MQTT_QOS_LEVEL);
  _mqttClient.print(payload);
  _mqttClient.endWill();
}

String Network::getJsonStatusMessage(bool online, bool addTimestamp) {
  JsonDocument doc;
  doc["DeviceId"] = MQTT_DEVICE_ID;
  doc["Online"] = online;

  if (addTimestamp) {
    time_t now;
    time(&now);
    doc["Timestamp"] = now;
  }
  
  serializeJson(doc, _strBuffer, STRING_BUFFER_SIZE);
  return String(_strBuffer);
}

void Network::handleIncomingMessage(int messageSize) {
  if (messageSize == 0) return;
  String topic = _mqttClient.messageTopic();
  String payload = _mqttClient.readString();

  if (topic == _addUserTopic)
    _addUserCallback(payload);
  else if (topic == _removeUserTopic)
    _removeUserCallback(payload);
}

bool Network::handleConnections() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost, attempting to reestablish it...");
    if (!connectWiFi()) return false;
  }

  if (!_mqttClient.connected()) {
    Serial.println("Connection to MQTT broker lost, attempting to reestablish it...");
    if (!connectToMqttBroker()) return false;
  }

  _mqttClient.poll();
  return true;
}

void Network::sendLogMessage(const String& payload) { publishMessage(_scanTopic, payload); }