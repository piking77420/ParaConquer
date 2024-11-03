#include "rhi_vulkan_parser.hpp"

#pragma region Format
vk::Format Vulkan::RHIFormatToVkFormat(PC_CORE::RHIFormat rhiFormat)
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
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8_SRGB:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_UNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_SNORM:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_USCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_SSCALED:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_SINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::R8G8_SRGB:
        return vk::Format::eUndefined;

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
        return vk::Format::eUndefined;

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
        return vk::Format::eUndefined;

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
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::S8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::D16_UNORM_S8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::D24_UNORM_S8_UINT:
        return vk::Format::eUndefined;

    case PC_CORE::RHIFormat::D32_SFLOAT_S8_UINT:
        return vk::Format::eUndefined;

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

vk::DescriptorType Vulkan::RHIDescriptorTypeToVulkan(PC_CORE::DescriptorType _descriptorType)
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
}

vk::ShaderStageFlagBits Vulkan::RHIShaderStageToVulkan(const std::vector<PC_CORE::ShaderStageType>& _shaderStages)
{
    int result = {};
    for (auto shaderStage : _shaderStages)
    {
        switch (shaderStage)
        {
        case PC_CORE::ShaderStageType::VERTEX:
            result |= static_cast<int>(vk::ShaderStageFlagBits::eVertex);
            break;
        case PC_CORE::ShaderStageType::FRAGMENT:
            result |= static_cast<int>(vk::ShaderStageFlagBits::eFragment);
            break;
        case PC_CORE::ShaderStageType::GEOMETRY:
            result |= static_cast<int>(vk::ShaderStageFlagBits::eGeometry);
            break;
        case PC_CORE::ShaderStageType::TESSELATION:
            result |= static_cast<int>(vk::ShaderStageFlagBits::eTessellationControl);
            break;
        case PC_CORE::ShaderStageType::COMPUTE:
            result |= static_cast<int>(vk::ShaderStageFlagBits::eCompute);
            break;
        case PC_CORE::ShaderStageType::COUNT:
            break;
        default: ;
        }
    }

    return static_cast<vk::ShaderStageFlagBits>(result);
}

vk::PipelineBindPoint Vulkan::RhiPipelineBindPointToVulkan(
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

vk::BufferUsageFlags Vulkan::GetVulkanUsage(PC_CORE::GPU_BUFFER_USAGE usage)
{
    vk::BufferUsageFlags result = {};

    switch (usage)
    {
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_NONE:
        throw std::exception("Invalid GPU_BUFFER_USAGE usage");
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_TRANSFER_SRC_BIT:
        result |= vk::BufferUsageFlagBits::eTransferSrc;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_TRANSFER_DST_BIT:
        result |= vk::BufferUsageFlagBits::eTransferDst;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT:
        result |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT:
        result |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_UNIFORM_BUFFER_BIT:
        result |= vk::BufferUsageFlagBits::eUniformBuffer;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_STORAGE_BUFFER_BIT:
        result |= vk::BufferUsageFlagBits::eStorageBuffer;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_INDEX_BUFFER_BIT:
        result |= vk::BufferUsageFlagBits::eIndexBuffer;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_VERTEX_BUFFER_BIT:
        result |= vk::BufferUsageFlagBits::eVertexBuffer;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_INDIRECT_BUFFER_BIT:
        result |= vk::BufferUsageFlagBits::eIndirectBuffer;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT:
        result |= vk::BufferUsageFlagBits::eShaderDeviceAddress;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_VIDEO_DECODE_SRC_BIT_KHR:
        result |= vk::BufferUsageFlagBits::eVideoDecodeSrcKHR;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_VIDEO_DECODE_DST_BIT_KHR:
        result |= vk::BufferUsageFlagBits::eVideoDecodeDstKHR;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT:
        result |= vk::BufferUsageFlagBits::eTransformFeedbackBufferEXT;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT:
        result |= vk::BufferUsageFlagBits::eTransformFeedbackCounterBufferEXT;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT:
        result |= vk::BufferUsageFlagBits::eConditionalRenderingEXT;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR:
        result |= vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR:
        result |= vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR:
        result |= vk::BufferUsageFlagBits::eShaderBindingTableKHR;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_VIDEO_ENCODE_DST_BIT_KHR:
        result |= vk::BufferUsageFlagBits::eVideoEncodeDstKHR;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_VIDEO_ENCODE_SRC_BIT_KHR:
        result |= vk::BufferUsageFlagBits::eVideoEncodeSrcKHR;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT:
        result |= vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT:
        result |= vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_PUSH_DESCRIPTORS_DESCRIPTOR_BUFFER_BIT_EXT:
        result |= vk::BufferUsageFlagBits::ePushDescriptorsDescriptorBufferEXT;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_MICROMAP_BUILD_INPUT_READ_ONLY_BIT_EXT:
        result |= vk::BufferUsageFlagBits::eMicromapBuildInputReadOnlyEXT;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT:
        result |= vk::BufferUsageFlagBits::eMicromapStorageEXT;
        break;
    case PC_CORE::GPU_BUFFER_USAGE::BUFFER_USAGE_FLAG_BITS_MAX_ENUM:
        throw std::exception("Invalid buffer usage BUFFER_USAGE_FLAG_BITS_MAX_ENUM");
        break;
    }
    return result;
}


vk::VertexInputRate Vulkan::RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate)
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

