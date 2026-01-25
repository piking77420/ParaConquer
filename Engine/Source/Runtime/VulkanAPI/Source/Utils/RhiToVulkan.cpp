#include "Utils/RhiToVulkan.hpp"

#include <Vulkan/vk_enum_string_helper.h>

#pragma region Format
vk::Format Vulkan::Utils::RhiFormatToVkFormat(PC_CORE::RhiFormat _rhiFormat)
{
    vk::Format format = {};
    // TODO MAKE ALL FORMAT

    switch (_rhiFormat)
    {
    case PC_CORE::RhiFormat::Undefined:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R4G4UnormPack8:
        return vk::Format::eR4G4UnormPack8;

    case PC_CORE::RhiFormat::R4G4B4A4UnormPack16:
        return vk::Format::eR4G4UnormPack8;

    case PC_CORE::RhiFormat::B4G4R4A4UnormPack16:
        return vk::Format::eB4G4R4A4UnormPack16;

    case PC_CORE::RhiFormat::R5G6B5UnormPack16:
        return vk::Format::eR5G6B5UnormPack16;

    case PC_CORE::RhiFormat::B5G6R5UnormPack16:
        return vk::Format::eB5G6R5UnormPack16;

    case PC_CORE::RhiFormat::R5G5B5A1UnormPack16:
        return vk::Format::eR5G5B5A1UnormPack16;

    case PC_CORE::RhiFormat::B5G5R5A1UnormPack16:
        return vk::Format::eB5G5R5A1UnormPack16;

    case PC_CORE::RhiFormat::A1R5G5B5UnormPack16:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R8Unorm:
        return vk::Format::eR8Unorm;

    case PC_CORE::RhiFormat::R8Snorm:
        return vk::Format::eR8Snorm;

    case PC_CORE::RhiFormat::R8Uscaled:
        return vk::Format::eR8Uscaled;

    case PC_CORE::RhiFormat::R8Sscaled:
        return vk::Format::eR8Sscaled;

    case PC_CORE::RhiFormat::R8Uint:
        return vk::Format::eR8Uint;

    case PC_CORE::RhiFormat::R8Sint:
        return vk::Format::eR8Sint;

    case PC_CORE::RhiFormat::R8Srgb:
        return vk::Format::eR8Srgb;

    case PC_CORE::RhiFormat::R8G8Unorm:
        return vk::Format::eR8G8Unorm;

    case PC_CORE::RhiFormat::R8G8Snorm:
        return vk::Format::eR8G8Snorm;

    case PC_CORE::RhiFormat::R8G8Uscaled:
        return vk::Format::eR8G8Uscaled;

    case PC_CORE::RhiFormat::R8G8Sscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R8G8Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R8G8Sint:
        return vk::Format::eR8G8Sint;

    case PC_CORE::RhiFormat::R8G8Srgb:
        return vk::Format::eR8G8Srgb;

    case PC_CORE::RhiFormat::R8G8B8Unorm:
        return vk::Format::eR8G8B8Unorm;

    case PC_CORE::RhiFormat::R8G8B8Snorm:
        return vk::Format::eR8G8B8Snorm;

    case PC_CORE::RhiFormat::R8G8B8Uscaled:
        return vk::Format::eR8G8B8Uscaled;

    case PC_CORE::RhiFormat::R8G8B8Sscaled:
        return vk::Format::eR8G8B8Sscaled;

    case PC_CORE::RhiFormat::R8G8B8Uint:
        return vk::Format::eR8G8B8Uint;

    case PC_CORE::RhiFormat::R8G8B8Sint:
        return vk::Format::eR8G8B8Sint;

    case PC_CORE::RhiFormat::R8G8B8Srgb:
        return vk::Format::eR8G8B8Srgb;

    case PC_CORE::RhiFormat::B8G8R8Unorm:
        return vk::Format::eB8G8R8Unorm;

    case PC_CORE::RhiFormat::B8G8R8Snorm:
        return vk::Format::eB8G8R8A8Snorm;

    case PC_CORE::RhiFormat::B8G8R8Uscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8Sscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8Srgb:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R8G8B8A8Unorm:
        return vk::Format::eR8G8B8A8Unorm;

    case PC_CORE::RhiFormat::R8G8B8A8Snorm:
        return vk::Format::eR8G8B8A8Snorm;

    case PC_CORE::RhiFormat::R8G8B8A8Uscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R8G8B8A8Sscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R8G8B8A8Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R8G8B8A8Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R8G8B8A8Srgb:
        return vk::Format::eR8G8B8A8Srgb;

    case PC_CORE::RhiFormat::B8G8R8A8Unorm:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8A8Snorm:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8A8Uscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8A8Sscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8A8Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8A8Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B8G8R8A8Srgb:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A8B8G8R8UnormPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A8B8G8R8SnormPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A8B8G8R8UscaledPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A8B8G8R8SscaledPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A8B8G8R8UintPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A8B8G8R8SintPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A8B8G8R8SrgbPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2R10G10B10UnormPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2R10G10B10SnormPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2R10G10B10UscaledPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2R10G10B10SscaledPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2R10G10B10UintPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2R10G10B10SintPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2B10G10R10UnormPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2B10G10R10SnormPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2B10G10R10UscaledPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2B10G10R10SscaledPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2B10G10R10UintPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::A2B10G10R10SintPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16Unorm:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16Snorm:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16Uscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16Sscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16Sfloat:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16Unorm:
        return vk::Format::eR16G16Unorm;

    case PC_CORE::RhiFormat::R16G16Snorm:
        return vk::Format::eR16G16Snorm;

    case PC_CORE::RhiFormat::R16G16Uscaled:
        return vk::Format::eR16G16Uscaled;

    case PC_CORE::RhiFormat::R16G16Sscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16Sfloat:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16Unorm:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16Snorm:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16Uscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16Sscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16Sfloat:
        return vk::Format::eR16G16B16Sfloat;

    case PC_CORE::RhiFormat::R16G16B16A16Unorm:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16A16Snorm:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16A16Uscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16A16Sscaled:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16A16Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16A16Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R16G16B16A16Sfloat:
        return vk::Format::eR16G16B16A16Sfloat;

    case PC_CORE::RhiFormat::R32Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R32Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R32Sfloat:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R32G32Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R32G32Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R32G32Sfloat:
        return vk::Format::eR32G32Sfloat;

    case PC_CORE::RhiFormat::R32G32B32Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R32G32B32Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R32G32B32Sfloat:
        return vk::Format::eR32G32B32Sfloat;

    case PC_CORE::RhiFormat::R32G32B32A32Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R32G32B32A32Sint:
        return vk::Format::eR32G32B32A32Sint;

    case PC_CORE::RhiFormat::R32G32B32A32Sfloat:
        return vk::Format::eR32G32B32A32Sfloat;

    case PC_CORE::RhiFormat::R64Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64Sfloat:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64G64Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64G64Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64G64Sfloat:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64G64B64Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64G64B64Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64G64B64Sfloat:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64G64B64A64Uint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64G64B64A64Sint:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::R64G64B64A64Sfloat:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::B10G11R11UfloatPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::E5B9G9R9UfloatPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::D16Unorm:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::X8D24UnormPack32:
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::D32Sfloat:
        return vk::Format::eD32Sfloat;

    case PC_CORE::RhiFormat::S8Uint:
        return vk::Format::eS8Uint;

    case PC_CORE::RhiFormat::D16UnormS8Uint:
        return vk::Format::eD16UnormS8Uint;

    case PC_CORE::RhiFormat::D24UnormS8Uint:
        return vk::Format::eD24UnormS8Uint;

    case PC_CORE::RhiFormat::D32SfloatS8Uint:
        return vk::Format::eD32SfloatS8Uint;

    default:
        break;
    }

    assert(false);

    return format;
}
#pragma endregion Format


