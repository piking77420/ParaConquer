#include "Utils/RhiToVulkan.hpp"

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
        return vk::Format::eUndefined;

    case PC_CORE::RhiFormat::D24UnormS8Uint:
        return vk::Format::eUndefined;

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
    PC_CORE::ShaderProgramPipelineType _shaderProgramPipelineType)
{
    switch (_shaderProgramPipelineType)
    {
    case PC_CORE::ShaderProgramPipelineType::Graphic:
        return vk::PipelineBindPoint::eGraphics;
    case PC_CORE::ShaderProgramPipelineType::Compute:
        return vk::PipelineBindPoint::eCompute;
    case PC_CORE::ShaderProgramPipelineType::RayTracing:
        return vk::PipelineBindPoint::eRayTracingKHR;
    }
    return vk::PipelineBindPoint::eGraphics;
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

vk::ImageType Vulkan::Utils::RhiImageToVkImageType(PC_CORE::TextureType _textureType)
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

vk::ImageViewType Vulkan::Utils::RhiImageToVkImageViewType(PC_CORE::TextureType _textureType)
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

vk::ShaderStageFlagBits Vulkan::Utils::RhiToShaderStage(PC_CORE::ShaderStageType _shaderStageType)
{
    switch (_shaderStageType)
    {
    case PC_CORE::ShaderStageType::Vertex:
        return vk::ShaderStageFlagBits::eVertex;
    case PC_CORE::ShaderStageType::Hull:
        return vk::ShaderStageFlagBits::eTessellationControl;
    case PC_CORE::ShaderStageType::Domain:
        return vk::ShaderStageFlagBits::eTessellationEvaluation;
    case PC_CORE::ShaderStageType::Geometry:
        return vk::ShaderStageFlagBits::eGeometry;
    case PC_CORE::ShaderStageType::Pixel:
        return vk::ShaderStageFlagBits::eFragment;
    case PC_CORE::ShaderStageType::Compute:
        return vk::ShaderStageFlagBits::eCompute;
    case PC_CORE::ShaderStageType::Raygen:
        return vk::ShaderStageFlagBits::eRaygenKHR;
    case PC_CORE::ShaderStageType::Intersection:
        return vk::ShaderStageFlagBits::eIntersectionKHR;
    case PC_CORE::ShaderStageType::Anyhit:
        return vk::ShaderStageFlagBits::eAnyHitKHR;
    case PC_CORE::ShaderStageType::Closesthit:
        return vk::ShaderStageFlagBits::eClosestHitKHR;
    case PC_CORE::ShaderStageType::Miss:
        return vk::ShaderStageFlagBits::eMissKHR;
    case PC_CORE::ShaderStageType::Callable:
        return vk::ShaderStageFlagBits::eCallableKHR;
    case PC_CORE::ShaderStageType::Task:
        return vk::ShaderStageFlagBits::eTaskEXT;
    case PC_CORE::ShaderStageType::Mesh:
        return vk::ShaderStageFlagBits::eMeshEXT;
    case PC_CORE::ShaderStageType::Count:
    default:
        assert(false);
        break;
    }

    return {};
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

vk::DescriptorType Vulkan::Utils::RhiToDescriptorType(
    const PC_CORE::ShaderProgramDescriptorType& shaderProgramDescriptorType)
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
    case PC_CORE::MemoryLocalisation::GpuOnly:
        return VMA_MEMORY_USAGE_GPU_ONLY;
    case PC_CORE::MemoryLocalisation::CpuOnly:
        return VMA_MEMORY_USAGE_CPU_ONLY;
    case PC_CORE::MemoryLocalisation::CpuToGpu:
        return VMA_MEMORY_USAGE_CPU_TO_GPU;
    case PC_CORE::MemoryLocalisation::GpuToCpu:
        return VMA_MEMORY_USAGE_GPU_TO_CPU;
    case PC_CORE::MemoryLocalisation::Count:
    default:
        assert(false);
    }
    return VMA_MEMORY_USAGE_MAX_ENUM;
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

    if ((usage & TextureUsage::Sampled) == static_cast<uint8_t>(TextureUsage::Sampled))
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

    if ((usage & TextureUsage::RenderTarget) == static_cast<uint8_t>(TextureUsage::RenderTarget))
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

    if (((usage & TextureUsage::Depth) == static_cast<uint8_t>(TextureUsage::Depth)) || ((usage & TextureUsage::Stencil)
        == static_cast<uint8_t>(TextureUsage::Stencil)))
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if ((usage & TextureUsage::Storage) == static_cast<uint8_t>(TextureUsage::Storage))
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

    if ((_textureUsage & TextureUsage::RenderTarget) == static_cast<uint8_t>(TextureUsage::RenderTarget))
        flags |= vk::ImageAspectFlagBits::eColor;

    if (((_textureUsage & TextureUsage::Depth) == static_cast<uint8_t>(TextureUsage::Depth)) || ((_textureUsage &
        TextureUsage::Stencil) == static_cast<uint8_t>(TextureUsage::Stencil)))
        flags |= vk::ImageAspectFlagBits::eDepth;

    return flags;
}
