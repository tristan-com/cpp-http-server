# C++ High-Performance HTTP Server

A multithreaded HTTP server built from scratch in C++ using low-level socket programming.

---

## Features

*  TCP socket server (POSIX sockets)
*  HTTP request parsing (GET support)
*  Static file serving (HTML, CSS, JS, images)
*  Thread pool for handling multiple clients
*  Multithreading using `std::thread`
*  Thread-safe logging system (file + console)
*  404 error handling
*  MIME type detection

---

## Technologies Used

* C++
* POSIX sockets (`sys/socket.h`)
* Multithreading (`std::thread`)
* Synchronization (`mutex`, `condition_variable`)
* File I/O (`fstream`)

---

## Project Structure

```
cpp-http-server/
├── src/        # Source files
├── include/    # Header files
├── www/        # Static website files
├── logs/       # Log output
├── README.md
└── .gitignore
```

---

##  How to Build & Run

### 1. Clone the repository

```bash
git clone https://github.com/tristan-com/cpp-http-server.git
cd cpp-http-server
```

---

### 2. Compile

```bash
g++ src/main.cpp src/server.cpp src/thread_pool.cpp src/logger.cpp -Iinclude -o server -pthread
```

---

### 3. Run the server

```bash
./server
```

---

### 4. Open in browser

```
http://localhost:8080
```

---

##  Example Requests

###  Successful request

```
GET /index.html HTTP/1.1
```

Response:

```
200 OK
```

---

###  Not Found

```
GET /unknown HTTP/1.1
```

Response:

```
404 Not Found
```

---

##  Logging

Logs are written to:

```
logs/server.log
```

Example output:

```
[INFO] 2026-03-20 18:10:05 - GET /index.html 200
[ERROR] 2026-03-20 18:10:07 - 404 Not Found: /test
```

---

##  How It Works

1. The server listens for incoming TCP connections
2. Accepted connections are added to a thread pool queue
3. Worker threads process client requests concurrently
4. HTTP requests are parsed (method, path)
5. Files are served from the `www/` directory
6. Responses are returned with proper headers and status codes
7. All requests are logged (thread-safe)

---

##  Author

Tristan Comtois
