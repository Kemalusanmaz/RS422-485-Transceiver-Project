#include "../include/common.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string Common::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto inTimeT = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&inTimeT), "%Y-%m-%d %X");
    return  ss.str();   
}
