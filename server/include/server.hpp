#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <netinet/in.h>
#include <mutex>
#include <unordered_map>

/*
  A class that implements a simple server for handling multiple clients
  via socket connections.
 */
class Server {

public:
    // Inicialize the server with the explicit port
    explicit Server(int port);
    // Starts the server
    void start();
    // Handles a connected client
    void handleClient(int clientSocket);

private:
    // The port number on which the server listens for connections
    int m_port;
    // The socket description for the server's main listening socket
    int m_server_socket;
     // A vector of socket descriptors for all connected clients
    std::vector<int> m_clients;
    // This map allows the server to track which client corresponds with each socket
    std::unordered_map<int, std::string> m_client_names;
    // A mutex to ensure thread-safe access to the clients list
    std::mutex m_clients_mutex;

    // Accept a new client connection
    int acceptConnection();
    // This method waits for a message from the specified client socket
    std::string receiveMessage(int clientSocket);
    // This method sends the provide message to the client identified by the socket descriptor
    void sendMessage(int clientSocket, const std::string& message);
    // This method sends the provide message to all connected clients
    void broadcastMessage(const std::string& message, int senderSocket);
};

#endif // SERVER_HPP
