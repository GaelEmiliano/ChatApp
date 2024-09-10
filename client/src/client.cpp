#include "../include/client.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>

Client::Client(const std::string& server_ip, int server_port)
    : m_server_ip(server_ip), m_server_port(server_port), m_server_socket(-1) {}

void Client::run() {
    std::cout << "Write your name: ";
    std::getline(std::cin, m_client_name);

    connectToServer();

    nlohmann::json hello_message = {
        {"type", "connect"},
        {"name", m_client_name}
    };
    sendMessage(hello_message.dump());

    while (true) {
        std::cout << "Write the message for send to the server (or 'exit' for disconnect): ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit")
            break;

        nlohmann::json message = {
            {"type", "message"},
            {"content", input}
        };
        sendMessage(message.dump());

        std::string response = receiveMessage();
        if (response.empty()) {
            std::cerr << "Fail receiving a server answear or a close connection"
                      << std::endl;
            break;
        }
        std::cout << "Server answear: " << response << std::endl;
    }

    closeConnection();
}

void Client::connectToServer() {
    m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_socket < 0) {
        throw std::runtime_error("Fail creating the socket client");
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_server_port);
    if (inet_pton(AF_INET, m_server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP");
    }

    if (connect(m_server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Can not connect to the server");
    }

    std::cout << "Connecting to the server " << m_server_ip << ":" << m_server_port << std::endl;
}

void Client::sendMessage(const std::string& message) {
    if (send(m_server_socket, message.c_str(), message.size(), 0) == -1) {
        throw std::runtime_error("Fail to send a message to the server");
    }
}

std::string Client::receiveMessage() {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int len = recv(m_server_socket, buffer, sizeof(buffer), 0);
    if (len <= 0) {
        return "";
    }
    return std::string(buffer, len);
}

void Client::closeConnection() {
    if (m_server_socket != -1) {
        close(m_server_socket);
        m_server_socket = -1;
    }
}
