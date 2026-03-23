#include <iostream>
#include "server.h"
#include "logger.h"

int main() {
    int port = 8080;

    Logger::init("logs/server.log");

    std::cout << "Starting HTTP Server on port " << port << std::endl;
    

    Server server(port);
    server.start();

    return 0;
}