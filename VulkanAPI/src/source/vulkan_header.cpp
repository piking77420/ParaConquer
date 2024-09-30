#include "vulkan_header.h"

vk::Format VK_NP::RhiFomatToVkFormat(PC_CORE::RHIFormat rhiFormat)
{
    // TODO
    vk::Format format = {};
    
    switch (rhiFormat)
    {
    case PC_CORE::RHIFormat::UNDIFINED:
        format =  vk::Format::eUndefined;
        break;
    case PC_CORE::RHIFormat::R8_UNORM:
        format = vk::Format::eR8Unorm;
        break;
    case PC_CORE::RHIFormat::R8_SNORM:
        format = vk::Format::eR8Snorm;
        break;
    case PC_CORE::RHIFormat::R8_USCALED:
        format  = vk::Format::eR8Uscaled;
        break;
    case PC_CORE::RHIFormat::R32G32_SFLOAT:
        format = vk::Format::eR32G32Sfloat;
        break;
    case PC_CORE::RHIFormat::R32G32B32_SFLOAT:
        format = vk::Format::eR32G32B32Sfloat;
        break;
    case PC_CORE::RHIFormat::COUNT:
        throw std::invalid_argument("RHIFormat::COUNT FORMAT");
        break;
    default:
        throw std::invalid_argument("Invalid rhiFormat");
    }

    return format;
}
