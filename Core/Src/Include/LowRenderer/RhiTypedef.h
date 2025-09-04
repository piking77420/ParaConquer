#pragma once

#include <Variant>
#include <Vector>
#include <Array>
#include <String>

#include "CoreHeader.hpp"
#include "Math/ToolboxTypedef.hpp"

constexpr const char* SHADER_CACHE_PATH = "ShaderCache/";



#define ALIGNAS_16 alignas(16)

constexpr int MAX_FRAMES_IN_FLIGHT = 3;
constexpr size_t MAX_COLOR_ATTACHMENTS = 7;

BEGIN_PCCORE
    enum class GraphicAPI
    {
        None,
        Vulkan,
#ifdef _WIN32
        D3d12,
#endif
        Count
    };

enum struct RhiExtension
{
    RayTracing,
    MeshShader,
    Count
};


    constexpr const char* ShaderCacheVulkanFolder = "SPRIV/";
    constexpr const char* ShaderCacheD3d12Folder = "DXIL/";


    struct CameraGpu
    {
        Tbx::Matrix4x4f view;
        Tbx::Matrix4x4f proj;
        Tbx::Matrix4x4f vp;
        Tbx::Matrix4x4f vpInv;
        Tbx::Matrix4x4f viewInv;
        Tbx::Matrix4x4f projInv;
        float time;
        float deltatime;
        float cameraNear;
        float cameraFar;
        Tbx::Vector3f cameraPos;
    };

    struct ALIGNAS_16 DrawObjectBufferGPU
    {
        Tbx::Matrix4x4f model;
    };

    struct ALIGNAS_16 PostProcessGpu
    {
        float gamma;
        float exposure;
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
        eSubtract,
        eReverseSubtract,
        eMin,
        eMax,
    };

    enum ColorComponent : uint8_t
    {
        None = 0,
        ColorComponent_R  = 1 << 0, 
        ColorComponent_G  = 1 << 1,
        ColorComponent_B  = 1 << 2, 
        ColorComponent_A  = 1 << 3  
    };
 

    enum class RHIFormat
    {
        UNDEFINED = 0,
        R4G4_UNORM_PACK8 = 1,
        R4G4B4A4_UNORM_PACK16 = 2,
        B4G4R4A4_UNORM_PACK16 = 3,
        R5G6B5_UNORM_PACK16 = 4,
        B5G6R5_UNORM_PACK16 = 5,
        R5G5B5A1_UNORM_PACK16 = 6,
        B5G5R5A1_UNORM_PACK16 = 7,
        A1R5G5B5_UNORM_PACK16 = 8,
        R8_UNORM = 9,
        R8_SNORM = 10,
        R8_USCALED = 11,
        R8_SSCALED = 12,
        R8_UINT = 13,
        R8_SINT = 14,
        R8_SRGB = 15,
        R8G8_UNORM = 16,
        R8G8_SNORM = 17,
        R8G8_USCALED = 18,
        R8G8_SSCALED = 19,
        R8G8_UINT = 20,
        R8G8_SINT = 21,
        R8G8_SRGB = 22,
        R8G8B8_UNORM = 23,
        R8G8B8_SNORM = 24,
        R8G8B8_USCALED = 25,
        R8G8B8_SSCALED = 26,
        R8G8B8_UINT = 27,
        R8G8B8_SINT = 28,
        R8G8B8_SRGB = 29,
        B8G8R8_UNORM = 30,
        B8G8R8_SNORM = 31,
        B8G8R8_USCALED = 32,
        B8G8R8_SSCALED = 33,
        B8G8R8_UINT = 34,
        B8G8R8_SINT = 35,
        B8G8R8_SRGB = 36,
        R8G8B8A8_UNORM = 37,
        R8G8B8A8_SNORM = 38,
        R8G8B8A8_USCALED = 39,
        R8G8B8A8_SSCALED = 40,
        R8G8B8A8_UINT = 41,
        R8G8B8A8_SINT = 42,
        R8G8B8A8_SRGB = 43,
        B8G8R8A8_UNORM = 44,
        B8G8R8A8_SNORM = 45,
        B8G8R8A8_USCALED = 46,
        B8G8R8A8_SSCALED = 47,
        B8G8R8A8_UINT = 48,
        B8G8R8A8_SINT = 49,
        B8G8R8A8_SRGB = 50,
        A8B8G8R8_UNORM_PACK32 = 51,
        A8B8G8R8_SNORM_PACK32 = 52,
        A8B8G8R8_USCALED_PACK32 = 53,
        A8B8G8R8_SSCALED_PACK32 = 54,
        A8B8G8R8_UINT_PACK32 = 55,
        A8B8G8R8_SINT_PACK32 = 56,
        A8B8G8R8_SRGB_PACK32 = 57,
        A2R10G10B10_UNORM_PACK32 = 58,
        A2R10G10B10_SNORM_PACK32 = 59,
        A2R10G10B10_USCALED_PACK32 = 60,
        A2R10G10B10_SSCALED_PACK32 = 61,
        A2R10G10B10_UINT_PACK32 = 62,
        A2R10G10B10_SINT_PACK32 = 63,
        A2B10G10R10_UNORM_PACK32 = 64,
        A2B10G10R10_SNORM_PACK32 = 65,
        A2B10G10R10_USCALED_PACK32 = 66,
        A2B10G10R10_SSCALED_PACK32 = 67,
        A2B10G10R10_UINT_PACK32 = 68,
        A2B10G10R10_SINT_PACK32 = 69,
        R16_UNORM = 70,
        R16_SNORM = 71,
        R16_USCALED = 72,
        R16_SSCALED = 73,
        R16_UINT = 74,
        R16_SINT = 75,
        R16_SFLOAT = 76,
        R16G16_UNORM = 77,
        R16G16_SNORM = 78,
        R16G16_USCALED = 79,
        R16G16_SSCALED = 80,
        R16G16_UINT = 81,
        R16G16_SINT = 82,
        R16G16_SFLOAT = 83,
        R16G16B16_UNORM = 84,
        R16G16B16_SNORM = 85,
        R16G16B16_USCALED = 86,
        R16G16B16_SSCALED = 87,
        R16G16B16_UINT = 88,
        R16G16B16_SINT = 89,
        R16G16B16_SFLOAT = 90,
        R16G16B16A16_UNORM = 91,
        R16G16B16A16_SNORM = 92,
        R16G16B16A16_USCALED = 93,
        R16G16B16A16_SSCALED = 94,
        R16G16B16A16_UINT = 95,
        R16G16B16A16_SINT = 96,
        R16G16B16A16_SFLOAT = 97,
        R32_UINT = 98,
        R32_SINT = 99,
        R32_SFLOAT = 100,
        R32G32_UINT = 101,
        R32G32_SINT = 102,
        R32G32_SFLOAT = 103,
        R32G32B32_UINT = 104,
        R32G32B32_SINT = 105,
        R32G32B32_SFLOAT = 106,
        R32G32B32A32_UINT = 107,
        R32G32B32A32_SINT = 108,
        R32G32B32A32_SFLOAT = 109,
        R64_UINT = 110,
        R64_SINT = 111,
        R64_SFLOAT = 112,
        R64G64_UINT = 113,
        R64G64_SINT = 114,
        R64G64_SFLOAT = 115,
        R64G64B64_UINT = 116,
        R64G64B64_SINT = 117,
        R64G64B64_SFLOAT = 118,
        R64G64B64A64_UINT = 119,
        R64G64B64A64_SINT = 120,
        R64G64B64A64_SFLOAT = 121,
        B10G11R11_UFLOAT_PACK32 = 122,
        E5B9G9R9_UFLOAT_PACK32 = 123,
        D16_UNORM = 124,
        X8_D24_UNORM_PACK32 = 125,
        D32_SFLOAT = 126,
        S8_UINT = 127,
        D16_UNORM_S8_UINT = 128,
        D24_UNORM_S8_UINT = 129,
        D32_SFLOAT_S8_UINT = 130,
        BC1_RGB_UNORM_BLOCK = 131,
        BC1_RGB_SRGB_BLOCK = 132,
        BC1_RGBA_UNORM_BLOCK = 133,
        BC1_RGBA_SRGB_BLOCK = 134,
        BC2_UNORM_BLOCK = 135,
        BC2_SRGB_BLOCK = 136,
        BC3_UNORM_BLOCK = 137,
        BC3_SRGB_BLOCK = 138,
        BC4_UNORM_BLOCK = 139,
        BC4_SNORM_BLOCK = 140,
        BC5_UNORM_BLOCK = 141,
        BC5_SNORM_BLOCK = 142,
        BC6H_UFLOAT_BLOCK = 143,
        BC6H_SFLOAT_BLOCK = 144,
        BC7_UNORM_BLOCK = 145,
        BC7_SRGB_BLOCK = 146,
        ETC2_R8G8B8_UNORM_BLOCK = 147,
        ETC2_R8G8B8_SRGB_BLOCK = 148,
        ETC2_R8G8B8A1_UNORM_BLOCK = 149,
        ETC2_R8G8B8A1_SRGB_BLOCK = 150,
        ETC2_R8G8B8A8_UNORM_BLOCK = 151,
        ETC2_R8G8B8A8_SRGB_BLOCK = 152,
        EAC_R11_UNORM_BLOCK = 153,
        EAC_R11_SNORM_BLOCK = 154,
        EAC_R11G11_UNORM_BLOCK = 155,
        EAC_R11G11_SNORM_BLOCK = 156,
        ASTC_4x4_UNORM_BLOCK = 157,
        ASTC_4x4_SRGB_BLOCK = 158,
        ASTC_5x4_UNORM_BLOCK = 159,
        ASTC_5x4_SRGB_BLOCK = 160,
        ASTC_5x5_UNORM_BLOCK = 161,
        ASTC_5x5_SRGB_BLOCK = 162,
        ASTC_6x5_UNORM_BLOCK = 163,
        ASTC_6x5_SRGB_BLOCK = 164,
        ASTC_6x6_UNORM_BLOCK = 165,
        ASTC_6x6_SRGB_BLOCK = 166,
        ASTC_8x5_UNORM_BLOCK = 167,
        ASTC_8x5_SRGB_BLOCK = 168,
        ASTC_8x6_UNORM_BLOCK = 169,
        ASTC_8x6_SRGB_BLOCK = 170,
        ASTC_8x8_UNORM_BLOCK = 171,
        ASTC_8x8_SRGB_BLOCK = 172,
        ASTC_10x5_UNORM_BLOCK = 173,
        ASTC_10x5_SRGB_BLOCK = 174,
        ASTC_10x6_UNORM_BLOCK = 175,
        ASTC_10x6_SRGB_BLOCK = 176,
        ASTC_10x8_UNORM_BLOCK = 177,
        ASTC_10x8_SRGB_BLOCK = 178,
        ASTC_10x10_UNORM_BLOCK = 179,
        ASTC_10x10_SRGB_BLOCK = 180,
        ASTC_12x10_UNORM_BLOCK = 181,
        ASTC_12x10_SRGB_BLOCK = 182,
        ASTC_12x12_UNORM_BLOCK = 183,
        ASTC_12x12_SRGB_BLOCK = 184,
        G8B8G8R8_422_UNORM = 1000156000,
        B8G8R8G8_422_UNORM = 1000156001,
        G8_B8_R8_3PLANE_420_UNORM = 1000156002,
        G8_B8R8_2PLANE_420_UNORM = 1000156003,
        G8_B8_R8_3PLANE_422_UNORM = 1000156004,
        G8_B8R8_2PLANE_422_UNORM = 1000156005,
        G8_B8_R8_3PLANE_444_UNORM = 1000156006,
        R10X6_UNORM_PACK16 = 1000156007,
        R10X6G10X6_UNORM_2PACK16 = 1000156008,
        R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
        G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
        B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
        G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
        G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
        G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
        G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
        G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
        R12X4_UNORM_PACK16 = 1000156017,
        R12X4G12X4_UNORM_2PACK16 = 1000156018,
        R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
        G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
        B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
        G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
        G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
        G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
        G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
        G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
        G16B16G16R16_422_UNORM = 1000156027,
        B16G16R16G16_422_UNORM = 1000156028,
        G16_B16_R16_3PLANE_420_UNORM = 1000156029,
        G16_B16R16_2PLANE_420_UNORM = 1000156030,
        G16_B16_R16_3PLANE_422_UNORM = 1000156031,
        G16_B16R16_2PLANE_422_UNORM = 1000156032,
        G16_B16_R16_3PLANE_444_UNORM = 1000156033,
        G8_B8R8_2PLANE_444_UNORM = 1000330000,
        G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16 = 1000330001,
        G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16 = 1000330002,
        G16_B16R16_2PLANE_444_UNORM = 1000330003,
        A4R4G4B4_UNORM_PACK16 = 1000340000,
        A4B4G4R4_UNORM_PACK16 = 1000340001,
        ASTC_4x4_SFLOAT_BLOCK = 1000066000,
        ASTC_5x4_SFLOAT_BLOCK = 1000066001,
        ASTC_5x5_SFLOAT_BLOCK = 1000066002,
        ASTC_6x5_SFLOAT_BLOCK = 1000066003,
        ASTC_6x6_SFLOAT_BLOCK = 1000066004,
        ASTC_8x5_SFLOAT_BLOCK = 1000066005,
        ASTC_8x6_SFLOAT_BLOCK = 1000066006,
        ASTC_8x8_SFLOAT_BLOCK = 1000066007,
        ASTC_10x5_SFLOAT_BLOCK = 1000066008,
        ASTC_10x6_SFLOAT_BLOCK = 1000066009,
        ASTC_10x8_SFLOAT_BLOCK = 1000066010,
        ASTC_10x10_SFLOAT_BLOCK = 1000066011,
        ASTC_12x10_SFLOAT_BLOCK = 1000066012,
        ASTC_12x12_SFLOAT_BLOCK = 1000066013,
        PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
        PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
        PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
        PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
        PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
        PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
        PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
        PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
        R16G16_S10_5_NV = 1000464000,
        A1B5G5R5_UNORM_PACK16_KHR = 1000470000,
        A8_UNORM_KHR = 1000470001,
        ASTC_4x4_SFLOAT_BLOCK_EXT = ASTC_4x4_SFLOAT_BLOCK,
        ASTC_5x4_SFLOAT_BLOCK_EXT = ASTC_5x4_SFLOAT_BLOCK,
        ASTC_5x5_SFLOAT_BLOCK_EXT = ASTC_5x5_SFLOAT_BLOCK,
        ASTC_6x5_SFLOAT_BLOCK_EXT = ASTC_6x5_SFLOAT_BLOCK,
        ASTC_6x6_SFLOAT_BLOCK_EXT = ASTC_6x6_SFLOAT_BLOCK,
        ASTC_8x5_SFLOAT_BLOCK_EXT = ASTC_8x5_SFLOAT_BLOCK,
        ASTC_8x6_SFLOAT_BLOCK_EXT = ASTC_8x6_SFLOAT_BLOCK,
        ASTC_8x8_SFLOAT_BLOCK_EXT = ASTC_8x8_SFLOAT_BLOCK,
        ASTC_10x5_SFLOAT_BLOCK_EXT = ASTC_10x5_SFLOAT_BLOCK,
        ASTC_10x6_SFLOAT_BLOCK_EXT = ASTC_10x6_SFLOAT_BLOCK,
        ASTC_10x8_SFLOAT_BLOCK_EXT = ASTC_10x8_SFLOAT_BLOCK,
        ASTC_10x10_SFLOAT_BLOCK_EXT = ASTC_10x10_SFLOAT_BLOCK,
        ASTC_12x10_SFLOAT_BLOCK_EXT = ASTC_12x10_SFLOAT_BLOCK,
        ASTC_12x12_SFLOAT_BLOCK_EXT = ASTC_12x12_SFLOAT_BLOCK,
        G8B8G8R8_422_UNORM_KHR = G8B8G8R8_422_UNORM,
        B8G8R8G8_422_UNORM_KHR = B8G8R8G8_422_UNORM,
        G8_B8_R8_3PLANE_420_UNORM_KHR = G8_B8_R8_3PLANE_420_UNORM,
        G8_B8R8_2PLANE_420_UNORM_KHR = G8_B8R8_2PLANE_420_UNORM,
        G8_B8_R8_3PLANE_422_UNORM_KHR = G8_B8_R8_3PLANE_422_UNORM,
        G8_B8R8_2PLANE_422_UNORM_KHR = G8_B8R8_2PLANE_422_UNORM,
        G8_B8_R8_3PLANE_444_UNORM_KHR = G8_B8_R8_3PLANE_444_UNORM,
        R10X6_UNORM_PACK16_KHR = R10X6_UNORM_PACK16,
        R10X6G10X6_UNORM_2PACK16_KHR = R10X6G10X6_UNORM_2PACK16,
        R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR = R10X6G10X6B10X6A10X6_UNORM_4PACK16,
        G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR = G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
        B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR = B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
        G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR = G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
        G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR = G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
        G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR = G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
        G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR = G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
        G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR = G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
        R12X4_UNORM_PACK16_KHR = R12X4_UNORM_PACK16,
        R12X4G12X4_UNORM_2PACK16_KHR = R12X4G12X4_UNORM_2PACK16,
        R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR = R12X4G12X4B12X4A12X4_UNORM_4PACK16,
        G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR = G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
        B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR = B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
        G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR = G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
        G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR = G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
        G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR = G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
        G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR = G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
        G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR = G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
        G16B16G16R16_422_UNORM_KHR = G16B16G16R16_422_UNORM,
        B16G16R16G16_422_UNORM_KHR = B16G16R16G16_422_UNORM,
        G16_B16_R16_3PLANE_420_UNORM_KHR = G16_B16_R16_3PLANE_420_UNORM,
        G16_B16R16_2PLANE_420_UNORM_KHR = G16_B16R16_2PLANE_420_UNORM,
        G16_B16_R16_3PLANE_422_UNORM_KHR = G16_B16_R16_3PLANE_422_UNORM,
        G16_B16R16_2PLANE_422_UNORM_KHR = G16_B16R16_2PLANE_422_UNORM,
        G16_B16_R16_3PLANE_444_UNORM_KHR = G16_B16_R16_3PLANE_444_UNORM,
        G8_B8R8_2PLANE_444_UNORM_EXT = G8_B8R8_2PLANE_444_UNORM,
        G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT = G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16,
        G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT = G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16,
        G16_B16R16_2PLANE_444_UNORM_EXT = G16_B16R16_2PLANE_444_UNORM,
        A4R4G4B4_UNORM_PACK16_EXT = A4R4G4B4_UNORM_PACK16,
        A4B4G4R4_UNORM_PACK16_EXT = A4B4G4R4_UNORM_PACK16,

        COUNT = 1000340002
    };

