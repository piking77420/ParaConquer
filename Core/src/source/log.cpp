#include "log.hpp"

#include <print>


using namespace PC_CORE;

PC_CORE_API void Log::PrintMetaData(int _lign, const char* _func, const char* _file)
{
	std::println("[{}, {}, {}]", _file, _func, _lign);
	std::print("\n");
}
