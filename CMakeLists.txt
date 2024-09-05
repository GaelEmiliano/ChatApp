# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(ChatApp)

# Enable C++11 standard
set(CMAKE_CXX_STANDARD 11)

# Add the client executable
add_executable(client_executable client/src/main_client.cpp)

# Add the server executable
add_executable(server_executable server/src/main_server.cpp)
