#include "time/timer.hpp"


void PC_CORE::Timer::StartTimer(const char* name)
{
    timePasses.emplace_back(name, std::chrono::system_clock::now());
}

void PC_CORE::Timer::EndTimer()
{
    TimePass& time_pass =  timePasses.at(timePasses.size() - 1);
    time_pass.time -= std::chrono::duration_cast<std::chrono::milliseconds>(time_pass.time.time_since_epoch());
}
