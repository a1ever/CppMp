#include "approvalWindow.h"
#include "main_window.h"
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QCoreApplication>

MainWindow::MainWindow(FinanceApiClient* apiClient, const User& user, QWidget *parent)
    : QWidget(parent), apiClient(apiClient), currentUser(user)
{
    this->parent = parent;
    setWindowTitle("Finances");

    nameLabel = new QLabel(user.name);
    balanceLabel = new QLabel("$ " + QString::number(user.balance, 'f', 2));
    roleLabel = new QLabel(user.role);


    QHBoxLayout* infoLayout = new QHBoxLayout();
    infoLayout->addWidget(nameLabel, 1);
    infoLayout->addWidget(balanceLabel, 1);
    infoLayout->addWidget(roleLabel, 1);
    infoLayout->addStretch();

    transferButton = new QPushButton("Transfer Money");
    topUpButton = new QPushButton("➕ Top Up");
    withdrawButton = new QPushButton("➖ Withdraw");
    deleteAccountButton = new QPushButton("Request Deletion");

    QHBoxLayout* moneyLayout = new QHBoxLayout();
    moneyLayout->addWidget(topUpButton, 1);
    moneyLayout->addWidget(withdrawButton, 1);
    moneyLayout->addStretch();

    approvalsList = new QListWidget();
    approvalsList->setVisible(user.role == "ADMIN");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(infoLayout);
    layout->addWidget(transferButton);
    layout->addLayout(moneyLayout);
    layout->addWidget(deleteAccountButton);

    if (user.role == "ADMIN") {
        QLabel* approvalsLabel = new QLabel("Pending Deletion Requests:");
        layout->addWidget(approvalsLabel);
        layout->addWidget(approvalsList);

        QPushButton* approvalsButton = new QPushButton("Open Approvals");
        layout->addWidget(approvalsButton);

        connect(approvalsButton, &QPushButton::clicked, this, [=]() {
            if (!aw)
                aw = new ApprovalWindow(apiClient, currentUser.name);
            aw->show();
        });

        refreshTimer = new QTimer(this);
        connect(refreshTimer, &QTimer::timeout, this, [=]() {
            if (!aw || !aw->isVisible())
                apiClient->getPendingApprovals(currentUser.name);
        });
        refreshTimer->start(5000);
    }

    // 🔌 Connections
    connect(transferButton, &QPushButton::clicked, this, &MainWindow::openTransferWindow);
    connect(topUpButton, &QPushButton::clicked, this, &MainWindow::topUpBalance);
    connect(withdrawButton, &QPushButton::clicked, this, &MainWindow::withdrawMoney);
    connect(deleteAccountButton, &QPushButton::clicked, this, &MainWindow::requestAccountDeletion);
    connect(apiClient, &FinanceApiClient::userReceived, this, &MainWindow::updateUserInfo);
    connect(apiClient, &FinanceApiClient::pendingApprovalsReceived, this, &MainWindow::updatePendingApprovals);
    connect(apiClient, &FinanceApiClient::errorOccurred, this, &MainWindow::handleApiError);


}


void MainWindow::handleApiError(const QString& error) {
    if (error.contains("User", Qt::CaseInsensitive) && error.contains("not found", Qt::CaseInsensitive)) {
        QMessageBox::critical(this, "Fatal Error", "User not found. The application will now exit.");
        QCoreApplication::quit();
    } else {
        QMessageBox::warning(this, "Error", error);
    }
}

void MainWindow::updateUserInfo(const User& updatedUser) {
    if (updatedUser.name == currentUser.name) {
        currentUser = updatedUser;
        balanceLabel->setText("$ " + QString::number(updatedUser.balance, 'f', 2));
    }
}

void MainWindow::openTransferWindow() {
    apiClient->getAllUsers();

    connect(apiClient, &FinanceApiClient::allUsersReceived, this, [=](const QList<User>& users) {
        QDialog dialog(this);
        dialog.setWindowTitle("Select Recipient");

        QVBoxLayout* layout = new QVBoxLayout(&dialog);
        QLineEdit* searchEdit = new QLineEdit();
        QListWidget* listWidget = new QListWidget();
        QPushButton* okButton = new QPushButton("OK");

        layout->addWidget(new QLabel("Search:"));
        layout->addWidget(searchEdit);
        layout->addWidget(listWidget);
        layout->addWidget(okButton);

        QStringList names;
        for (const User& u : users)
            if (u.name != currentUser.name)
                names << u.name;

        listWidget->addItems(names);

        connect(searchEdit, &QLineEdit::textChanged, listWidget, [=](const QString& text) {
            listWidget->clear();
            for (const QString& name : names) {
                if (name.contains(text, Qt::CaseInsensitive))
                    listWidget->addItem(name);
            }
        });

        QString selected;
        connect(okButton, &QPushButton::clicked, &dialog, [&]() {
            QListWidgetItem* item = listWidget->currentItem();
            if (item) {
                selected = item->text();
                dialog.accept();
            }
        });

        if (dialog.exec() == QDialog::Accepted && !selected.isEmpty()) {
            bool ok;
            double amount = QInputDialog::getDouble(this, "Amount", "Enter amount to transfer:", 0, 0, 1e9, 2, &ok);
            if (ok) {
                apiClient->transferMoney(currentUser.name, selected, amount);
                waitAndRefreshUser();
            }
        }
    });
}

void MainWindow::topUpBalance() {
    bool ok;
    double amount = QInputDialog::getDouble(this, "Top Up", "Enter amount to top up:", 0, 0, 1e9, 2, &ok);
    if (ok) {
        apiClient->addUserMoney(currentUser.name, amount);
        waitAndRefreshUser();
    }
}

void MainWindow::withdrawMoney() {
    bool ok;
    double amount = QInputDialog::getDouble(this, "Withdraw", "Enter amount to withdraw:", 0, 0, 1e9, 2, &ok);
    if (ok) {
        apiClient->subtractUserMoney(currentUser.name, amount);
        waitAndRefreshUser();
    }
}

void MainWindow::requestAccountDeletion() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm", "Send deletion request?");
    if (reply == QMessageBox::Yes) {
        apiClient->requestUserDeletion(currentUser.name, currentUser.name);
        waitAndRefreshUser();
    }
}

void MainWindow::waitAndRefreshUser() {
    connect(apiClient, &FinanceApiClient::operationCompleted, this, [=](const QString&) {
        apiClient->getUser(currentUser.name);
    }, Qt::SingleShotConnection);
}


void MainWindow::updatePendingApprovals(const QList<ApprovalRequest>& approvals) {
    approvalsList->clear();
    for (const auto& a : approvals) {
        approvalsList->addItem(a.username + " requested by " + a.requester + " (" + QString::number(a.approval_count) + ")");
    }
}
