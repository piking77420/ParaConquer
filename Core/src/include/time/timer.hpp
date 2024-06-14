#pragma once

#include <chrono>
#include <stack>
#include <vector>

#include "core_header.hpp"


BEGIN_PCCORE

class Timer
{
public:
    struct TimePass
    {
        const char* timePassname = nullptr;
        std::chrono::system_clock::time_point time;
    };
    
    static inline std::vector<TimePass> timePasses;

    static void StartTimer(const char* name);

    static void EndTimer();

    
private:
   
    
};

END_PCCORE