//
// Created by alever.
//

#ifndef FINANCEAPP_PRESENTATION_HTTPSERVER_H_
#define FINANCEAPP_PRESENTATION_HTTPSERVER_H_

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "../BusinessLogic/FinanceOperations.h"

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = net::ip::tcp;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    HttpSession(tcp::socket socket, FinanceOperations& financeOps)
        : socket_(std::move(socket)), financeOps_(financeOps) {
    }

    void run() {
        std::cout << "[Request] Incoming request\n";
        readRequest();
    }

private:
    void readRequest() {
        auto self = shared_from_this();
        http::async_read(socket_, buffer_, request_,
            [self, this](beast::error_code ec, size_t bytes) {
                if (!ec) {
                    std::cout << "[Request] "
                        << request_.method_string() << "\n";

                    if (!request_.body().empty()) {
                        std::cout << "[Request Body] " << request_.body() << "\n";
                    }
                }
                self->handleRead(ec, bytes);
            });
    }

    void writeResponse() {
        auto self = shared_from_this();

        std::cout << "[Response]"
            << response_.result_int() << " "
            << response_.reason() << "\n";

        if (!response_.body().empty()) {
            std::cout << "[Response Body] " << response_.body() << "\n";
        }

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

        response_.version(request_.version());
        response_.keep_alive(request_.keep_alive());

        try {
            if (request_.method() == http::verb::post) {
                handlePostRequest();
            }
            else if (request_.method() == http::verb::get) {
                handleGetRequest();
            }
            else {
                throw std::runtime_error("Unsupported HTTP method");
            }
        }
        catch (const std::exception& e) {
            response_.result(http::status::bad_request);
            response_.set(http::field::content_type, "text/plain");
            response_.body() = "Error: " + std::string(e.what());
            response_.prepare_payload();
        }

        writeResponse();
    }


    void handlePostRequest() {
        auto path = request_.target();
        auto body = request_.body();
        if (path == "/transfer") {
            handleTransfer(body);
        }
        else if (path == "/add_user") {
            handleAddUser(body);
        }
        else if (path == "/add_money") {
            handleAddUserMoney(body);
        }
        else if (path == "/sub_money") {
            handleSubUserMoney(body);
        }
        else if (path == "/delete_user") {
            handleDeleteUser(body);
        }
        else if (path == "/approve_deletion") {
            handleApproveDeletion(body);
        }
        //поиск по пользователям
        else {
            throw std::runtime_error("Unknown endpoint");
        }
    }

    void handleGetRequest() {
        auto path = request_.target();
        auto body = request_.body();

        if (path == "/user") {
            handleGetUser(body);
        }
        else if (path == "/pending_approvals") {
            handleGetPendingApprovals(body);
        } else if (path == "/all_users") {
            handleAllUsers(body);
        }
        else {
            throw std::runtime_error("Unknown endpoint");
        }
    }


    void handleTransfer(const std::string& body) {
        std::istringstream iss(body);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(iss, pt);

        financeOps_.TransferMoney(
            pt.get<std::string>("from").c_str(),
            pt.get<std::string>("to").c_str(),
            pt.get<double>("amount"));

        createSuccessResponse("Transfer completed");
    }

    void handleAddUserMoney(const std::string& body) {
        std::istringstream iss(body);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(iss, pt);

        financeOps_.AddUserMoney(
            pt.get<std::string>("name").c_str(),
            pt.get<double>("amount"));

        createSuccessResponse("User money added");
    }

    void handleSubUserMoney(const std::string& body) {
        std::istringstream iss(body);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(iss, pt);

        financeOps_.SubtractUserMoney(
            pt.get<std::string>("name").c_str(),
            pt.get<double>("amount"));

        createSuccessResponse("User money added");
    }

    void handleAddUser(const std::string& body) {
        std::istringstream iss(body);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(iss, pt);

        financeOps_.CreateUser(
            pt.get<std::string>("name").c_str(),
            pt.get<std::string>("password").c_str(),
            pt.get<double>("balance"));

        createSuccessResponse("User created");
    }

    void handleDeleteUser(const std::string& body) {
        std::istringstream iss(body);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(iss, pt);

        financeOps_.RequestUserDeletion(
            pt.get<std::string>("requester").c_str(),
            pt.get<std::string>("username").c_str());

        createSuccessResponse("Deletion requested");
    }

    void handleApproveDeletion(const std::string& body) {
        std::istringstream iss(body);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(iss, pt);

        financeOps_.ApproveDeletion(
            pt.get<std::string>("admin").c_str(),
            pt.get<std::string>("username").c_str());

        createSuccessResponse("Deletion approved");
    }


    void handleGetUser(const std::string& body) {
        std::istringstream issread(body);
        boost::property_tree::ptree pt_body;
        boost::property_tree::read_json(issread, pt_body);

        const User& user = financeOps_.GetUser(pt_body.get<std::string>("user").c_str());

        boost::property_tree::ptree pt;
        pt.put("name", user.GetName());
        pt.put("balance", user.GetBalance());
        pt.put("password", user.GetPassword());
        pt.put("role", userRoleToString(user.GetRole()));

        createSuccessResponse("user", pt);
    }

    void handleGetPendingApprovals(const std::string& body) {
        std::istringstream issread(body);
        boost::property_tree::ptree pt_body;
        boost::property_tree::read_json(issread, pt_body);

        auto approvals = financeOps_.GetPendingApprovals(pt_body.get<std::string>("admin").c_str());

        boost::property_tree::ptree pt;
        boost::property_tree::ptree approvals_array;

        for (const auto& req : approvals) {
            boost::property_tree::ptree approval_pt;
            approval_pt.put("requester", req.requester);
            approval_pt.put("username", req.username);
            approval_pt.put("approval_count", req.approval_count);
            approvals_array.push_back(std::make_pair("", approval_pt));
        }

        pt.add_child("pending_approvals", approvals_array);

        createSuccessResponse("pending", pt);
    }

    void handleAllUsers(const std::string& body) {
        auto users = financeOps_.GetAllUsers();

        boost::property_tree::ptree pt;
        boost::property_tree::ptree users_array;

        for (const auto& user : users) {
            boost::property_tree::ptree user_pt;
            user_pt.put("name", user.GetName());
            user_pt.put("balance", user.GetBalance());
            user_pt.put("role", userRoleToString(user.GetRole()));
            users_array.push_back(std::make_pair("", user_pt));
        }

        pt.add_child("users", users_array);

        createSuccessResponse("users", pt);
    }

    void createSuccessResponse(const std::string& message) {
        boost::property_tree::ptree pt;
        pt.put("status", "success");
        pt.put("message", message);

        std::ostringstream oss;
        boost::property_tree::write_json(oss, pt);

        response_.result(http::status::ok);
        response_.set(http::field::content_type, "application/json");
        response_.body() = oss.str();
        response_.prepare_payload();
    }

    void createSuccessResponse(const std::string& type, const boost::property_tree::ptree& json_response) {
        boost::property_tree::ptree pt;
        pt.put("status", "success");
        pt.add_child(type, json_response);

        std::ostringstream oss;
        boost::property_tree::write_json(oss, pt);

        response_.result(http::status::ok);
        response_.set(http::field::content_type, "application/json");
        response_.body() = oss.str();
        response_.prepare_payload();
    }

    tcp::socket socket_;
    beast::flat_buffer buffer_{ 8192 };
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
