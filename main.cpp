#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include "DataAccess/Repository.h"
#include "Presentation/HttpServer.h"

int main() {
    try {
        // Инициализация бизнес-логики
        Repository repo;
        DeletionApprovalSystem approvalSystem;
        FinanceOperations financeOps(repo, approvalSystem);

        financeOps.CreateAdminUser("admin", "123", 10000.0);

        net::io_context ioc;

        tcp::endpoint endpoint{tcp::v4(), 8080};
        HttpServer server{ioc, endpoint, financeOps};
        server.run();

        std::vector<std::thread> threads;
        for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
            threads.emplace_back([&ioc] { ioc.run(); });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}