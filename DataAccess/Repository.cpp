//
// Created by alever.
//

#include "Repository.h"
void Repository::CreateUser(const std::string& name, double balance) {
    std::lock_guard<std::mutex> lock(usersMutex);
    if (users.find(name) != users.end()) {
        throw std::runtime_error("User already defined");
    }
    users.insert({name, User(name, balance)});
}

void Repository::UpdateUser(const std::string& name, double balance) {
    std::lock_guard<std::mutex> lock(usersMutex);
    auto it = users.find(name);
    if (it != users.end()) {
        it->second.SetBalance(balance);
        return;
    }
}

const User& Repository::GetUser(const std::string& name) {
    std::lock_guard<std::mutex> lock(usersMutex);
    auto it = users.find(name);
    if (it != users.end()) {
        return it->second;
    }
    throw std::runtime_error("User not found");
}
