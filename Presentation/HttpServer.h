//
// Created by alever.
//

#ifndef FINANCEAPP_PRESENTATION_HTTPSERVER_H_
#define FINANCEAPP_PRESENTATION_HTTPSERVER_H_

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include "../BusinessLogic/FinanceOperations.h"

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

using tcp = net::ip::tcp;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    HttpSession(tcp::socket socket, FinanceOperations& financeOps)
        : socket_(std::move(socket)), financeOps_(financeOps) {}

    void run() {
        logRequestStart();
        readRequest();
    }

private:
    void logRequestStart() {
        std::cout << "[Request] Incoming request\n";
    }

    void logRequestDetails() {
        std::cout << "[Request] "
                  << request_.method_string() << " "
                  << request_.target() << " HTTP/"
                  << (request_.version() / 10) << "."
                  << (request_.version() % 10) << "\n";

        if (!request_.body().empty()) {
            std::cout << "[Request Body] " << request_.body() << "\n";
        }
    }

    void logResponseDetails() {
        std::cout << "[Response] HTTP/"
                  << (response_.version() / 10) << "."
                  << (response_.version() % 10) << " "
                  << response_.result_int() << " "
                  << response_.reason() << "\n";

        if (!response_.body().empty()) {
            std::cout << "[Response Body] " << response_.body() << "\n";
        }
    }

    void readRequest() {
        auto self = shared_from_this();
        http::async_read(socket_, buffer_, request_,
                         [self](beast::error_code ec, size_t bytes) {
                             if (!ec) {
                                 self->logRequestDetails();
                             }
                             self->handleRead(ec, bytes);
                         });
    }

    void writeResponse() {
        auto self = shared_from_this();
        logResponseDetails();

        http::async_write(socket_, response_,
                          [self](beast::error_code ec, size_t) {
                              self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                          });
    }

    void handleRead(beast::error_code ec, size_t) {
        if (ec == http::error::end_of_stream) {
            socket_.shutdown(tcp::socket::shutdown_send, ec);
            return;
        }
        if (ec) return;

        processRequest();
    }

    void processRequest() {
        response_.version(request_.version());
        response_.keep_alive(request_.keep_alive());

        try {
            if (request_.method() == http::verb::post) {
                handlePostRequest();
            } else {
                throw std::runtime_error("Unsupported HTTP method");
            }
        } catch (const std::exception& e) {
            response_.result(http::status::bad_request);
            response_.set(http::field::content_type, "text/plain");
            response_.body() = "Error: " + std::string(e.what()); // Исправлено здесь
            response_.prepare_payload();
        }

        writeResponse();
    }

    void handlePostRequest() {
        auto path = request_.target();
        auto body = request_.body();

        if (path == "/transfer") {
            handleTransfer(body);
        } else if (path == "/add_user") {
            handleAddUser(body);
        } else if (path == "/delete_user") {
            handleDeleteUser(body);
        } else if (path == "/approve_deletion") {
            handleApproveDeletion(body);
        } else {
            throw std::runtime_error("Unknown endpoint");
        }
    }

    void handleTransfer(const std::string& body) {
        auto value = json::parse(body);
        financeOps_.TransferMoney(
            value.at("from").as_string().c_str(),
            value.at("to").as_string().c_str(),
            value.at("amount").as_double());

        createSuccessResponse("Transfer completed");
    }

    void handleAddUser(const std::string& body) {
        auto value = json::parse(body);
        financeOps_.CreateUser(
            value.at("name").as_string().c_str(),
            value.at("balance").as_double());

        createSuccessResponse("User created");
    }

    void handleDeleteUser(const std::string& body) {
        auto value = json::parse(body);
        financeOps_.RequestUserDeletion(
            value.at("requester").as_string().c_str(),
            value.at("username").as_string().c_str());

        createSuccessResponse("Deletion requested");
    }

    void handleApproveDeletion(const std::string& body) {
        auto value = json::parse(body);
        financeOps_.ApproveDeletion(
            value.at("admin").as_string().c_str(),
            value.at("username").as_string().c_str());

        createSuccessResponse("Deletion approved");
    }

    void createSuccessResponse(const std::string& message) {
        response_.result(http::status::ok);
        response_.set(http::field::content_type, "application/json");
        response_.body() = json::serialize(json::value{
            {"status", "success"},
            {"message", message}
        });
        response_.prepare_payload();
    }

    tcp::socket socket_;
    beast::flat_buffer buffer_{8192};
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    FinanceOperations& financeOps_;
};

class HttpServer {
public:
    HttpServer(net::io_context& ioc, tcp::endpoint endpoint, FinanceOperations& financeOps)
        : acceptor_(ioc), financeOps_(financeOps) {
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(net::socket_base::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen(net::socket_base::max_listen_connections);
    }

    void run() {
        doAccept();
    }

private:
    void doAccept() {
        acceptor_.async_accept(
            [this](beast::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<HttpSession>(std::move(socket), financeOps_)->run();
                }
                doAccept();
            });
    }

    tcp::acceptor acceptor_;
    FinanceOperations& financeOps_;
};

#endif //FINANCEAPP_PRESENTATION_HTTPSERVER_H_
