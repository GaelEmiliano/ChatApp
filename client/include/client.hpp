#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../../common/json.hpp"
#include <string>

class Client {

public:
    explicit Client(const std::string& server_ip, int server_port);
    void run();
    
private:
    std::string m_server_ip;
    int m_server_port;
    int m_server_socket;
    std::string m_client_name;

    void connectToServer();
    void sendMessage(const std::string& message);
    std::string receiveMessage();
    void closeConnection();
};

#endif // CLIENT_HPP
