#ifndef PER_REGION
#define PER_REGION

#ifdef _WIN32
#ifdef PROFILER_EXPORTS
#define PROFILER_API __declspec(dllexport)
#else
#define PROFILER_API __declspec(dllimport)
#endif
#else
#define PROFILER_API
#endif

#ifdef PROFILING
#include "tracy/Tracy.hpp"

#define PROFILER_NOOP TracyNoop

#define PERF_REGION_SCOPED ZoneScoped
#define PERF_REGION_SCOPEDN ZoneScopedN
#define PERF_REGION_SCOPEDC ZoneScopedC(x)
#define PERF_REGION_SCOPEDNC ZoneScopedNC(x,y)



#define PERF_FRAME_MARK FrameMark
#define PERF_FRAME_MARK_NAMED FrameMarkNamed(x)
#define PERF_FRAME_MARK_START FrameMarkStart(x)
#define PERF_FRAME_MARK_END FrameMarkEnd(x)

#else

#define PROFILER_NOOP

#define PERF_REGION_SCOPED 
#define PERF_REGION_SCOPEDN 
#define PERF_REGION_SCOPEDC 
#define PERF_REGION_SCOPEDNC

#define PERF_FRAME_MARK 
#define PERF_FRAME_MARK_NAMED 
#define PERF_FRAME_MARK_START 
#define PERF_FRAME_MARK_END

#endif // PROFILING



#endif