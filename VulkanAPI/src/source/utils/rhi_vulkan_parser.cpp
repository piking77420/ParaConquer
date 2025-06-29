#include "utils/rhi_vulkan_parser.hpp"

#pragma region Format
vk::Format Vulkan::Utils::RHIFormatToVkFormat(PC_CORE::RHIFormat rhiFormat)
{
    vk::Format format = {};
    // TODO MAKE ALL FORMAT

    switch (rhiFormat)
    {
    case PC_CORE::RHIFormat::UNDEFINED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R4G4_UNORM_PACK8:
        return vk::Format::eR4G4UnormPack8;

    case PC_CORE::RHIFormat::R4G4B4A4_UNORM_PACK16:
        return vk::Format::eR4G4UnormPack8;

    case PC_CORE::RHIFormat::B4G4R4A4_UNORM_PACK16:
        return vk::Format::eB4G4R4A4UnormPack16;

    case PC_CORE::RHIFormat::R5G6B5_UNORM_PACK16:
        return vk::Format::eR5G6B5UnormPack16;

    case PC_CORE::RHIFormat::B5G6R5_UNORM_PACK16:
        return vk::Format::eB5G6R5UnormPack16;

    case PC_CORE::RHIFormat::R5G5B5A1_UNORM_PACK16:
        return vk::Format::eR5G5B5A1UnormPack16;

    case PC_CORE::RHIFormat::B5G5R5A1_UNORM_PACK16:
        return vk::Format::eB5G5R5A1UnormPack16;

    case PC_CORE::RHIFormat::A1R5G5B5_UNORM_PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8_UNORM:
        return vk::Format::eR8Unorm;

    case PC_CORE::RHIFormat::R8_SNORM:
        return vk::Format::eR8Snorm;

    case PC_CORE::RHIFormat::R8_USCALED:
        return vk::Format::eR8Uscaled;

    case PC_CORE::RHIFormat::R8_SSCALED:
        return vk::Format::eR8Sscaled;

    case PC_CORE::RHIFormat::R8_UINT:
        return vk::Format::eR8Uint;

    case PC_CORE::RHIFormat::R8_SINT:
        return vk::Format::eR8Sint;

    case PC_CORE::RHIFormat::R8_SRGB:
        return vk::Format::eR8Srgb;

    case PC_CORE::RHIFormat::R8G8_UNORM:
        return vk::Format::eR8G8Unorm;

    case PC_CORE::RHIFormat::R8G8_SNORM:
        return vk::Format::eR8G8Snorm;

    case PC_CORE::RHIFormat::R8G8_USCALED:
        return vk::Format::eR8G8Uscaled;

    case PC_CORE::RHIFormat::R8G8_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_SRGB:
        return vk::Format::eR8G8Srgb;

    case PC_CORE::RHIFormat::R8G8B8_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8_SRGB:
        return vk::Format::eR8G8B8Srgb;

    case PC_CORE::RHIFormat::B8G8R8_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8_SRGB:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8A8_UNORM:
        return vk::Format::eR8G8B8A8Unorm;

    case PC_CORE::RHIFormat::R8G8B8A8_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8A8_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8A8_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8A8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8A8_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8B8A8_SRGB:
        return vk::Format::eR8G8B8A8Srgb;

    case PC_CORE::RHIFormat::B8G8R8A8_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8A8_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8A8_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8A8_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8A8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8A8_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8A8_SRGB:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A8B8G8R8_UNORM_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A8B8G8R8_SNORM_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A8B8G8R8_USCALED_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A8B8G8R8_SSCALED_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A8B8G8R8_UINT_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A8B8G8R8_SINT_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A8B8G8R8_SRGB_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2R10G10B10_UNORM_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2R10G10B10_SNORM_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2R10G10B10_USCALED_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2R10G10B10_SSCALED_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2R10G10B10_UINT_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2R10G10B10_SINT_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2B10G10R10_UNORM_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2B10G10R10_SNORM_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2B10G10R10_USCALED_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2B10G10R10_SSCALED_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2B10G10R10_UINT_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A2B10G10R10_SINT_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16A16_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16A16_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16A16_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16A16_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16A16_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16A16_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16B16A16_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32G32_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32G32_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32G32_SFLOAT:
        return vk::Format::eR32G32Sfloat;

    case PC_CORE::RHIFormat::R32G32B32_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32G32B32_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32G32B32_SFLOAT:
        return vk::Format::eR32G32B32Sfloat;

    case PC_CORE::RHIFormat::R32G32B32A32_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32G32B32A32_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R32G32B32A32_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64G64_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64G64_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64G64_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64G64B64_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64G64B64_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64G64B64_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64G64B64A64_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64G64B64A64_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R64G64B64A64_SFLOAT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B10G11R11_UFLOAT_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::E5B9G9R9_UFLOAT_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::D16_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::X8_D24_UNORM_PACK32:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::D32_SFLOAT:
        return vk::Format::eD32Sfloat;

    case PC_CORE::RHIFormat::S8_UINT:
        return vk::Format::eS8Uint;

    case PC_CORE::RHIFormat::D16_UNORM_S8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::D24_UNORM_S8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::D32_SFLOAT_S8_UINT:
        return vk::Format::eD32SfloatS8Uint;

    case PC_CORE::RHIFormat::BC1_RGB_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC1_RGB_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC1_RGBA_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC1_RGBA_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC2_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC2_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC3_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC3_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC4_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC4_SNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC5_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC5_SNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC6H_UFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC6H_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC7_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::BC7_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ETC2_R8G8B8_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ETC2_R8G8B8_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ETC2_R8G8B8A1_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ETC2_R8G8B8A1_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ETC2_R8G8B8A8_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ETC2_R8G8B8A8_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::EAC_R11_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::EAC_R11_SNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::EAC_R11G11_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::EAC_R11G11_SNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_4x4_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_4x4_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_5x4_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_5x4_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_5x5_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_5x5_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_6x5_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_6x5_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_6x6_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_6x6_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_8x5_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_8x5_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_8x6_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_8x6_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_8x8_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_8x8_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x5_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x5_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x6_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x6_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x8_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x8_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x10_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x10_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_12x10_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_12x10_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_12x12_UNORM_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_12x12_SRGB_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G8B8G8R8_422_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B8G8R8G8_422_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G8_B8_R8_3PLANE_420_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G8_B8R8_2PLANE_420_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G8_B8_R8_3PLANE_422_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G8_B8R8_2PLANE_422_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G8_B8_R8_3PLANE_444_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R10X6_UNORM_PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R10X6G10X6_UNORM_2PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R10X6G10X6B10X6A10X6_UNORM_4PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R12X4_UNORM_PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R12X4G12X4_UNORM_2PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R12X4G12X4B12X4A12X4_UNORM_4PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G16B16G16R16_422_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::B16G16R16G16_422_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G16_B16_R16_3PLANE_420_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G16_B16R16_2PLANE_420_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G16_B16_R16_3PLANE_422_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G16_B16R16_2PLANE_422_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G16_B16_R16_3PLANE_444_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G8_B8R8_2PLANE_444_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::G16_B16R16_2PLANE_444_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A4R4G4B4_UNORM_PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A4B4G4R4_UNORM_PACK16:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_4x4_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_5x4_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_5x5_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_6x5_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_6x6_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_8x5_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_8x6_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_8x8_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x5_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x6_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x8_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_10x10_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_12x10_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::ASTC_12x12_SFLOAT_BLOCK:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::PVRTC1_2BPP_UNORM_BLOCK_IMG:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::PVRTC1_4BPP_UNORM_BLOCK_IMG:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::PVRTC2_2BPP_UNORM_BLOCK_IMG:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::PVRTC2_4BPP_UNORM_BLOCK_IMG:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::PVRTC1_2BPP_SRGB_BLOCK_IMG:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::PVRTC1_4BPP_SRGB_BLOCK_IMG:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::PVRTC2_2BPP_SRGB_BLOCK_IMG:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::PVRTC2_4BPP_SRGB_BLOCK_IMG:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R16G16_S10_5_NV:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A1B5G5R5_UNORM_PACK16_KHR:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::A8_UNORM_KHR:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::COUNT:
        break;
    }

    return format;
}
#pragma endregion Format

