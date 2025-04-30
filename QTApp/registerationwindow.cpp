#include "registerationwindow.h"
#include <QVBoxLayout>

RegistrationWindow::RegistrationWindow(FinanceApiClient *client, QWidget *parent) : QWidget(parent), apiClient(client) {
    setWindowTitle("Register");
    this->parent = parent;
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Username");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *registerButton = new QPushButton("Register", this);
    statusLabel = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(usernameEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(registerButton);
    layout->addWidget(statusLabel);
    setLayout(layout);

    connect(registerButton, &QPushButton::clicked, this, &RegistrationWindow::handleRegistration);
    connect(apiClient, &FinanceApiClient::operationCompleted, this, &RegistrationWindow::onOperationCompleted);
    connect(apiClient, &FinanceApiClient::errorOccurred, this, &RegistrationWindow::onErrorOccurred);
}

void RegistrationWindow::handleRegistration() {
    const QString username = usernameEdit->text();
    const QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText("Fill all fields correctly.");
        return;
    }

    apiClient->addUser(username, password, 0.0, "user");
}

void RegistrationWindow::onOperationCompleted(const QString &message) {
    statusLabel->setText("Success: " + message);

    newUser.name = usernameEdit->text();
    newUser.password = passwordEdit->text();
    newUser.balance = 0.0;
    newUser.role = "USER";
    if (mainWindow == nullptr)
        mainWindow = new MainWindow(apiClient, newUser);
    mainWindow->show();
    this->close();
}

void RegistrationWindow::onErrorOccurred(const QString &error) {
    statusLabel->setText("Error: " + error);
}
