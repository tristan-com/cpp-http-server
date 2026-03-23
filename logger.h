#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
public:
    static void init(const std::string& filename);

    static void info(const std::string& message);
    static void error(const std::string& message);

private:
    static void log(const std::string& level, const std::string& message);
};

#endif