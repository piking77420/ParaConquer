#pragma once

#include <shaderc/shaderc.hpp>

#include "resources/resource.hpp"
#include "low_renderer/rhi_typedef.h"
#include "rendering/shader_program.hpp"


BEGIN_PCCORE


class ShaderSource : public PC_CORE::Resource
{
public:

    static void InitShadersCompiler(PC_CORE::GraphicAPI graphicApi, bool _optimise);

    static void DestroyShadersCompiler();

    void Reload() override;
    
    bool GetCompiledShaderSource(std::vector<uint32_t>* _buffer);

    IMP_DYNAMIC_REFLECT()

    explicit ShaderSource();

    explicit ShaderSource(const fs::path& _path);
    
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
    
    std::string GetShaderBinarySprivName();

   bool PreprocessShader(const std::string& source_name,
                              shaderc_shader_kind kind,
                              const char* source, std::string* outCode);

    bool CompileFileToAssembly(const std::string& source_name, shaderc_shader_kind kind,
                                      const std::string& source,  std::string* outCode,
                                      bool optimize);

    bool CompileFile(const std::string& source_name,
                            shaderc_shader_kind kind,
                            const std::string& source, std::vector<uint32_t>* _outCode,
                            bool optimize = false);


    static void AddPreProcessorDefVulkan();
};

REFLECT(ShaderSource)

END_PCCORE