#include "Utils/RhiVulkanParser.hpp"

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
        return vk::Format::eR8G8Sint;

    case PC_CORE::RHIFormat::R8G8_SRGB:
        return vk::Format::eR8G8Srgb;

    case PC_CORE::RHIFormat::R8G8B8_UNORM:
        return vk::Format::eR8G8B8Unorm;

    case PC_CORE::RHIFormat::R8G8B8_SNORM:
        return vk::Format::eR8G8B8Snorm;

    case PC_CORE::RHIFormat::R8G8B8_USCALED:
        return vk::Format::eR8G8B8Uscaled;

    case PC_CORE::RHIFormat::R8G8B8_SSCALED:
        return vk::Format::eR8G8B8Sscaled;

    case PC_CORE::RHIFormat::R8G8B8_UINT:
        return vk::Format::eR8G8B8Uint;

    case PC_CORE::RHIFormat::R8G8B8_SINT:
        return vk::Format::eR8G8B8Sint;

    case PC_CORE::RHIFormat::R8G8B8_SRGB:
        return vk::Format::eR8G8B8Srgb;

    case PC_CORE::RHIFormat::B8G8R8_UNORM:
        return vk::Format::eB8G8R8Unorm;

    case PC_CORE::RHIFormat::B8G8R8_SNORM:
        return vk::Format::eB8G8R8A8Snorm;

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
        return vk::Format::eR8G8B8A8Snorm;

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
        return vk::Format::eR16G16Unorm;

    case PC_CORE::RHIFormat::R16G16_SNORM:
        return vk::Format::eR16G16Snorm;

    case PC_CORE::RHIFormat::R16G16_USCALED:
        return vk::Format::eR16G16Uscaled;

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
        return vk::Format::eR16G16B16Sfloat;

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
        return vk::Format::eR16G16B16A16Sfloat;

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
        return vk::Format::eR32G32B32A32Sint;

    case PC_CORE::RHIFormat::R32G32B32A32_SFLOAT:
        return vk::Format::eR32G32B32A32Sfloat;

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
// here
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

    assert(false);

    return format;
}
#pragma endregion Format


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
    return vk::PipelineBindPoint::eGraphics;
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

vk::ImageType Vulkan::Utils::RHIImageToVkImageType(PC_CORE::TextureType _textureType)
{
    switch (_textureType)
    {
    case PC_CORE::TextureType::Texture2D:
        return vk::ImageType::e2D;
        break;
    case PC_CORE::TextureType::TextureArray2D:
        return vk::ImageType::e3D; 
        break;
    case PC_CORE::TextureType::CubeMap:
        return vk::ImageType::e2D;
    case PC_CORE::TextureType::CubeMapArray:
        return vk::ImageType::e3D;
    case PC_CORE::TextureType::Count:
    default: assert(false);
    }

    return {};
}

vk::ImageViewType Vulkan::Utils::RHIImageToVkImageViewType(PC_CORE::TextureType _textureType)
{
    switch (_textureType)
    {
    case PC_CORE::TextureType::Texture2D:
        return vk::ImageViewType::e2D;
        break;
    case PC_CORE::TextureType::TextureArray2D:
        return vk::ImageViewType::e2DArray;
        break;
    case PC_CORE::TextureType::CubeMap:
        return vk::ImageViewType::eCube;
        break;
    case PC_CORE::TextureType::Count:
        break;
    default: 
        assert(false);
    }

}

vk::ImageCreateFlags Vulkan::Utils::ImageCreateFlagFromTextureType(PC_CORE::TextureType _textureType)
{
    vk::ImageCreateFlags createFlag = {};

    switch (_textureType)
    {
    case PC_CORE::TextureType::Texture2D:
        break;
    case PC_CORE::TextureType::TextureArray2D:
        createFlag |= vk::ImageCreateFlagBits::e2DArrayCompatible;
        break;
    case PC_CORE::TextureType::CubeMap:
    case PC_CORE::TextureType::CubeMapArray:
        createFlag |= vk::ImageCreateFlagBits::eCubeCompatible;
        break;
    case PC_CORE::TextureType::Count:
        break;
    default: ;
    }

    return createFlag;
}



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


