#include "ShaderCompiler.hpp"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>

#include <PerfRegion.hpp>
#include <Log.hpp>
#include "LowRenderer/Rhi.hpp"


// Keep it here cringe windows headers
#include <Windows.h>
#include <dxc/dxcapi.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;
using namespace PC_EDITOR_CORE;

constexpr auto INCLUDE_PATH = EDITOR_RESOURCE_PATH_W L"/Shaders/Include/";

struct DXCContext
{
    ComPtr<IDxcLibrary> library;
    ComPtr<IDxcCompiler3> compiler;
    ComPtr<IDxcUtils> utils;
};

// TODO to regular code
const std::array<std::pair<std::wstring, std::wstring>, 14> ShaderFormats =
{
    {
        {L".vs", L"vs_6_1"}, // Vertex Shader
        {L".hs", L"hs_6_1"}, // Hull Shader
        {L".ds", L"ds_6_1"}, // Domain Shader
        {L".gs", L"gs_6_1"}, // Geometry Shader
        {L".ps", L"ps_6_1"}, // Pixel Shader
        {L".cs", L"cs_6_1"}, // Compute Shader

        // DXR raytracing stages
        {L".rgen", L"lib_6_3"}, // Ray generation (DXC: lib_6_3 with [shader("raygeneration")])
        {L".rint", L"lib_6_3"}, // Intersection
        {L".ahit", L"lib_6_3"}, // Any hit
        {L".chit", L"lib_6_3"}, // Closest hit
        {L".miss", L"lib_6_3"}, // Miss
        {L".call", L"lib_6_3"}, // Callable

        // Mesh shaders (DirectX 12 Ultimate)
        {L".as", L"as_6_5"}, // Amplification Shader
        {L".ms", L"ms_6_5"} // Mesh Shader
    }
};

static DXCContext& GetContext()
{
    static thread_local DXCContext Context;

    HRESULT hres;

    if (!Context.library)
    {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        hres = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&Context.library));
        if (FAILED(hres))
        {
            PC_LOGERROR("Failed to create CLSID_DxcLibrary error = {}", hres);
            exit(-1);
        }
    }
   
    if (!Context.compiler)
    {
        hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Context.compiler));
        if (FAILED(hres))
        {
            PC_LOGERROR("Failed to create CLSID_DxcLibrary error = {}", hres);
            exit(-1);
        }
    }
    if (!Context.utils)
    {
        hres = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&Context.utils));
        if (FAILED(hres))
        {
            PC_LOGERROR("Failed to create CLSID_DxcUtils error = {}", hres);
            exit(-1);
        }
    }

    return Context;
}

// Thanks to https://simoncoenen.com/blog/programming/graphics/DxcCompiling
class CustomIncludeHandler : public IDxcIncludeHandler
{
public:
    HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR pFilename,
                                         _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override
    {
        ComPtr<IDxcBlobEncoding> pEncoding;
        auto path = std::string(&pFilename[0], pFilename + wcslen(pFilename));
        if (IncludedFiles.contains(path))
        {
            // Return empty string blob if this file has been included before
            static constexpr char nullStr[] = " ";
            GetContext().utils->CreateBlobFromPinned(nullStr, ARRAYSIZE(nullStr), DXC_CP_ACP, pEncoding.GetAddressOf());
            *ppIncludeSource = pEncoding.Detach();
            return S_OK;
        }

        HRESULT hr = GetContext().utils->LoadFile(pFilename, nullptr, pEncoding.GetAddressOf());
        if (SUCCEEDED(hr))
        {
            IncludedFiles.insert(path);
            *ppIncludeSource = pEncoding.Detach();
        }
        return hr;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
    {
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef(void) override { return 0; }
    ULONG STDMETHODCALLTYPE Release(void) override { return 0; }

    std::unordered_set<std::string> IncludedFiles;
};


static const wchar_t* GetTargetProfile(const std::wstring_view& _fileFormat)
{
    for (const auto& pair : ShaderFormats)
    {
        if (_fileFormat == pair.first)
            return pair.second.c_str();
    }

    PC_LOGERROR("Faile to find TargetProfile _fileFormat = {}", std::string(_fileFormat.begin(), _fileFormat.end()));

    return nullptr;
}

static bool GetExtension(const wchar_t* _file, wchar_t* _buffer, size_t _bufferSize,
    size_t _extensionBegin, size_t _extensionSize)
{
    if (_extensionSize >= _bufferSize) // buffer overflow
        return false;

    wcsncpy(&_buffer[0], _file + _extensionBegin, _extensionSize);
    _buffer[_extensionSize] = L'\0';

    return true;
}

std::vector<uint32_t> ShaderCompiler::CompileFile(PC_CORE::GraphicAPI _api, const std::wstring& _fileName, const std::vector<std::wstring>& _Args)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::EditorResource);