/*
vk::DescriptorType Vulkan::Utils::RHIDescriptorTypeToVulkan(PC_CORE::DescriptorType _descriptorType)
{
    vk::DescriptorType result = {};
    switch (_descriptorType)
    {
    case PC_CORE::DescriptorType::SAMPLER:
        result = vk::DescriptorType::eSampler;
        break;
    case PC_CORE::DescriptorType::COMBINED_IMAGE_SAMPLER:
        result = vk::DescriptorType::eCombinedImageSampler;
        break;
    case PC_CORE::DescriptorType::SAMPLED_IMAGE:
        result = vk::DescriptorType::eSampledImage;
        break;
    case PC_CORE::DescriptorType::STORAGE_IMAGE:
        result = vk::DescriptorType::eStorageImage;
        break;
    case PC_CORE::DescriptorType::UNIFORM_TEXEL_BUFFER:
        result = vk::DescriptorType::eUniformTexelBuffer;
        break;
    case PC_CORE::DescriptorType::STORAGE_TEXEL_BUFFER:
        result = vk::DescriptorType::eStorageTexelBuffer;
        break;
    case PC_CORE::DescriptorType::UNIFORM_BUFFER:
        result = vk::DescriptorType::eUniformBuffer;
        break;
    case PC_CORE::DescriptorType::STORAGE_BUFFER:
        result = vk::DescriptorType::eStorageBuffer;
        break;
    case PC_CORE::DescriptorType::UNIFORM_BUFFER_DYNAMIC:
        result = vk::DescriptorType::eUniformBufferDynamic;
        break;
    case PC_CORE::DescriptorType::STORAGE_BUFFER_DYNAMIC:
        result = vk::DescriptorType::eStorageBufferDynamic;
        break;
    case PC_CORE::DescriptorType::INPUT_ATTACHMENT:
        result = vk::DescriptorType::eInputAttachment;
        break;
    case PC_CORE::DescriptorType::INLINE_UNIFORM_BLOCK:
        result = vk::DescriptorType::eInlineUniformBlock;
        break;
    case PC_CORE::DescriptorType::ACCELERATION_STRUCTURE_KHR:
        result = vk::DescriptorType::eAccelerationStructureKHR;
        break;
    case PC_CORE::DescriptorType::ACCELERATION_STRUCTURE_NV:
        result = vk::DescriptorType::eAccelerationStructureNV;
        break;
    case PC_CORE::DescriptorType::SAMPLE_WEIGHT_IMAGE_QCOM:
        result = vk::DescriptorType::eSampleWeightImageQCOM;
        break;
    case PC_CORE::DescriptorType::BLOCK_MATCH_IMAGE_QCOM:
        result = vk::DescriptorType::eBlockMatchImageQCOM;

        break;
    case PC_CORE::DescriptorType::MUTABLE_EXT:
        result = vk::DescriptorType::eMutableEXT;
        break;
    case PC_CORE::DescriptorType::COUNT:
        break;
    }

    return result;
}*/