vk::PipelineBindPoint Vulkan::Utils::RhiPipelineBindPointToVulkan(
    PC_CORE::RhiShaderProgram::PipelineType _shaderProgramPipelineType)
{
    switch (_shaderProgramPipelineType)
    {
    case PC_CORE::RhiShaderProgram::PipelineType::Graphic:
        return vk::PipelineBindPoint::eGraphics;
    case PC_CORE::RhiShaderProgram::PipelineType::Compute:
        return vk::PipelineBindPoint::eCompute;
    case PC_CORE::RhiShaderProgram::PipelineType::RayTracing:
        return vk::PipelineBindPoint::eRayTracingKHR;
    case PC_CORE::RhiShaderProgram::PipelineType::MeshShader:
        return vk::PipelineBindPoint::eCompute;
    case PC_CORE::RhiShaderProgram::PipelineType::Count:
        return {};
    }
    
    return {};
}


vk::VertexInputRate Vulkan::Utils::RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate)
{
    switch (_vertexInputRate)
    {
    case PC_CORE::VertexInputRate::Vertex:
        return vk::VertexInputRate::eVertex;
    case PC_CORE::VertexInputRate::Instance:
        return vk::VertexInputRate::eInstance;
    case PC_CORE::VertexInputRate::Count:
        throw std::invalid_argument("VertexInputRate::COUNT VertexInputRate");
    default: ;
        throw std::invalid_argument("Invalid VertexInputRate");
    }
}

