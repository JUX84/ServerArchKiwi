#include <iostream>
#include <ctime>

#include "logger.hpp"

void Logger::log(std::string message) {
	std::time_t time = std::time(nullptr);
        char timestr[9];
        if (std::strftime(timestr, sizeof(timestr), "%H:%M:%S", std::localtime(&time)))
                std::cout << "[" << timestr << "] ";
        std::cout << message << '\n';
}
