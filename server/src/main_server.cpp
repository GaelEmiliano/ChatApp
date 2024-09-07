#include "../include/server.hpp"
#include <iostream>
#include <thread>
#include <chrono>

#define PORT 8080

int main() {
    try {        
        Server server(PORT);
        server.start();
        // Mantaining an active server
        std::this_thread::sleep_for(std::chrono::minutes(10));
    } catch (const std::exception& e) {
        std::cerr << "Server failure: " << e.what() << std::endl;
    }
    return 0;
}
