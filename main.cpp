#include <iostream>
#include "DataAccess/Repository.h"
#include "Presentation/Interface.h"

int main() {
    Repository repository;
    FinanceOperations fm(repository);
    boost::asio::thread_pool pool(2);

    Interface server(fm, pool);

    // Путь к JSON файлу с командами
    std::string jsonFilePath = "commands.json";

    // Запуск сервера
    server.run(jsonFilePath);

    return 0;
}