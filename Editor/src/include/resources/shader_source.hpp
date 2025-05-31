#pragma once

#include <shaderc/shaderc.hpp>

#include "resources/resource.hpp"
#include "low_renderer/rhi_typedef.h"
#include "resources/shader_program.h"


BEGIN_PCCORE


class ShaderSource : public PC_CORE::Resource
{
public:

    static void InitShadersCompiler(PC_CORE::GraphicAPI graphicApi, bool _optimise);

    static void DestroyShadersCompiler();

    void CompileToSpriv();

    bool GetCompiledShaderSource(std::vector<uint32_t>* _buffer);

    IMP_DYNAMIC_REFLECT()

    ShaderSource();

    ShaderSource(const fs::path& _path);
    
    ~ShaderSource() override = default;

private:
    
    ShaderStageType m_ShaderType;

    fs::path m_PathToSource;

    struct ShaderCompiler   
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
    };

    static inline ShaderCompiler* shaderCompiler = nullptr;
    
    std::vector<char> GetShaderSourceFile();

    std::string PreprocessShader(const std::string& source_name, shaderc_shader_kind kind, const char* source);
    std::string CompileFileToAssembly(const std::string& source_name, shaderc_shader_kind kind,
                                      const std::string& source,
                                      bool optimize);

    std::vector<uint32_t> CompileFile(const std::string& source_name,
                            shaderc_shader_kind kind,
                            const std::string& source,
                            bool optimize = false);


    static void AddPreProcessorDefVulkan();
};

REFLECT(ShaderSource)

END_PCCORE