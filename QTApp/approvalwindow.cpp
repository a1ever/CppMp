#include "approvalwindow.h"

#include <QLineEdit>

ApprovalWindow::ApprovalWindow(FinanceApiClient* apiClient, const QString& adminName, QWidget *parent)
    : QWidget(parent), apiClient(apiClient), adminName(adminName)
{
    setWindowTitle("Approve Deletion Requests");
    approvalsList = new QListWidget();
    approveButton = new QPushButton("Approve Selected");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(approvalsList);
    layout->addWidget(approveButton);
    QLineEdit* searchEdit = new QLineEdit();
    layout->insertWidget(0, searchEdit);

    connect(searchEdit, &QLineEdit::textChanged, this, [=](const QString& text) {
        approvalsList->clear();
        for (const auto& req : currentApprovals) {
            QString displayText = req.username + " requested by " + req.requester;
            if (displayText.contains(text, Qt::CaseInsensitive))
                approvalsList->addItem(displayText);
        }
    });

    connect(approveButton, &QPushButton::clicked, this, &ApprovalWindow::approveSelected);

    connect(apiClient, &FinanceApiClient::pendingApprovalsReceived, this, [=](const QList<ApprovalRequest>& approvals){
        currentApprovals = approvals;
        approvalsList->clear();
        for (const auto& req : approvals) {
            approvalsList->addItem(req.username + " requested by " + req.requester);
        }
    });

    apiClient->getPendingApprovals(adminName);
}

void ApprovalWindow::approveSelected() {
    QList<QListWidgetItem*> selected = approvalsList->selectedItems();
    for (QListWidgetItem* item : selected) {
        QString username = item->text().split(" ").first();
        apiClient->approveDeletion(adminName, username);
        this->close();
    }
}

