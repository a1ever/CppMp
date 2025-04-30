cmake_minimum_required(VERSION 3.16)

project(FinanceApiClientTest LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Network)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(FinanceApiClientTest tst_financeapiclienttest.cpp
    financeapiclient.cpp financeapiclient.h request.cpp request.h)
add_test(NAME FinanceApiClientTest COMMAND FinanceApiClientTest)

target_link_libraries(FinanceApiClientTest PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Network)

