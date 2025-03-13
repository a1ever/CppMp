//
// Created by alever.
//

#ifndef CPPMP_BUSINESSLOGIC_USER_H_
#define CPPMP_BUSINESSLOGIC_USER_H_

#include <string>

class User {
public:
    User(const std::string& name, double balance)
        : name(name), balance(balance) {}

    const std::string& GetName() const;
    void SetName(const std::string& name);
    double GetBalance() const;
    void SetBalance(double balance);
private:
    std::string name;
    double balance;
};

#endif //CPPMP_BUSINESSLOGIC_USER_H_
