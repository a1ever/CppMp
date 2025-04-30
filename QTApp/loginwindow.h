#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "main_window.h"
#include "registerationwindow.h"

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(FinanceApiClient* api, QWidget *parent = nullptr);

private slots:
    void handleLogin();
    void openRegistration();
    void onUserReceived(User user);
    void onErrorOccurred(const QString &error);

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLabel *statusLabel;
    FinanceApiClient *apiClient;
    QString currentUserName;

    MainWindow* mainWindow = nullptr;
    RegistrationWindow* regWindow = nullptr;
};
