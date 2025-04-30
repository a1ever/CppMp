#include "loginwindow.h"
#include "registerationwindow.h"
#include <QVBoxLayout>

LoginWindow::LoginWindow(FinanceApiClient* api, QWidget *parent) : QWidget(parent) {
    setWindowTitle("Login");

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Username");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *loginButton = new QPushButton("Login", this);
    QPushButton *registerButton = new QPushButton("Register", this);
    statusLabel = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(usernameEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    layout->addWidget(statusLabel);
    setLayout(layout);

    apiClient =api;
    connect(apiClient, &FinanceApiClient::userReceived, this, &LoginWindow::onUserReceived);
    connect(apiClient, &FinanceApiClient::errorOccurred, this, &LoginWindow::onErrorOccurred);
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::openRegistration);
}

void LoginWindow::handleLogin() {
    const QString username = usernameEdit->text();
    const QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText("Please fill in all fields.");
        return;
    }

    apiClient->getUser(username);
}

void LoginWindow::onUserReceived(User user) {
    if (user.password == passwordEdit->text()) {
        currentUserName = user.name;
        if (mainWindow == nullptr)
            mainWindow = new MainWindow(apiClient, user);
        mainWindow->show();
        this->close();
    } else {
        statusLabel->setText("Invalid password.");
    }
}

void LoginWindow::onErrorOccurred(const QString &error) {
    statusLabel->setText("Error: " + error);
}

void LoginWindow::openRegistration() {
    regWindow = new RegistrationWindow(apiClient);
    regWindow->show();
    this->close();
}
