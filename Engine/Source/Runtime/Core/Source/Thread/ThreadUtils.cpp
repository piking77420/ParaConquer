#include "Thread/ThreadUtils.hpp"
#ifdef _WIN32
#include <windows.h>
#elif defined(_GNU_SOURCE)
#include <string.h> // strncpy
#include <pthread.h>
#elif defined(__linux__)
#include <sys/prctl.h>
#endif

// https://www.siliceum.com/en/blog/post/etw-events-thread-names/
void PC_CORE::Utils::SetThreadName(const char* _name)
{
#ifdef _WIN32
    // see https://learn.microsoft.com/en-us/visualstudio/debugger/tips-for-debugging-threads?view=vs-2022&tabs=csharp
    // We're throwing the exception as long as a debugger is connected.
    // This means we may both throw the exception and call SetThreadDescription, 
    // which is a bit redundant but will work with all tools.
    if (IsDebuggerPresent())
    {
        constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
        using THREADNAME_INFO = struct tagTHREADNAME_INFO
        {
            DWORD dwType; // Must be 0x1000.
            LPCSTR szName; // Pointer to name (in user addr space).
            DWORD dwThreadID; // Thread ID (-1=caller thread).
            DWORD dwFlags; // Reserved for future use, must be zero.
        };
#pragma pack(pop)

        DWORD dwThreadID = GetCurrentThreadId();

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = _name;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;

        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }

    using SetThreadDescriptionType = decltype(SetThreadDescription)*;
    static auto sSetThreadDescriptionPtr = (SetThreadDescriptionType)GetProcAddress(
        GetModuleHandleA("kernel32.dll"), "SetThreadDescription");

    WCHAR threadNameWStr[512];
    if (sSetThreadDescriptionPtr && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, _name, -1, threadNameWStr, 512))
    {
        sSetThreadDescriptionPtr(GetCurrentThread(), threadNameWStr);
    }
#elif defined(_GNU_SOURCE)

# ifdef PTHREAD_MAX_NAMELEN_NP
#  define PM_PTHREAD_MAX_NAMELEN_NP PTHREAD_MAX_NAMELEN_NP
# elif defined(__APPLE__)
#  include <sys/procinfo.h>
#  define PM_PTHREAD_MAX_NAMELEN_NP MAXTHREADNAMESIZE
# elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#  define PM_PTHREAD_MAX_NAMELEN_NP (MAXCOMLEN + 1)
# else
#  define PM_PTHREAD_MAX_NAMELEN_NP 16
#endif
    // We don't want the call to fail, so we silently truncate the names to the platform limit.
    char truncatedName[PM_PTHREAD_MAX_NAMELEN_NP];
    strncpy(truncatedName, _name, PM_PTHREAD_MAX_NAMELEN_NP - 1);
    truncatedName[PM_PTHREAD_MAX_NAMELEN_NP - 1] = '\0';

# undef PM_PTHREAD_MAX_NAMELEN_NP

# if defined __APPLE__
    pthread_setname_np(truncatedName);
# elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    pthread_set_name_np(pthread_self(), truncatedName);
# else
    pthread_setname_np(pthread_self(), truncatedName);
# endif

#elif defined(__linux__)
    // Note: up to 15chars max (16 including NUL terminator), will be truncated silently
    prctl(PR_SET_NAME, _name, 0, 0, 0);
#endif
}
