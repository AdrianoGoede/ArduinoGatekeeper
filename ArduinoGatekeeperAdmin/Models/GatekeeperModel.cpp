#include "GatekeeperModel.h"
#include "../Config/config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

GatekeeperModel::GatekeeperModel(QObject* parent) : QObject(parent), _mqttClient(new QMqttClient(this)) {
    connect(_mqttClient, &QMqttClient::connected, this, &GatekeeperModel::mqttConnected);
    connect(_mqttClient, &QMqttClient::disconnected, this, &GatekeeperModel::mqttDisconnected);
    connect(_mqttClient, &QMqttClient::messageReceived, this, &GatekeeperModel::mqttMessageReceived);
}

void GatekeeperModel::connectToBroker(const QString& address, uint16_t port) {
    _mqttClient->setHostname(address);
    _mqttClient->setPort(port);
    _mqttClient->setClientId(MQTT_CLIENT_ID);
    _mqttClient->connectToHost();
}

void GatekeeperModel::mqttConnected() {
    emit connectionStatusChanged(true);
    _mqttClient->subscribe(QMqttTopicFilter(MQTT_TOPIC_DEVICE_STATUS), MQTT_CLIENT_QOS);
    _mqttClient->subscribe(QMqttTopicFilter(MQTT_TOPIC_ACTIVITY_LOG), MQTT_CLIENT_QOS);
}

void GatekeeperModel::mqttDisconnected() { emit connectionStatusChanged(false); }

void GatekeeperModel::mqttMessageReceived(const QByteArray& message, const QMqttTopicName& topic) {
    QStringList topicParts = topic.name().split('/');
    if (topicParts.size() != 3) return;

    if (topicParts.at(2) == "Log")
        processLogMessage(topicParts.at(1), message);
    else if (topic.name().endsWith("Status"))
        processDeviceStatusMessage(topicParts.at(1), message);
}

void GatekeeperModel::processLogMessage(const QString& deviceId, const QByteArray& payload) {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError);
    if (parseError.error != QJsonParseError::NoError) return;

    QJsonObject obj = doc.object();
    LogEntry entry;

    entry.deviceId = deviceId.trimmed();
    entry.userId = obj["UID"].toString();
    entry.accessGranted = obj["Granted"].toBool();
    entry.timestamp = QDateTime::fromSecsSinceEpoch(obj["Timestamp"].toInteger());

    if (entry.accessGranted)
        _grantedCount++;
    else
        _deniedCount++;

    _logEntries += entry;
    emit newLogEntry(entry);
    emit metricsUpdated(_connectedDeviceIds.size(), _grantedCount, _deniedCount);
}

void GatekeeperModel::processDeviceStatusMessage(const QString& deviceId, const QByteArray &payload)
{
    int8_t status = QStringList(MQTT_DEVICE_STATUS_VALUES).indexOf(payload);
    if (status < 0) return;

    DeviceStatusEntry entry { deviceId.trimmed(), (bool)status, QDateTime::currentDateTime() };
    if (entry.online)
        _connectedDeviceIds.insert(entry.deviceId);
    else
        _connectedDeviceIds.remove(entry.deviceId);

    emit newDeviceStatusEntry(entry);
    emit metricsUpdated(_connectedDeviceIds.size(), _grantedCount, _deniedCount);
}

QString GatekeeperModel::getBrokerAddress() const { return _mqttClient->hostname(); }

quint16 GatekeeperModel::getBrokerPort() const { return _mqttClient->port(); }
