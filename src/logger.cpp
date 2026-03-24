#include "logger.h"

#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::mutex log_mutex;
std::ofstream log_file;

std::string get_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::tm* tm = std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(log_mutex);
    log_file.open(filename, std::ios::app); // append mode
}

void Logger::log(const std::string& level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);

    std::string full_message =
        "[" + level + "] " + get_time() + " - " + message;

    // Write to console
    std::cout << full_message << std::endl;

    // Write to file
    if (log_file.is_open()) {
        log_file << full_message << std::endl;
    }
}

void Logger::info(const std::string& message) {
    log("INFO", message);
}

void Logger::error(const std::string& message) {
    log("ERROR", message);
}