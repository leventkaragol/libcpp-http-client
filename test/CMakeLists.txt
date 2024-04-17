cmake_minimum_required(VERSION 3.27)

project(test)

find_package(CURL CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)

add_executable(test test.cpp)

target_link_libraries(test PRIVATE libcpp-http-client CURL::libcurl GTest::gtest GTest::gtest_main nlohmann_json::nlohmann_json)
