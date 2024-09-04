# Especific version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(ChatApp)

# Client configuration
add_executable(main_client main_client.cpp)

# Server configuration
add_executable(main_server main_server.cpp)
