#include "back_end/vulkan_shader_compiler.hpp"

#include <shaderc/shaderc.hpp>

std::string CompileFileToAssembly(const std::string& _source_name,
                                  const shaderc_shader_kind _kind,
                                  const std::string& _source,
                                  const bool optimize = false)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    options.AddMacroDefinition("MY_DEFINE", "1");
    if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

    shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(
        _source, _kind, _source_name.c_str(), options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << result.GetErrorMessage();
        return "";
    }

    return {result.cbegin(), result.cend()};
}


std::string PreProcesShader(const std::string& _sourceName, const shaderc_shader_kind _kind,
                            const std::string& _source)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    options.AddMacroDefinition("MY_DEFINE", "1");

    shaderc::PreprocessedSourceCompilationResult result =
        compiler.PreprocessGlsl(_source, _kind, _sourceName.c_str(), options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << result.GetErrorMessage();
        return "";
    }

    return {result.cbegin(), result.cend()};
}

std::vector<uint32_t> CompileFile(const std::string& _sourceName,
                                  const shaderc_shader_kind _kind,
                                  const std::string& _source,
                                  const bool _optimize = false)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    options.AddMacroDefinition("MY_DEFINE", "1");
    if (_optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

    shaderc::SpvCompilationResult module =
        compiler.CompileGlslToSpv(_source, _kind, _sourceName.c_str(), options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << module.GetErrorMessage();
        return {};
    }

    return {module.cbegin(), module.cend()};
}

#pragma region LowLevelShaderStageToShaderrcKind

shaderc_shader_kind GetShaderStageToShaderrcKind(PC_CORE::LowLevelShaderStageType _lowLevelShaderStage)
{
    switch (_lowLevelShaderStage)
    {
    case PC_CORE::LowLevelShaderStageType::VERTEX:
        return shaderc_glsl_vertex_shader;
        break;
    case PC_CORE::LowLevelShaderStageType::FRAGMENT:
        return shaderc_glsl_fragment_shader;
        break;
    case PC_CORE::LowLevelShaderStageType::GEOMETRY:
        return shaderc_glsl_geometry_shader;
        break;
    case PC_CORE::LowLevelShaderStageType::TESSELATION:
        return shaderc_glsl_tess_control_shader;
        break;
    case PC_CORE::LowLevelShaderStageType::COMPUTE:
        return shaderc_glsl_compute_shader;
        break;
    default: ;
    }

    return static_cast<shaderc_shader_kind>(std::numeric_limits<uint32_t>::max());
}


#pragma endregion LowLevelShaderStageToShaderrcKind


void VK_NP::VulkanShaderCompiler::CompileShaderToSpv(const std::string _shaderName, const std::string& _shaderSource,
                                                     PC_CORE::LowLevelShaderStageType _lowLevelShaderStage,
                                                     std::vector<uint32_t>* _sprivSource)
{
    const shaderc_shader_kind kind = GetShaderStageToShaderrcKind(_lowLevelShaderStage);

#ifdef _DEBUG
    bool optimise = false;
#else
    bool optimise = true;
#endif

    /*
    // Preprocessing
    std::string preprocessed = PreProcesShader(
        "shader_src", kind, _shaderSource.c_str());
    std::cout << "Compiled a vertex shader resulting in preprocessed text:"
        << std::endl
        << preprocessed << std::endl;
    */

    /*
    // Compiling
    std::string assembly = CompileFileToAssembly(
        "shader_src", kind, _shaderSource.c_str());
    std::cout << "SPIR-V assembly:" << std::endl << assembly << std::endl;
*/

    std::vector<uint32_t> spirv =
        CompileFile("shader_src", shaderc_glsl_vertex_shader, _shaderSource.c_str(), optimise);
    std::cout << "Compiled to a binary module with " << spirv.size()
        << " words." << std::endl;

    *_sprivSource = spirv;
}
