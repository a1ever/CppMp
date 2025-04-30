//
// Created by alever.
//

#include <gtest/gtest.h>
#include "../BusinessLogic/FinanceOperations.h"
#include "../BusinessLogic/Exceptions/UserException.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class FinanceOperationsTest : public ::testing::Test {
protected:
    FinanceOperationsTest() : operations(repository_, <#initializer#>) {
    }

    void SetUp() override {
        operations.CreateUser("Alice", 1000);
        operations.CreateUser("Bob", 500);
    }
    Repository repository_;
    FinanceOperations operations;
};

// Тест на успешный перевод денег
TEST_F(FinanceOperationsTest, TransferMoneySuccess) {
    EXPECT_NO_THROW(operations.TransferMoney("Alice", "Bob", 200));
    EXPECT_DOUBLE_EQ(operations.GetUser("Alice").GetBalance(), 800);
    EXPECT_DOUBLE_EQ(operations.GetUser("Bob").GetBalance(), 700);
}

// Тест на попытку перевода больше, чем есть на счету
TEST_F(FinanceOperationsTest, TransferMoneyInsufficientFunds) {
    EXPECT_THROW(operations.TransferMoney("Alice", "Bob", 1500), UserBalanceIsLessThanAmount);
}

// Тест на создание пользователя
TEST_F(FinanceOperationsTest, CreateUserSuccess) {
    EXPECT_NO_THROW(operations.CreateUser("Charlie", 300));
    EXPECT_DOUBLE_EQ(operations.GetUser("Charlie").GetBalance(), 300);
}

// Тест на попытку создания уже существующего пользователя
TEST_F(FinanceOperationsTest, CreateUserAlreadyExists) {
    EXPECT_THROW(operations.CreateUser("Alice", 200), UserAlreadyExistsException);
}

// Тест на получение несуществующего пользователя
TEST_F(FinanceOperationsTest, GetUserNotFound) {
    EXPECT_THROW(operations.GetUser("Unknown"), UserNotFoundException);
}

// Тест на добавление денег пользователю
TEST_F(FinanceOperationsTest, AddUserMoneySuccess) {
    EXPECT_NO_THROW(operations.AddUserMoney("Alice", 200));
    EXPECT_DOUBLE_EQ(operations.GetUser("Alice").GetBalance(), 1200);
}

// Тест на списание денег с пользователя
TEST_F(FinanceOperationsTest, SubtractUserMoneySuccess) {
    EXPECT_NO_THROW(operations.SubtractUserMoney("Alice", 200));
    EXPECT_DOUBLE_EQ(operations.GetUser("Alice").GetBalance(), 800);
}

// Тест на попытку списания больше, чем есть на счету
TEST_F(FinanceOperationsTest, SubtractUserMoneyInsufficientFunds) {
    EXPECT_THROW(operations.SubtractUserMoney("Alice", 1500), UserBalanceIsLessThanAmount);
}