cmake_minimum_required(VERSION 3.14)

project(libcpp-http-client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_library(libcpp-http-client INTERFACE)

target_include_directories(libcpp-http-client INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(examples)
add_subdirectory(test)