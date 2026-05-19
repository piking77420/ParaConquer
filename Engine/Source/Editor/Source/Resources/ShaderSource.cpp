#include "Resources/ShaderSource.hpp"


#include <fstream>
#include <iostream>
#include <PerfRegion.hpp>

#include "Io/InOut.h"
#include "Resources/ResourceManager.hpp"
#include "Resources/ShaderSourceBinary.hpp"
#include "Editor.hpp"

#include <filesystem>

#include "ShaderCompiler.hpp"

using namespace PC_CORE;
using namespace PC_EDITOR_CORE;


void ShaderSource::AddPreProcessorDefVulkan()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    /*
    shaderc::CompileOptions& options = shaderCompiler->options;
    //lIGHT

    options.AddMacroDefinition("MAX_DIRLIGHT", std::to_string(MAX_DIRLIGHT));
    options.AddMacroDefinition("MAX_SPOTLIGHT", std::to_string(MAX_SPOTLIGHT));
    options.AddMacroDefinition("MAX_POINTLIGHT", std::to_string(MAX_POINTLIGHT));

    options.AddMacroDefinition("SCENE_DESCRIPTOR_SET", std::to_string(SCENE_DESCRIPTOR_SET));
    options.AddMacroDefinition("CAMERA_BINDING", std::to_string(CAMERA_BINDING));
    options.AddMacroDefinition("LIGHTDATA_BINDING", std::to_string(LIGHTDATA_BINDING));
    options.AddMacroDefinition("FORWARD_SKYBOX_CUBEMAP", std::to_string(FORWARD_SKYBOX_CUBEMAP));


    options.AddMacroDefinition("MATERIAL_DESCRIPTOR_SET", std::to_string(MATERIAL_DESCRIPTOR_SET));
    options.AddMacroDefinition("ALBEDO_BINDING", std::to_string(ALBEDO_BINDING));

    
    options.AddMacroDefinition("ENVIRONEMENT_DESCRIPTOR_SET", std::to_string(ENVIRONEMENT_DESCRIPTOR_SET));
    options.AddMacroDefinition("SKYBOX_BINDING", std::to_string(SKYBOX_BINDING));


    // cam 
    options.AddMacroDefinition("CAM_DEPTH_MAX", std::to_string(CAM_DEPTH_MAX));
    options.AddMacroDefinition("CAM_DEPTH_MIN", std::to_string(CAM_DEPTH_MIN));

    // Math
    options.AddMacroDefinition("MAX_FLOAT", std::to_string(std::numeric_limits<float>::max()));


    // SPRITE
    options.AddMacroDefinition("SPRITE_SET", std::to_string(SPRITE_SET));
    options.AddMacroDefinition("SPRITE_TEXTURE", std::to_string(SPRITE_TEXTURE));

    //Deffered
    options.AddMacroDefinition("GBUFFER_SET", std::to_string(GBUFFER_SET));
    options.AddMacroDefinition("G_ALBEDO", std::to_string(G_ALBEDO));
    options.AddMacroDefinition("G_NORMAL", std::to_string(G_NORMAL));
    options.AddMacroDefinition("G_ROUGNESS_METALLIC_AO", std::to_string(G_ROUGNESS_METALLIC_AO));
    options.AddMacroDefinition("G_WORLD_POSITION", std::to_string(G_WORLD_POSITION));*/
}

ShaderSource::ShaderSource() : Resource()
{
    DYNAMIC_REFLECT_INIT
}

ShaderSource::ShaderSource(const std::string& _name, ShaderFeatureFlag _ShaderFeatureFlag) 
    : Resource(_name)
    , m_ShaderFeatureFlag(_ShaderFeatureFlag)
{
    DYNAMIC_REFLECT_INIT
}

ShaderSource::ShaderSource(const std::string& _name, const std::filesystem::path& _path, ShaderFeatureFlag _ShaderFeatureFlag) 
    : Resource(_name)
    , m_ShaderFeatureFlag(_ShaderFeatureFlag)
{
    DYNAMIC_REFLECT_INIT

    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    uint32_t formatIndex = -1;

    if (!PC_CORE::IsFormatValid(PC_CORE::RhiShaderProgram::ShaderSourceFormat, GetFullExtension(_path.generic_string()), &formatIndex))
    {
        PC_LOGERROR("Shader invalid format")
    }

    m_ShaderType = static_cast<PC_CORE::RhiShaderProgram::ShaderStageTypeBits>(formatIndex);
    m_PathToSource = _path;


    PC_LOG("Compiling {} ", Name);

    std::vector<uint32_t> sourceSpriv;
    if (!GetCompiledShaderSource(&sourceSpriv))
    {
        PC_LOGERROR("Failed to read shader source file for writing shader spriv cache");
        return;
    }

    std::string binaryName = GetShaderBinaryPath();
    if (!ResourceManager::Exist(binaryName))
    {
        auto s = ResourceManager::Create<ShaderSourceBinary>(std::move(binaryName), &sourceSpriv, m_ShaderType,
                                                             Editor::instance->editorData.projectData.graphicApi);
        LinkDependencies(this, s.get());
    }
}

void ShaderSource::Reload()
{
    Resource::Reload();

    std::vector<uint32_t> sourceSpriv;
    if (!GetCompiledShaderSource(&sourceSpriv))
    {
        PC_LOGERROR("Failed to read shader source file for writing shader spriv cache");
        return;
    }
    auto s = ResourceManager::Get<ShaderSourceBinary>(GetShaderBinaryPath());

    s->WriteSprivToFile(&sourceSpriv, App::Instance->RenderHarwareInteface.GetGraphicsApi());
    BroadCastReload();
}


bool ShaderSource::GetCompiledShaderSource(std::vector<uint32_t>* _buffer)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    assert(Editor::instance != nullptr);
    std::vector<uint32_t> code = Editor::instance->shaderCompiler.CompileFile(
        Editor::instance->editorData.projectData.graphicApi, m_PathToSource, GetDefineFromShaderFeatures());

    if (code.empty())
        return false;

    *_buffer = std::move(code);
    return true;
}

std::vector<std::wstring> ShaderSource::GetDefineFromShaderFeatures() const
{
    std::vector<std::wstring> Features;

    if (m_ShaderFeatureFlag & ShaderFeatureFlagBits::Lit)
    {
        Features.push_back(L"-DLIT=1");
    }

    if (m_ShaderFeatureFlag & ShaderFeatureFlagBits::UseUV)
    {
        Features.push_back(L"-DUSE_UV=1");
    }

    if (m_ShaderFeatureFlag & ShaderFeatureFlagBits::UseUV)
    {
        Features.push_back(L"-DUSE_NORMAL_MAP=1");
    }

    if (m_ShaderFeatureFlag & ShaderFeatureFlagBits::UseColor)
    {
        Features.push_back(L"-DUSE_COLOR=1");
    }

    if (m_ShaderFeatureFlag & ShaderFeatureFlagBits::DrawTriangle)
    {
        Features.push_back(L"-DDRAW_TRIANGLE=1");
    }

    if (m_ShaderFeatureFlag & ShaderFeatureFlagBits::Instanced)
    {
        Features.push_back(L"-DINSTANCED=1");
    }

    if (m_ShaderFeatureFlag & ShaderFeatureFlagBits::Frustum)
    {
        Features.push_back(L"-DFRUSTUM=1");
    }

    return Features;
}

std::string ShaderSource::GetShaderBinaryPath()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    return std::filesystem::path(Name).filename().generic_string() + ".binary";
}
