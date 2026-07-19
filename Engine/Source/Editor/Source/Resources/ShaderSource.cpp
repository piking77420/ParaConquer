#include "Resources/ShaderSource.hpp"

#include <filesystem>

#include <PerfRegion.hpp>
#include <Editor.hpp>
#include <Rendering/PipelineCache.hpp>
#include "Resources/ShaderSourceBinary.hpp"
#include "Resources/ResourceManager.hpp"
#include <ShaderCompiler.hpp>
#include <Io/FileLoader.hpp>


using namespace PC_CORE;
using namespace PC_EDITOR_CORE;

ShaderSource::ShaderSource() : Resource()
{
    DYNAMIC_REFLECT_INIT
}

ShaderSource::ShaderSource(const std::string& _name, const std::filesystem::path& _path)
    : Resource(_name)
    , m_PathToSource((std::filesystem::path(EDITOR_RESOURCE_PATH) / _path.relative_path()).lexically_normal())
{
    DYNAMIC_REFLECT_INIT

    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    if (!PC_CORE::RhiPipeline::FormatToShaderStageTypeBits(&m_ShaderType, GetFullExtension(m_PathToSource.generic_string()))) // TO DO use SV
    {
        PC_LOGERROR("Shader invalid format");
        m_ShaderType = {};
    }
    /*
    PC_LOG("Compiling {} ", Name);
    std::string binaryName = GetShaderBinaryPath();
    std::vector<uint32_t> sourceSpriv;
    if (!GetCompiledShaderSource(&sourceSpriv, binaryName))
    {
        PC_LOGERROR("Failed to read shader source file for writing shader spriv cache");
        return;
    }

    
    if (!ResourceManager::Exist(binaryName))
    {
        auto s = ResourceManager::Create<ShaderSourceBinary>(std::move(binaryName), &sourceSpriv, m_ShaderType,
                                                             Editor::instance->editorData.projectData.graphicApi);
        LinkDependencies(this, s.get());
    }*/
}

void ShaderSource::Reload()
{
    /*
    Resource::Reload();

    std::vector<uint32_t> sourceSpriv;
    std::string binaryName = GetShaderBinaryPath();
    if (!GetCompiledShaderSource(&sourceSpriv, binaryName))
    {
        PC_LOGERROR("Failed to read shader source file for writing shader spriv cache");
        return;
    }
    auto s = ResourceManager::Get<ShaderSourceBinary>(GetShaderBinaryPath());

    s->WriteSprivToFile(&sourceSpriv, App::Instance->RenderHarwareInteface.GetGraphicsApi());
    BroadCastReload();*/
}


ShaderSource::GenerateVariantResult ShaderSource::GenerateVariant(const std::string& _VariantPath, const std::span<std::pair<std::string, int>>& _MacroDefinitions, PC_CORE::Rendering::ShaderFeatureFlags _ShaderFeatureFlag)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);
    PERF_MESSAGE("Generate New Variant");

    assert(Editor::instance != nullptr);
    auto& Compiler = Editor::instance->shaderCompiler;
    const auto GraphicApi = Editor::instance->editorData.projectData.graphicApi;
    std::vector<uint32_t> Code = Compiler.CompileFile(GraphicApi, m_PathToSource, _VariantPath, GetDefineFromShaderFeatures(_ShaderFeatureFlag));

    // TODO handl async loading
    return !Code.empty() && FileLoader::WriteFile(_VariantPath, Code.data(), sizeof(uint32_t) * Code.size(), true) ? ShaderSource::GenerateVariantResult::Sucess : ShaderSource::GenerateVariantResult::Failed;
}

std::vector<std::wstring> ShaderSource::GetDefineFromShaderFeatures(PC_CORE::Rendering::ShaderFeatureFlags _Flags)
{
    std::vector<std::wstring> Features;

    using namespace PC_CORE::Rendering;

    if (_Flags & ShaderFeature::Lit)
    {
        Features.push_back(L"-DLIT=1");
    }

    if (_Flags & ShaderFeature::UseUV)
    {
        Features.push_back(L"-DUSE_UV=1");
    }

    if (_Flags & ShaderFeature::UseUV)
    {
        Features.push_back(L"-DUSE_NORMAL_MAP=1");
    }

    if (_Flags & ShaderFeature::UseColor)
    {
        Features.push_back(L"-DUSE_COLOR=1");
    }

    if (_Flags & ShaderFeature::DrawTriangle)
    {
        Features.push_back(L"-DDRAW_TRIANGLE=1");
    }

    if (_Flags & ShaderFeature::Instanced)
    {
        Features.push_back(L"-DINSTANCED=1");
    }

    if (_Flags & ShaderFeature::FrustumCulling)
    {
        Features.push_back(L"-DFRUSTUM=1");
    }

    if (_Flags & ShaderFeature::SkyboxForceFarDepth)
    {
        Features.push_back(L"-DSKYBOX_FORCE_FAR_DEPTH=1");
    }

    if (auto* AppInstance = App::Instance)
    {
        Rhi& rhi = AppInstance->RenderHarwareInteface;

        if (rhi.IsYUpFrameBuffer())
        {
            Features.push_back(L"-DY_UP_FRAMEBUFFER=1");
        }
        else
        {
            Features.push_back(L"-DY_UP_FRAMEBUFFER=0");
        }

        if (rhi.IsYUpNdc())
        {
            Features.push_back(L"-DY_UP_NDC=1");
        }
        else
        {
            Features.push_back(L"-DY_UP_NDC=0");
        }
    }
    return Features;
}


