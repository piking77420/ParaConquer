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

vk::VertexInputRate VK_NP::RhiInputRateToVkInputRate(PC_CORE::VertexInputRate _vertexInputRate)
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

vk::CommandPoolCreateFlagBits VK_NP::CommandPoolCreateFlagBitsToVulkan(
    PC_CORE::CommandPoolBufferFlag _flag)
{
    vk::CommandPoolCreateFlagBits commandPoolCreateFlagBits {};

    if (_flag & PC_CORE::COMMAND_POOL_BUFFER_RESET)
        commandPoolCreateFlagBits = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    if (_flag & PC_CORE::COMMAND_POOL_BUFFER_TRANSIENT)
        commandPoolCreateFlagBits = vk::CommandPoolCreateFlagBits::eTransient;

    if (_flag & PC_CORE::COMMAND_POOL_BUFFER_PROTECTED)
        commandPoolCreateFlagBits = vk::CommandPoolCreateFlagBits::eProtected;

    return commandPoolCreateFlagBits;
}

uint32_t VK_NP::GetQueueFamiliesIndexFromType(VulkanContext* _context, PC_CORE::QueuType queueType)
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
    else if(queueType & PC_CORE::QueuType::COMPUTE)
    {
        queueFamiliesIndex = _context->queuFamiliesIndicies.computeFamily;
    }
  
    return queueFamiliesIndex;
}