vk::PipelineBindPoint Vulkan::Utils::RhiPipelineBindPointToVulkan(
    PC_CORE::ShaderProgramPipelineType _shaderProgramPipelineType)
{
    switch (_shaderProgramPipelineType)
    {
    case PC_CORE::ShaderProgramPipelineType::POINT_GRAPHICS:
        return vk::PipelineBindPoint::eGraphics;
    case PC_CORE::ShaderProgramPipelineType::COMPUTE:
        return vk::PipelineBindPoint::eCompute;
    case PC_CORE::ShaderProgramPipelineType::RAYTRACING:
        return vk::PipelineBindPoint::eRayTracingKHR;
    }
}


vk::VertexInputRate Vulkan::Utils::RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate)
{
    switch (_vertexInputRate)
    {
    case PC_CORE::VertexInputRate::VERTEX:
        return vk::VertexInputRate::eVertex;
    case PC_CORE::VertexInputRate::INSTANCE:
        return vk::VertexInputRate::eInstance;
    case PC_CORE::VertexInputRate::COUNT:
        throw std::invalid_argument("VertexInputRate::COUNT VertexInputRate");
    default: ;
        throw std::invalid_argument("Invalid VertexInputRate");
    }
}

/*
vk::ImageType Vulkan::Utils::RHIImageToVkImageType(PC_CORE::ImageType _imageType)
{
    switch (_imageType)
    {
    case PC_CORE::ImageType::TYPE_1D:
        return vk::ImageType::e1D;
    case PC_CORE::ImageType::TYPE_2D:
        return vk::ImageType::e2D;
    case PC_CORE::ImageType::TYPE_e3D:
        return vk::ImageType::e3D;
    case PC_CORE::ImageType::TYPE_CUBE:
        return vk::ImageType::e2D;
    case PC_CORE::ImageType::TYPE_1DARRAY:
        return vk::ImageType::e2D;
    case PC_CORE::ImageType::TYPE_2DARRAY:
        return vk::ImageType::e3D;
    case PC_CORE::ImageType::TYPE_CUBEARRAY:
        return vk::ImageType::e3D;
    }
    return vk::ImageType::e1D;
}


vk::ImageViewType Vulkan::Utils::RHIImageTypeToVulkanImageViewType(PC_CORE::ImageType _imageViewType)
{
    switch (_imageViewType)
    {
    case PC_CORE::ImageType::TYPE_1D:
        return vk::ImageViewType::e1D;
    case PC_CORE::ImageType::TYPE_2D:
        return vk::ImageViewType::e2D;
    case PC_CORE::ImageType::TYPE_e3D:
        return vk::ImageViewType::e3D;
    case PC_CORE::ImageType::TYPE_CUBE:
        return vk::ImageViewType::eCube;
    case PC_CORE::ImageType::TYPE_1DARRAY:
        return vk::ImageViewType::e1DArray;
    case PC_CORE::ImageType::TYPE_2DARRAY:
        return vk::ImageViewType::e2DArray;
    case PC_CORE::ImageType::TYPE_CUBEARRAY:
        return vk::ImageViewType::eCubeArray;
    }

    throw std::runtime_error("Unknown ImageViewType");
}
*/