vk::ShaderStageFlagBits Vulkan::Utils::RhiToShaderStage(PC_CORE::ShaderStageTypeFlag _shaderStage)
{
    switch (_shaderStage)
    {
    case PC_CORE::ShaderStageTypeFlag::Vertex:
        return vk::ShaderStageFlagBits::eVertex;
    case PC_CORE::ShaderStageTypeFlag::TessControl:
        return vk::ShaderStageFlagBits::eTessellationControl;
    case PC_CORE::ShaderStageTypeFlag::Tessevaluation:
        return vk::ShaderStageFlagBits::eTessellationEvaluation;
    case PC_CORE::ShaderStageTypeFlag::Geometry:
        return vk::ShaderStageFlagBits::eGeometry;
    case PC_CORE::ShaderStageTypeFlag::Fragment:
        return vk::ShaderStageFlagBits::eFragment;
    case PC_CORE::ShaderStageTypeFlag::Compute:
        return vk::ShaderStageFlagBits::eCompute;
    case PC_CORE::ShaderStageTypeFlag::Raygen:
        return vk::ShaderStageFlagBits::eRaygenNV;
    case PC_CORE::ShaderStageTypeFlag::Intersect:
        return vk::ShaderStageFlagBits::eIntersectionNV;
    case PC_CORE::ShaderStageTypeFlag::Anyhit:
        return vk::ShaderStageFlagBits::eAnyHitNV;
    case PC_CORE::ShaderStageTypeFlag::Closesthit:
        return vk::ShaderStageFlagBits::eClosestHitNV;
    case PC_CORE::ShaderStageTypeFlag::Miss:
        return vk::ShaderStageFlagBits::eMissNV;
    case PC_CORE::ShaderStageTypeFlag::Callable:
        return vk::ShaderStageFlagBits::eCallableNV;
    case PC_CORE::ShaderStageTypeFlag::Task:
        return vk::ShaderStageFlagBits::eTaskNV;
    case PC_CORE::ShaderStageTypeFlag::Mesh:
        return vk::ShaderStageFlagBits::eMeshNV;
    case PC_CORE::ShaderStageTypeFlag::Count:
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
    vk::DescriptorType::eInputAttachment;

    switch (shaderProgramDescriptorType)
    {
    case PC_CORE::ShaderProgramDescriptorType::Sampler:
        return vk::DescriptorType::eSampler;
    case PC_CORE::ShaderProgramDescriptorType::CombinedImageSampler:
        return vk::DescriptorType::eCombinedImageSampler;
    case PC_CORE::ShaderProgramDescriptorType::SampledImage:
        return vk::DescriptorType::eSampledImage;
    case PC_CORE::ShaderProgramDescriptorType::StorageImage:
        return vk::DescriptorType::eStorageImage;
    case PC_CORE::ShaderProgramDescriptorType::UniformBuffer:
        return vk::DescriptorType::eUniformBuffer;
    case PC_CORE::ShaderProgramDescriptorType::StorageBuffer:
        return vk::DescriptorType::eStorageBuffer;
    case PC_CORE::ShaderProgramDescriptorType::InputAttachment:
        return vk::DescriptorType::eInputAttachment;
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

vk::PipelineStageFlags Vulkan::Utils::RhiPipelineStageToVulkan(PC_CORE::GpuPipelineStageFlagBits stageFlags)
{
    vk::PipelineStageFlags vkFlags = {};

    using StageBit = PC_CORE::GpuPipelineStageFlagBits;

    if (stageFlags & StageBit::TopOfPipe)
        vkFlags |= vk::PipelineStageFlagBits::eTopOfPipe;
    if (stageFlags & StageBit::DrawIndirect)
        vkFlags |= vk::PipelineStageFlagBits::eDrawIndirect;
    if (stageFlags & StageBit::VertexInput)
        vkFlags |= vk::PipelineStageFlagBits::eVertexInput;
    if (stageFlags & StageBit::VertexShader)
        vkFlags |= vk::PipelineStageFlagBits::eVertexShader;
    if (stageFlags & StageBit::TessellationControlShader)
        vkFlags |= vk::PipelineStageFlagBits::eTessellationControlShader;
    if (stageFlags & StageBit::TessellationEvaluationShader)
        vkFlags |= vk::PipelineStageFlagBits::eTessellationEvaluationShader;
    if (stageFlags & StageBit::GeometryShader)
        vkFlags |= vk::PipelineStageFlagBits::eGeometryShader;
    if (stageFlags & StageBit::FragmentShader)
        vkFlags |= vk::PipelineStageFlagBits::eFragmentShader;
    if (stageFlags & StageBit::EarlyFragmentTests)
        vkFlags |= vk::PipelineStageFlagBits::eEarlyFragmentTests;
    if (stageFlags & StageBit::LateFragmentTests)
        vkFlags |= vk::PipelineStageFlagBits::eLateFragmentTests;
    if (stageFlags & StageBit::ColorAttachmentOutput)
        vkFlags |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
    if (stageFlags & StageBit::ComputeShader)
        vkFlags |= vk::PipelineStageFlagBits::eComputeShader;
    if (stageFlags & StageBit::Transfer)
        vkFlags |= vk::PipelineStageFlagBits::eTransfer;
    if (stageFlags & StageBit::BottomOfPipe)
        vkFlags |= vk::PipelineStageFlagBits::eBottomOfPipe;
    if (stageFlags & StageBit::Host)
        vkFlags |= vk::PipelineStageFlagBits::eHost;
    if (stageFlags & StageBit::AllGraphics)
        vkFlags |= vk::PipelineStageFlagBits::eAllGraphics;
    if (stageFlags & StageBit::AllCommands)
        vkFlags |= vk::PipelineStageFlagBits::eAllCommands;

    if (stageFlags & StageBit::TransformFeedbackEXT)
        vkFlags |= vk::PipelineStageFlagBits::eTransformFeedbackEXT;
    if (stageFlags & StageBit::ConditionalRenderingEXT)
        vkFlags |= vk::PipelineStageFlagBits::eConditionalRenderingEXT;
    if (stageFlags & StageBit::AccelerationStructureBuildKHR)
        vkFlags |= vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR;
    if (stageFlags & StageBit::AccelerationStructureBuildNV)
        vkFlags |= vk::PipelineStageFlagBits::eAccelerationStructureBuildNV;
    if (stageFlags & StageBit::RayTracingShaderKHR)
        vkFlags |= vk::PipelineStageFlagBits::eRayTracingShaderKHR;
    if (stageFlags & StageBit::RayTracingShaderNV)
        vkFlags |= vk::PipelineStageFlagBits::eRayTracingShaderNV;
    if (stageFlags & StageBit::FragmentDensityProcessEXT)
        vkFlags |= vk::PipelineStageFlagBits::eFragmentDensityProcessEXT;
    if (stageFlags & StageBit::FragmentShadingRateAttachmentKHR)
        vkFlags |= vk::PipelineStageFlagBits::eFragmentShadingRateAttachmentKHR;
    if (stageFlags & StageBit::ShadingRateImageNV)
        vkFlags |= vk::PipelineStageFlagBits::eShadingRateImageNV;
    if (stageFlags & StageBit::CommandPreprocessNV)
        vkFlags |= vk::PipelineStageFlagBits::eCommandPreprocessNV;
    if (stageFlags & StageBit::CommandPreprocessEXT)
        vkFlags |= vk::PipelineStageFlagBits::eCommandPreprocessEXT;
    if (stageFlags & StageBit::TaskShaderEXT)
        vkFlags |= vk::PipelineStageFlagBits::eTaskShaderEXT;
    if (stageFlags & StageBit::TaskShaderNV)
        vkFlags |= vk::PipelineStageFlagBits::eTaskShaderNV;
    if (stageFlags & StageBit::MeshShaderEXT)
        vkFlags |= vk::PipelineStageFlagBits::eMeshShaderEXT;
    if (stageFlags & StageBit::MeshShaderNV)
        vkFlags |= vk::PipelineStageFlagBits::eMeshShaderNV;

    return vkFlags;
}

vk::AccessFlags Vulkan::Utils::RhiAccessFlagToVulkan(PC_CORE::GpuAccessFlag accessFlags)
{
    vk::AccessFlags vkFlags{};
    if ((accessFlags & PC_CORE::GpuAccessFlag::IndirectCommandRead))
        vkFlags |= vk::AccessFlagBits::eIndirectCommandRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::IndexRead))
        vkFlags |= vk::AccessFlagBits::eIndexRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::VertexAttributeRead))
        vkFlags |= vk::AccessFlagBits::eVertexAttributeRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::UniformRead))
        vkFlags |= vk::AccessFlagBits::eUniformRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::InputAttachmentRead))
        vkFlags |= vk::AccessFlagBits::eInputAttachmentRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::ShaderRead))
        vkFlags |= vk::AccessFlagBits::eShaderRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::ShaderWrite))
        vkFlags |= vk::AccessFlagBits::eShaderWrite;
    if ((accessFlags & PC_CORE::GpuAccessFlag::ColorAttachmentRead))
        vkFlags |= vk::AccessFlagBits::eColorAttachmentRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::ColorAttachmentWrite))
        vkFlags |= vk::AccessFlagBits::eColorAttachmentWrite;
    if ((accessFlags & PC_CORE::GpuAccessFlag::DepthStencilAttachmentRead))
        vkFlags |= vk::AccessFlagBits::eDepthStencilAttachmentRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::DepthStencilAttachmentWrite))
        vkFlags |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    if ((accessFlags & PC_CORE::GpuAccessFlag::TransferRead))
        vkFlags |= vk::AccessFlagBits::eTransferRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::TransferWrite))
        vkFlags |= vk::AccessFlagBits::eTransferWrite;
    if ((accessFlags & PC_CORE::GpuAccessFlag::HostRead))
        vkFlags |= vk::AccessFlagBits::eHostRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::HostWrite))
        vkFlags |= vk::AccessFlagBits::eHostWrite;
    if ((accessFlags & PC_CORE::GpuAccessFlag::MemoryRead))
        vkFlags |= vk::AccessFlagBits::eMemoryRead;
    if ((accessFlags & PC_CORE::GpuAccessFlag::MemoryWrite))
        vkFlags |= vk::AccessFlagBits::eMemoryWrite;

    // Extensions
    if ((accessFlags & PC_CORE::GpuAccessFlag::TransformFeedbackWriteEXT))
        vkFlags |= vk::AccessFlagBits::eTransformFeedbackWriteEXT;
    if ((accessFlags & PC_CORE::GpuAccessFlag::TransformFeedbackCounterReadEXT))
        vkFlags |= vk::AccessFlagBits::eTransformFeedbackCounterReadEXT;
    if ((accessFlags & PC_CORE::GpuAccessFlag::TransformFeedbackCounterWriteEXT))
        vkFlags |= vk::AccessFlagBits::eTransformFeedbackCounterWriteEXT;
    if ((accessFlags & PC_CORE::GpuAccessFlag::ConditionalRenderingReadEXT))
        vkFlags |= vk::AccessFlagBits::eConditionalRenderingReadEXT;
    if ((accessFlags & PC_CORE::GpuAccessFlag::ColorAttachmentReadNoncoherentEXT))
        vkFlags |= vk::AccessFlagBits::eColorAttachmentReadNoncoherentEXT;
    if ((accessFlags & PC_CORE::GpuAccessFlag::AccelerationStructureReadKHR))
        vkFlags |= vk::AccessFlagBits::eAccelerationStructureReadKHR;
    if ((accessFlags & PC_CORE::GpuAccessFlag::AccelerationStructureReadNV))
        vkFlags |= vk::AccessFlagBits::eAccelerationStructureReadNV;
    if ((accessFlags & PC_CORE::GpuAccessFlag::AccelerationStructureWriteKHR))
        vkFlags |= vk::AccessFlagBits::eAccelerationStructureWriteKHR;
    if ((accessFlags & PC_CORE::GpuAccessFlag::AccelerationStructureWriteNV))
        vkFlags |= vk::AccessFlagBits::eAccelerationStructureWriteNV;
    if ((accessFlags & PC_CORE::GpuAccessFlag::FragmentDensityMapReadEXT))
        vkFlags |= vk::AccessFlagBits::eFragmentDensityMapReadEXT;
    if ((accessFlags & PC_CORE::GpuAccessFlag::FragmentShadingRateAttachmentReadKHR))
        vkFlags |= vk::AccessFlagBits::eFragmentShadingRateAttachmentReadKHR;
    if ((accessFlags & PC_CORE::GpuAccessFlag::ShadingRateImageReadNV))
        vkFlags |= vk::AccessFlagBits::eShadingRateImageReadNV;
    if ((accessFlags & PC_CORE::GpuAccessFlag::CommandPreprocessReadNV))
        vkFlags |= vk::AccessFlagBits::eCommandPreprocessReadNV;
    if ((accessFlags & PC_CORE::GpuAccessFlag::CommandPreprocessReadEXT))
        vkFlags |= vk::AccessFlagBits::eCommandPreprocessReadEXT;
    if ((accessFlags & PC_CORE::GpuAccessFlag::CommandPreprocessWriteNV))
        vkFlags |= vk::AccessFlagBits::eCommandPreprocessWriteNV;
    if ((accessFlags & PC_CORE::GpuAccessFlag::CommandPreprocessWriteEXT))
        vkFlags |= vk::AccessFlagBits::eCommandPreprocessWriteEXT;

    return vkFlags;
}

