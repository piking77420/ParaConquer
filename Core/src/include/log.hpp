#pragma once
#include "core_header.hpp"
#include <string>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PC_LOG(message) \
    Log::Debug(message,__FILE__,std::to_string(__LINE__).c_str());\

#define PC_LOGERROR(message) \
 Log::Error(message,__FILE__,std::to_string(__LINE__).c_str());\


BEGIN_PCCORE

class Log
{
public:
    
    PC_CORE_API static void Debug(const std::string& message,const char* file,const char* line);

    PC_CORE_API static void Debug(const std::string& message);

    PC_CORE_API static void Error(const std::string& message,const char* file,const char* line);
};

END_PCCORE