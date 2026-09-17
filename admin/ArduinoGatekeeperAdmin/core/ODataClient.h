#ifndef ODATACLIENT_H
#define ODATACLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>
#include <QMap>

enum HttpMethod { GET = 1, POST = 2, PATCH = 3, DELETE = 4 };

class ODataClient : public QObject
{
    Q_OBJECT
public:
    explicit ODataClient(const QString* baseUrl, const QSslConfiguration* sslConfig, QObject* parent = nullptr);
    void getRequest(const QString& entity, const QMap<QString, QString>& params) const;
    void getRequest(const QString& entity, int key, const QMap<QString, QString>& params) const;
    void getRequest(const QString& entity, const QMap<QString, int>& key, const QMap<QString, QString>& params) const;
    void postRequest(const QString& entity, const QJsonObject& body) const;
    void patchRequest(const QString& entity, int key, const QJsonObject& body) const;
    void patchRequest(const QString& entity, const QMap<QString, int>& key, const QJsonObject& body) const;
    void deleteRequest(const QString& entity, int key) const;
    void deleteRequest(const QString& entity, const QMap<QString, int>& key) const;
signals:
    void getRequestFinished(QString entity, QJsonObject result) const;
    void getCollectionRequestFinished(QString entity, QJsonArray results) const;
    void postRequestFinished(QString entity, QJsonObject result) const;
    void patchRequestFinished(QString entity, QJsonObject result) const;
    void deleteRequestFinished(QString entity) const;
    void requestFailed(QString entity, QString error) const;
private slots:
    void handleReply(const QString& entity, QNetworkReply* reply, HttpMethod method) const;
private:
    const QString* _baseUrl;
    const QSslConfiguration* _sslConfig;
    QNetworkAccessManager* _manager;
    QNetworkReply* sendRequest(const QUrl& url, const QByteArray& method, const QJsonObject& body = {}) const;
    QString getKeyString(const QMap<QString, int>& key) const;
    QUrlQuery getQuery(const QMap<QString, QString>& params) const;
};

#endif // ODATACLIENT_H
