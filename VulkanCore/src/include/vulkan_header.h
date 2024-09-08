#pragma once

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cassert>

#include <vulkan/vulkan.h>

#ifdef VULKAN_DLL

#ifdef VULKA_API_EXPORT 
#define VULKA_API __declspec(dllexport)
#elif VULKA_API_IMPORT
#define VULKA_API __declspec(dllimport)
#endif
#else
#define VULKA_API
#endif // VULKAN_DLL




namespace VK_NP
{
    #define VK_CHECK_ERROR(x, message) \
        if (x != VK_SUCCESS) \
        { \
            std::cout << std::string(message) << " in file " << __FILE__ << " at line " << std::to_string(__LINE__) << '\n'; \
            throw std::runtime_error(message); \
        }
    
    

    
}


