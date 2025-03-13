//
// Created by alever.
//

#ifndef CPPMP_PRESENTATION_SERVERINTERFACE_H_
#define CPPMP_PRESENTATION_SERVERINTERFACE_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include "../BusinessLogic/FinanceOperations.h"
#include "../BusinessLogic/Exceptions/UserException.h"

class Interface {
public:
    Interface(FinanceOperations& fm, boost::asio::thread_pool& pool)
        : financeManager(fm), pool(pool) {}

    void processCommand(const boost::property_tree::ptree& pt) {
        auto command = pt.get<std::string>("command");

        if (command == "add_user") {
            auto name = pt.get<std::string>("name");
            auto balance = pt.get<double>("balance");
            financeManager.CreateUser(name, balance);
            std::cout << "User added: " << name << " with balance " << financeManager.GetUser(name).GetBalance() << std::endl;
        } else if (command == "add_user_money") {
            auto name = pt.get<std::string>("name");
            auto amount = pt.get<double>("amount");
            try {
                financeManager.AddUserMoney(name, amount);
                std::cout << "User: " << name << " with new balance " << financeManager.GetUser(name).GetBalance() << std::endl;
            } catch (UserException& e) {
                std::cout << e.what() << std::endl;
            }
        } else if (command == "sub_user_money") {
            auto name = pt.get<std::string>("name");
            auto amount = pt.get<double>("amount");
            try {
                financeManager.SubtractUserMoney(name, amount);
                std::cout << "User: " << name << " with new balance " << amount << std::endl;
            } catch (UserException& e) {
                std::cout << e.what() << std::endl;
            }
        } else if (command == "transfer") {
            auto from = pt.get<std::string>("from");
            auto to = pt.get<std::string>("to");
            auto amount = pt.get<double>("amount");
            try {
                financeManager.TransferMoney(from, to, amount);
                std::cout << "Transfer successful: " << amount << " from " << from << " to " << to << std::endl;
            } catch (UserException& e) {
                std::cout << e.what() << std::endl;
            }
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    void run(const std::string& jsonFilePath) {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(jsonFilePath, pt);

        for (const auto& item : pt) {
            boost::asio::post(pool, [this, item]() {
                this->processCommand(item.second);
            });
        }

        pool.join();
    }

private:
    FinanceOperations& financeManager;
    boost::asio::thread_pool& pool;
};
#endif //CPPMP_PRESENTATION_SERVERINTERFACE_H_
