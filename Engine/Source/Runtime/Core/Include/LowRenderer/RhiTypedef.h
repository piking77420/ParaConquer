#pragma once

#include <Variant>
#include <Vector>

#include "CoreHeader.hpp"
#include "Reflection/Reflector.hpp"
#include "Math/ToolboxTypedef.hpp"

#define ALIGNAS_16 alignas(16)


constexpr auto ShaderCachePath = "ShaderCache/";
constexpr int MaxFramesInFlight = 3;

BEGIN_PCCORE
    enum class GraphicAPI
    {
        None,
        Vulkan,
        D3d12,
        Count
    };

    REFLECT(GraphicAPI);

    constexpr auto ShaderCacheVulkanFolder = "SPRIV/";
    constexpr auto ShaderCacheD3d12Folder = "DXIL/";

    enum struct RhiExtension
    {
        RayTracing,
        MeshShader,
        Count
    };

    REFLECT(RhiExtension);


    struct ALIGNAS_16 DrawObjectBufferGPU
    {
        Tbx::Matrix4x4f Model;
    };

    struct ALIGNAS_16 PostProcessGpu
    {
        float Gamma;
        float Exposure;
    };


#pragma region LOG_TYPE

    enum class LogType
    {
        Info,
        Warning,
        Error
    };


