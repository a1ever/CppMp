//
// Created by alever.
//

#ifndef CPPMP_BUSINESSLOGIC_USER_H_
#define CPPMP_BUSINESSLOGIC_USER_H_

#include <string>
#include "UserRole.h"

class User {
public:
    User(const std::string& name, double balance, UserRole userRole)
        : name(name), balance(balance), role(userRole) {}

    const std::string& GetName() const;
    void SetName(const std::string& name);
    double GetBalance() const;
    void SetBalance(double balance);
    UserRole GetRole() const;
    void SetRole(const UserRole& user_role);
private:
    std::string name;
    double balance;
    UserRole role;
};

#endif //CPPMP_BUSINESSLOGIC_USER_H_
