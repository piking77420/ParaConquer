#pragma once

#include "Resources/Resource.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "Rendering/ShaderProgram.hpp"


BEGIN_PCCORE


class ShaderSource : public PC_CORE::Resource
{
public:

    static void InitShadersCompiler(PC_CORE::GraphicAPI graphicApi, bool _optimise);

    static void DestroyShadersCompiler();

    void Reload() override;
    
    bool GetCompiledShaderSource(std::vector<uint32_t>* _buffer);

    PC_CORE_API void LoadFromFile(const std::string& _path) override;

    IMP_DYNAMIC_REFLECT()

    explicit ShaderSource();

    explicit ShaderSource(const std::string& _name);

    explicit ShaderSource(const std::string& _name, std::string _path);
    
    ~ShaderSource() override = default;

private:
    
    ShaderStageTypeFlag m_ShaderType;

    std::string m_PathToSource;

    
    std::vector<char> GetShaderSourceFile();
    
    std::string GetShaderBinarySprivName();

    static void AddPreProcessorDefVulkan();
};

REFLECT(ShaderSource)

END_PCCORE