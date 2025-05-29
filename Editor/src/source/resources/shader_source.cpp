#include "resources/shader_source.hpp"

#include <fstream>
#include <iostream>

#include "io/in_out.h"
#include "low_renderer/rhi.hpp"
#include "vulkan_header.h"

using namespace PC_CORE;

constexpr int GLSL_VERSION = 450;
constexpr const char* INCLUDE_PATH = EDITOR_RESOURCE_PATH"/shaders/include/";

class Includer : public shaderc::CompileOptions::IncluderInterface
{
public:
    shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type,
        const char* requesting_source, size_t include_depth) override
    {
        std::string full_path = std::string(INCLUDE_PATH) + requested_source;
        std::ifstream file(full_path);
        if (!file.is_open()) return nullptr;

        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

        auto* result = new shaderc_include_result;
        result->source_name = strdup(requested_source);
        result->source_name_length = strlen(result->source_name);
        result->content = strdup(content.c_str());
        result->content_length = content.size();
        result->user_data = nullptr;
        return result;
    }
    void ReleaseInclude(shaderc_include_result* data) override
    {
        free((void*)data->source_name);
        free((void*)data->content);
        delete data;
    }
    ~Includer() override = default;
};


void ShaderSource::InitShadersCompiler(PC_CORE::GraphicAPI graphicApi, bool _optimise)
{
    PC_LOG("Init ShadersCompiler")
    
    shaderCompiler = new ShaderCompiler();
    shaderCompiler->options.SetIncluder(std::make_unique<Includer>());
    
    if (_optimise) shaderCompiler->options.SetOptimizationLevel(shaderc_optimization_level_size);

    switch (graphicApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        shaderCompiler->options.SetForcedVersionProfile(GLSL_VERSION, shaderc_profile_core);
        shaderCompiler->options.SetSourceLanguage(shaderc_source_language_glsl);
        AddPreProcessorDefVulkan();
        break;
    case GraphicAPI::DX3D12:
        shaderCompiler->options.SetSourceLanguage(shaderc_source_language_hlsl);
        break;
    case GraphicAPI::COUNT:
        break;
    default: ;
    }
    
}

void ShaderSource::DestroyShadersCompiler()
{
    PC_LOG("Destroy Shaders Compiler")

    delete shaderCompiler;
    shaderCompiler = nullptr;
}

void ShaderSource::AddPreProcessorDefVulkan()
{
    shaderc::CompileOptions& options = shaderCompiler->options;

    options.AddMacroDefinition("SCENE_DESCRIPTOR_SET", std::to_string(SCENE_DESCRIPTOR_SET));
    options.AddMacroDefinition("MATERIAL_DESCRIPTOR_SET", std::to_string(MATERIAL_DESCRIPTOR_SET));

    options.AddMacroDefinition("CAMERA_BINDING", std::to_string(CAMERA_BINDING));
    options.AddMacroDefinition("LIGHTDATA_BINDING", std::to_string(LIGHTDATA_BINDING));
    options.AddMacroDefinition("ALBEDO_BINDING", std::to_string(ALBEDO_BINDING));

}



std::string ShaderSource::PreprocessShader(const std::string& source_name,
                              shaderc_shader_kind kind,
                              const char* source) {
    // Like -DMY_DEFINE=1

    shaderc::PreprocessedSourceCompilationResult result =
        shaderCompiler->compiler.PreprocessGlsl(source, kind, source_name.c_str(), shaderCompiler->options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        PC_LOGERROR("Failed to preprocess shader: {}", result.GetErrorMessage());
        return "";
    }

    return {result.cbegin(), result.cend()};
}


std::string ShaderSource::CompileFileToAssembly(const std::string& source_name,
                                  shaderc_shader_kind kind,
                                  const std::string& source,
                                  bool optimize = false) {
    
    shaderc::AssemblyCompilationResult result = shaderCompiler->compiler.CompileGlslToSpvAssembly(
        source, kind, source_name.c_str(), shaderCompiler->options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::cerr << result.GetErrorMessage();
        return "";
    }

    return {result.cbegin(), result.cend()};
}


