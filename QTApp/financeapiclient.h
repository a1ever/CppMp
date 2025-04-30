#ifndef FINANCEAPICLIENT_H
#define FINANCEAPICLIENT_H

#include <QObject>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include "request.h"

struct User {
    QString name;
    QString password;
    double balance;
    QString role;

    static User fromJson(const QJsonObject& json);
    QJsonObject toJson() const;
};

struct ApprovalRequest {
    QString requester;
    QString username;
    int approval_count;

    static ApprovalRequest fromJson(const QJsonObject& json);
    QJsonObject toJson() const;
};

class FinanceApiClient : public QObject
{
    Q_OBJECT
public:
    Request* m_request;
    explicit FinanceApiClient(const QString& baseUrl, QObject *parent = nullptr);
    ~FinanceApiClient();

    // User operations
    void getUser(const QString& username);
    void getAllUsers();
    void addUser(const QString& name, const QString& password, double balance, const QString& role);
    void requestUserDeletion(const QString& requester, const QString& username);
    void approveDeletion(const QString& admin, const QString& username);

    // Money operations
    void transferMoney(const QString& from, const QString& to, double amount);
    void addUserMoney(const QString& name, double amount);
    void subtractUserMoney(const QString& name, double amount);

    // Approvals
    void getPendingApprovals(const QString& admin);

signals:
    void userReceived(const User& user);
    void allUsersReceived(const QList<User>& users);
    void pendingApprovalsReceived(const QList<ApprovalRequest>& approvals);
    void operationCompleted(const QString& message);
    void errorOccurred(const QString& error);

private slots:
    void handleHttpData(QByteArray data);
    void handleHttpFinished();

private:
    QString m_baseUrl;
    QByteArray m_currentResponse;

    void sendGetRequest(const QString& path, const QJsonObject& body = QJsonObject());
    void sendPostRequest(const QString& path, const QJsonObject& body);
    void processResponse(const QString& path, const QJsonObject& response);
};

#endif // FINANCEAPICLIENT_H
