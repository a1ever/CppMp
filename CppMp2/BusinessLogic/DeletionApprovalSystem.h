//
// Created by alever.
//

#ifndef FINANCEAPP_BUSINESSLOGIC_DELETIONAPPROVALSYSTEM_H_
#define FINANCEAPP_BUSINESSLOGIC_DELETIONAPPROVALSYSTEM_H_

#include <condition_variable>
#include <queue>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

class DeletionApprovalSystem {
public:
    struct DeletionRequest {
        std::string requester;
        std::string username;
        int approval_count = 0;
        bool operator==(const std::string& uname) const { return username == uname; }
    };

    void requestDeletion(const std::string& requester, const std::string& username);

    bool waitForApproval(const std::string& username, std::chrono::seconds timeout);

    void approveDeletion(const std::string& username);

    std::vector<DeletionRequest> getPendingRequests() const;

private:
    const int REQUIRED_APPROVALS = 1;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::unordered_map<std::string, DeletionRequest> requests_map_;
};

#endif //FINANCEAPP_BUSINESSLOGIC_DELETIONAPPROVALSYSTEM_H_