vk::Filter Vulkan::Utils::RHIToVulkanFilter(PC_CORE::Filter _filter)
{
    switch (_filter)
    {
    case PC_CORE::Filter::NEAREST:
        return vk::Filter::eNearest;
    case PC_CORE::Filter::LINEAR:
        return vk::Filter::eLinear;
    case PC_CORE::Filter::CUBIC_IMG:
        return vk::Filter::eLinear;
    case PC_CORE::Filter::CUBIC_EXT:
        return vk::Filter::eLinear;
    }

    throw std::runtime_error("Unknown Filter");
}

vk::SamplerMipmapMode Vulkan::Utils::RHIToSamplerMipmapMode(PC_CORE::SamplerMipmapMode _mipmapMode)
{
    switch (_mipmapMode)
    {
    case PC_CORE::SamplerMipmapMode::NEAREST:
        return vk::SamplerMipmapMode::eNearest;
    case PC_CORE::SamplerMipmapMode::LINEAR:
        return vk::SamplerMipmapMode::eLinear;
    }


    throw std::runtime_error("Unknown SamplerMipmapMode");
}

vk::SamplerAddressMode Vulkan::Utils::RHIToVulkanSamplerAddressMode(PC_CORE::SamplerAddressMode _samplerAddressMode)
{
    switch (_samplerAddressMode)
    {
    case PC_CORE::SamplerAddressMode::REPEAT:
        return vk::SamplerAddressMode::eRepeat;
    case PC_CORE::SamplerAddressMode::MIRRORED_REPEAT:
        return vk::SamplerAddressMode::eMirroredRepeat;
    case PC_CORE::SamplerAddressMode::CLAMP_TO_EDGE:
        return vk::SamplerAddressMode::eClampToEdge;
    case PC_CORE::SamplerAddressMode::CLAMP_TO_BORDER:
        return vk::SamplerAddressMode::eClampToBorder;
    case PC_CORE::SamplerAddressMode::MIRROR_CLAMP_TO_EDGE:
        return vk::SamplerAddressMode::eMirrorClampToEdge;
    }

    throw std::runtime_error("Unknown SamplerAddressMode");
}

vk::CompareOp Vulkan::Utils::RHIToVulkanCompareOp(PC_CORE::CompareOp _compareOp)
{
    switch (_compareOp)
    {
    case PC_CORE::CompareOp::NEVER:
        return vk::CompareOp::eNever;
    case PC_CORE::CompareOp::LESS:
        return vk::CompareOp::eLess;
    case PC_CORE::CompareOp::EQUAL:
        return vk::CompareOp::eEqual;
    case PC_CORE::CompareOp::LESS_OR_EQUAL:
        return vk::CompareOp::eLessOrEqual;
    case PC_CORE::CompareOp::GREATER:
        return vk::CompareOp::eGreater;
    case PC_CORE::CompareOp::NOT_EQUAL:
        return vk::CompareOp::eNotEqual;
    case PC_CORE::CompareOp::GREATER_OR_EQUAL:
        return vk::CompareOp::eGreaterOrEqual;
    case PC_CORE::CompareOp::ALWAYS:
        return vk::CompareOp::eAlways;
    }

    throw std::runtime_error("Unknown CompareOp");
}

vk::BorderColor Vulkan::Utils::RHIToBorderColor(PC_CORE::BorderColor _borderColor)
{
    switch (_borderColor)
    {
    case PC_CORE::BorderColor::FLOAT_TRANSPARENT_BLACK:
        return vk::BorderColor::eFloatTransparentBlack;
    case PC_CORE::BorderColor::INT_TRANSPARENT_BLACK:
        return vk::BorderColor::eIntTransparentBlack;
    case PC_CORE::BorderColor::FLOAT_OPAQUE_BLACK:
        return vk::BorderColor::eFloatOpaqueBlack;
    case PC_CORE::BorderColor::INT_OPAQUE_BLACK:
        return vk::BorderColor::eIntOpaqueBlack;
    case PC_CORE::BorderColor::FLOAT_OPAQUE_WHITE:
        return vk::BorderColor::eFloatOpaqueWhite;
    case PC_CORE::BorderColor::INT_OPAQUE_WHITE:
        return vk::BorderColor::eIntOpaqueWhite;
    case PC_CORE::BorderColor::FLOAT_CUSTOM_EXT:
        return vk::BorderColor::eFloatCustomEXT;
    case PC_CORE::BorderColor::INT_CUSTOM_EXT:
        return vk::BorderColor::eIntCustomEXT;
    }

    throw std::runtime_error("Unknown BorderColor");
}

