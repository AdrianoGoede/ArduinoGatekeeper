#ifndef GATEKEEPERMODEL_H
#define GATEKEEPERMODEL_H

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

class GatekeeperModel : public QObject
{
    Q_OBJECT

private:
    QMqttClient* _mqttClient = nullptr;
    uint32_t _grantedCount = 0, _deniedCount = 0;
    QSet<QString> _connectedDeviceIds;
    QList<LogEntry> _logEntries;
    void processLogMessage(const QString& deviceId, const QByteArray& payload);
    void processDeviceStatusMessage(const QString& deviceId, const QByteArray& payload);

private slots:
    void mqttConnected();
    void mqttDisconnected();
    void mqttMessageReceived(const QByteArray& message, const QMqttTopicName& topic);
    void mqttClientStateChanged(QMqttClient::ClientState state);

public slots:
    void connectToBroker(const QString& address, qint16 port, const QString& userName, const QString& password);

signals:
    void clientStateChanged(QMqttClient::ClientState state, QMqttClient::ClientError error = QMqttClient::ClientError::NoError);
    void newDeviceStatusEntry(const DeviceStatusEntry& entry);
    void newLogEntry(const LogEntry& entry);
    void metricsUpdated(uint32_t connectedDevicesCount, uint32_t granted, uint32_t denied);

public:
    explicit GatekeeperModel(QObject* parent = nullptr);
    QString getBrokerAddress() const;
    quint16 getBrokerPort() const;
};

#endif // GATEKEEPERMODEL_H
