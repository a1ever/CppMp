cmake_minimum_required(VERSION 3.15)
project(FinanceApp)

# Include Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

include_directories("C:/Program Files/Boost")

# Main application executable
add_executable(FinanceApp main.cpp)

# Library for your project
add_library(cppMp
        Models/User.h
        Models/User.cpp
        DataAccess/Repository.h
        Presentation/Interface.h
        BusinessLogic/FinanceOperations.h
        BusinessLogic/Exceptions/UserException.h
        DataAccess/Repository.cpp
        Presentation/Interface.cpp
        BusinessLogic/FinanceOperations.cpp)

# Link the main application to the library
target_link_libraries(FinanceApp PRIVATE cppMp)
target_include_directories(FinanceApp PRIVATE .)

# Test executable
add_executable(FinanceOperationsTest
        tests/FinanceOperationsTest.cpp)

# Link the test executable to the library and Google Test
target_link_libraries(FinanceOperationsTest PRIVATE cppMp gtest_main)
target_include_directories(FinanceOperationsTest PRIVATE .)

# Add a test target
enable_testing()
add_test(NAME FinanceOperationsTest COMMAND FinanceOperationsTest)