#include "ODataClient.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonDocument>

ODataClient::ODataClient(const QString* baseUrl, const QSslConfiguration* sslConfig, QObject* parent) : QObject(parent), _baseUrl(baseUrl), _sslConfig(sslConfig), _manager(new QNetworkAccessManager(this)) {}

void ODataClient::getRequest(const QString& entity, const QMap<QString, QString>& params) const
{
    if (!_baseUrl || _baseUrl->isEmpty())
        requestFailed("Base URL is not set");
    else if (!_sslConfig || _sslConfig->isNull())
        requestFailed("SSL configuration not set");
    else {
        QUrl url(QString("https://%1/api/%2").arg(*_baseUrl, entity));
        url.setQuery(getQuery(params));
        QNetworkReply* reply = sendRequest(url, "GET");
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleReply(reply, HttpMethod::GET); });
    }
}

void ODataClient::getRequest(const QString& entity, int key, const QMap<QString, QString>& params) const
{
    if (!_baseUrl || _baseUrl->isEmpty())
        requestFailed("Base URL is not set");
    else if (!_sslConfig || _sslConfig->isNull())
        requestFailed("SSL configuration not set");
    else {
        QUrl url(QString("https://%1/api/%2(%3)").arg(*_baseUrl, entity).arg(key));
        url.setQuery(getQuery(params));
        QNetworkReply* reply = sendRequest(url, "GET");
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleReply(reply, HttpMethod::GET); });
    }
}

void ODataClient::getRequest(const QString& entity, const QMap<QString, int>& key, const QMap<QString, QString>& params) const
{
    if (!_baseUrl || _baseUrl->isEmpty())
        requestFailed("Base URL is not set");
    else if (!_sslConfig || _sslConfig->isNull())
        requestFailed("SSL configuration not set");
    else {
        QUrl url(QString("https://%1/api/%2(%3)").arg(*_baseUrl, entity, getKeyString(key)));
        url.setQuery(getQuery(params));
        QNetworkReply* reply = sendRequest(url, "GET");
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleReply(reply, HttpMethod::GET); });
    }
}

void ODataClient::postRequest(const QString& entity, const QJsonObject& body) const
{
    if (!_baseUrl || _baseUrl->isEmpty())
        requestFailed("Base URL is not set");
    else if (!_sslConfig || _sslConfig->isNull())
        requestFailed("SSL configuration not set");
    else {
        QUrl url(QString("https://%1/api/%2").arg(*_baseUrl, entity));
        QNetworkReply* reply = sendRequest(url, "POST", body);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleReply(reply, HttpMethod::POST); });
    }
}

void ODataClient::patchRequest(const QString& entity, int key, const QJsonObject& body) const
{
    if (!_baseUrl || _baseUrl->isEmpty())
        requestFailed("Base URL is not set");
    else if (!_sslConfig || _sslConfig->isNull())
        requestFailed("SSL configuration not set");
    else {
        QUrl url(QString("https://%1/api/%2(%3)").arg(*_baseUrl, entity).arg(key));
        QNetworkReply* reply = sendRequest(url, "PATCH", body);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleReply(reply, HttpMethod::PATCH); });
    }
}

void ODataClient::patchRequest(const QString& entity, const QMap<QString, int>& key, const QJsonObject& body) const
{
    if (!_baseUrl || _baseUrl->isEmpty())
        requestFailed("Base URL is not set");
    else if (!_sslConfig || _sslConfig->isNull())
        requestFailed("SSL configuration not set");
    else {
        QUrl url(QString("https://%1/api/%2(%3)").arg(*_baseUrl, entity, getKeyString(key)));
        QNetworkReply* reply = sendRequest(url, "PATCH", body);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleReply(reply, HttpMethod::PATCH); });
    }
}

void ODataClient::deleteRequest(const QString& entity, int key) const
{
    if (!_baseUrl || _baseUrl->isEmpty())
        requestFailed("Base URL is not set");
    else if (!_sslConfig || _sslConfig->isNull())
        requestFailed("SSL configuration not set");
    else {
        QUrl url(QString("https://%1/api/%2(%3)").arg(*_baseUrl, entity).arg(key));
        QNetworkReply* reply = sendRequest(url, "DELETE");
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleReply(reply, HttpMethod::DELETE); });
    }
}

void ODataClient::deleteRequest(const QString& entity, const QMap<QString, int>& key) const
{
    if (!_baseUrl || _baseUrl->isEmpty())
        requestFailed("Base URL is not set");
    else if (!_sslConfig || _sslConfig->isNull())
        requestFailed("SSL configuration not set");
    else {
        QUrl url(QString("https://%1/api/%2(%3)").arg(*_baseUrl, entity, getKeyString(key)));
        QNetworkReply* reply = sendRequest(url, "DELETE");
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleReply(reply, HttpMethod::DELETE); });
    }
}

void ODataClient::handleReply(QNetworkReply* reply, HttpMethod method) const
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        requestFailed(reply->errorString());
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::ParseError::NoError) {
        requestFailed(error.errorString());
        return;
    }

    QJsonObject root = doc.object();
    switch (method) {
        case GET: {
            if (root.contains("value"))
                getCollectionRequestFinished(root["value"].toArray());
            else
                getRequestFinished(root);
        }; break;
        case POST: postRequestFinished(root); break;
        case PATCH: patchRequestFinished(root); break;
        case DELETE: deleteRequestFinished(); break;
        default: requestFailed("Unexpected response format"); break;
    }
}

QNetworkReply* ODataClient::sendRequest(const QUrl& url, const QByteArray& method, const QJsonObject& body) const
{
    QNetworkRequest request(url);
    request.setSslConfiguration(*_sslConfig);

    if (!body.isEmpty()) {
        request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
        return _manager->sendCustomRequest(request, method, QJsonDocument(body).toJson(QJsonDocument::JsonFormat::Compact));
    }
    else
        return _manager->sendCustomRequest(request, method);
}

QString ODataClient::getKeyString(const QMap<QString, int>& key) const
{
    QString result;
    for (auto it = key.constBegin(); it != key.constEnd(); it++)
        result += QString("%1=%2,").arg(it.key(), it.value());
    result.removeLast();
    return result;
}

QUrlQuery ODataClient::getQuery(const QMap<QString, QString>& params) const
{
    QUrlQuery query;
    for (auto it = params.constBegin(); it != params.constEnd(); it++)
        query.addQueryItem(it.key(), it.value());
    return query;
}