vk::PolygonMode Vulkan::Utils::RhiPolygonModeToVulkan(PC_CORE::PolygonMode _polygonMode)
{
    switch (_polygonMode)
    {
    case PC_CORE::PolygonMode::Fill:
        return vk::PolygonMode::eFill;
    case PC_CORE::PolygonMode::Line:
        return vk::PolygonMode::eLine;
    case PC_CORE::PolygonMode::Point:
        return vk::PolygonMode::ePoint;
    case PC_CORE::PolygonMode::FillRectangleNV:
        return vk::PolygonMode::eFillRectangleNV;
    }

    throw std::runtime_error("Unknown PolygonMode");
}


vk::ShaderStageFlagBits Vulkan::Utils::RhiToShaderStage(PC_CORE::ShaderStageType _shaderStage)
{
    switch (_shaderStage)
    {
    case PC_CORE::ShaderStageType::VERTEX:
        return vk::ShaderStageFlagBits::eVertex;
    case PC_CORE::ShaderStageType::TESSCONTROL:
        return vk::ShaderStageFlagBits::eTessellationControl;
    case PC_CORE::ShaderStageType::TESSEVALUATION:
        return vk::ShaderStageFlagBits::eTessellationEvaluation;
    case PC_CORE::ShaderStageType::GEOMETRY:
        return vk::ShaderStageFlagBits::eGeometry;
    case PC_CORE::ShaderStageType::FRAGMENT:
        return vk::ShaderStageFlagBits::eFragment;
    case PC_CORE::ShaderStageType::COMPUTE:
        return vk::ShaderStageFlagBits::eCompute;
    case PC_CORE::ShaderStageType::RAYGEN:
        return vk::ShaderStageFlagBits::eRaygenNV;
    case PC_CORE::ShaderStageType::INTERSECT:
        return vk::ShaderStageFlagBits::eIntersectionNV;
    case PC_CORE::ShaderStageType::ANYHIT:
        return vk::ShaderStageFlagBits::eAnyHitNV;
    case PC_CORE::ShaderStageType::CLOSESTHIT:
        return vk::ShaderStageFlagBits::eClosestHitNV;
    case PC_CORE::ShaderStageType::MISS:
        return vk::ShaderStageFlagBits::eMissNV;
    case PC_CORE::ShaderStageType::CALLABLE:
        return vk::ShaderStageFlagBits::eCallableNV;
    case PC_CORE::ShaderStageType::TASK:
        return vk::ShaderStageFlagBits::eTaskNV;
    case PC_CORE::ShaderStageType::MESH:
        return vk::ShaderStageFlagBits::eMeshNV;
    case PC_CORE::ShaderStageType::COUNT:
    default:
        throw std::runtime_error("Unknown ShaderStageType");
    }
}

vk::CullModeFlags Vulkan::Utils::RhiToCullMode(PC_CORE::CullModeFlagBit _cullModeFlagBit)
{
    vk::CullModeFlags cullModeFlags = vk::CullModeFlagBits::eNone;

    switch (_cullModeFlagBit)
    {
    case PC_CORE::CullModeFlagBit::None:
        break;
    case PC_CORE::CullModeFlagBit::Front:
        cullModeFlags |= vk::CullModeFlagBits::eFront;
        break;
    case PC_CORE::CullModeFlagBit::Back:
        cullModeFlags |= vk::CullModeFlagBits::eBack;
        break;
    case PC_CORE::CullModeFlagBit::FrontAndBack:
        cullModeFlags |= vk::CullModeFlagBits::eFrontAndBack;
        break;
    case PC_CORE::CullModeFlagBit::Count:
        break;
    default: ;
    }
    
    return cullModeFlags;
}

