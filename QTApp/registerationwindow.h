#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include "main_window.h"

class RegistrationWindow : public QWidget {
    Q_OBJECT

public:
    explicit RegistrationWindow(FinanceApiClient *client, QWidget *parent = nullptr);

private slots:
    void handleRegistration();
    void onOperationCompleted(const QString &message);
    void onErrorOccurred(const QString &error);

private:
    QWidget* parent;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *balanceEdit;
    QLabel *statusLabel;
    FinanceApiClient *apiClient;
    User newUser;
    MainWindow* mainWindow = nullptr;
};
