#include "time/timer.hpp"


void PC_CORE::Timer::StartTimer(const char* name)
{
    timePasses.emplace_back(name, std::chrono::high_resolution_clock::now());
}

void PC_CORE::Timer::EndTimer()
{
    TimePass& time_pass = timePasses.back();
    auto end = std::chrono::high_resolution_clock::now();
    time_pass.elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - time_pass.time).count();
    
}

void PC_CORE::Timer::Clear()
{
    timePasses.clear();
}
