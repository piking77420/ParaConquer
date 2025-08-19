#include "Resources/ShaderSource.hpp"

/*include <Windows.h>     
#include <dxc/dxcapi.h>  
*/#

#include <fstream>
#include <iostream>
#include <PerfRegion.hpp>

#include "Io/InOut.h"
#include "LowRenderer/Rhi.hpp"
#include "VulkanHeader.h"
#include "Resources/ResourceManager.hpp"
#include "Resources/ShaderSourceBinary.hpp"

#include <filesystem>
#include <Rendering/RenderingTypedef.h>




using namespace PC_CORE;
//using Microsoft::WRL::ComPtr;

constexpr const char* INCLUDE_PATH = EDITOR_RESOURCE_PATH"/Shaders/Include/";
constexpr int GLSL_VERSION = 450;
/*
class MyIncluder : public IDxcIncludeHandler {
public:
    class CustomIncludeHandler : public IDxcIncludeHandler {
    public:
        CustomIncludeHandler(const std::unordered_map<std::wstring, std::wstring>& files)
            : m_files(files) {
        }

        // IDxcIncludeHandler
        HRESULT STDMETHODCALLTYPE LoadSource(
            _In_ LPCWSTR pFilename,
            _COM_Outptr_ IDxcBlob** ppIncludeSource
        ) override {
            auto it = m_files.find(pFilename);
            if (it == m_files.end()) {
                return E_FAIL; // fichier non trouvé
            }

            const std::wstring& source = it->second;
            ComPtr<IDxcBlobEncoding> blob;
            HRESULT hr = m_library->CreateBlobWithEncodingFromPinned(
                source.c_str(),
                static_cast<UINT32>(source.size() * sizeof(wchar_t)),
                CP_UTF16,
                &blob
            );
            if (FAILED(hr)) return hr;

            *ppIncludeSource = blob.Detach();
            return S_OK;
        }

        // Setter pour la librairie DXC nécessaire pour CreateBlobWithEncodingFromPinned
        void SetLibrary(IDxcLibrary* library) {
            m_library = library;
        }

        // IUnknown
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override {
            if (!ppvObject) return E_POINTER;
            if (riid == __uuidof(IDxcIncludeHandler) || riid == IID_IUnknown) {
                *ppvObject = static_cast<IDxcIncludeHandler*>(this);
                AddRef();
                return S_OK;
            }
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        ULONG STDMETHODCALLTYPE AddRef() override { return ++m_ref; }
        ULONG STDMETHODCALLTYPE Release() override {
            ULONG ref = --m_ref;
            if (ref == 0) delete this;
            return ref;
        }

    private:
        std::unordered_map<std::wstring, std::wstring> m_files;
        ComPtr<IDxcLibrary> m_library;
        ULONG m_ref = 1;
    };
};
*/
void ShaderSource::InitShadersCompiler(PC_CORE::GraphicAPI graphicApi, bool _optimise)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);
    
    PC_LOG("Init ShadersCompiler")
    
    
    switch (graphicApi)
    {
    case GraphicAPI::None:
        break;
    case GraphicAPI::Vulkan:
        break;
    case GraphicAPI::D3d12:
        break;
    case GraphicAPI::Count:
        break;
    default: ;
    }
    
}

void ShaderSource::DestroyShadersCompiler()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);
    PC_LOG("Destroy Shaders Compiler")
}



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

ShaderSource::ShaderSource(const std::string& _name) : Resource(_name)
{
    DYNAMIC_REFLECT_INIT
    
}

ShaderSource::ShaderSource(const std::string& _name, std::string _path) : Resource(_name)
{
    DYNAMIC_REFLECT_INIT
    ShaderSource::LoadFromFile(_path);
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
    auto s = ResourceManager::Get<ShaderSourceBinary>(GetShaderBinarySprivName());
    s->WriteSprivToFile(&sourceSpriv);
    BroadCastReload();
}


std::vector<char> ShaderSource::GetShaderSourceFile()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    if (m_PathToSource.empty())
    {
        PC_LOGERROR("Resource path is empty while trying to get data from it")
        return {};
    }

    std::vector<char> source = PC_CORE::InOut::ReadFile(m_PathToSource);
    source.emplace_back('\0');
    return source;
}



bool ShaderSource::GetCompiledShaderSource(std::vector<uint32_t>* _buffer)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);
    /*
    // Load shader File to memory 
    std::vector<char> RawSourceCode = GetShaderSourceFile();

    shaderc_shader_kind kind = GetGlangShaderStage(m_ShaderType);

    std::string sourceCode;
    if (!PreprocessShader(name, kind, RawSourceCode.data(), &sourceCode))
        return false;

    std::vector<uint32_t> spriv;
    if (!CompileFile(name, kind, sourceCode, &spriv))
    {
        return false;
    }

   *_buffer = std::move(spriv);*/
    return true;
}

void ShaderSource::LoadFromFile(const std::string& _path)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    Resource::LoadFromFile(_path);
    uint32_t formatIndex = -1;

    if (!IsFormatValid(ShaderSourceFormat, extension, &formatIndex))
    {
        PC_LOGERROR("Shader invalid format")
    }

     extension = ShaderSourceFormat[formatIndex];
    m_ShaderType = static_cast<ShaderStageTypeFlag>(formatIndex);
    m_PathToSource = _path;


    PC_LOG("Compile {} into SPRIV", name);

    std::vector<uint32_t> sourceSpriv;
    if (!GetCompiledShaderSource(&sourceSpriv))
    {
        PC_LOGERROR("Failed to read shader source file for writing shader spriv cache");
        return;
    }

    std::string sprivName = GetShaderBinarySprivName();
    auto s = ResourceManager::Create<ShaderSourceBinary>(std::move(sprivName), &sourceSpriv, m_ShaderType);

    Resource::LinkDependencies(this, s.get());
}



std::string ShaderSource::GetShaderBinarySprivName()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Resource);

    return std::filesystem::path(name).filename().stem().generic_string() + "Spv" + extension;
}



