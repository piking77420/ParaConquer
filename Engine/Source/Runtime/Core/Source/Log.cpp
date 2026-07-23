#include "Log.hpp"

#include <print>


using namespace PC_CORE;

void Log::PrintMetaData(int _lign, const char* _func, const char* _file)
{
    std::println("[{}, {}, {}]", _file, _func, _lign);
    std::print("\n");
}
