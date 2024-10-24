#include "rhi_vulkan_parser.hpp"

vk::DescriptorType VK_NP::RHIDescriptorTypeToVulkan(PC_CORE::DESCRIPTOR_TYPE _descriptorType)
{
      vk::DescriptorType result = {};
    switch (_descriptorType)
    {
    case PC_CORE::DESCRIPTOR_TYPE::SAMPLER:
        result = vk::DescriptorType::eSampler;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::COMBINED_IMAGE_SAMPLER:
        result = vk::DescriptorType::eCombinedImageSampler;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::SAMPLED_IMAGE:
        result = vk::DescriptorType::eSampledImage;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::STORAGE_IMAGE:
        result = vk::DescriptorType::eStorageImage;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::UNIFORM_TEXEL_BUFFER:
        result = vk::DescriptorType::eUniformTexelBuffer;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::STORAGE_TEXEL_BUFFER:
        result = vk::DescriptorType::eStorageTexelBuffer;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::UNIFORM_BUFFER:
        result = vk::DescriptorType::eUniformBuffer;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::STORAGE_BUFFER:
        result = vk::DescriptorType::eStorageBuffer;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::UNIFORM_BUFFER_DYNAMIC:
        result = vk::DescriptorType::eUniformBufferDynamic;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::STORAGE_BUFFER_DYNAMIC:
        result = vk::DescriptorType::eStorageBufferDynamic;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::INPUT_ATTACHMENT:
        result = vk::DescriptorType::eInputAttachment;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::INLINE_UNIFORM_BLOCK:
        result = vk::DescriptorType::eInlineUniformBlock;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::ACCELERATION_STRUCTURE_KHR:
        result = vk::DescriptorType::eAccelerationStructureKHR;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::ACCELERATION_STRUCTURE_NV:
        result = vk::DescriptorType::eAccelerationStructureNV;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::SAMPLE_WEIGHT_IMAGE_QCOM:
        result = vk::DescriptorType::eSampleWeightImageQCOM;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::BLOCK_MATCH_IMAGE_QCOM:
        result = vk::DescriptorType::eBlockMatchImageQCOM;

        break;
    case PC_CORE::DESCRIPTOR_TYPE::MUTABLE_EXT:
        result = vk::DescriptorType::eMutableEXT;
        break;
    case PC_CORE::DESCRIPTOR_TYPE::COUNT:
        break;
    }

    return result;
}

vk::ShaderStageFlagBits VK_NP::RHIShaderStageToVulkan(const std::vector<PC_CORE::ShaderStageType>& _shaderStages)
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
