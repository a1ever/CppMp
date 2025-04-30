#include "financeapiclient.h"
#include <QNetworkRequest>
#include <QJsonDocument>

User User::fromJson(const QJsonObject& json) {
    return User{
        json["name"].toString(),
        json["password"].toString(),
        json["balance"].toString().toDouble(),
        json["role"].toString()
    };
}

QJsonObject User::toJson() const {
    QJsonObject obj;
    obj["name"] = name;
    obj["password"] = password;
    obj["balance"] = balance;
    obj["role"] = role;
    return obj;
}

ApprovalRequest ApprovalRequest::fromJson(const QJsonObject& json) {
    return ApprovalRequest{
        json["requester"].toString(),
        json["username"].toString(),
        json["approval_count"].toInt()
    };
}

QJsonObject ApprovalRequest::toJson() const {
    QJsonObject obj;
    obj["requester"] = requester;
    obj["username"] = username;
    obj["approval_count"] = approval_count;
    return obj;
}

FinanceApiClient::FinanceApiClient(const QString& baseUrl, QObject *parent)
    : QObject(parent), m_baseUrl(baseUrl), m_request(new Request())
{
    connect(m_request, &Request::gotHttpData, this, &FinanceApiClient::handleHttpData);
    connect(m_request, &Request::httpFinished, this, &FinanceApiClient::handleHttpFinished);
}

FinanceApiClient::~FinanceApiClient()
{
    m_request->deleteLater();
}

void FinanceApiClient::getUser(const QString& username) {
    QJsonObject body;
    body["user"] = username;
    sendGetRequest("/user", body);
}

void FinanceApiClient::getAllUsers() {
    sendGetRequest("/all_users");
}

void FinanceApiClient::addUser(const QString& name, const QString& password, double balance, const QString& role) {
    QJsonObject body;
    body["name"] = name;
    body["password"] = password;
    body["balance"] = balance;
    body["role"] = role;
    sendPostRequest("/add_user", body);
}

void FinanceApiClient::requestUserDeletion(const QString& requester, const QString& username) {
    QJsonObject body;
    body["requester"] = requester;
    body["username"] = username;
    sendPostRequest("/delete_user", body);
}

void FinanceApiClient::approveDeletion(const QString& admin, const QString& username) {
    QJsonObject body;
    body["admin"] = admin;
    body["username"] = username;
    sendPostRequest("/approve_deletion", body);
}

void FinanceApiClient::transferMoney(const QString& from, const QString& to, double amount) {
    QJsonObject body;
    body["from"] = from;
    body["to"] = to;
    body["amount"] = amount;
    sendPostRequest("/transfer", body);
}

void FinanceApiClient::addUserMoney(const QString& name, double amount) {
    QJsonObject body;
    body["name"] = name;
    body["amount"] = amount;
    sendPostRequest("/add_money", body);
}

void FinanceApiClient::subtractUserMoney(const QString& name, double amount) {
    QJsonObject body;
    body["name"] = name;
    body["amount"] = amount;
    sendPostRequest("/sub_money", body);
}

void FinanceApiClient::getPendingApprovals(const QString& admin) {
    QJsonObject body;
    body["admin"] = admin;
    sendGetRequest("/pending_approvals", body);
}

void FinanceApiClient::sendGetRequest(const QString& path, const QJsonObject& body) {
    QUrl url(m_baseUrl + path);
    QJsonDocument doc(body);
    m_request->onGetRequestWithBody(url, doc.toJson());
}

void FinanceApiClient::sendPostRequest(const QString& path, const QJsonObject& body) {
    QUrl url(m_baseUrl + path);
    QJsonDocument doc(body);
    m_request->onPostRequestWithBody(url, doc.toJson());
}

void FinanceApiClient::handleHttpData(QByteArray data) {
    m_currentResponse.append(data);
}

void FinanceApiClient::handleHttpFinished() {
    QJsonDocument doc = QJsonDocument::fromJson(m_currentResponse);
    if (doc.isNull()) {
        emit errorOccurred(m_currentResponse);
        m_currentResponse.clear();
        return;
    }

    m_currentResponse.clear();

    QJsonObject response = doc.object();
    QString status = response["status"].toString();

    if (status != "success") {
        QString error = response["message"].toString();
        emit errorOccurred(error);
        return;
    }

    if (response.contains("user")) {
        QJsonObject userJson = response["user"].toObject();
        emit userReceived(User::fromJson(userJson));
    }
    else if (response.contains("users")) {
        QJsonArray usersArray = response["users"].toObject()["users"].toArray();
        QList<User> users;
        for (const QJsonValue& value : usersArray) {
            users.append(User::fromJson(value.toObject()));
        }
        emit allUsersReceived(users);
    }
    else if (response.contains("pending")) {
        QJsonArray approvalsArray = response["pending"].toObject()["pending_approvals"].toArray();
        QList<ApprovalRequest> approvals;
        for (const QJsonValue& value : approvalsArray) {
            approvals.append(ApprovalRequest::fromJson(value.toObject()));
        }
        emit pendingApprovalsReceived(approvals);
    }
    else if (response.contains("message")) {
        QString message = response["message"].toString();
        emit operationCompleted(message);
    }
    else {
        emit errorOccurred("Unknown response format");
    }
}
