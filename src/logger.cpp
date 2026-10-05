#include "../include/logger.h"

#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

void Logger::log(const std::string& message) {

    std::ofstream file(
        "logs/activity.log",
        std::ios::app
    );

    if (!file) {
        return;
    }

    auto now = std::chrono::system_clock::now();

    std::time_t currentTime =
        std::chrono::system_clock::to_time_t(now);

    file << std::put_time(
        std::localtime(&currentTime),
        "%Y-%m-%d %H:%M:%S"
    );

    file << " | " << message << '\n';
}