vk::BufferUsageFlags Vulkan::Utils::RhiToBufferUsage(PC_CORE::BufferUsage _usage)
{
    vk::BufferUsageFlags bufferUsageFlags = {};

    switch (_usage)
    {
    case PC_CORE::BufferUsage::VertexBuffer:
        return bufferUsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
    case PC_CORE::BufferUsage::IndexBuffer:
        return bufferUsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer;

    case PC_CORE::BufferUsage::UniformBuffer:
        return bufferUsageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;

    case PC_CORE::BufferUsage::ShaderStorageBuffer:
        return bufferUsageFlags |= vk::BufferUsageFlagBits::eStorageBuffer;

    case PC_CORE::BufferUsage::Count:
    default:
        throw std::runtime_error("Unknown BufferUsage");
    }
}

vk::IndexType Vulkan::Utils::RhiToIndexType(PC_CORE::IndexFormat _format)
{
    switch (_format)
    {
    case PC_CORE::IndexFormat::Uiunt8:
        return vk::IndexType::eUint8;
    case PC_CORE::IndexFormat::Uint16:
        return vk::IndexType::eUint16;
    case PC_CORE::IndexFormat::Uint32:
        return vk::IndexType::eUint32;
    default: throw std::runtime_error("Unknown IndexType");
    }
}

vk::DescriptorType Vulkan::Utils::RhiToDescriptorType(const PC_CORE::ShaderProgramDescriptorType& shaderProgramDescriptorType)
{
    switch (shaderProgramDescriptorType)
    {
    case PC_CORE::ShaderProgramDescriptorType::Sampler:
        return vk::DescriptorType::eSampler;
    case PC_CORE::ShaderProgramDescriptorType::CombineImageSampler:
        return vk::DescriptorType::eCombinedImageSampler;
    case PC_CORE::ShaderProgramDescriptorType::SampledImage:
        return vk::DescriptorType::eSampledImage;
    case PC_CORE::ShaderProgramDescriptorType::UniformBuffer:
        return vk::DescriptorType::eUniformBuffer;
    case PC_CORE::ShaderProgramDescriptorType::StorageBuffer:
        return vk::DescriptorType::eStorageBuffer;
    case PC_CORE::ShaderProgramDescriptorType::InlineUniformBlock:
        return vk::DescriptorType::eInlineUniformBlock;
    case PC_CORE::ShaderProgramDescriptorType::AccelerationStructure:
        return vk::DescriptorType::eAccelerationStructureKHR;
    case PC_CORE::ShaderProgramDescriptorType::Count:
    default:
        throw std::runtime_error("Unknown ShaderProgramDescriptorType");
    }
}

vk::SampleCountFlagBits Vulkan::Utils::RhiSampleCountToVuklan(uint32_t _sampleCount)
{
    switch (_sampleCount)
    {
    case 2:
        return vk::SampleCountFlagBits::e2;
    case 4:
        return vk::SampleCountFlagBits::e4;
    case 8:
        return vk::SampleCountFlagBits::e8;
    case 16:
        return vk::SampleCountFlagBits::e16;
    case 32:
        return vk::SampleCountFlagBits::e32;
    case 64:
        return vk::SampleCountFlagBits::e64;
    default:
        return vk::SampleCountFlagBits::e1;
    }
}

vk::PrimitiveTopology Vulkan::Utils::RhiPrimitiveTopology(PC_CORE::PrimitiveTopology _primitiveTopology)
{
    switch (_primitiveTopology)
    {
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyPointList:
        return vk::PrimitiveTopology::ePointList;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyLineList:
        return vk::PrimitiveTopology::eLineList;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyLineStrip:
        return vk::PrimitiveTopology::eLineStrip;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangleList:
        return vk::PrimitiveTopology::eTriangleList;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangleStrip:
        return vk::PrimitiveTopology::eTriangleStrip;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangle_FAN:
        return vk::PrimitiveTopology::eTriangleFan;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyLineListWithAdjacency:
        return vk::PrimitiveTopology::eLineListWithAdjacency;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyLineStripWithAdjacency:
        return vk::PrimitiveTopology::eLineStripWithAdjacency;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangleListWithAdjacency:
        return vk::PrimitiveTopology::eTriangleListWithAdjacency;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangleStripWithAdjacency: 
        return vk::PrimitiveTopology::eTriangleStripWithAdjacency;
    case PC_CORE::PrimitiveTopology::PrimitiveTopologyPathList:
        // Not directly supported in Vulkan; return something or assert
        throw std::runtime_error("PrimitiveTopologyPathList is not supported in Vulkan.");
    case PC_CORE::PrimitiveTopology::Count:
    default:
        throw std::runtime_error("Invalid or unsupported primitive topology.");
    }
}

