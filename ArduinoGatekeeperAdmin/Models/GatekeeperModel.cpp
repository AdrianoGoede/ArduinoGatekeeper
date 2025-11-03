#include "GatekeeperModel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

GatekeeperModel::GatekeeperModel(QObject* parent) : QObject(parent), _mqttClient(new QMqttClient(this)) {
    connect(_mqttClient, &QMqttClient::connected, this, &GatekeeperModel::mqttConnected);
    connect(_mqttClient, &QMqttClient::disconnected, this, &GatekeeperModel::mqttDisconnected);
    connect(_mqttClient, &QMqttClient::messageReceived, this, &GatekeeperModel::mqttMessageReceived);
    connect(_mqttClient, &QMqttClient::stateChanged, this, &GatekeeperModel::mqttClientStateChanged);
}

void GatekeeperModel::connectToBroker(const QString& address, qint16 port, const QString& userName, const QString& password) {
    if (_mqttClient->state() != QMqttClient::ClientState::Disconnected)
        _mqttClient->disconnectFromHost();

    _mqttClient->setHostname(address);
    _mqttClient->setPort(port);
    _mqttClient->setUsername(userName);
    _mqttClient->setPassword(password);
    _mqttClient->setClientId(MQTT_CLIENT_ID);
    _mqttClient->connectToHost();
}

void GatekeeperModel::updateUserList(const QList<UserEntry>& entries)
{
    QJsonArray users;
    for (const UserEntry& entry : entries) {
        QJsonObject obj;
        QJsonArray key;
        for (const uint8_t& byte : entry.key)
            key.append(byte);
        obj["UID"] = entry.uid;
        obj["key"] = key;
        users.append(obj);
    }
    QJsonDocument doc;
    doc.setArray(users);

    _mqttClient->publish(
        QString(MQTT_TOPIC_USER_LIST).replace('+', MQTT_CLIENT_ID),
        doc.toJson(QJsonDocument::JsonFormat::Compact),
        MQTT_CLIENT_QOS,
        true
    );
}

void GatekeeperModel::mqttConnected() {
    _mqttClient->subscribe(QMqttTopicFilter(MQTT_TOPIC_DEVICE_STATUS), MQTT_CLIENT_QOS);
    _mqttClient->subscribe(QMqttTopicFilter(MQTT_TOPIC_ACTIVITY_LOG), MQTT_CLIENT_QOS);
    _mqttClient->subscribe(QMqttTopicFilter(MQTT_TOPIC_USER_LIST), MQTT_CLIENT_QOS);
    emit clientStateChanged(QMqttClient::ClientState::Connected, _mqttClient->error());
}

void GatekeeperModel::mqttDisconnected() { emit clientStateChanged(QMqttClient::ClientState::Disconnected, _mqttClient->error()); }

void GatekeeperModel::mqttMessageReceived(const QByteArray& message, const QMqttTopicName& topic) {
    QStringList topicParts = topic.name().split('/');
    if (topicParts.size() != 3) return;

    if (topicParts.at(2) == "Log")
        processLogMessage(topicParts.at(1), message);
    else if (topic.name().endsWith("Status"))
        processDeviceStatusMessage(topicParts.at(1), message);
    else if (topic.name().endsWith("UserList"))
        processUserListMessage(topicParts.at(1), message);
}

void GatekeeperModel::mqttClientStateChanged(QMqttClient::ClientState state) { emit clientStateChanged(state, _mqttClient->error()); }

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

void GatekeeperModel::processDeviceStatusMessage(const QString& deviceId, const QByteArray& payload)
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

void GatekeeperModel::processUserListMessage(const QString& deviceId, const QByteArray& payload)
{
    if (deviceId == MQTT_CLIENT_ID && _receivedRetainedMessages) return;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isArray()) return;

    QVector<UserEntry> entries;
    for (const QJsonValue& value : doc.array()) {
        if (!value.isObject()) continue;
        QJsonObject obj = value.toObject();
        UserEntry entry{ obj["UID"].toString() };

        QJsonArray key = obj["key"].toArray();
        for (int i = 0; i < std::min(key.size(), (qsizetype)USER_RFID_KEY_LENGHT); i++)
            entry.key[i] = key[i].toInt(0);

        entries.push_back(entry);
    }

    emit newUserList(entries);
    _receivedRetainedMessages = true;
}

QString GatekeeperModel::getBrokerAddress() const { return _mqttClient->hostname(); }

quint16 GatekeeperModel::getBrokerPort() const { return _mqttClient->port(); }
