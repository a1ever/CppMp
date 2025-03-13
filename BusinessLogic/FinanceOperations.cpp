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

void FinanceOperations::CreateUser(const std::string& name, double base_amount) {
    try {
        repository_.CreateUser(name, base_amount);
    } catch (std::exception& e){
        std::cout<<e.what();
        throw UserAlreadyExistsException(name);
    }
}

const User& FinanceOperations::GetUser(const std::string& name) {
    try {
        return repository_.GetUser(name);
    } catch (std::exception& e){
        std::cout<<e.what();
        throw UserNotFoundException(name);
    }
}