vk::ImageType Vulkan::Utils::RhiImageToVkImageType(PC_CORE::RhiTexture::Type _textureType)
{
    switch (_textureType)
    {
    case PC_CORE::RhiTexture::Type::Texture2D:
        return vk::ImageType::e2D;
        break;
    case PC_CORE::RhiTexture::Type::TextureArray2D:
        return vk::ImageType::e3D;
        break;
    case PC_CORE::RhiTexture::Type::CubeMap:
        return vk::ImageType::e2D;
    case PC_CORE::RhiTexture::Type::CubeMapArray:
        return vk::ImageType::e3D;
    case PC_CORE::RhiTexture::Type::Count:
    default: assert(false);
    }

    return {};
}

vk::ImageViewType Vulkan::Utils::RhiImageToVkImageViewType(PC_CORE::RhiTexture::Type _textureType)
{
    switch (_textureType)
    {
    case PC_CORE::RhiTexture::Type::Texture2D:
        return vk::ImageViewType::e2D;
        break;
    case PC_CORE::RhiTexture::Type::TextureArray2D:
        return vk::ImageViewType::e2DArray;
        break;
    case PC_CORE::RhiTexture::Type::CubeMap:
        return vk::ImageViewType::eCube;
        break;
    case PC_CORE::RhiTexture::Type::Count:
        break;
    default:
        assert(false);
    }
    return {};
}

vk::ImageCreateFlags Vulkan::Utils::ImageCreateFlagFromTextureType(PC_CORE::RhiTexture::Type _textureType)
{
    vk::ImageCreateFlags createFlag = {};

    switch (_textureType)
    {
    case PC_CORE::RhiTexture::Type::Texture2D:
        break;
    case PC_CORE::RhiTexture::Type::TextureArray2D:
        createFlag |= vk::ImageCreateFlagBits::e2DArrayCompatible;
        break;
    case PC_CORE::RhiTexture::Type::CubeMap:
    case PC_CORE::RhiTexture::Type::CubeMapArray:
        createFlag |= vk::ImageCreateFlagBits::eCubeCompatible;
        break;
    case PC_CORE::RhiTexture::Type::Count:
        break;
    default: ;
    }

    return createFlag;
}


vk::Filter Vulkan::Utils::RhiToVulkanFilter(PC_CORE::Filter _filter)
{
    switch (_filter)
    {
    case PC_CORE::Filter::Nearest:
        return vk::Filter::eNearest;
    case PC_CORE::Filter::Linear:
        return vk::Filter::eLinear;
    }

    throw std::runtime_error("Unknown Filter");
}

vk::SamplerMipmapMode Vulkan::Utils::RhiToSamplerMipmapMode(PC_CORE::SamplerMipmapMode _mipmapMode)
{
    switch (_mipmapMode)
    {
    case PC_CORE::SamplerMipmapMode::Nearest:
        return vk::SamplerMipmapMode::eNearest;
    case PC_CORE::SamplerMipmapMode::Linear:
        return vk::SamplerMipmapMode::eLinear;
    }


    throw std::runtime_error("Unknown SamplerMipmapMode");
}

vk::SamplerAddressMode Vulkan::Utils::RhiToVulkanSamplerAddressMode(PC_CORE::SamplerAddressMode _samplerAddressMode)
{
    switch (_samplerAddressMode)
    {
    case PC_CORE::SamplerAddressMode::Repeat:
        return vk::SamplerAddressMode::eRepeat;
    case PC_CORE::SamplerAddressMode::MirroredRepeat:
        return vk::SamplerAddressMode::eMirroredRepeat;
    case PC_CORE::SamplerAddressMode::ClampToEdge:
        return vk::SamplerAddressMode::eClampToEdge;
    case PC_CORE::SamplerAddressMode::ClampToBorder:
        return vk::SamplerAddressMode::eClampToBorder;
    case PC_CORE::SamplerAddressMode::MirrorClampToEdge:
        return vk::SamplerAddressMode::eMirrorClampToEdge;
    }

    throw std::runtime_error("Unknown SamplerAddressMode");
}

vk::CompareOp Vulkan::Utils::RhiToVulkanCompareOp(PC_CORE::CompareOp _compareOp)
{
    switch (_compareOp)
    {
    case PC_CORE::CompareOp::Never:
        return vk::CompareOp::eNever;
    case PC_CORE::CompareOp::Less:
        return vk::CompareOp::eLess;
    case PC_CORE::CompareOp::Equal:
        return vk::CompareOp::eEqual;
    case PC_CORE::CompareOp::LessOrEqual:
        return vk::CompareOp::eLessOrEqual;
    case PC_CORE::CompareOp::Greater:
        return vk::CompareOp::eGreater;
    case PC_CORE::CompareOp::NotEqual:
        return vk::CompareOp::eNotEqual;
    case PC_CORE::CompareOp::GreaterOrEqual:
        return vk::CompareOp::eGreaterOrEqual;
    case PC_CORE::CompareOp::Always:
        return vk::CompareOp::eAlways;
    }

    throw std::runtime_error("Unknown CompareOp");
}