vk::CommandPoolCreateFlagBits Vulkan::CommandPoolCreateFlagBitsToVulkan(
    PC_CORE::CommandPoolBufferFlag _flag)
{
    vk::CommandPoolCreateFlagBits commandPoolCreateFlagBits{};

    if (_flag & PC_CORE::COMMAND_POOL_BUFFER_RESET)
        commandPoolCreateFlagBits = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    if (_flag & PC_CORE::COMMAND_POOL_BUFFER_TRANSIENT)
        commandPoolCreateFlagBits = vk::CommandPoolCreateFlagBits::eTransient;

    if (_flag & PC_CORE::COMMAND_POOL_BUFFER_PROTECTED)
        commandPoolCreateFlagBits = vk::CommandPoolCreateFlagBits::eProtected;

    return commandPoolCreateFlagBits;
}

uint32_t Vulkan::GetQueueFamiliesIndexFromType(VulkanContext* _context, PC_CORE::QueuType queueType)
{
    uint32_t queueFamiliesIndex = 0;

    if (queueType & PC_CORE::QueuType::GRAPHICS && queueType & PC_CORE::QueuType::TRANSFERT)
    {
        queueFamiliesIndex = _context->queuFamiliesIndicies.graphicsFamily;
    }
    else if (queueType & PC_CORE::QueuType::TRANSFERT)
    {
        queueFamiliesIndex = _context->queuFamiliesIndicies.transferFamily;
    }
    else if (queueType & PC_CORE::QueuType::GRAPHICS)
    {
        queueFamiliesIndex = _context->queuFamiliesIndicies.graphicsFamily;
    }
    else if (queueType & PC_CORE::QueuType::COMPUTE)
    {
        queueFamiliesIndex = _context->queuFamiliesIndicies.computeFamily;
    }

    return queueFamiliesIndex;
}

vk::ImageTiling Vulkan::RHiImageToVkImageTiling(PC_CORE::ImageTiling _imageTiling)
{
    switch (_imageTiling)
    {
    case PC_CORE::ImageTiling::IMAGE_TILING_OPTIMAL:
        return vk::ImageTiling::eOptimal;
    case PC_CORE::ImageTiling::IMAGE_TILING_LINEAR:
        return vk::ImageTiling::eLinear;
    case PC_CORE::ImageTiling::IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT:
        return vk::ImageTiling::eDrmFormatModifierEXT;
    default: ;
    }
}

vk::ImageType Vulkan::RHIImageToVkImageType(PC_CORE::ImageType _imageType)
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

vk::ImageAspectFlagBits Vulkan::RhiToVKImageAspectFlagBits(PC_CORE::ImageAspectFlagBits _imageAspectFlagBits)
{
    return static_cast<vk::ImageAspectFlagBits>(_imageAspectFlagBits);
}

