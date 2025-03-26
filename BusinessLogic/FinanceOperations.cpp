//
// Created by alever.
//

#include <iostream>
#include "FinanceOperations.h"
#include "Exceptions/UserException.h"

void FinanceOperations::TransferMoney(const std::string& from, const std::string& to, double amount)  {
    SubtractUserMoney(from, amount);
    AddUserMoney(to, amount);
}

void FinanceOperations::AddUserMoney(const std::string& name, double amount) {
    auto user = GetUser(name);
    repository_.UpdateUser(user.GetName(), user.GetBalance()+amount);
}

void FinanceOperations::SubtractUserMoney(const std::string& name, double amount) {
    auto user = GetUser(name);
    if (user.GetBalance() >= amount) {
        repository_.UpdateUser(name, user.GetBalance()-amount);
        return;
    }
    throw UserBalanceIsLessThanAmount(name, user.GetBalance(), amount);
}

void FinanceOperations::CreateUser(const std::string& name, double base_amount, UserRole role) {
    try {
        repository_.CreateUser(name, base_amount, role);
    } catch (std::exception& e){
        std::cout<<e.what();
        throw UserAlreadyExistsException(name);
    }
}

void FinanceOperations::CreateUser(const std::string& name, double base_amount) {
    CreateUser(name, base_amount, UserRole::USER);
}

void FinanceOperations::CreateAdminUser(const std::string& name, double base_amount) {
    CreateUser(name, base_amount, UserRole::ADMIN);
}

const User& FinanceOperations::GetUser(const std::string& name) {
    try {
        return repository_.GetUser(name);
    } catch (std::exception& e){
        std::cout<<e.what();
        throw UserNotFoundException(name);
    }
}
std::vector<DeletionApprovalSystem::DeletionRequest> FinanceOperations::GetPendingApprovals(const std::string& admin)  {
    try {
        if (repository_.GetUser(admin).GetRole() != UserRole::ADMIN) {
            throw std::runtime_error("Only admins can view pending approvals");
        }
        return approvalSystem_.getPendingRequests();
    } catch (const std::exception& e) {
        std::cout << "Error getting approvals: " << e.what() << "\n";
        throw;
    }
}

void FinanceOperations::ApproveDeletion(const std::string& admin, const std::string& username) {
    try {
        // Проверяем что approving user действительно admin
        if (repository_.GetUser(admin).GetRole() != UserRole::ADMIN) {
            throw std::runtime_error("Only admins can approve deletions");
        }

        approvalSystem_.approveDeletion(username);
        //std::cout << "Admin " << admin << " approved deletion of " << username << "\n";
    } catch (const std::exception& e) {
        //std::cout << "Approval error: " << e.what() << "\n";
        throw;
    }
}

void FinanceOperations::RequestUserDeletion(const std::string& requester, const std::string& username) {
    try {
        // Если пользователь пытается удалить себя
        if (requester == username) {
            approvalSystem_.requestDeletion(requester, username);

            // Ожидаем подтверждения
            if (!approvalSystem_.waitForApproval(username, std::chrono::minutes(5))) {
                throw std::runtime_error("Deletion request timed out");
            }
        }

        // Выполняем удаление
        repository_.DeleteUser(username);
        std::cout << "User " << username << " deleted successfully\n";

    } catch (const std::exception& e) {
        std::cout << "Deletion error: " << e.what() << "\n";
        throw;
    }
}
