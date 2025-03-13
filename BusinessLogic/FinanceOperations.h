//
// Created by alever.
//

#ifndef CPPMP_BUSINESSLOGIC_DOOPERATIONS_H_
#define CPPMP_BUSINESSLOGIC_DOOPERATIONS_H_

#include <string>
#include <mutex>
#include "../DataAccess/Repository.h"

class FinanceOperations {
public:
    void TransferMoney(const std::string& from, const std::string& to, double amount);

    void AddUserMoney(const std::string& name, double amount);

    void SubtractUserMoney(const std::string& name, double amount);

    void CreateUser(const std::string& name, double base_amount);

    const User& GetUser(const std::string& name);

    explicit FinanceOperations(Repository& repository) : repository_(repository) {}
private:
    Repository& repository_;
};

#endif //CPPMP_BUSINESSLOGIC_DOOPERATIONS_H_
