#include "log.hpp"

#include <filesystem>
#include <iostream>

using namespace PC_CORE;


void Log::Debug(const std::string& message, const char* file, const char* line)
{

    std::cout << ANSI_COLOR_RESET << message << ' '
        << '[' << file << ',' << ' ' << line << ']' << '\n';
}

void Log::Debug(const std::string& message)
{
    std::cout << message  << '\n';
}

void Log::Error(const std::string& message, const char* file, const char* line)
{
    std::cout << ANSI_COLOR_RED << message << ' '
        << '[' << file << ',' << ' ' << line << ']' << '\n' << ANSI_COLOR_RESET;
}