vk::BorderColor Vulkan::Utils::RhiToBorderColor(PC_CORE::BorderColor _borderColor)
{
    switch (_borderColor)
    {
    case PC_CORE::BorderColor::FloatTransparentBlack:
        return vk::BorderColor::eFloatTransparentBlack;
    case PC_CORE::BorderColor::IntTransparentBlack:
        return vk::BorderColor::eIntTransparentBlack;
    case PC_CORE::BorderColor::FloatOpaqueBlack:
        return vk::BorderColor::eFloatOpaqueBlack;
    case PC_CORE::BorderColor::IntOpaqueBlack:
        return vk::BorderColor::eIntOpaqueBlack;
    case PC_CORE::BorderColor::FloatOpaqueWhite:
        return vk::BorderColor::eFloatOpaqueWhite;
    case PC_CORE::BorderColor::IntOpaqueWhite:
        return vk::BorderColor::eIntOpaqueWhite;
    case PC_CORE::BorderColor::FloatCustomExt:
        return vk::BorderColor::eFloatCustomEXT;
    case PC_CORE::BorderColor::IntCustomExt:
        return vk::BorderColor::eIntCustomEXT;
    }

    throw std::runtime_error("Unknown BorderColor");
}

vk::PolygonMode Vulkan::Utils::RhiPolygonModeToVulkan(PC_CORE::RhiShaderProgram::PolygonMode _polygonMode)
{
    switch (_polygonMode)
    {
    case PC_CORE::RhiShaderProgram::PolygonMode::Fill:
        return vk::PolygonMode::eFill;
    case PC_CORE::RhiShaderProgram::PolygonMode::Line:
        return vk::PolygonMode::eLine;
    case PC_CORE::RhiShaderProgram::PolygonMode::Point:
        return vk::PolygonMode::ePoint;
    case PC_CORE::RhiShaderProgram::PolygonMode::FillRectangleNV:
        return vk::PolygonMode::eFillRectangleNV;
    }

    throw std::runtime_error("Unknown PolygonMode");
}

vk::CullModeFlags Vulkan::Utils::RhiToCullMode(PC_CORE::RhiShaderProgram::CullModeFlag _cullModeFlagBit)
{
    using Cull = PC_CORE::RhiShaderProgram::CullModeFlagBits;

    vk::CullModeFlags cullModeFlags = vk::CullModeFlagBits::eNone;

    if (_cullModeFlagBit & Cull::CullFront)
        cullModeFlags |= vk::CullModeFlagBits::eFront;
    else if (_cullModeFlagBit & Cull::CullFront)
        cullModeFlags |= vk::CullModeFlagBits::eBack;
    else if (_cullModeFlagBit & Cull::CullFront)
        cullModeFlags |= vk::CullModeFlagBits::eFrontAndBack;

    return cullModeFlags;
}

vk::ShaderStageFlags Vulkan::Utils::RhiToShaderStage(RhiShaderStageTypeFlag _RhiShaderStageTypeFlag)
{
    vk::ShaderStageFlags ShaderStageFlag;

    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Vertex)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eVertex;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Hull)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eTessellationControl;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Domain)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eTessellationEvaluation;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Geometry)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eGeometry;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Pixel)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eFragment;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Compute)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eCompute;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Raygen)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eRaygenKHR;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Intersection)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eIntersectionKHR;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Anyhit)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eAnyHitKHR;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Closesthit)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eClosestHitKHR;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Miss)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eMissKHR;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Callable)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eCallableKHR;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Task)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eTaskEXT;
    if (_RhiShaderStageTypeFlag & PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Mesh)
        ShaderStageFlag |= vk::ShaderStageFlagBits::eMeshEXT;

    return ShaderStageFlag;
}

vk::ShaderStageFlagBits Vulkan::Utils::RhiToShaderStageBits(RhiShaderStageBits _RhiShaderStageBits)
{
    vk::ShaderStageFlagBits ShaderStageFlag;

    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Vertex)
        ShaderStageFlag = vk::ShaderStageFlagBits::eVertex;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Hull)
        ShaderStageFlag = vk::ShaderStageFlagBits::eTessellationControl;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Domain)
        ShaderStageFlag = vk::ShaderStageFlagBits::eTessellationEvaluation;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Geometry)
        ShaderStageFlag = vk::ShaderStageFlagBits::eGeometry;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Pixel)
        ShaderStageFlag = vk::ShaderStageFlagBits::eFragment;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Compute)
        ShaderStageFlag = vk::ShaderStageFlagBits::eCompute;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Raygen)
        ShaderStageFlag = vk::ShaderStageFlagBits::eRaygenKHR;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Intersection)
        ShaderStageFlag = vk::ShaderStageFlagBits::eIntersectionKHR;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Anyhit)
        ShaderStageFlag = vk::ShaderStageFlagBits::eAnyHitKHR;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Closesthit)
        ShaderStageFlag = vk::ShaderStageFlagBits::eClosestHitKHR;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Miss)
        ShaderStageFlag = vk::ShaderStageFlagBits::eMissKHR;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Callable)
        ShaderStageFlag = vk::ShaderStageFlagBits::eCallableKHR;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Task)
        ShaderStageFlag = vk::ShaderStageFlagBits::eTaskEXT;
    if (_RhiShaderStageBits == PC_CORE::RhiShaderProgram::ShaderStageTypeBits::Mesh)
        ShaderStageFlag = vk::ShaderStageFlagBits::eMeshEXT;

    return ShaderStageFlag;
    
}


