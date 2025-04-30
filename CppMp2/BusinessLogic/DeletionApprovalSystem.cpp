//
// Created by alever.
//

#include "DeletionApprovalSystem.h"

void DeletionApprovalSystem::requestDeletion(const std::string& requester, const std::string& username) {
    std::lock_guard lock(mutex_);

    if (requests_map_.find(username) != requests_map_.end()) {
        throw std::runtime_error("Deletion request for username '" + username + "' already exists");
    }

    requests_map_[username] = { requester, username, 0 };
}

bool DeletionApprovalSystem::waitForApproval(const std::string& username, std::chrono::seconds timeout) {
    std::unique_lock lock(mutex_);
    auto pred = [&] {
        auto it = requests_map_.find(username);
        return it != requests_map_.end() && it->second.approval_count >= REQUIRED_APPROVALS;
        };

    if (!cv_.wait_for(lock, timeout, pred)) {
        requests_map_.erase(username);
        return false;
    }

    requests_map_.erase(username);
    return true;
}

void DeletionApprovalSystem::approveDeletion(const std::string& username) {
    std::unique_lock lock(mutex_);
    auto it = requests_map_.find(username);
    if (it != requests_map_.end()) {
        it->second.approval_count++;
    }

    lock.unlock();
    cv_.notify_all();
}

std::vector<DeletionApprovalSystem::DeletionRequest> DeletionApprovalSystem::getPendingRequests() const {
    std::lock_guard lock(mutex_);
    std::vector<DeletionRequest> result;
    result.reserve(requests_map_.size());
    for (const auto& [_, request] : requests_map_) {
        result.push_back(request);
    }
    return result;
}
