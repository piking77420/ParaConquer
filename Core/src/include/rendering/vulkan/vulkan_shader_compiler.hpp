#pragma once

#include <filesystem>

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanShaderCompiler
{
public:
    static void GetEnvironementVariable();
    
    static bool CompileToSpv(const std::filesystem::path& _shaderSourcePath, const std::string& _extension, std::vector<char>* _data);

private:
    static inline std::string vulkanEnvironementPath {};
    
    static inline constexpr const char* vulkanEnvironmentName = "VULKAN_SDK";

    static void ReadFile(const std::string& _filename,std::vector<char>* data);

};

END_PCCORE