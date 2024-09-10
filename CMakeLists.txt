# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(ChatApp)

# Enable C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directories from the client and the server
include_directories(server/include client/include common)

# Add the client executable
add_executable(client_executable
    client/src/main_client.cpp
    client/src/client.cpp
)

# Add the server executable
add_executable(server_executable
    server/src/main_server.cpp
    server/src/server.cpp
)

# Link needed libraries
target_link_libraries(server_executable pthread)
