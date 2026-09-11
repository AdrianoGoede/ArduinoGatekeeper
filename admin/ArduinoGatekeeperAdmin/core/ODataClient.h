#ifndef ODATACLIENT_H
#define ODATACLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>

enum HttpMethod { GET = 1, POST = 2, PATCH = 3, DELETE = 4 };

class ODataClient : public QObject
{
    Q_OBJECT
public:
    explicit ODataClient(const QString* baseUrl, const QSslConfiguration* sslConfig, QObject* parent = nullptr);
    void getRequest(const QString& entity, const QMap<QString, QString>& params);
    void postRequest(const QString& entity, const QJsonObject& body);
    void patchRequest(const QString& entity, int key, const QJsonObject& body);
    void patchRequest(const QString& entity, const QMap<QString, int>& key, const QJsonObject& body);
    void deleteRequest(const QString& entity, int key);
    void deleteRequest(const QString& entity, const QMap<QString, int>& key);
signals:
    void getRequestFinished(QJsonObject result);
    void getCollectionRequestFinished(QJsonArray results);
    void postRequestFinished(QJsonObject result);
    void patchRequestFinished(QJsonObject result);
    void deleteRequestFinished();
    void requestFailed(QString error);
private slots:
    void handleReply(QNetworkReply* reply, HttpMethod method);
private:
    const QString* _baseUrl;
    const QSslConfiguration* _sslConfig;
    QNetworkAccessManager* _manager;
    QNetworkReply* sendRequest(const QUrl& url, const QByteArray& method, const QJsonObject& body = {});
};

#endif // ODATACLIENT_H
