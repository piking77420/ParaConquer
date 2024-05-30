#include "log.hpp"

#include <filesystem>
#include <iostream>

void Log::Debug(const std::string& message, const char* file, const char* line)
{
    std::filesystem::path p = std::filesystem::path(std::string(file));
    
    std::cout << ANSI_COLOR_RESET << message << ' '
    << '[' << p.filename().generic_string().c_str() << ',' << ' ' << line << ']'<< '\n';
}

void Log::Debug(const std::string& message)
{
    std::cout << message  << '\n';
}

void Log::Error(const std::string& message, const char* file, const char* line)
{
    std::filesystem::path p = std::filesystem::path(std::string(file));
    
    std::cout << ANSI_COLOR_RED << message << ' '
    << '[' << p.filename().generic_string().c_str() << ',' << ' ' << line << ']'<< '\n';
}