VmaMemoryUsage Vulkan::Utils::RhiMemoryUsageToVulkan(PC_CORE::MemoryLocalisation _memoryVisibility)
{
    switch (_memoryVisibility)
    {
    case PC_CORE::MemoryLocalisation::GPU_Only:
        return VMA_MEMORY_USAGE_GPU_ONLY;
    case PC_CORE::MemoryLocalisation::CPU_Only:
        return VMA_MEMORY_USAGE_CPU_ONLY;
    case PC_CORE::MemoryLocalisation::CPU_To_GPU:
        return VMA_MEMORY_USAGE_CPU_TO_GPU;
    case PC_CORE::MemoryLocalisation::GPU_To_CPU:
        return VMA_MEMORY_USAGE_GPU_TO_CPU;
    case PC_CORE::MemoryLocalisation::Count:
    default:
        assert(false);
    }
    return VMA_MEMORY_USAGE_MAX_ENUM;
}

vk::BlendFactor Vulkan::Utils::RhiBlendFactorToVulkan(PC_CORE::BlendFactor _blendFactor)
{
    vk::BlendFactor blendFactor = vk::BlendFactor::eZero;
    switch (_blendFactor)
    {
    case PC_CORE::BlendFactor::Zero:
        break;
    case PC_CORE::BlendFactor::One:
        blendFactor = vk::BlendFactor::eOne;
        break;
    case PC_CORE::BlendFactor::SrcColor:
        blendFactor = vk::BlendFactor::eSrcColor;
        break;
    case PC_CORE::BlendFactor::OneMinusSrcColor:
        blendFactor = vk::BlendFactor::eOneMinusSrcColor;
        break;
    case PC_CORE::BlendFactor::DstColor:
        blendFactor = vk::BlendFactor::eDstColor;
        break;
    case PC_CORE::BlendFactor::OneMinusDstColor:
        blendFactor = vk::BlendFactor::eOneMinusDstColor;
        break;
    case PC_CORE::BlendFactor::SrcAlpha:
        blendFactor = vk::BlendFactor::eSrcAlpha;
        break;
    case PC_CORE::BlendFactor::OneMinusSrcAlpha:
        blendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        break;
    case PC_CORE::BlendFactor::DstAlpha:
        blendFactor = vk::BlendFactor::eDstAlpha;
        break;
    case PC_CORE::BlendFactor::OneMinusDstAlpha:
        blendFactor = vk::BlendFactor::eOneMinusDstAlpha;
        break;
    case PC_CORE::BlendFactor::ConstantColor:
        blendFactor = vk::BlendFactor::eConstantColor;
        break;
    case PC_CORE::BlendFactor::OneMinusConstantColor:
        blendFactor = vk::BlendFactor::eOneMinusConstantColor;
        break;
    case PC_CORE::BlendFactor::ConstantAlpha:
        blendFactor = vk::BlendFactor::eConstantAlpha;
        break;
    case PC_CORE::BlendFactor::OneMinusConstantAlpha:
        blendFactor = vk::BlendFactor::eOneMinusConstantAlpha;
        break;
    case PC_CORE::BlendFactor::SrcAlphaSaturate:
        blendFactor = vk::BlendFactor::eSrcAlphaSaturate;
        break;
    case PC_CORE::BlendFactor::Src1Color:
        blendFactor = vk::BlendFactor::eSrc1Color;
        break;
    case PC_CORE::BlendFactor::OneMinusSrc1Color:
        blendFactor = vk::BlendFactor::eOneMinusSrc1Color;
        break;
    case PC_CORE::BlendFactor::Src1Alpha:
        blendFactor = vk::BlendFactor::eSrc1Alpha;
        break;
    case PC_CORE::BlendFactor::OneMinusSrc1Alpha:
        blendFactor = vk::BlendFactor::eOneMinusSrc1Alpha;
        break;
    default: assert(false);
    }
    return blendFactor;
}

