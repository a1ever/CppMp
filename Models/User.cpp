//
// Created by alever.
//

#include "User.h"

void User::SetBalance(double balance)  {
    User::balance = balance;
}
double User::GetBalance() const {
    return balance;
}
void User::SetName(const std::string& name) {
    User::name = name;
}
const std::string& User::GetName() const {
    return name;
}
