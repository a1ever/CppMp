//
// Created by alever.
//

#ifndef FINANCEAPP_BUSINESSLOGIC_EXCEPTIONS_USEREXCEPTION_H_
#define FINANCEAPP_BUSINESSLOGIC_EXCEPTIONS_USEREXCEPTION_H_

#include <stdexcept>
#include <string>

class UserException : public std::exception {
public:
    explicit UserException(const std::string& message)
        : message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

class UserAlreadyExistsException : public UserException {
public:
    explicit UserAlreadyExistsException(const std::string& name)
        : UserException("User '" + name + "' already exists") {}
};

class UserNotFoundException : public UserException {
public:
    explicit UserNotFoundException(const std::string& name)
        : UserException("User '" + name + "' not found") {}
};

class UserBalanceIsLessThanAmount : public UserException {
public:
    explicit UserBalanceIsLessThanAmount(const std::string& name, int balance, int amount)
        : UserException("User " + name + " balance " + std::to_string(balance) + " < amount " + std::to_string(amount)) {}
};

#endif //FINANCEAPP_BUSINESSLOGIC_EXCEPTIONS_USEREXCEPTION_H_