vk::BlendOp Vulkan::Utils::RhiBlendOpToVulkan(PC_CORE::BlendOp _blendOp)
{
    vk::BlendOp blendOp = vk::BlendOp::eAdd;
    switch (_blendOp)
    {
    case PC_CORE::BlendOp::Add:
        break;
    case PC_CORE::BlendOp::eSubtract:
        blendOp = vk::BlendOp::eSubtract;
        break;
    case PC_CORE::BlendOp::eReverseSubtract:
        blendOp = vk::BlendOp::eReverseSubtract;
        break;
    case PC_CORE::BlendOp::eMin:
        blendOp = vk::BlendOp::eMin;
        break;
    case PC_CORE::BlendOp::eMax:
        blendOp = vk::BlendOp::eMax;
        break;
    default: assert(false);
    }

    return blendOp;
}

vk::ColorComponentFlags Vulkan::Utils::RhiColorComponent(PC_CORE::ColorComponent _colorComponent)
{
    vk::ColorComponentFlags f = {};
    if (_colorComponent & PC_CORE::ColorComponent_R)
    {
        f |= vk::ColorComponentFlagBits::eR;
    }
    if (_colorComponent & PC_CORE::ColorComponent_G)
    {
        f |= vk::ColorComponentFlagBits::eG;
    }
    if (_colorComponent & PC_CORE::ColorComponent_B)
    {
        f |= vk::ColorComponentFlagBits::eB;
    }
    if (_colorComponent & PC_CORE::ColorComponent_A)
    {
        f |= vk::ColorComponentFlagBits::eA;
    }
    return f;
}

