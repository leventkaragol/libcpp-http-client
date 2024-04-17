cmake_minimum_required(VERSION 3.27)

project(examples)

add_executable(examples main.cpp)

find_package(CURL CONFIG REQUIRED)

target_link_libraries(examples PRIVATE libcpp-http-client CURL::libcurl)
