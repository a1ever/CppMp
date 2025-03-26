//
// Created by alever.
//

#ifndef CPPMP_BUSINESSLOGIC_DOOPERATIONS_H_
#define CPPMP_BUSINESSLOGIC_DOOPERATIONS_H_

#include <string>
#include <mutex>
#include "../DataAccess/Repository.h"
#include "DeletionApprovalSystem.h"

class FinanceOperations {
public:
    void TransferMoney(const std::string& from, const std::string& to, double amount);

    void AddUserMoney(const std::string& name, double amount);

    void SubtractUserMoney(const std::string& name, double amount);

    void CreateUser(const std::string& name, double base_amount, UserRole role);
    void CreateUser(const std::string& name, double base_amount);

    void CreateAdminUser(const std::string& name, double base_amount);

    const User& GetUser(const std::string& name);

    void RequestUserDeletion(const std::string& requester, const std::string& username);

    void ApproveDeletion(const std::string& admin, const std::string& username);
    std::vector<DeletionApprovalSystem::DeletionRequest> GetPendingApprovals(const std::string& admin);


    explicit FinanceOperations(Repository& repository, DeletionApprovalSystem& approval_system)
        : repository_(repository), approvalSystem_(approval_system) {}
private:
    Repository& repository_;
    DeletionApprovalSystem& approvalSystem_;
};

#endif //CPPMP_BUSINESSLOGIC_DOOPERATIONS_H_