vk::ImageLayout Vulkan::Utils::RhiImageStateToVulkanImageLayout(PC_CORE::ImageState _imageState)
{
    switch (_imageState)
    {
    case PC_CORE::ImageState::Undefined:
        return vk::ImageLayout::eUndefined;

    case PC_CORE::ImageState::General:
        return vk::ImageLayout::eGeneral;

    case PC_CORE::ImageState::RenderTargetOptimal:
        return vk::ImageLayout::eColorAttachmentOptimal;

    case PC_CORE::ImageState::DepthStencilOptimal:
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;

    case PC_CORE::ImageState::DepthStencilReadOptimal:
        return vk::ImageLayout::eDepthStencilReadOnlyOptimal;

    case PC_CORE::ImageState::ShaderReadOptimal:
        return vk::ImageLayout::eShaderReadOnlyOptimal;

    case PC_CORE::ImageState::TransferSrcOptimal:
        return vk::ImageLayout::eTransferSrcOptimal;

    case PC_CORE::ImageState::TransferDstOptimal:
        return vk::ImageLayout::eTransferSrcOptimal;

    case PC_CORE::ImageState::Count:
    default:
        assert(false);
    }

    return {};
}


vk::ImageUsageFlags Vulkan::Utils::GetImageUsageFlags(PC_CORE::TextureUsage usage)
{
    using namespace PC_CORE;

    VkImageUsageFlags flags = 0;

    if ((usage & TextureUsage::Sampled) == TextureUsage::Sampled)
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

    if ((usage & TextureUsage::RenderTarget) == TextureUsage::RenderTarget)
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

    if (((usage & TextureUsage::Depth) == TextureUsage::Depth) || ((usage & TextureUsage::Stencil) == TextureUsage::Stencil))
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if ((usage & TextureUsage::Storage) == TextureUsage::Storage)
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;

    // Fallback/default
    if (flags == 0)
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

    return static_cast<vk::ImageUsageFlags>(flags);
}

vk::ImageAspectFlags Vulkan::Utils::RhiTextureUsageToImageAspectFlagFlags(PC_CORE::TextureUsage _textureUsage)
{
    using namespace PC_CORE;

    vk::ImageAspectFlags flags = {};

    if ((_textureUsage & TextureUsage::RenderTarget) == TextureUsage::RenderTarget)
        flags |= vk::ImageAspectFlagBits::eColor;

    if (((_textureUsage & TextureUsage::Depth) == TextureUsage::Depth) || ((_textureUsage & TextureUsage::Stencil) == TextureUsage::Stencil))
        flags |= vk::ImageAspectFlagBits::eDepth;

    return flags;
}