vk::AttachmentLoadOp Vulkan::Utils::RhiLoadOperationToVulkan(PC_CORE::LoadOperation _loadOperation)
{
    switch (_loadOperation)
         {
    case PC_CORE::LoadOperation::Load:
        return vk::AttachmentLoadOp::eLoad;
    case PC_CORE::LoadOperation::Clear:
        return vk::AttachmentLoadOp::eClear;
    case PC_CORE::LoadOperation::DontCare:
        return vk::AttachmentLoadOp::eDontCare;
    default: ;
        throw std::runtime_error("Invalid or unsupported load operation.");
    }
}

vk::AttachmentStoreOp Vulkan::Utils::RhiStoreOperationToVulkan(PC_CORE::StoreOperation _storeOperation)
{
    switch (_storeOperation)
    {
    case PC_CORE::StoreOperation::Store:
        return vk::AttachmentStoreOp::eStore;
    case PC_CORE::StoreOperation::DontCare:
        return vk::AttachmentStoreOp::eDontCare;
    default: ;
        throw std::runtime_error("Invalid or unsupported store operation.");
    }
}

vk::PipelineStageFlags Vulkan::Utils::RhiPipelineStageToVulkan(PC_CORE::PipelineStageFlags stageFlags)
{
    vk::PipelineStageFlags vkFlags = {};

    using StageBit = PC_CORE::PipelineStageFlagBits;

    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::TopOfPipe))
        vkFlags |= vk::PipelineStageFlagBits::eTopOfPipe;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::DrawIndirect))
        vkFlags |= vk::PipelineStageFlagBits::eDrawIndirect;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::VertexInput))
        vkFlags |= vk::PipelineStageFlagBits::eVertexInput;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::VertexShader))
        vkFlags |= vk::PipelineStageFlagBits::eVertexShader;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::TessellationControlShader))
        vkFlags |= vk::PipelineStageFlagBits::eTessellationControlShader;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::TessellationEvaluationShader))
        vkFlags |= vk::PipelineStageFlagBits::eTessellationEvaluationShader;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::GeometryShader))
        vkFlags |= vk::PipelineStageFlagBits::eGeometryShader;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::FragmentShader))
        vkFlags |= vk::PipelineStageFlagBits::eFragmentShader;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::EarlyFragmentTests))
        vkFlags |= vk::PipelineStageFlagBits::eEarlyFragmentTests;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::LateFragmentTests))
        vkFlags |= vk::PipelineStageFlagBits::eLateFragmentTests;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::ColorAttachmentOutput))
        vkFlags |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::ComputeShader))
        vkFlags |= vk::PipelineStageFlagBits::eComputeShader;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::Transfer))
        vkFlags |= vk::PipelineStageFlagBits::eTransfer;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::BottomOfPipe))
        vkFlags |= vk::PipelineStageFlagBits::eBottomOfPipe;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::Host))
        vkFlags |= vk::PipelineStageFlagBits::eHost;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::AllGraphics))
        vkFlags |= vk::PipelineStageFlagBits::eAllGraphics;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::AllCommands))
        vkFlags |= vk::PipelineStageFlagBits::eAllCommands;

    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::TransformFeedbackEXT))
        vkFlags |= vk::PipelineStageFlagBits::eTransformFeedbackEXT;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::ConditionalRenderingEXT))
        vkFlags |= vk::PipelineStageFlagBits::eConditionalRenderingEXT;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::AccelerationStructureBuildKHR))
        vkFlags |= vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::AccelerationStructureBuildNV))
        vkFlags |= vk::PipelineStageFlagBits::eAccelerationStructureBuildNV;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::RayTracingShaderKHR))
        vkFlags |= vk::PipelineStageFlagBits::eRayTracingShaderKHR;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::RayTracingShaderNV))
        vkFlags |= vk::PipelineStageFlagBits::eRayTracingShaderNV;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::FragmentDensityProcessEXT))
        vkFlags |= vk::PipelineStageFlagBits::eFragmentDensityProcessEXT;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::FragmentShadingRateAttachmentKHR))
        vkFlags |= vk::PipelineStageFlagBits::eFragmentShadingRateAttachmentKHR;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::ShadingRateImageNV))
        vkFlags |= vk::PipelineStageFlagBits::eShadingRateImageNV;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::CommandPreprocessNV))
        vkFlags |= vk::PipelineStageFlagBits::eCommandPreprocessNV;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::CommandPreprocessEXT))
        vkFlags |= vk::PipelineStageFlagBits::eCommandPreprocessEXT;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::TaskShaderEXT))
        vkFlags |= vk::PipelineStageFlagBits::eTaskShaderEXT;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::TaskShaderNV))
        vkFlags |= vk::PipelineStageFlagBits::eTaskShaderNV;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::MeshShaderEXT))
        vkFlags |= vk::PipelineStageFlagBits::eMeshShaderEXT;
    if (stageFlags & static_cast<PC_CORE::PipelineStageFlags>(StageBit::MeshShaderNV))
        vkFlags |= vk::PipelineStageFlagBits::eMeshShaderNV;

    return vkFlags;
}