#pragma endregion
    enum class BlendFactor : uint8_t
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SrcAlphaSaturate,
        Src1Color,
        OneMinusSrc1Color,
        Src1Alpha,
        OneMinusSrc1Alpha
    };

    enum class BlendOp : uint8_t
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max,
    };

    enum ColorComponent : uint8_t
    {
        None = 0,
        ColorComponentR = 1 << 0,
        ColorComponentG = 1 << 1,
        ColorComponentB = 1 << 2,
        ColorComponentA = 1 << 3
    };


    enum class RhiFormat : uint8_t
    {
        Undefined = 0,
        R4G4UnormPack8 = 1,
        R4G4B4A4UnormPack16 = 2,
        B4G4R4A4UnormPack16 = 3,
        R5G6B5UnormPack16 = 4,
        B5G6R5UnormPack16 = 5,
        R5G5B5A1UnormPack16 = 6,
        B5G5R5A1UnormPack16 = 7,
        A1R5G5B5UnormPack16 = 8,
        R8Unorm = 9,
        R8Snorm = 10,
        R8Uscaled = 11,
        R8Sscaled = 12,
        R8Uint = 13,
        R8Sint = 14,
        R8Srgb = 15,
        R8G8Unorm = 16,
        R8G8Snorm = 17,
        R8G8Uscaled = 18,
        R8G8Sscaled = 19,
        R8G8Uint = 20,
        R8G8Sint = 21,
        R8G8Srgb = 22,
        R8G8B8Unorm = 23,
        R8G8B8Snorm = 24,
        R8G8B8Uscaled = 25,
        R8G8B8Sscaled = 26,
        R8G8B8Uint = 27,
        R8G8B8Sint = 28,
        R8G8B8Srgb = 29,
        B8G8R8Unorm = 30,
        B8G8R8Snorm = 31,
        B8G8R8Uscaled = 32,
        B8G8R8Sscaled = 33,
        B8G8R8Uint = 34,
        B8G8R8Sint = 35,
        B8G8R8Srgb = 36,
        R8G8B8A8Unorm = 37,
        R8G8B8A8Snorm = 38,
        R8G8B8A8Uscaled = 39,
        R8G8B8A8Sscaled = 40,
        R8G8B8A8Uint = 41,
        R8G8B8A8Sint = 42,
        R8G8B8A8Srgb = 43,
        B8G8R8A8Unorm = 44,
        B8G8R8A8Snorm = 45,
        B8G8R8A8Uscaled = 46,
        B8G8R8A8Sscaled = 47,
        B8G8R8A8Uint = 48,
        B8G8R8A8Sint = 49,
        B8G8R8A8Srgb = 50,
        A8B8G8R8SnormPack32 = 52,
        A8B8G8R8UnormPack32 = 51,
        A8B8G8R8UscaledPack32 = 53,
        A8B8G8R8SscaledPack32 = 54,
        A8B8G8R8UintPack32 = 55,
        A8B8G8R8SintPack32 = 56,
        A8B8G8R8SrgbPack32 = 57,
        A2R10G10B10UnormPack32 = 58,
        A2R10G10B10SnormPack32 = 59,
        A2R10G10B10UscaledPack32 = 60,
        A2R10G10B10SscaledPack32 = 61,
        A2R10G10B10UintPack32 = 62,
        A2R10G10B10SintPack32 = 63,
        A2B10G10R10UnormPack32 = 64,
        A2B10G10R10SnormPack32 = 65,
        A2B10G10R10UscaledPack32 = 66,
        A2B10G10R10SscaledPack32 = 67,
        A2B10G10R10UintPack32 = 68,
        A2B10G10R10SintPack32 = 69,
        R16Unorm = 70,
        R16Snorm = 71,
        R16Uscaled = 72,
        R16Sscaled = 73,
        R16Uint = 74,
        R16Sint = 75,
        R16Sfloat = 76,
        R16G16Unorm = 77,
        R16G16Snorm = 78,
        R16G16Uscaled = 79,
        R16G16Sscaled = 80,
        R16G16Uint = 81,
        R16G16Sint = 82,
        R16G16Sfloat = 83,
        R16G16B16Unorm = 84,
        R16G16B16Snorm = 85,
        R16G16B16Uscaled = 86,
        R16G16B16Sscaled = 87,
        R16G16B16Uint = 88,
        R16G16B16Sint = 89,
        R16G16B16Sfloat = 90,
        R16G16B16A16Unorm = 91,
        R16G16B16A16Snorm = 92,
        R16G16B16A16Uscaled = 93,
        R16G16B16A16Sscaled = 94,
        R16G16B16A16Uint = 95,
        R16G16B16A16Sint = 96,
        R16G16B16A16Sfloat = 97,
        R32Uint = 98,
        R32Sint = 99,
        R32Sfloat = 100,
        R32G32Uint = 101,
        R32G32Sint = 102,
        R32G32Sfloat = 103,
        R32G32B32Uint = 104,
        R32G32B32Sint = 105,
        R32G32B32Sfloat = 106,
        R32G32B32A32Uint = 107,
        R32G32B32A32Sint = 108,
        R32G32B32A32Sfloat = 109,
        R64Uint = 110,
        R64Sint = 111,
        R64Sfloat = 112,
        R64G64Uint = 113,
        R64G64Sint = 114,
        R64G64Sfloat = 115,
        R64G64B64Uint = 116,
        R64G64B64Sint = 117,
        R64G64B64Sfloat = 118,
        R64G64B64A64Uint = 119,
        R64G64B64A64Sint = 120,
        R64G64B64A64Sfloat = 121,
        B10G11R11UfloatPack32 = 122,
        E5B9G9R9UfloatPack32 = 123,
        D16Unorm = 124,
        X8D24UnormPack32 = 125,
        D32Sfloat = 126,
        S8Uint = 127,
        D16UnormS8Uint = 128,
        D24UnormS8Uint = 129,
        D32SfloatS8Uint = 130,
    };
    REFLECT(RhiFormat)

    enum struct SampleCount
    {
        S1 = 1,
        S2 = 2,
        S4 = 4,
        S8 = 8,
        S16 = 16,
        S32 = 32,
        S64 = 64,
    };
    REFLECT(SampleCount)


