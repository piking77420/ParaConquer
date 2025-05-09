﻿#include "rhi_vulkan_parser.hpp"

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
}*/


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

vk::PolygonMode Vulkan::RhiPolygonModeToVulkan(PC_CORE::PolygonMode _polygonMode)
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


vk::ShaderStageFlagBits Vulkan::RhiToShaderStage(PC_CORE::ShaderStageType _shaderStage)
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

vk::CullModeFlags Vulkan::RhiToCullMode(PC_CORE::CullModeFlagBit _cullModeFlagBit)
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

vk::BufferUsageFlags Vulkan::RhiToBufferUsage(PC_CORE::BufferUsage _usage)
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

vk::IndexType Vulkan::RhiToIndexType(PC_CORE::IndexFormat _format)
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

vk::DescriptorType Vulkan::RhiToDescriptorType(const PC_CORE::ShaderProgramDescriptorType& shaderProgramDescriptorType)
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
