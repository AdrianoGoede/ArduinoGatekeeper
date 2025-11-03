#ifndef GATEKEEPERMODEL_H
#define GATEKEEPERMODEL_H

#include "../Config/config.h"
#include <QMqttClient>
#include <QDateTime>
#include <QList>
#include <QSet>

struct DeviceStatusEntry {
    QString deviceId;
    bool online;
    QDateTime timestamp;
};

struct LogEntry {
    QString deviceId, userId;
    QDateTime timestamp;
    bool accessGranted = false;
};

struct UserEntry {
    QString uid;
    uint8_t key[USER_RFID_KEY_LENGHT];
};

class GatekeeperModel : public QObject
{
    Q_OBJECT

public:
    explicit GatekeeperModel(QObject* parent = nullptr);
    QString getBrokerAddress() const;
    quint16 getBrokerPort() const;

private slots:
    void mqttConnected();
    void mqttDisconnected();
    void mqttMessageReceived(const QByteArray& message, const QMqttTopicName& topic);
    void mqttClientStateChanged(QMqttClient::ClientState state);

public slots:
    void connectToBroker(const QString& address, qint16 port, const QString& userName, const QString& password);
    void updateUserList(const QList<UserEntry>& entries);

signals:
    void clientStateChanged(QMqttClient::ClientState state, QMqttClient::ClientError error = QMqttClient::ClientError::NoError);
    void newDeviceStatusEntry(const DeviceStatusEntry& entry);
    void newLogEntry(const LogEntry& entry);
    void newUserList(const QVector<UserEntry>& entries);
    void metricsUpdated(uint32_t connectedDevicesCount, uint32_t granted, uint32_t denied);

private:
    QMqttClient* _mqttClient = nullptr;
    uint32_t _grantedCount = 0, _deniedCount = 0;
    QSet<QString> _connectedDeviceIds;
    QList<LogEntry> _logEntries;
    bool _receivedRetainedMessages = false;
    void processLogMessage(const QString& deviceId, const QByteArray& payload);
    void processDeviceStatusMessage(const QString& deviceId, const QByteArray& payload);
    void processUserListMessage(const QString& deviceId, const QByteArray& payload);
};

#endif // GATEKEEPERMODEL_H