vk::IndexType Vulkan::Utils::RhiToIndexType(PC_CORE::RhiBuffer::IndexFormat _format)
{
    switch (_format)
    {
    case PC_CORE::RhiBuffer::IndexFormat::Uint8:
        return vk::IndexType::eUint8;
    case PC_CORE::RhiBuffer::IndexFormat::Uint16:
        return vk::IndexType::eUint16;
    case PC_CORE::RhiBuffer::IndexFormat::Uint32:
        return vk::IndexType::eUint32;
    default: throw std::runtime_error("Unknown IndexType");
    }
}

vk::DescriptorType Vulkan::Utils::RhiToDescriptorType(
    const PC_CORE::DescriptorType& _DescriptorType)
{
    switch (_DescriptorType)
    {
    case PC_CORE::DescriptorType::Sampler:
        return vk::DescriptorType::eSampler;
    case PC_CORE::DescriptorType::CombinedImageSampler:
        return vk::DescriptorType::eCombinedImageSampler;
    case PC_CORE::DescriptorType::SampledImage:
        return vk::DescriptorType::eSampledImage;
    case PC_CORE::DescriptorType::StorageImage:
        return vk::DescriptorType::eStorageImage;
    case PC_CORE::DescriptorType::UniformBuffer:
        return vk::DescriptorType::eUniformBuffer;
    case PC_CORE::DescriptorType::DynamicStorageBuffer:
        return vk::DescriptorType::eStorageBufferDynamic;
    case PC_CORE::DescriptorType::DynamicUniformBuffer:
        return vk::DescriptorType::eUniformBufferDynamic;
    case PC_CORE::DescriptorType::StorageBuffer:
        return vk::DescriptorType::eStorageBuffer;
    case PC_CORE::DescriptorType::InputAttachment:
        return vk::DescriptorType::eInputAttachment;
    case PC_CORE::DescriptorType::InlineUniformBlock:
        return vk::DescriptorType::eInlineUniformBlock;
    case PC_CORE::DescriptorType::AccelerationStructure:
        return vk::DescriptorType::eAccelerationStructureKHR;
    case PC_CORE::DescriptorType::Count:
    default:
        throw std::runtime_error("Unknown DescriptorType");
    }

    return {};
}

vk::SampleCountFlagBits Vulkan::Utils::RhSampleCountToVulkan(uint32_t _sampleCount)
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

