//
// Created by alever.
//

#ifndef FINANCEAPP_MODELS_USERROLE_H_
#define FINANCEAPP_MODELS_USERROLE_H_

#include <string>
#include <optional>

enum class UserRole {
    USER,    // Обычный пользователь
    ADMIN    // Администратор рассматривающий удаления
};

inline std::string userRoleToString(UserRole role) {
    switch(role) {
        case UserRole::USER: return "USER";
        case UserRole::ADMIN: return "ADMIN";
        default: return "UNKNOWN";
    }
}

inline std::optional<UserRole> userRoleFromString(const std::string& str) {
    if (str == "USER") return UserRole::USER;
    if (str == "ADMIN") return UserRole::ADMIN;
    return std::nullopt;
}

#endif //FINANCEAPP_MODELS_USERROLE_H_
