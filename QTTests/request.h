#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QUrl>

class Request : public QObject
{
    Q_OBJECT

public:
    Request();

Q_SIGNALS:
    void  gotHttpData(QByteArray data);
    void httpFinished();

public Q_SLOTS:
    void onProcessRequest(QUrl request);
    virtual void onGetRequestWithBody(QUrl url, const QByteArray &body);
    virtual void onPostRequestWithBody(QUrl url, const QByteArray &body);
    void httpReadyRead();

private:
    // QNetworkAccessManager* m_qnam;
    QScopedPointer<QNetworkAccessManager, QScopedPointerDeleter<QNetworkAccessManager>> m_qnam;
    QScopedPointer<QNetworkReply, QScopedPointerDeleter<QNetworkReply>> m_reply;
};