vk::ImageLayout Vulkan::RHIToVKImageLayout(PC_CORE::ImageLayout _imageLayout)
{
    switch (_imageLayout)
    {
    case PC_CORE::LAYOUT_UNDEFINED:
        return vk::ImageLayout::eUndefined;
    case PC_CORE::LAYOUT_GENERAL:
        return vk::ImageLayout::eGeneral;
    case PC_CORE::LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        return vk::ImageLayout::eColorAttachmentOptimal;
    case PC_CORE::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    case PC_CORE::LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
    case PC_CORE::LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    case PC_CORE::LAYOUT_TRANSFER_SRC_OPTIMAL:
        return vk::ImageLayout::eTransferSrcOptimal;
    case PC_CORE::LAYOUT_TRANSFER_DST_OPTIMAL:
        return vk::ImageLayout::eTransferDstOptimal;
    case PC_CORE::LAYOUT_PREINITIALIZED:
        return vk::ImageLayout::ePreinitialized;
    case PC_CORE::LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
        return vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal;
    case PC_CORE::LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
        return vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
    case PC_CORE::LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        return vk::ImageLayout::eDepthAttachmentOptimal;
    case PC_CORE::LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
        return vk::ImageLayout::eDepthReadOnlyOptimal;
    case PC_CORE::LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
        return vk::ImageLayout::eStencilAttachmentOptimal;
    case PC_CORE::LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
        return vk::ImageLayout::eStencilAttachmentOptimal;
    case PC_CORE::LAYOUT_READ_ONLY_OPTIMAL:
        return vk::ImageLayout::eReadOnlyOptimal;
    case PC_CORE::LAYOUT_ATTACHMENT_OPTIMAL:
        return vk::ImageLayout::eAttachmentOptimal;
    case PC_CORE::LAYOUT_PRESENT_SRC_KHR:
        return vk::ImageLayout::eSharedPresentKHR;
    case PC_CORE::LAYOUT_VIDEO_DECODE_DST_KHR:
        return vk::ImageLayout::eVideoDecodeDstKHR;
    case PC_CORE::LAYOUT_VIDEO_DECODE_SRC_KHR:
        return vk::ImageLayout::eVideoDecodeSrcKHR;
    case PC_CORE::LAYOUT_VIDEO_DECODE_DPB_KHR:
        return vk::ImageLayout::eVideoDecodeDpbKHR;
    case PC_CORE::LAYOUT_SHARED_PRESENT_KHR:
        return vk::ImageLayout::eSharedPresentKHR;
    case PC_CORE::LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
        return vk::ImageLayout::eFragmentDensityMapOptimalEXT;
    case PC_CORE::LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
        return vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR;
    case PC_CORE::LAYOUT_RENDERING_LOCAL_READ_KHR:
        return vk::ImageLayout::eRenderingLocalReadKHR;
    case PC_CORE::LAYOUT_VIDEO_ENCODE_DST_KHR:
        return vk::ImageLayout::eVideoEncodeDstKHR;
    case PC_CORE::LAYOUT_VIDEO_ENCODE_SRC_KHR:
        return vk::ImageLayout::eVideoEncodeSrcKHR;
    case PC_CORE::LAYOUT_VIDEO_ENCODE_DPB_KHR:
        return vk::ImageLayout::eVideoEncodeDpbKHR;
    case PC_CORE::LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT:
        return vk::ImageLayout::eAttachmentFeedbackLoopOptimalEXT;
    default: ;
    }
}

vk::ImageViewType Vulkan::RHIImageTypeToVulkanImageViewType(PC_CORE::ImageType _imageViewType)
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

vk::SamplerCreateFlags Vulkan::RHIToVulkanSamplerCreateInfoFlags(PC_CORE::SamplerCreateInfoFlags _createInfoFlags)
{
    vk::SamplerCreateFlags samplerCreateInfo = {};
    if (_createInfoFlags & PC_CORE::SamplerCreateInfoFlags::SUBSAMPLED_BIT_EXT)
    {
        samplerCreateInfo |= vk::SamplerCreateFlagBits::eSubsampledEXT;
    }
    else if(_createInfoFlags & PC_CORE::SamplerCreateInfoFlags::SUBSAMPLED_COARSE_RECONSTRUCTION_BIT_EXT)
    {
        samplerCreateInfo |= vk::SamplerCreateFlagBits::eSubsampledCoarseReconstructionEXT;
    }
    else if(_createInfoFlags & PC_CORE::SamplerCreateInfoFlags::DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT)
    {
        samplerCreateInfo |= vk::SamplerCreateFlagBits::eDescriptorBufferCaptureReplayEXT;
    }
    else if(_createInfoFlags & PC_CORE::SamplerCreateInfoFlags::NON_SEAMLESS_CUBE_MAP_BIT_EXT)
    {
        samplerCreateInfo |= vk::SamplerCreateFlagBits::eNonSeamlessCubeMapEXT;
    }
    else if(_createInfoFlags & PC_CORE::SamplerCreateInfoFlags::IMAGE_PROCESSING_BIT_QCOM)
    {
        samplerCreateInfo |= vk::SamplerCreateFlagBits::eImageProcessingQCOM;
    }
    
    return samplerCreateInfo;
}

vk::Filter Vulkan::RHIToVulkanFilter(PC_CORE::Filter _filter)
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

vk::SamplerMipmapMode Vulkan::RHIToSamplerMipmapMode(PC_CORE::SamplerMipmapMode _mipmapMode)
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

vk::SamplerAddressMode Vulkan::RHIToVulkanSamplerAddressMode(PC_CORE::SamplerAddressMode _samplerAddressMode)
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

vk::CompareOp Vulkan::RHIToVulkanCompareOp(PC_CORE::CompareOp _compareOp)
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

vk::BorderColor Vulkan::RHIToBorderColor(PC_CORE::BorderColor _borderColor)
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