#pragma region GetBytePerPixel
    static inline uint32_t GetBytePerPixel(RhiFormat RhiFormat)
    {
        switch (RhiFormat)
        {
        case PC_CORE::RhiFormat::Undefined:
            return 0;
        case PC_CORE::RhiFormat::R4G4UnormPack8:
            return 0;
        case PC_CORE::RhiFormat::R4G4B4A4UnormPack16:
            return 0;
        case PC_CORE::RhiFormat::B4G4R4A4UnormPack16:
            return 0;
        case PC_CORE::RhiFormat::R5G6B5UnormPack16:
            return 0;
        case PC_CORE::RhiFormat::B5G6R5UnormPack16:
            return 0;
        case PC_CORE::RhiFormat::R5G5B5A1UnormPack16:
            return 0;
        case PC_CORE::RhiFormat::B5G5R5A1UnormPack16:
            return 0;
        case PC_CORE::RhiFormat::A1R5G5B5UnormPack16:
            return 0;
        case PC_CORE::RhiFormat::R8Unorm:
        case PC_CORE::RhiFormat::R8Snorm:
        case PC_CORE::RhiFormat::R8Uscaled:
        case PC_CORE::RhiFormat::R8Sscaled:
        case PC_CORE::RhiFormat::R8Uint:
        case PC_CORE::RhiFormat::R8Sint:
        case PC_CORE::RhiFormat::R8Srgb:
            return 1;
        case PC_CORE::RhiFormat::R8G8Unorm:
        case PC_CORE::RhiFormat::R8G8Snorm:
        case PC_CORE::RhiFormat::R8G8Uscaled:
        case PC_CORE::RhiFormat::R8G8Sscaled:
        case PC_CORE::RhiFormat::R8G8Uint:
        case PC_CORE::RhiFormat::R8G8Sint:
        case PC_CORE::RhiFormat::R8G8Srgb:
            return 2;
        case PC_CORE::RhiFormat::R8G8B8Unorm:
        case PC_CORE::RhiFormat::R8G8B8Snorm:
        case PC_CORE::RhiFormat::R8G8B8Uscaled:
        case PC_CORE::RhiFormat::R8G8B8Sscaled:
        case PC_CORE::RhiFormat::R8G8B8Uint:
        case PC_CORE::RhiFormat::R8G8B8Sint:
        case PC_CORE::RhiFormat::R8G8B8Srgb:
        case PC_CORE::RhiFormat::B8G8R8Unorm:
        case PC_CORE::RhiFormat::B8G8R8Snorm:
        case PC_CORE::RhiFormat::B8G8R8Uscaled:
        case PC_CORE::RhiFormat::B8G8R8Sscaled:
        case PC_CORE::RhiFormat::B8G8R8Uint:
        case PC_CORE::RhiFormat::B8G8R8Sint:
        case PC_CORE::RhiFormat::B8G8R8Srgb:
            return 3;
        case PC_CORE::RhiFormat::R8G8B8A8Unorm:
        case PC_CORE::RhiFormat::R8G8B8A8Snorm:
        case PC_CORE::RhiFormat::R8G8B8A8Uscaled:
        case PC_CORE::RhiFormat::R8G8B8A8Sscaled:
        case PC_CORE::RhiFormat::R8G8B8A8Uint:
        case PC_CORE::RhiFormat::R8G8B8A8Sint:
        case PC_CORE::RhiFormat::R8G8B8A8Srgb:
        case PC_CORE::RhiFormat::B8G8R8A8Unorm:
        case PC_CORE::RhiFormat::B8G8R8A8Snorm:
        case PC_CORE::RhiFormat::B8G8R8A8Uscaled:
        case PC_CORE::RhiFormat::B8G8R8A8Sscaled:
        case PC_CORE::RhiFormat::B8G8R8A8Uint:
        case PC_CORE::RhiFormat::B8G8R8A8Sint:
        case PC_CORE::RhiFormat::B8G8R8A8Srgb:
            return 4;
        case PC_CORE::RhiFormat::A8B8G8R8SnormPack32:
            return 0;
        case PC_CORE::RhiFormat::A8B8G8R8UnormPack32:
            return 0;
        case PC_CORE::RhiFormat::A8B8G8R8UscaledPack32:
            return 0;
        case PC_CORE::RhiFormat::A8B8G8R8SscaledPack32:
            return 0;
        case PC_CORE::RhiFormat::A8B8G8R8UintPack32:
            return 0;
        case PC_CORE::RhiFormat::A8B8G8R8SintPack32:
            return 0;
        case PC_CORE::RhiFormat::A8B8G8R8SrgbPack32:
            return 0;
        case PC_CORE::RhiFormat::A2R10G10B10UnormPack32:
            return 0;
        case PC_CORE::RhiFormat::A2R10G10B10SnormPack32:
            return 0;
        case PC_CORE::RhiFormat::A2R10G10B10UscaledPack32:
            return 0;
        case PC_CORE::RhiFormat::A2R10G10B10SscaledPack32:
            return 0;
        case PC_CORE::RhiFormat::A2R10G10B10UintPack32:
            return 0;
        case PC_CORE::RhiFormat::A2R10G10B10SintPack32:
            return 0;
        case PC_CORE::RhiFormat::A2B10G10R10UnormPack32:
            return 0;
        case PC_CORE::RhiFormat::A2B10G10R10SnormPack32:
            return 0;
        case PC_CORE::RhiFormat::A2B10G10R10UscaledPack32:
            return 0;
        case PC_CORE::RhiFormat::A2B10G10R10SscaledPack32:
            return 0;
        case PC_CORE::RhiFormat::A2B10G10R10UintPack32:
            return 0;
        case PC_CORE::RhiFormat::A2B10G10R10SintPack32:
            return 0;
        case PC_CORE::RhiFormat::R16Unorm:
        case PC_CORE::RhiFormat::R16Snorm:
        case PC_CORE::RhiFormat::R16Uscaled:
        case PC_CORE::RhiFormat::R16Sscaled:
        case PC_CORE::RhiFormat::R16Uint:
        case PC_CORE::RhiFormat::R16Sint:
        case PC_CORE::RhiFormat::R16Sfloat:
            return 2;
        case PC_CORE::RhiFormat::R16G16Unorm:
        case PC_CORE::RhiFormat::R16G16Snorm:
        case PC_CORE::RhiFormat::R16G16Uscaled:
        case PC_CORE::RhiFormat::R16G16Sscaled:
        case PC_CORE::RhiFormat::R16G16Uint:
        case PC_CORE::RhiFormat::R16G16Sint:
        case PC_CORE::RhiFormat::R16G16Sfloat:
            return 4;
        case PC_CORE::RhiFormat::R16G16B16Unorm:
        case PC_CORE::RhiFormat::R16G16B16Snorm:
        case PC_CORE::RhiFormat::R16G16B16Uscaled:
        case PC_CORE::RhiFormat::R16G16B16Sscaled:
        case PC_CORE::RhiFormat::R16G16B16Uint:
        case PC_CORE::RhiFormat::R16G16B16Sint:
        case PC_CORE::RhiFormat::R16G16B16Sfloat:
            return 6;
        case PC_CORE::RhiFormat::R16G16B16A16Unorm:
            return 0;
        case PC_CORE::RhiFormat::R16G16B16A16Snorm:
        case PC_CORE::RhiFormat::R16G16B16A16Uscaled:
        case PC_CORE::RhiFormat::R16G16B16A16Sscaled:
        case PC_CORE::RhiFormat::R16G16B16A16Uint:
        case PC_CORE::RhiFormat::R16G16B16A16Sint:
        case PC_CORE::RhiFormat::R16G16B16A16Sfloat:
            return 8;
        case PC_CORE::RhiFormat::R32Uint:
        case PC_CORE::RhiFormat::R32Sint:
        case PC_CORE::RhiFormat::R32Sfloat:
            return 4;
        case PC_CORE::RhiFormat::R32G32Uint:
        case PC_CORE::RhiFormat::R32G32Sint:
        case PC_CORE::RhiFormat::R32G32Sfloat:
            return 8;
        case PC_CORE::RhiFormat::R32G32B32Uint:
        case PC_CORE::RhiFormat::R32G32B32Sint:
        case PC_CORE::RhiFormat::R32G32B32Sfloat:
            return 12;
        case PC_CORE::RhiFormat::R32G32B32A32Uint:
        case PC_CORE::RhiFormat::R32G32B32A32Sint:
        case PC_CORE::RhiFormat::R32G32B32A32Sfloat:
            return 16;
        case PC_CORE::RhiFormat::R64Uint:
            return 0;
        case PC_CORE::RhiFormat::R64Sint:
        case PC_CORE::RhiFormat::R64Sfloat:
            return 8;
        case PC_CORE::RhiFormat::R64G64Uint:
        case PC_CORE::RhiFormat::R64G64Sint:
        case PC_CORE::RhiFormat::R64G64Sfloat:
            return 16;
        case PC_CORE::RhiFormat::R64G64B64Uint:
        case PC_CORE::RhiFormat::R64G64B64Sint:
        case PC_CORE::RhiFormat::R64G64B64Sfloat:
            return 24;
        case PC_CORE::RhiFormat::R64G64B64A64Uint:
        case PC_CORE::RhiFormat::R64G64B64A64Sint:
        case PC_CORE::RhiFormat::R64G64B64A64Sfloat:
            return 32;
        case PC_CORE::RhiFormat::B10G11R11UfloatPack32:
            return 0;
        case PC_CORE::RhiFormat::E5B9G9R9UfloatPack32:
            return 0;
        case PC_CORE::RhiFormat::D16Unorm:
            return 4;
        case PC_CORE::RhiFormat::X8D24UnormPack32:
            return 0;
        case PC_CORE::RhiFormat::D32Sfloat:
            return 8;
        case PC_CORE::RhiFormat::S8Uint:
            return 1;
        case PC_CORE::RhiFormat::D16UnormS8Uint:
            return 4;
        case PC_CORE::RhiFormat::D24UnormS8Uint:
            return 4;
        case PC_CORE::RhiFormat::D32SfloatS8Uint:
            return 8;
        default:
            return static_cast<uint32_t>(-1);
        }

        return static_cast<uint32_t>(-1);
    }