vk::AccessFlags Vulkan::Utils::RhiAccessFlagToVulkan(PC_CORE::AccessFlags accessFlags)
{
    vk::AccessFlags vkFlags{};

    using AccessBit = PC_CORE::AccessFlagBits;
    using AccessFlags = PC_CORE::AccessFlags;

    if ((accessFlags & static_cast<AccessFlags>(AccessBit::IndirectCommandRead)))
        vkFlags |= vk::AccessFlagBits::eIndirectCommandRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::IndexRead)))
        vkFlags |= vk::AccessFlagBits::eIndexRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::VertexAttributeRead)))
        vkFlags |= vk::AccessFlagBits::eVertexAttributeRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::UniformRead)))
        vkFlags |= vk::AccessFlagBits::eUniformRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::InputAttachmentRead)))
        vkFlags |= vk::AccessFlagBits::eInputAttachmentRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::ShaderRead)))
        vkFlags |= vk::AccessFlagBits::eShaderRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::ShaderWrite)))
        vkFlags |= vk::AccessFlagBits::eShaderWrite;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::ColorAttachmentRead)))
        vkFlags |= vk::AccessFlagBits::eColorAttachmentRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::ColorAttachmentWrite)))
        vkFlags |= vk::AccessFlagBits::eColorAttachmentWrite;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::DepthStencilAttachmentRead)))
        vkFlags |= vk::AccessFlagBits::eDepthStencilAttachmentRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::DepthStencilAttachmentWrite)))
        vkFlags |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::TransferRead)))
        vkFlags |= vk::AccessFlagBits::eTransferRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::TransferWrite)))
        vkFlags |= vk::AccessFlagBits::eTransferWrite;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::HostRead)))
        vkFlags |= vk::AccessFlagBits::eHostRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::HostWrite)))
        vkFlags |= vk::AccessFlagBits::eHostWrite;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::MemoryRead)))
        vkFlags |= vk::AccessFlagBits::eMemoryRead;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::MemoryWrite)))
        vkFlags |= vk::AccessFlagBits::eMemoryWrite;

    // Extensions
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::TransformFeedbackWriteEXT)))
        vkFlags |= vk::AccessFlagBits::eTransformFeedbackWriteEXT;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::TransformFeedbackCounterReadEXT)))
        vkFlags |= vk::AccessFlagBits::eTransformFeedbackCounterReadEXT;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::TransformFeedbackCounterWriteEXT)))
        vkFlags |= vk::AccessFlagBits::eTransformFeedbackCounterWriteEXT;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::ConditionalRenderingReadEXT)))
        vkFlags |= vk::AccessFlagBits::eConditionalRenderingReadEXT;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::ColorAttachmentReadNoncoherentEXT)))
        vkFlags |= vk::AccessFlagBits::eColorAttachmentReadNoncoherentEXT;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::AccelerationStructureReadKHR)))
        vkFlags |= vk::AccessFlagBits::eAccelerationStructureReadKHR;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::AccelerationStructureReadNV)))
        vkFlags |= vk::AccessFlagBits::eAccelerationStructureReadNV;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::AccelerationStructureWriteKHR)))
        vkFlags |= vk::AccessFlagBits::eAccelerationStructureWriteKHR;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::AccelerationStructureWriteNV)))
        vkFlags |= vk::AccessFlagBits::eAccelerationStructureWriteNV;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::FragmentDensityMapReadEXT)))
        vkFlags |= vk::AccessFlagBits::eFragmentDensityMapReadEXT;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::FragmentShadingRateAttachmentReadKHR)))
        vkFlags |= vk::AccessFlagBits::eFragmentShadingRateAttachmentReadKHR;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::ShadingRateImageReadNV)))
        vkFlags |= vk::AccessFlagBits::eShadingRateImageReadNV;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::CommandPreprocessReadNV)))
        vkFlags |= vk::AccessFlagBits::eCommandPreprocessReadNV;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::CommandPreprocessReadEXT)))
        vkFlags |= vk::AccessFlagBits::eCommandPreprocessReadEXT;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::CommandPreprocessWriteNV)))
        vkFlags |= vk::AccessFlagBits::eCommandPreprocessWriteNV;
    if ((accessFlags & static_cast<AccessFlags>(AccessBit::CommandPreprocessWriteEXT)))
        vkFlags |= vk::AccessFlagBits::eCommandPreprocessWriteEXT;

    return vkFlags;
}