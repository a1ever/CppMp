//
// Created by alever.
//

#include "Repository.h"
void Repository::CreateUser(const std::string& name, const std::string& password, double balance, UserRole role) {
    std::unique_lock<std::shared_mutex> lock(usersMutex);
    if (users.find(name) != users.end()) {
        throw std::runtime_error("User already defined");
    }
    users.insert({name, User(name, password, balance, role)});
}

void Repository::UpdateUser(const std::string& name, double balance) {
    std::unique_lock<std::shared_mutex> lock(usersMutex);
    auto it = users.find(name);
    if (it != users.end()) {
        it->second.SetBalance(balance);
        return;
    }
}

const User& Repository::GetUser(const std::string& name) {
    std::shared_lock<std::shared_mutex> lock(usersMutex);
    auto it = users.find(name);
    if (it != users.end()) {
        return it->second;
    }
    throw std::runtime_error("User not found");
}

std::vector<User> Repository::GetAllUsers()
{
    std::shared_lock<std::shared_mutex> lock(usersMutex);
    std::vector<User> ans;
    ans.reserve(users.size());
    for (auto& [key, el] : users) {
        ans.push_back(el);
    }
    return ans;
}

void Repository::DeleteUser(const std::string& username)  {
    std::unique_lock<std::shared_mutex> lock(usersMutex);
    if (users.erase(username) == 0) {
        throw std::runtime_error("User not found");
    }
}