    HRESULT hres;

    uint32_t codePage = DXC_CP_ACP;
    ComPtr<IDxcBlobEncoding> sourceBlob;
    hres = GetContext().utils->LoadFile(_fileName.c_str(), &codePage, &sourceBlob);
    if (FAILED(hres) || !sourceBlob)
    {
        PC_LOGERROR("Failed to load file FromDisk = {} file path {}", hres, std::string(_fileName.begin(), _fileName.end()));
        exit(-1);
    }

    LPCWSTR targetProfile = nullptr;
    const size_t eidx = _fileName.rfind(L'.'); // last . from the end ".hlsl"
    const size_t idx = _fileName.rfind(L'.', eidx - 1);


    if (idx == std::wstring::npos)
    {
        PC_LOGERROR("There is no available format for {}", std::string(_fileName.begin(), _fileName.end()));
        assert(eidx != std::wstring::npos && idx != std::wstring::npos);
        exit(-1);
    }
    wchar_t bufferExt[128];

    bool bext = GetExtension(_fileName.c_str(), bufferExt, sizeof(bufferExt), idx, eidx - idx);
    assert(bext && "Buffer overflow");

    targetProfile = GetTargetProfile(bufferExt);

    if (!targetProfile)
        return {};

    std::vector<LPCWSTR> arguments = {
        _fileName.c_str(), // Shader path
        L"-E", L"Main", // Entry point
        L"-T", targetProfile, // Target profile
        L"-I", INCLUDE_PATH,
        L"-Zpr",
    };
    for (const auto& args : _Args)
    {
        arguments.push_back(args.c_str());
    }

    switch (_api)
    {
    case PC_CORE::GraphicAPI::Vulkan:
        arguments.push_back(L"-Zpr");
        arguments.push_back(L"-spirv");
        arguments.push_back(L"-fspv-target-env=vulkan1.3");
        arguments.push_back(L"-fspv-extension=SPV_EXT_mesh_shader");
        break;
    case PC_CORE::GraphicAPI::D3d12:
        arguments.push_back(L"-Zpc");
        break;
    case PC_CORE::GraphicAPI::None:
    case PC_CORE::GraphicAPI::Count:
        break;
    }
    DxcBuffer buffer{};
    buffer.Encoding = DXC_CP_ACP;
    buffer.Ptr = sourceBlob->GetBufferPointer();
    buffer.Size = sourceBlob->GetBufferSize();

    CustomIncludeHandler includer;
    ComPtr<IDxcResult> result;
    hres = GetContext().compiler->Compile(
        &buffer,
        arguments.data(),
        static_cast<uint32_t>(arguments.size()),
        &includer,
        IID_PPV_ARGS(&result)
    );

    if (SUCCEEDED(hres) && result)
    {
        result->GetStatus(&hres);
    }

    if (FAILED(hres) && result)
    {
        ComPtr<IDxcBlobEncoding> errorBlob;
        hres = result->GetErrorBuffer(&errorBlob);
        if (SUCCEEDED(hres) && errorBlob)
        {
            PC_LOGERROR("Shader compilation failed, {} \n {}", std::string(_fileName.begin(), _fileName.end()),
                        static_cast<const char*>(errorBlob->GetBufferPointer()));
            return {};
        }
    }

    ComPtr<IDxcBlob> code;
    if (result)
    {
        hres = result->GetResult(&code);
        if (FAILED(hres) || !code)
        {
            PC_LOGERROR("Failed to get compiled shader code, HRESULT={}", hres);
            return {};
        }
    }

    const void* dataPtr = code->GetBufferPointer();
    size_t dataSize = code->GetBufferSize();
    if (!dataPtr || dataSize == 0)
    {
        PC_LOGERROR("Compiled shader code is empty");
        return {};
    }

    std::vector<uint32_t> shaderCode(
        reinterpret_cast<const uint32_t*>(dataPtr),
        reinterpret_cast<const uint32_t*>(dataPtr) + (dataSize / sizeof(uint32_t))
    );

    return shaderCode;
}


ShaderCompiler::ShaderCompiler()
{
    
}

ShaderCompiler::~ShaderCompiler()
{
}