#pragma endregion GetBytePerPixel


    enum class ComponentSwizzle : uint8_t
    {
        ComponentSwizzleIdentity = 0,
        ComponentSwizzleZero = 1,
        ComponentSwizzleOne = 2,
        ComponentSwizzleR = 3,
        ComponentSwizzleG = 4,
        ComponentSwizzleB = 5,
        ComponentSwizzleA = 6,
        ComponentSwizzleMaxEnum = std::numeric_limits<uint8_t>::max()
    };

    REFLECT(ComponentSwizzle)

    struct ComponentMapping
    {
        ComponentSwizzle R;
        ComponentSwizzle G;
        ComponentSwizzle B;
        ComponentSwizzle A;
    };

    REFLECT(ComponentMapping)

    enum class RhiChannel : uint8_t
        {
            Default = 0,

            Grey = 1,
            Alpha = 2,
            Rgb = 3,
            Rgba = 4
        };
    REFLECT(RhiChannel)

    enum class Filter
    {
        Nearest,
        Linear,
    };

    REFLECT(Filter)

    enum class SamplerMipmapMode
    {
        Nearest = 0,
        Linear = 1,
    };

    REFLECT(SamplerMipmapMode)

    enum class SamplerAddressMode
    {
        Repeat = 0,
        MirroredRepeat = 1,
        ClampToEdge = 2,
        ClampToBorder = 3,
        MirrorClampToEdge = 4,
    };

    REFLECT(SamplerAddressMode)


    enum class CompareOp
    {
        Never = 0,
        Less = 1,
        Equal = 2,
        LessOrEqual = 3,
        Greater = 4,
        NotEqual = 5,
        GreaterOrEqual = 6,
        Always = 7,
    };

    REFLECT(CompareOp)

    enum class BorderColor
    {
        FloatTransparentBlack = 0,
        IntTransparentBlack = 1,
        FloatOpaqueBlack = 2,
        IntOpaqueBlack = 3,
        FloatOpaqueWhite = 4,
        IntOpaqueWhite = 5,
        FloatCustomExt = 1000287003,
        IntCustomExt
    };

    REFLECT(BorderColor)

    enum struct LoadOperation : uint8_t
    {
        Load,
        Clear,
        DontCare,
    };

    REFLECT(LoadOperation)

    enum struct StoreOperation : uint8_t
    {
        Store,
        DontCare,
    };

    REFLECT(StoreOperation)


     enum class GpuPipelineStage : uint64_t
    {
        None = 0,

        // Core pipeline stages
        TopOfPipe = 1ULL << 0,
        DrawIndirect = 1ULL << 1,
        VertexInput = 1ULL << 2,

        VertexShader = 1ULL << 3,
        TessellationControlShader = 1ULL << 4,
        TessellationEvalShader = 1ULL << 5,
        GeometryShader = 1ULL << 6,
        FragmentShader = 1ULL << 7,

        EarlyFragmentTests = 1ULL << 8,
        LateFragmentTests = 1ULL << 9,
        ColorAttachmentOutput = 1ULL << 10,

        ComputeShader = 1ULL << 11,
        Transfer = 1ULL << 12,
        BottomOfPipe = 1ULL << 13,
        Host = 1ULL << 14,

        // Aggregate stages
        AllGraphics = 1ULL << 15,
        AllCommands = 1ULL << 16,

     
        TransformFeedback = 1ULL << 17,
        ConditionalRendering = 1ULL << 18,

        AccelerationStructureBuild = 1ULL << 19,
        RayTracingShader = 1ULL << 20,

        FragmentDensityProcess = 1ULL << 21,
        FragmentShadingRate = 1ULL << 22,

        CommandPreprocess = 1ULL << 23,

        TaskShader = 1ULL << 24,
        MeshShader = 1ULL << 25,
    };

    ENUM_FLAGS(GpuPipelineStage)


    enum class VertexInputRate
    {
        Vertex = 0,
        Instance = 1,

        Count
    };

    struct VertexInputBindingDescrition
    {
        uint32_t Binding = 0;
        uint32_t Stride = 0;
        VertexInputRate VertexInputRate = VertexInputRate::Vertex;
    };

    struct VertexAttributeDescription
    {
        uint32_t Binding = 0;
        uint32_t Location = 0;
        RhiFormat Format = RhiFormat::Undefined;
        uint32_t Offset = 0;
    };

