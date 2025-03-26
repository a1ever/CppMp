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

        // Создаем администратора
        financeOps.CreateUser("admin", 10000.0);
        // Здесь должен быть метод для назначения роли ADMIN

        // Определяем размер пула потоков
        const unsigned num_threads = std::thread::hardware_concurrency();

        // Создаем io_context и пул потоков
        net::io_context ioc(num_threads);
        net::thread_pool pool(num_threads);

        // Запускаем сервер
        tcp::endpoint endpoint{tcp::v4(), 8080};
        HttpServer server{ioc, endpoint, financeOps};
        server.run();

        for (unsigned i = 0; i < num_threads; ++i) {
            net::post(pool, [&ioc] { ioc.run(); });
        }

        std::cout << "Server started on port 8080 with "
                  << num_threads << " worker threads\n";

        pool.join();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}