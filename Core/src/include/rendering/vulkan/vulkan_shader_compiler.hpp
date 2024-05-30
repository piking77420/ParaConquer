#pragma once

#include <filesystem>

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanShaderCompiler
{
public:
    static void GetEnvironementVariable();
    
    void CompileToSpv(const std::filesystem::path& _shaderSourcePath);

    uint8_t* GetData();
    
private:
    static inline std::string vulkanEnvironementPath = "";
    
    static inline constexpr const char* vulkanEnvironmentName = "VULKAN_SDK";
        
    uint8_t* m_Data = nullptr;
};

END_PCCORE