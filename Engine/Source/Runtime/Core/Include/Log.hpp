#pragma once

#include <Iostream>

#include <String>
#include <Format>
#include <Ranges>
#include <mutex>

#include "CoreHeader.hpp"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_ORANGE  "\x1b[38;5;208m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_DARK_GRAY "\x1b[90m"

#ifndef __FUNCTION_NAME__
#ifdef WIN32   //WINDOWS
#define FUNCTION_NAME  __FUNCTION__
#else          //*NIX
#define __FUNCTION_NAME__   __func__
#endif
#endif

static inline const char* ExtractFileName(const char* _path)
{
    const char* slash = strrchr(_path, '\\');
    return slash ? slash + 1 : _path;
}

#define FILENAME ExtractFileName(__FILE__)


#define PC_LOG(unformatted, ...) \
PC_CORE::Log::Debug(__LINE__, FUNCTION_NAME, FILENAME, unformatted, ##__VA_ARGS__);\

#define PC_LOG_VERBOSE(unformatted, ...) \
PC_CORE::Log::Verbose(__LINE__, FUNCTION_NAME, FILENAME, unformatted, ##__VA_ARGS__);\

#define PC_LOGERROR(unformatted, ...) \
PC_CORE::Log::Error(__LINE__, FUNCTION_NAME, FILENAME, unformatted, ##__VA_ARGS__);\

#define PC_LOGCRITICAL(unformatted, ...) \
PC_CORE::Log::Critical(__LINE__, FUNCTION_NAME, FILENAME, unformatted, ##__VA_ARGS__);\

BEGIN_PCCORE
    class Log
    {
    public:
        template <typename... Args>
        static void Debug(int _lign, const char* _func, const char* _file, const std::string& unformatted, Args&&... args)
        {
            std::scoped_lock _(m_lock);

            std::cout << ANSI_COLOR_RESET;
            PrintFormat(unformatted, std::forward<Args>(args)...);
            PrintMetaData(_lign, _func, _file);
        }

        template <typename... Args>
        static void Verbose(int _lign, const char* _func, const char* _file, const std::string& unformatted, Args&&... args)
        {
            std::scoped_lock _(m_lock);

            std::cout << ANSI_COLOR_DARK_GRAY;
            PrintFormat(unformatted, std::forward<Args>(args)...);
            PrintMetaData(_lign,_func,_file);
        }

        template <typename... Args>
        static void Error(int _lign, const char* _func, const char* _file, const std::string& unformatted, Args&&... args)
        {
            std::scoped_lock _(m_lock);

            std::cout << ANSI_COLOR_ORANGE;
            PrintFormat(unformatted, std::forward<Args>(args)...);
            PrintMetaData(_lign,_func,_file);
        }

        template <typename... Args>
        static void Critical(int _lign, const char* _func, const char* _file, const std::string& unformatted, Args&&... args)
        {
            std::scoped_lock _(m_lock);

            std::cout << ANSI_COLOR_RED;
            PrintFormat(unformatted, std::forward<Args>(args)...);
            PrintMetaData(_lign, _func, _file);
        }


    private:
        template <typename... Args>
        static void PrintFormat(const std::string& unformatted, Args&&... args)
        {
            std::cout << std::vformat(unformatted, std::make_format_args(args...)) << '\n';
        }

        PC_CORE_API static void PrintMetaData(int _lign, const char* _func, const char* _file);

        static inline std::mutex m_lock;
    };

END_PCCORE
