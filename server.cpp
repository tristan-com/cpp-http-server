#include "server.h"
#include "thread_pool.h"
#include "logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Determine the correct MIME type based on file extension
// This tells the browser how to interpret the file
std::string get_content_type(const std::string& path) {
    if (path.size() >= 5 && path.substr(path.size() - 5) == ".html") return "text/html";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".css") return "text/css";
    if (path.size() >= 3 && path.substr(path.size() - 3) == ".js") return "application/javascript";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".png") return "image/png";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".jpg") return "image/jpeg";
    if (path.size() >= 5 && path.substr(path.size() - 5) == ".jpeg") return "image/jpeg";
    return "text/plain";
}

// Function that handles a single client request
// This will be executed by a worker thread from the thread pool
void handle_client(int client_socket) {
    char buffer[30000] = {0};

    // Read incoming HTTP request from client
    int bytes_read = read(client_socket, buffer, sizeof(buffer));

    // If nothing was read, close connection
    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }

    // Convert raw request into a string for parsing
    std::string request(buffer);
    Logger::info("Request received");

    // Parse HTTP request line (e.g., GET /index.html HTTP/1.1)
    std::istringstream request_stream(request);

    std::string method;
    std::string path;
    std::string version;

    request_stream >> method >> path >> version;

    Logger::info(method + " " + path);

    // Default route: if user requests "/", serve index.html
    if (path == "/") {
        path = "/index.html";
    }

    // Build full file path
    std::string file_path = "www" + path;

    // Open file in binary mode (important for images and non-text files)
    std::ifstream file(file_path, std::ios::binary);

    if (file) {
        // Read entire file into memory
        std::stringstream buffer_stream;
        buffer_stream << file.rdbuf();
        std::string body = buffer_stream.str();

        // Log successful request
        Logger::info(method + " " + path + " 200");

        // Build HTTP response headers
        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: " + get_content_type(path) + "\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n";

        // Send headers first
        send(client_socket, response.c_str(), response.size(), 0);

        // Then send file content (body)
        send(client_socket, body.c_str(), body.size(), 0);
    }
    else {
        // File not found → send 404 response
        std::string body = "404 Not Found";

        Logger::error("404 Not Found: " + path);

        std::string response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" +
            body;

        send(client_socket, response.c_str(), response.size(), 0);
    }

    // Close connection after response is sent
    close(client_socket);
}

// Constructor: initialize server port
Server::Server(int port) {
    this->port = port;
}

// Start the HTTP server
void Server::start() {

    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == 0) {
        Logger::error("Socket creation failed");
        return;
    }

    // Allow reuse of the same port after restarting server
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server address
    address.sin_family = AF_INET;          // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
    address.sin_port = htons(port);        // Convert port to network byte order

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        Logger::error("Bind failed");
        return;
    }

    // Start listening for incoming connections
    if (listen(server_fd, 10) < 0) {
        Logger::error("Listen failed");
        return;
    }

    Logger::info("Server listening on port " + std::to_string(port));

    // Create thread pool with 4 worker threads
    ThreadPool pool(4);

    // Main server loop
    while (true) {

        // Accept incoming client connection
        int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        if (client_socket < 0) {
            Logger::error("Accept failed");
            continue;
        }

        Logger::info("Client connected");

        // Add client handling task to thread pool
        pool.enqueue([client_socket]() {
            handle_client(client_socket);
        });
    }
}