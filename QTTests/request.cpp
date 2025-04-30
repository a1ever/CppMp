#include "request.h"

Request::Request()
{}

void Request::onProcessRequest(QUrl url)
{
    m_reply.reset();
    m_qnam.reset(new QNetworkAccessManager());
    m_reply.reset(m_qnam->get(QNetworkRequest(url)));
    connect(m_reply.get(), &QNetworkReply::finished, this, &Request::httpFinished);
    connect(m_reply.get(), &QIODevice::readyRead, this, &Request::httpReadyRead);
}

void Request::onGetRequestWithBody(QUrl url, const QByteArray &body) {
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_reply.reset();
    m_qnam.reset(new QNetworkAccessManager());
    m_reply.reset(m_qnam->sendCustomRequest(request, "GET", body));

    connect(m_reply.get(), &QNetworkReply::finished, this, &Request::httpFinished);
    connect(m_reply.get(), &QIODevice::readyRead, this, &Request::httpReadyRead);
}

void Request::onPostRequestWithBody(QUrl url, const QByteArray &body) {
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_reply.reset();
    m_qnam.reset(new QNetworkAccessManager());
    m_reply.reset(m_qnam->post(request, body));

    connect(m_reply.get(), &QNetworkReply::finished, this, &Request::httpFinished);
    connect(m_reply.get(), &QIODevice::readyRead, this, &Request::httpReadyRead);
}

void Request::httpReadyRead()
{
    emit gotHttpData(m_reply->readAll());
}
