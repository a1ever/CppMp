//
// Created by alever.
//


#ifndef CPPMP_DATAACCESS_REPOSITORY_H_
#define CPPMP_DATAACCESS_REPOSITORY_H_

#include <unordered_map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include "../Models/User.h"

class Repository {
public:
    void CreateUser(const std::string& name, double balance);

    void UpdateUser(const std::string& name, double balance);

    const User& GetUser(const std::string& name);

private:
    std::unordered_map<std::string, User> users;
    mutable std::mutex usersMutex;
};

#endif //CPPMP_DATAACCESS_REPOSITORY_H_