vk::PrimitiveTopology Vulkan::Utils::RhiPrimitiveTopology(PC_CORE::RhiShaderProgram::PrimitiveTopology _primitiveTopology)
{
    switch (_primitiveTopology)
    {
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyPointList:
        return vk::PrimitiveTopology::ePointList;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyLineList:
        return vk::PrimitiveTopology::eLineList;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyLineStrip:
        return vk::PrimitiveTopology::eLineStrip;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleList:
        return vk::PrimitiveTopology::eTriangleList;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleStrip:
        return vk::PrimitiveTopology::eTriangleStrip;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangle_FAN:
        return vk::PrimitiveTopology::eTriangleFan;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyLineListWithAdjacency:
        return vk::PrimitiveTopology::eLineListWithAdjacency;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyLineStripWithAdjacency:
        return vk::PrimitiveTopology::eLineStripWithAdjacency;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleListWithAdjacency:
        return vk::PrimitiveTopology::eTriangleListWithAdjacency;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleStripWithAdjacency:
        return vk::PrimitiveTopology::eTriangleStripWithAdjacency;
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyPathList:
        // Not directly supported in Vulkan; return something or assert
        throw std::runtime_error("PrimitiveTopologyPathList is not supported in Vulkan.");
    case PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyCount:
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

vk::PipelineStageFlags Vulkan::Utils::RhiPipelineStageToVulkan(PC_CORE::GpuPipelineStage stageFlags)
{
    vk::PipelineStageFlags vkFlags = {};

    using StageBit = PC_CORE::GpuPipelineStage;

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
    if (stageFlags & StageBit::TessellationEvalShader)
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

    if (stageFlags & StageBit::TransformFeedback)
        vkFlags |= vk::PipelineStageFlagBits::eTransformFeedbackEXT;
    if (stageFlags & StageBit::ConditionalRendering)
        vkFlags |= vk::PipelineStageFlagBits::eConditionalRenderingEXT;
    if (stageFlags & StageBit::AccelerationStructureBuild)
        vkFlags |= vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR;
    if (stageFlags & StageBit::RayTracingShader)
        vkFlags |= vk::PipelineStageFlagBits::eRayTracingShaderKHR;
    if (stageFlags & StageBit::FragmentDensityProcess)
        vkFlags |= vk::PipelineStageFlagBits::eFragmentDensityProcessEXT;
    if (stageFlags & StageBit::FragmentShadingRate)
        vkFlags |= vk::PipelineStageFlagBits::eFragmentShadingRateAttachmentKHR;
    if (stageFlags & StageBit::CommandPreprocess)
        vkFlags |= vk::PipelineStageFlagBits::eCommandPreprocessEXT;
    if (stageFlags & StageBit::TaskShader)
        vkFlags |= vk::PipelineStageFlagBits::eTaskShaderEXT;
    if (stageFlags & StageBit::MeshShader)
        vkFlags |= vk::PipelineStageFlagBits::eMeshShaderEXT;

    return vkFlags;
}

vk::BlendFactor Vulkan::Utils::RhiBlendFactorToVulkan(PC_CORE::BlendFactor _blendFactor)
{
    auto blendFactor = vk::BlendFactor::eZero;
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
    auto blendOp = vk::BlendOp::eAdd;
    switch (_blendOp)
    {
    case PC_CORE::BlendOp::Add:
        break;
    case PC_CORE::BlendOp::Subtract:
        blendOp = vk::BlendOp::eSubtract;
        break;
    case PC_CORE::BlendOp::ReverseSubtract:
        blendOp = vk::BlendOp::eReverseSubtract;
        break;
    case PC_CORE::BlendOp::Min:
        blendOp = vk::BlendOp::eMin;
        break;
    case PC_CORE::BlendOp::Max:
        blendOp = vk::BlendOp::eMax;
        break;
    default: assert(false);
    }

    return blendOp;
}

vk::ColorComponentFlags Vulkan::Utils::RhiColorComponent(PC_CORE::ColorComponent _colorComponent)
{
    vk::ColorComponentFlags f = {};
    if (_colorComponent & PC_CORE::ColorComponentR)
    {
        f |= vk::ColorComponentFlagBits::eR;
    }
    if (_colorComponent & PC_CORE::ColorComponentG)
    {
        f |= vk::ColorComponentFlagBits::eG;
    }
    if (_colorComponent & PC_CORE::ColorComponentB)
    {
        f |= vk::ColorComponentFlagBits::eB;
    }
    if (_colorComponent & PC_CORE::ColorComponentA)
    {
        f |= vk::ColorComponentFlagBits::eA;
    }
    return f;
}



vk::ImageUsageFlags Vulkan::Utils::GetImageUsageFlags(PC_CORE::RhiTexture::TextureUsageFlag _usage)
{
    using namespace PC_CORE;
    VkImageUsageFlags flags = 0;

    assert(_usage != 0);

    if (_usage & PC_CORE::RhiTexture::TextureUsageFlagBits::Sampled)
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

    if (_usage & PC_CORE::RhiTexture::TextureUsageFlagBits::RenderTarget)
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (_usage & PC_CORE::RhiTexture::TextureUsageFlagBits::DepthStencil)
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (_usage & PC_CORE::RhiTexture::TextureUsageFlagBits::LoadAndStore)
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;

    if (_usage & PC_CORE::RhiTexture::TextureUsageFlagBits::TransferSrc)
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    if (_usage & PC_CORE::RhiTexture::TextureUsageFlagBits::TransferDst)
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if (_usage & PC_CORE::RhiTexture::TextureUsageFlagBits::InputAttachement)
        flags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
 
    return static_cast<vk::ImageUsageFlags>(flags);
}

vk::ImageAspectFlags Vulkan::Utils::RhiTextureFormatToImageAspectFlagFlags(PC_CORE::RhiFormat _format)
{
    using namespace PC_CORE;
    
    
    switch (_format)
    {
    // Color
    case PC_CORE::RhiFormat::R8G8B8A8Unorm:
    case PC_CORE::RhiFormat::R8G8B8A8Snorm:
    case PC_CORE::RhiFormat::B8G8R8A8Snorm:
    case PC_CORE::RhiFormat::R8G8B8A8Srgb:
    case PC_CORE::RhiFormat::R16G16B16A16Sfloat:
    case PC_CORE::RhiFormat::R32Sfloat:
    case PC_CORE::RhiFormat::R16G16Snorm:
        return vk::ImageAspectFlagBits::eColor;
        // Depth and STENCIL
    case PC_CORE::RhiFormat::D32Sfloat:
    case PC_CORE::RhiFormat::S8Uint:
    case PC_CORE::RhiFormat::D16UnormS8Uint:
    case PC_CORE::RhiFormat::D24UnormS8Uint:
    case PC_CORE::RhiFormat::D32SfloatS8Uint:
        return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    }

    
    assert(false && "Unsupported texture usage From Format");
    return {};
}

vk::ImageType Vulkan::Utils::RhiTextureTypeToVulkanImageType(PC_CORE::RhiTexture::Type _textureType)
{
    switch (_textureType) 
    {
    case PC_CORE::RhiTexture::Type::Count:
    case PC_CORE::RhiTexture::Type::None:
        return {};
    case PC_CORE::RhiTexture::Type::Texture1D:
        return vk::ImageType::e1D;
    case PC_CORE::RhiTexture::Type::Texture2D:
        return vk::ImageType::e2D;
    case PC_CORE::RhiTexture::Type::TextureArray2D:
        return vk::ImageType::e2D;
    case PC_CORE::RhiTexture::Type::CubeMap:
        return vk::ImageType::e3D;
    case PC_CORE::RhiTexture::Type::CubeMapArray:
         return vk::ImageType::e3D;;
    }
    
    PC_LOGERROR("Invalid texture type");
    return {};
}

vk::BufferUsageFlags Vulkan::Utils::RhiBufferUsageToVulkan(PC_CORE::RhiBuffer::BufferUsageFlag _bufferUsage)
{
    vk::BufferUsageFlags flags{};

    using U = PC_CORE::RhiBuffer::BufferUsageFlagBits;

    if (_bufferUsage & U::Uniform)        flags |= vk::BufferUsageFlagBits::eUniformBuffer;
    if (_bufferUsage & U::Vertex)         flags |= vk::BufferUsageFlagBits::eVertexBuffer;
    if (_bufferUsage & U::Index)          flags |= vk::BufferUsageFlagBits::eIndexBuffer;
    if (_bufferUsage & U::ShaderStorage)  flags |= vk::BufferUsageFlagBits::eStorageBuffer;
    if (_bufferUsage & U::TransferSrc)    flags |= vk::BufferUsageFlagBits::eTransferSrc;
    if (_bufferUsage & U::TransferDst)    flags |= vk::BufferUsageFlagBits::eTransferDst;

    return flags; 
}

vk::ImageLayout Vulkan::Utils::RhiResourceStateToVulkanImageLayout(RhiResourceState _rhiResourceState)
{
    switch (_rhiResourceState)
    {
    case RhiResourceState::Undefined:
        return vk::ImageLayout::eUndefined;

    // Copy 
    case RhiResourceState::CopySrc:
        return vk::ImageLayout::eTransferSrcOptimal;
    case RhiResourceState::CopyDst:
        return vk::ImageLayout::eTransferDstOptimal;
    // Shader
    case RhiResourceState::FragmentShaderResource:
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    case RhiResourceState::RenderTarget:
        return vk::ImageLayout::eColorAttachmentOptimal;
    case RhiResourceState::DepthStencilWrite:
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    case RhiResourceState::DepthStencilRead:
        return vk::ImageLayout::eDepthStencilReadOnlyOptimal;

    // Compute
    case RhiResourceState::ComputeRead:
        return vk::ImageLayout::eReadOnlyOptimal;
    case RhiResourceState::ComputeWrite:
        return vk::ImageLayout::eGeneral;
    case RhiResourceState::ComputeReadWrite:
        return vk::ImageLayout::eGeneral;
    // Present 
    case RhiResourceState::Present:
        return vk::ImageLayout::ePresentSrcKHR;
    default:
        assert(false);
        break;
    }

    return vk::ImageLayout::eUndefined;
}

RhiResourceState Vulkan::Utils::VulkanImageLayoutToResourceState(vk::ImageLayout layout)
{
    switch (layout)
    {
    case vk::ImageLayout::eUndefined:
        return RhiResourceState::Undefined;
    case vk::ImageLayout::eGeneral:
        return RhiResourceState::ComputeReadWrite;
    case vk::ImageLayout::eColorAttachmentOptimal:
        return RhiResourceState::RenderTarget;
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
        return RhiResourceState::RenderTarget;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        return RhiResourceState::FragmentShaderResource;
    case vk::ImageLayout::eTransferSrcOptimal:
        return RhiResourceState::CopySrc;
    case vk::ImageLayout::eTransferDstOptimal:
        return RhiResourceState::CopyDst;
    case vk::ImageLayout::ePreinitialized:
        break;
    case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
        return RhiResourceState::DepthStencilRead;
    case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
        return RhiResourceState::DepthStencilRead;;
    //case vk::ImageLayout::eDepthAttachmentOptimal:
    //case vk::ImageLayout::eDepthReadOnlyOptimal:
    //case vk::ImageLayout::eStencilAttachmentOptimal:
    //case vk::ImageLayout::eStencilReadOnlyOptimal:
      //  break;
    case vk::ImageLayout::eReadOnlyOptimal:
        return RhiResourceState::ComputeRead;
    case vk::ImageLayout::eAttachmentOptimal:
        return RhiResourceState::RenderTarget;
    case vk::ImageLayout::ePresentSrcKHR:
        return RhiResourceState::Present;
    default:
        break;
    }

    PC_LOG("Unsupported VulkanImageLayoutToResourceState layout {}", string_VkImageLayout(static_cast<VkImageLayout>(layout)));
    return RhiResourceState::Undefined;
}

vk::AccessFlags Vulkan::Utils::RhiResourceStateToAccesFlag(RhiResourceState _RhiResourceState)
{
    switch (_RhiResourceState)
    {
    case PC_CORE::RhiResource::State::Undefined:
        return {};
    case PC_CORE::RhiResource::State::CopySrc:
        return vk::AccessFlagBits::eTransferRead;
    case PC_CORE::RhiResource::State::CopyDst:
        return vk::AccessFlagBits::eTransferWrite;
    case PC_CORE::RhiResource::State::VertexBuffer:
        return vk::AccessFlagBits::eVertexAttributeRead;
    case PC_CORE::RhiResource::State::IndexBuffer:
        return vk::AccessFlagBits::eIndexRead;
    case PC_CORE::RhiResource::State::UniformBuffer:
        return vk::AccessFlagBits::eUniformRead;
    case PC_CORE::RhiResource::State::FragmentShaderResource:
        return vk::AccessFlagBits::eShaderRead;
    case PC_CORE::RhiResource::State::RenderTarget:
        return vk::AccessFlagBits::eColorAttachmentWrite;
    case PC_CORE::RhiResource::State::DepthStencilWrite:
        return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    case PC_CORE::RhiResource::State::DepthStencilRead:
        return vk::AccessFlagBits::eDepthStencilAttachmentRead;
    case PC_CORE::RhiResource::State::ComputeRead:
        return vk::AccessFlagBits::eShaderRead;
    case PC_CORE::RhiResource::State::ComputeWrite:
        return vk::AccessFlagBits::eShaderWrite;
    case PC_CORE::RhiResource::State::ComputeReadWrite:
        return vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
    case PC_CORE::RhiResource::State::Present:
    default:
        break;
    }

    assert(false && "Unsuported");

    PC_LOGERROR("Unsuported RhiResourceStateToAccesFlag")

    return {};
}

vk::PipelineStageFlags Vulkan::Utils::PipelineStageFlagsFromRhiResourceState(
    PC_CORE::RhiResource::State _RhiResourceState)
{

    using State = PC_CORE::RhiResource::State;

    switch (_RhiResourceState)
    {
    case State::Undefined:
        return vk::PipelineStageFlagBits::eTopOfPipe;

        // Transfer
    case State::CopySrc:
    case State::CopyDst:
        return vk::PipelineStageFlagBits::eTransfer;

        // Buffer input
    case State::VertexBuffer:
    case State::IndexBuffer:
        return vk::PipelineStageFlagBits::eVertexInput;

    case State::UniformBuffer:
        return
            vk::PipelineStageFlagBits::eVertexShader |
            vk::PipelineStageFlagBits::eFragmentShader |
            vk::PipelineStageFlagBits::eComputeShader;

        // Shader resources
    case State::VertexShaderResource:
        return vk::PipelineStageFlagBits::eVertexShader;

    case State::FragmentShaderResource:
        return vk::PipelineStageFlagBits::eFragmentShader;

        // Render targets
    case State::RenderTarget:
        return vk::PipelineStageFlagBits::eColorAttachmentOutput;

        // Depth / stencil
    case State::DepthStencilWrite:
    case State::DepthStencilRead:
        return
            vk::PipelineStageFlagBits::eEarlyFragmentTests |
            vk::PipelineStageFlagBits::eLateFragmentTests;

        // Compute
    case State::ComputeRead:
    case State::ComputeWrite:
    case State::ComputeReadWrite:
        return vk::PipelineStageFlagBits::eComputeShader;

        // Presentation
    case State::Present:
        return vk::PipelineStageFlagBits::eBottomOfPipe;

    default:
        PC_LOGERROR("Unsupported PC_CORE::RhiResource::State");
        assert(false);
        return vk::PipelineStageFlagBits::eAllCommands;
    }
}
