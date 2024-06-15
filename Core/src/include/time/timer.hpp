#pragma once

#include <chrono>
#include <stack>
#include <vector>

#include "core_header.hpp"


BEGIN_PCCORE

#ifndef NDEBUG
#define BEGIN_TIMER(x) Timer::StartTimer(x);
#else
#define BEGIN_TIMER(x) ((void)0)
#endif

#ifndef NDEBUG
#define END_TIMER() Timer::EndTimer();
#else
#define END_TIMER() ((void)0)
#endif

class Timer
{
public:
    struct TimePass
    {
        const char* timePassname = nullptr;
        std::chrono::time_point<std::chrono::high_resolution_clock> time;
        long long elapsed_ms = 0;
    };
    
    static inline std::vector<TimePass> timePasses;

    static void StartTimer(const char* name);

    static void EndTimer();

    static void Clear();

    
private:
   
    
};

END_PCCORE