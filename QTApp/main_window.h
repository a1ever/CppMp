#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <QPointer>
#include "approvalWindow.h"

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(FinanceApiClient* apiClient, const User& user, QWidget *parent = nullptr);

private slots:
    void openTransferWindow();
    void topUpBalance();
    void withdrawMoney();
    void requestAccountDeletion();
    void updatePendingApprovals(const QList<ApprovalRequest>& approvals);
    void updateUserInfo(const User& updatedUser);

private:
    QWidget* parent;
    FinanceApiClient* apiClient;
    User currentUser;

    QLabel* nameLabel;
    QLabel* balanceLabel;
    QLabel* roleLabel;

    QPushButton* logoutButton;
    QPushButton* transferButton;
    QPushButton* topUpButton;
    QPushButton* withdrawButton;
    QPushButton* deleteAccountButton;
    QPointer<ApprovalWindow> aw;

    QListWidget* approvalsList;
    QTimer* refreshTimer;
    void waitAndRefreshUser();
    void handleApiError(const QString &error);
    void logout();
};

#endif // MAINWINDOW_H
