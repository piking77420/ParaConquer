#include "Thread/ThreadUtils.hpp"

#ifdef _WIN32 
#include <Windows.h>  
const DWORD MS_VC_EXCEPTION = 0x406D1388;  
#pragma pack(push,8)  
typedef struct tagTHREADNAME_INFO  
{  
    DWORD dwType; // Must be 0x1000.  
    LPCSTR szName; // Pointer to name (in user addr space).  
    DWORD dwThreadID; // Thread ID (-1=caller thread).  
    DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;  
#pragma pack(pop)  

void PC_CORE::Utils::SetThreadName(const char* _name, uint32_t _hint)
{

    // for os
    static auto SetThreadDescription = reinterpret_cast<HRESULT (__stdcall *)(HANDLE, PCWSTR)>(GetProcAddress(GetModuleHandleA("kernel32.dll"),
        "SetThreadDescription"));
    if(SetThreadDescription)
    {
        wchar_t buf[256];
        mbstowcs( buf, _name, 256 );
        SetThreadDescription( GetCurrentThread(), buf );
    }

    // for msvc info
#ifdef _MSC_VER
    THREADNAME_INFO info;  
    info.dwType = 0x1000;  
    info.szName = _name;  
    info.dwThreadID = GetCurrentThreadId();  
    info.dwFlags = 0;  
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
    __try{  
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);  
    }  
    __except (EXCEPTION_EXECUTE_HANDLER){  
    }  
#pragma warning(pop)  
#endif
}

#endif
