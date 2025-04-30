//
// Created by alever.
//

#ifndef CPPMP_BUSINESSLOGIC_USER_H_
#define CPPMP_BUSINESSLOGIC_USER_H_

#include <string>
#include "UserRole.h"

class User {
public:
    User(const std::string& name, const std::string& password, double balance, UserRole userRole)
        : name(name), password(password), balance(balance), role(userRole) { }

    const std::string& GetName() const;
    void SetName(const std::string& name);
    const std::string& GetPassword() const;
    void SetPassword(const std::string& password);
    double GetBalance() const;
    void SetBalance(double balance);
    UserRole GetRole() const;
    void SetRole(const UserRole& user_role);
private:
    std::string name;
    std::string password;
    double balance;
    UserRole role;
};

#endif //CPPMP_BUSINESSLOGIC_USER_H_
