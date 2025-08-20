#pragma once

#include "EditorHeader.hpp"
#include "Resources/Resource.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "Rendering/ShaderProgram.hpp"


BEGIN_EDITOR_PCCORE

class ShaderSource : public PC_CORE::Resource
{
public:
    void Reload() override;
    
    bool GetCompiledShaderSource(std::vector<uint32_t>* _buffer);

    PC_CORE_API void LoadFromFile(const std::string& _path) override;

    IMP_DYNAMIC_REFLECT()

    explicit ShaderSource();

    explicit ShaderSource(const std::string& _name);

    explicit ShaderSource(const std::string& _name, const std::string& _path);
    
    ~ShaderSource() override = default;

private:
    
    PC_CORE::ShaderStageTypeFlag m_ShaderType;

    std::string m_PathToSource;

    std::string GetShaderBinaryPath();

    static void AddPreProcessorDefVulkan();
};

REFLECT(ShaderSource)

END_PCCORE