#pragma endregion

    enum class ShaderStageTypeFlag : size_t
    {
        Vertex,
        Hull,
        Domain,
        Geometry,
        Pixel,
        Compute,
        Raygen,
        Intersect,
        Anyhit,
        Closesthit,
        Miss,
        Callable,
        Task,
        Mesh,

        Count
    };

    ENUM_FLAGS(ShaderStageTypeFlag)

   

    enum struct MemoryUsage
    {
        Static, // Not modified over its lifetime
        Mutable, // Occasionally modified (e.g., once per frame)
        Dynamic, // Frequently modified (e.g., multiple times per frame)

        Count // Total enum values
    };

    enum struct MemoryLocalisation
    {
        GPU_Only, // Device-local
        CPU_Only, // Host Only
        CPU_To_GPU, // Host-visible (upload)
        GPU_To_CPU, // Host-readable (readback)

        Count // Total enum values
    };


    enum class IndexFormat : int
    {
        Uiunt8 = 1,
        Uint16 = 2,
        Uint32 = 4
    };

#pragma region Image

    enum class Channel
    {
        DEFAULT = 0,

        GREY = 1,
        ALPHA = 2,
        RGB = 3,
        RGBA = 4
    };

    enum class ComponentSwizzle
    {
        COMPONENT_SWIZZLE_IDENTITY = 0,
        COMPONENT_SWIZZLE_ZERO = 1,
        COMPONENT_SWIZZLE_ONE = 2,
        COMPONENT_SWIZZLE_R = 3,
        COMPONENT_SWIZZLE_G = 4,
        COMPONENT_SWIZZLE_B = 5,
        COMPONENT_SWIZZLE_A = 6,
        COMPONENT_SWIZZLE_MAX_ENUM = 0x7FFFFFFF
    };

    struct ComponentMapping
    {
        ComponentSwizzle r;
        ComponentSwizzle g;
        ComponentSwizzle b;
        ComponentSwizzle a;
    };


    enum class AttachmentType
    {
        None,
        Color = 1,
        Depth,
        Stencil,
        DepthStencil,
    };

    enum class TextureUsage : uint8_t
    {
        None = 0,
        Sampled = 1 << 0, // Shader-readable (SRV)
        RenderTarget = 1 << 1, // Color attachment (ex: RGBA render target)
        Depth = 1 << 2, // Depth attachment
        Stencil = 1 << 3, // Stencil attachment
        Storage = 1 << 4, // Shader-writable (UAV)

        //vk::eMetadata for Virtual texturing TODO reasearch

        All = Sampled | RenderTarget | Depth | Stencil | Storage,
    };


    inline TextureUsage operator|(TextureUsage a, TextureUsage b)
    {
        return static_cast<TextureUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline TextureUsage operator&(TextureUsage a, TextureUsage b)
    {
        return static_cast<TextureUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline bool HasUsage(TextureUsage usage, TextureUsage flag)
    {
        return (static_cast<uint32_t>(usage) & static_cast<uint32_t>(flag)) != 0;
    }

    // may in future rename as resoure state like D3D12
    enum class ImageState : uint8_t
    {
        Undefined,
        General,
        RenderTargetOptimal,
        DepthStencilOptimal,
        DepthStencilReadOptimal,
        ShaderReadOptimal,
        TransferSrcOptimal,
        TransferDstOptimal,

        Count,
    };
    static_assert((uint8_t)ImageState::Count < 255, "Out of bound enum");

    enum class TextureType
    {
        Texture2D,
        TextureArray2D,
        CubeMap,
        CubeMapArray,
        Count,
    };


    struct CreateImageInfo
    {
        int32_t width;
        int32_t height;
        int32_t depth;
        uint32_t layerCount;
        uint32_t mipsLevels;

        TextureType textureType;
        RHIFormat format;
        Channel channel;

        TextureUsage textureUsage;
        MemoryLocalisation memoryVisibility;

        uint32_t samples;
        bool GenerateMipMap = false;
        std::vector<void*> datas;
    };


    enum class Filter
    {
        NEAREST,
        LINEAR,
        CUBIC_IMG,
        CUBIC_EXT
    };

    enum class SamplerMipmapMode
    {
        NEAREST = 0,
        LINEAR = 1,
    };

    enum class SamplerAddressMode
    {
        REPEAT = 0,
        MIRRORED_REPEAT = 1,
        CLAMP_TO_EDGE = 2,
        CLAMP_TO_BORDER = 3,
        MIRROR_CLAMP_TO_EDGE = 4,
    };

    enum class CompareOp
    {
        NEVER = 0,
        LESS = 1,
        EQUAL = 2,
        LESS_OR_EQUAL = 3,
        GREATER = 4,
        NOT_EQUAL = 5,
        GREATER_OR_EQUAL = 6,
        ALWAYS = 7,
    };

    enum class BorderColor
    {
        FLOAT_TRANSPARENT_BLACK = 0,
        INT_TRANSPARENT_BLACK = 1,
        FLOAT_OPAQUE_BLACK = 2,
        INT_OPAQUE_BLACK = 3,
        FLOAT_OPAQUE_WHITE = 4,
        INT_OPAQUE_WHITE = 5,
        FLOAT_CUSTOM_EXT = 1000287003,
        INT_CUSTOM_EXT
    };


#pragma endregion


#pragma region RenderPass


#pragma endregion RenderPass


    enum struct LoadOperation
    {
        Load,
        Clear,
        DontCare,
    };

    enum struct StoreOperation
    {
        Store,
        DontCare,
    };

    enum class ShaderProgramPipelineType
    {
        POINT_GRAPHICS,
        COMPUTE,
        RAYTRACING,

        COUT
    };

    enum class GpuPipelineStageFlagBits : uint64_t
    {
        None = 0,
        TopOfPipe = 1ULL << 0,
        DrawIndirect = 1ULL << 1,
        VertexInput = 1ULL << 2,
        VertexShader = 1ULL << 3,
        TessellationControlShader = 1ULL << 4,
        TessellationEvaluationShader = 1ULL << 5,
        GeometryShader = 1ULL << 6,
        FragmentShader = 1ULL << 7,
        EarlyFragmentTests = 1ULL << 8,
        LateFragmentTests = 1ULL << 9,
        ColorAttachmentOutput = 1ULL << 10,
        ComputeShader = 1ULL << 11,
        Transfer = 1ULL << 12,
        BottomOfPipe = 1ULL << 13,
        Host = 1ULL << 14,
        AllGraphics = 1ULL << 15,
        AllCommands = 1ULL << 16,
        NoneKHR = 1ULL << 17,
        TransformFeedbackEXT = 1ULL << 18,
        ConditionalRenderingEXT = 1ULL << 19,
        AccelerationStructureBuildKHR = 1ULL << 20,
        AccelerationStructureBuildNV = 1ULL << 21,
        RayTracingShaderKHR = 1ULL << 22,
        RayTracingShaderNV = 1ULL << 23,
        FragmentDensityProcessEXT = 1ULL << 24,
        FragmentShadingRateAttachmentKHR = 1ULL << 25,
        ShadingRateImageNV = 1ULL << 26,
        CommandPreprocessNV = 1ULL << 27,
        CommandPreprocessEXT = 1ULL << 28,
        TaskShaderEXT = 1ULL << 29,
        TaskShaderNV = 1ULL << 30,
        MeshShaderEXT = 1ULL << 31,
        MeshShaderNV = 1ULL << 32
    };

    ENUM_FLAGS(GpuPipelineStageFlagBits)

    //----------------------------------------

    enum class GpuAccessFlag : uint64_t
    {
        None = 0,
        IndirectCommandRead = 1ULL << 0,
        IndexRead = 1ULL << 1,
        VertexAttributeRead = 1ULL << 2,
        UniformRead = 1ULL << 3,
        InputAttachmentRead = 1ULL << 4,
        ShaderRead = 1ULL << 5,
        ShaderWrite = 1ULL << 6,
        ColorAttachmentRead = 1ULL << 7,
        ColorAttachmentWrite = 1ULL << 8,
        DepthStencilAttachmentRead = 1ULL << 9,
        DepthStencilAttachmentWrite = 1ULL << 10,
        TransferRead = 1ULL << 11,
        TransferWrite = 1ULL << 12,
        HostRead = 1ULL << 13,
        HostWrite = 1ULL << 14,
        MemoryRead = 1ULL << 15,
        MemoryWrite = 1ULL << 16,
        NoneKHR = 1ULL << 17,
        TransformFeedbackWriteEXT = 1ULL << 18,
        TransformFeedbackCounterReadEXT = 1ULL << 19,
        TransformFeedbackCounterWriteEXT = 1ULL << 20,
        ConditionalRenderingReadEXT = 1ULL << 21,
        ColorAttachmentReadNoncoherentEXT = 1ULL << 22,
        AccelerationStructureReadKHR = 1ULL << 23,
        AccelerationStructureReadNV = 1ULL << 24,
        AccelerationStructureWriteKHR = 1ULL << 25,
        AccelerationStructureWriteNV = 1ULL << 26,
        FragmentDensityMapReadEXT = 1ULL << 27,
        FragmentShadingRateAttachmentReadKHR = 1ULL << 28,
        ShadingRateImageReadNV = 1ULL << 29,
        CommandPreprocessReadNV = 1ULL << 30,
        CommandPreprocessReadEXT = 1ULL << 31,
        CommandPreprocessWriteNV = 1ULL << 32,
        CommandPreprocessWriteEXT = 1ULL << 33
    };

    ENUM_FLAGS(GpuAccessFlag)
   

  


END_PCCORE


template <typename T, typename U>
inline T* SafeCastReinterpreCast(U* ptr)
{
#ifdef DEBUG

    T* ptr = dynamic_cast<T*>(ptr);
    assert(ptr != nullptr);
    return ptr;
#else
    return reinterpret_cast<T*>(ptr);

#endif // DEBUG
}


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