std::vector<uint32_t> ShaderSource::CompileFile(const std::string& source_name, shaderc_shader_kind kind,
    const std::string& source, bool optimize)
{
    // Like -DMY_DEFINE=1

    shaderc::SpvCompilationResult module =
        shaderCompiler->compiler.CompileGlslToSpv(source, kind, source_name.c_str(), shaderCompiler->options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::cerr << module.GetErrorMessage();
        return {};
    }

    return {module.cbegin(), module.cend()};
}


static shaderc_shader_kind GetGlangShaderStage(ShaderStageType _shaderType)
{
    switch (_shaderType)
    {
    case ShaderStageType::VERTEX:
        return shaderc_glsl_vertex_shader;
        break;
    case ShaderStageType::TESSCONTROL:
        return shaderc_tess_control_shader;
        break;
    case ShaderStageType::TESSEVALUATION:
        return shaderc_tess_evaluation_shader;
        break;
    case ShaderStageType::GEOMETRY:
        return shaderc_geometry_shader;
        break;
    case ShaderStageType::FRAGMENT:
        return shaderc_fragment_shader;
        break;
    case ShaderStageType::COMPUTE:
        return shaderc_compute_shader;
        break;
    case ShaderStageType::RAYGEN:
        return shaderc_raygen_shader;
        break;
    case ShaderStageType::INTERSECT:
        return shaderc_intersection_shader;
        break;
    case ShaderStageType::ANYHIT:
        return shaderc_anyhit_shader;
        break;
    case ShaderStageType::CLOSESTHIT:
        return shaderc_closesthit_shader;
        break;
    case ShaderStageType::MISS:
        return shaderc_miss_shader;
        break;
    case ShaderStageType::CALLABLE:
        return shaderc_callable_shader;
        break;
    case ShaderStageType::TASK:
        return shaderc_task_shader;
        break;
    case ShaderStageType::MESH:
        return shaderc_mesh_shader;
        break;
    case ShaderStageType::COUNT:
    default:
        throw std::invalid_argument("Invalid shader stage");

    }

    throw std::invalid_argument("Invalid shader stage");
}



ShaderSource::ShaderSource() : ResourceInterface<ShaderSource>()
{

}

ShaderSource::ShaderSource(const fs::path& _path) : ResourceInterface<ShaderSource>(_path)
{
    uint32_t formatIndex = -1;

    if (!IsFormatValid(ShaderSourceFormat, extension, &formatIndex))
    {
        PC_LOGERROR("Shader invalid format")
    }
    
    extension = ShaderSourceFormat[formatIndex];
    m_ShaderType = static_cast<ShaderStageType>(formatIndex);
    m_PathToSource = _path;
}

std::vector<char> ShaderSource::GetShaderSourceFile()
{
    if (m_PathToSource.empty())
    {
        PC_LOGERROR("Resource path is empty while trying to get data from it")
        return {};
    }

    std::vector<char> source = PC_CORE::InOut::ReadFile(m_PathToSource.generic_string());
    source.emplace_back('\0');
    return source;
}



bool ShaderSource::GetCompiledShaderSource(std::vector<uint32_t>* _buffer)
{
    // Load shader File to memory 
    std::vector<char> RawSourceCode = GetShaderSourceFile();

    shaderc_shader_kind kind = GetGlangShaderStage(m_ShaderType);
    
    std::string sourceCode = PreprocessShader(name, kind, RawSourceCode.data());

    std::vector<uint32_t> spriv = CompileFile(name, kind, sourceCode);
   *_buffer = std::move(spriv);

    return true;
}

void ShaderSource::CompileToSpriv()
{
    PC_LOG("Compile {} into SPRIV", name);

    
    std::string filePath = SHADER_CACHE_PATH + fs::path(name).filename().stem().generic_string() + "_spv" + extension;
    std::fstream f(filePath, std::ios::binary | std::ios::out | std::ios::trunc);
    std::vector<uint32_t> sourceSpriv;

    if (!GetCompiledShaderSource(&sourceSpriv))
    {
        PC_LOGERROR("Failed to read shader source file for writing shader spriv cache");
        return;
    }

    if (!f.is_open())
    {
        std::cerr << "Failed to open file " << filePath << std::endl;
        PC_LOGERROR("File is not open");
        return;
    }

    // Write the contents of the vector to the file
    f.write(reinterpret_cast<const char*>(sourceSpriv.data()), sourceSpriv.size() * sizeof(uint32_t));

    f.close();
}




