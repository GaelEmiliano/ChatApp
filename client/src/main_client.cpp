#include "../include/client.hpp"
#include <iostream>

#define PORT 8080
#define IP "127.0.0.1"

int main()
{
    Client client(IP, PORT);
    try {
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Fail: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
