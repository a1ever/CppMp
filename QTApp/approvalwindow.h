#ifndef APPROVALWINDOW_H
#define APPROVALWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "FinanceApiClient.h"

class ApprovalWindow : public QWidget {
    Q_OBJECT

public:
    explicit ApprovalWindow(FinanceApiClient* apiClient, const QString& adminName, QWidget *parent = nullptr);
private slots:
    void approveSelected();

private:
    FinanceApiClient* apiClient;
    QString adminName;

    QListWidget* approvalsList;
    QPushButton* approveButton;
    QList<ApprovalRequest> currentApprovals;
};

#endif // APPROVALWINDOW_H
