//
// Created by alever.
//

#include "DeletionApprovalSystem.h"

void DeletionApprovalSystem::requestDeletion(std::string requester, std::string username) {
    std::lock_guard lock(mutex_);
    requests_.push_back({std::move(requester), std::move(username), 0});
    cv_.notify_all();
}

bool DeletionApprovalSystem::waitForApproval(const std::string& username, std::chrono::seconds timeout) {
    std::unique_lock lock(mutex_);
    auto pred = [&] {
        return std::ranges::any_of(requests_, [&](const DeletionRequest& req) {
            return req.username == username && req.approval_count >= REQUIRED_APPROVALS;
        });
    };

    if (!cv_.wait_for(lock, timeout, pred)) {
        return false;
    }

    // Удаляем подтвержденный запрос
    std::erase_if(requests_, [&](const auto& req) {
        return req.username == username;
    });

    return true;
}

void DeletionApprovalSystem::approveDeletion(const std::string& username) {
    std::unique_lock lock(mutex_);
    auto it = std::ranges::find_if(requests_,
                                   [&](const DeletionRequest& req) {
                                       return req.username == username;
                                   });

    if (it != requests_.end()) {
        it->approval_count++;
    }

    lock.unlock();
    cv_.notify_all();
}

std::vector<DeletionApprovalSystem::DeletionRequest> DeletionApprovalSystem::getPendingRequests() const {
    std::lock_guard lock(mutex_);
    return requests_;
}
