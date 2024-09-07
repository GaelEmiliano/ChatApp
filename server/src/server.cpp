#include "../include/server.hpp"
#include "../../common/json.hpp"
#include <iostream>
#include <unistd.h>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <algorithm>

Server::Server(int port) : m_port(port) {
    // Create a TCP socket
    m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_socket < 0)
        throw std::runtime_error("Failed to create the socket");


    /*
      Configure the sockadrr_in structure for the server
    */
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address
    server_addr.sin_port = htons(port); // Convert port to network byte order

    /*
      Bind the socket to the specified port 
    */
    if (bind(m_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(m_server_socket);
        throw std::runtime_error("Binding failed");
    }

    /*
      Start listening for incoming connections
     */
    if (listen(m_server_socket, 10) < 0) {
        close(m_server_socket);
        throw std::runtime_error("Failed to listen on the socket");
    }
}

void Server::start() {
    std::cout << "Server started on port " << m_port << std::endl;
    while (true) {
        int client_socket = acceptConnection();
        if (client_socket != -1)
            std::thread(&Server::handleClient, this, client_socket).detach();
    }
}

void Server::handleClient(int clientSocket) {
    try {
        // Receive the initial message with the name of the client
        std::string initial_message = receiveMessage(clientSocket);
        if (initial_message.empty()) {
            std::cerr << "Failed receiving the first message of the client"
                      << std::endl;
            close(clientSocket);
            return;
        }
        auto json_message = nlohmann::json::parse(initial_message);
        if (json_message.contains("type")
            && json_message["type"] == "connect"
            && json_message.contains("name")) {
            std::string client_name = json_message["name"];
            {
                std::lock_guard<std::mutex> lock(m_clients_mutex);
                m_client_names[clientSocket] = client_name;
            }
            std::cout << "Connected client: " << clientSocket
                      << " with name " << client_name << std::endl;
            {
                std::lock_guard<std::mutex> lock(m_clients_mutex);
                std::cout << "Connected clients: "
                          << m_client_names.size() << std::endl;
            }
        } else {
            std::cerr << "Invalid first message from the client"
                      << initial_message << std::endl;
            return;
        }
        // Handling future messages from the client
        while (true) {
            std::string message = receiveMessage(clientSocket);
            if (message.empty()) {
                std::cout << "Client " << m_client_names[clientSocket]
                          << " disconnected or failure reading" << std::endl;
                break;
            }
            std::cout << "Message received by " << m_client_names[clientSocket]
                      << ": " << message << std::endl;
            nlohmann::json response_json;
            try {
                nlohmann::json received_json = nlohmann::json::parse(message);
                response_json["received"] = received_json;
                response_json["status"] = "success";
            } catch (const std::exception& e) {
                response_json["status"] = "error";
                response_json["message"] = "Invalid JSON message";
            }
            sendMessage(clientSocket, response_json.dump());
        }
        // Close connection and clear
        close(clientSocket);
        {
            std::lock_guard<std::mutex> lock(m_clients_mutex);
            m_client_names.erase(clientSocket);
            std::cout << "Connected clients: " << m_client_names.size()
                      << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "handleClient Exception: " << e.what() << std::endl;
        close(clientSocket);
        {
            std::lock_guard<std::mutex> lock(m_clients_mutex);
            m_client_names.erase(clientSocket);
            std::cout << "Connected clients: " << m_client_names.size()
                      << std::endl;
        }
    }
}

int Server::acceptConnection() {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(m_server_socket, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket < 0) {
        std::cerr << "Error al aceptar conexión" << std::endl;
        return -1; // MAGIC NUMBER !!!
    }

    {
        std::lock_guard<std::mutex> lock(m_clients_mutex);
        m_clients.push_back(client_socket);
    }

    return client_socket;
}

std::string Server::receiveMessage(int clientSocket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int len = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (len <= 0) {
        return "";
    }
    return std::string(buffer, len);
}

void Server::sendMessage(int clientSocket, const std::string& message) {
    if (send(clientSocket, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Fail to sending client message "
                  << clientSocket << std::endl;
}

void Server::broadcastMessage(const std::string& message, int senderSocket) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    for (int client_socket : m_clients) {
        if (client_socket != senderSocket) {
            sendMessage(client_socket, message);
        }
    }
}