END_PCCORE


template <typename T, typename U>
T* SafeCastReinterpreCast(U* _ptr)
{
#ifdef DEBUG

    T* ptr = dynamic_cast<T*>(ptr);
    assert(ptr != nullptr);
    return ptr;
#else
    return reinterpret_cast<T*>(_ptr);
#endif // DEBUG
}

//TODO do bindles descritpro
// DescriptorSet
#define SCENE_DESCRIPTOR_SET 0
#define MATERIAL_DESCRIPTOR_SET 1
#define ENVIRONEMENT_DESCRIPTOR_SET 1

// Binding
// SCENE_DESCRIPTOR_SET
#define CAMERA_BINDING 0
#define LIGHTDATA_BINDING 1
#define FORWARD_SKYBOX_CUBEMAP 2
// MATERIAL_DESCRIPTOR_SET
#define ALBEDO_BINDING 0

// ENVIRONEMENT_DESCRIPTOR_SET
#define SKYBOX_BINDING 0

#define CAM_DEPTH_MAX 1.f
#define CAM_DEPTH_MIN 0.f

// SPRITE
#define SPRITE_SET 1
#define SPRITE_TEXTURE 0


// Deffered 
#define GBUFFER_SET 1
#define G_ALBEDO 0
#define G_NORMAL 1
#define G_ROUGNESS_METALLIC_AO 2
#define G_WORLD_POSITION 3

// PREPROCESSOR

#if defined(_DEBUG) || defined(PROFILING)
#define DEBUG_GPU_ON 1
#endif
