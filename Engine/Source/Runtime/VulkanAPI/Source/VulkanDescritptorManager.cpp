#include "VulkanDescritptorManager.hpp"

#include <map>

#include "VulkanContext.hpp"
#include "VulkanDescriptorSet.hpp"
#include "Utils/RhiToVulkan.hpp"


struct DescriptorVkBindingHash {

    inline [[nodiscard]] std::size_t hash_combine(std::size_t seed, std::size_t h) const noexcept {
        // classic combine
        seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
        return seed;
    }

    size_t operator()(const vk::DescriptorSetLayoutBinding& w) const noexcept {
        size_t h = 0;
        h = hash_combine(h, static_cast<size_t>(w.binding));
        h = hash_combine(h, static_cast<size_t>(w.descriptorCount));
        h = hash_combine(h, static_cast<size_t>(w.descriptorType));
        h = hash_combine(h, static_cast<size_t>(static_cast<uint32_t>(w.stageFlags)));
        assert(w.pImmutableSamplers == nullptr);

        return h;
    }
};

vk::ShaderStageFlags SpvShaderStageBitsToVk(SpvReflectShaderStageFlagBits _SpvReflectShaderStageFlagBits)
{
    switch (_SpvReflectShaderStageFlagBits)
    {
    case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
        return vk::ShaderStageFlagBits::eVertex  ;
    case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        return vk::ShaderStageFlagBits::eTessellationControl;
    case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        return vk::ShaderStageFlagBits::eTessellationEvaluation;
    case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
        return vk::ShaderStageFlagBits::eGeometry;
    case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
        return vk::ShaderStageFlagBits::eFragment;
    case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
        return vk::ShaderStageFlagBits::eCompute;
    case SPV_REFLECT_SHADER_STAGE_TASK_BIT_EXT:
        return vk::ShaderStageFlagBits::eTaskEXT;
    case SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV:
        return vk::ShaderStageFlagBits::eMeshEXT;
    case SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR:
        return vk::ShaderStageFlagBits::eRaygenKHR;
    case SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR:
        return vk::ShaderStageFlagBits::eAnyHitKHR;
    case SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
        return vk::ShaderStageFlagBits::eClosestHitKHR;
    case SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR:
        return vk::ShaderStageFlagBits::eMissKHR;
    case SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR:
        return vk::ShaderStageFlagBits::eIntersectionKHR;
    case SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR:
        return vk::ShaderStageFlagBits::eCallableKHR;
    default:
        break;
    }

    assert(false);
    return {};
}


Vulkan::VulkanDescritptorManager::VulkanDescritptorManager(VulkanContext& _Context)
    : m_Context(_Context)
{

}

Vulkan::VulkanDescritptorManager::~VulkanDescritptorManager()
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Context.GetDevice())->GetDevice();

    for (auto& it : m_Layouts)
    {
        if (it.second != VK_NULL_HANDLE)
        {
            device.destroyDescriptorSetLayout(it.second);
        }
    }

    device.destroyDescriptorPool(m_Pool);
}

void Vulkan::VulkanDescritptorManager::CreateDesciptorPool()
{
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Context.GetDevice())->GetDevice();

    constexpr size_t maxSet = 10;
    constexpr size_t MaxElementBinding = 2048;

    /*VK_DESCRIPTOR_TYPE_SAMPLER = 0,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
        VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
        VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
        VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
        VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK = 1000138000,
        VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR = 1000150000,*/

    std::array<vk::DescriptorPoolSize, 11> PoolSize;

    // TODO CHECK IF EXTENSION SUPPORTED
    for (size_t i = 0; i < PoolSize.size(); i++)
    {
        vk::DescriptorType type = static_cast<vk::DescriptorType>(i);

        PoolSize[i]
            .setDescriptorCount(MaxElementBinding)
            .setType(type);
    }

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind)
        .setMaxSets(MaxElementBinding * 2)
        .setPoolSizeCount(PoolSize.size())
        .setPoolSizes(PoolSize);

    m_Pool = device.createDescriptorPool(poolInfo);
    assert(m_Pool != VK_NULL_HANDLE);
}

std::vector<vk::DescriptorSetLayout> Vulkan::VulkanDescritptorManager::GetDescriptorLayouts(const std::vector<SpvReflectShaderModule>& _SpvReflectShaderModule)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> BindingsPerSet;
    uint32_t maxSet = 0;

    for (auto& ShaderModules : _SpvReflectShaderModule) // for each module
    {
        vk::ShaderStageFlags ModuleFlag = SpvShaderStageBitsToVk(ShaderModules.shader_stage);

        for (size_t s = 0; s < ShaderModules.descriptor_set_count; s++) // for each desciptor set
        {
            uint32_t currentSet = ShaderModules.descriptor_sets[s].set;
            std::vector<vk::DescriptorSetLayoutBinding>& Binding = BindingsPerSet[currentSet];
            maxSet = std::max(maxSet, currentSet);

            for (size_t j = 0; j < ShaderModules.descriptor_sets[s].binding_count; j++) // for binding
            {
                const SpvReflectDescriptorBinding& SpvBinding = *ShaderModules.descriptor_sets[s].bindings[j];

                vk::DescriptorSetLayoutBinding VkBinding{};
                VkBinding.binding = SpvBinding.binding;
                VkBinding.descriptorCount = SpvBinding.count;


                static_assert((uint32_t)vk::DescriptorType::eAccelerationStructureKHR == (uint32_t)SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR);
                VkBinding.descriptorType = static_cast<vk::DescriptorType>(SpvBinding.descriptor_type);

                if (VkBinding.descriptorType == vk::DescriptorType::eUniformBuffer || VkBinding.descriptorType == vk::DescriptorType::eStorageBuffer)
                {
                    const bool isDynamic = std::string(SpvBinding.name).find(PC_CORE::RhiBuffer::DynamicBufferKey) != std::string::npos;
                    if (isDynamic)
                        VkBinding.descriptorType = (VkBinding.descriptorType == vk::DescriptorType::eUniformBuffer) ? vk::DescriptorType::eUniformBufferDynamic : vk::DescriptorType::eStorageBufferDynamic;
                }
                
                auto it = std::ranges::find_if(Binding, [&VkBinding](const vk::DescriptorSetLayoutBinding& DescriptorSetLayoutBinding)
                    {
                        return DescriptorSetLayoutBinding == VkBinding;
                    });

                if (it != Binding.end())
                {
                    it->stageFlags |= ModuleFlag;
                }
                else
                {
                    VkBinding.stageFlags = ModuleFlag;
                    Binding.push_back(VkBinding);
                }
            }

        }
    }
    
    // out
    std::vector<vk::DescriptorSetLayoutBinding> BindingBuffer;
    std::vector<vk::DescriptorSetLayout> OutSet(maxSet + 1, VK_NULL_HANDLE);
    
    for (auto it = BindingsPerSet.begin(); it != BindingsPerSet.end(); it++)
    {
        BindingBuffer.resize(it->second.size());
        size_t i = 0;
        for (const auto& BindingInSet : it->second)
            BindingBuffer[i++] = BindingInSet;
       
        OutSet[it->first] = CreateLayout(BindingBuffer);
    }


    return OutSet;
}

vk::DescriptorSetLayout Vulkan::VulkanDescritptorManager::GetDescriptorLayout(const std::vector<PC_CORE::DescriptorWrite>& _DescriptorWrites)
{

    PERF_REGION_SCOPED;
   PERF_REGION_COLOR(PerfRegion::Rhi);

    // Compute Layout
    std::vector<vk::DescriptorSetLayoutBinding> Bindings;
    Bindings.reserve(_DescriptorWrites.size());

    for (size_t i = 0; i < _DescriptorWrites.size(); i++)
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding;
        uboLayoutBinding
            .setBinding(_DescriptorWrites[i].bindingIndex)
            .setDescriptorType(Utils::RhiToDescriptorType(_DescriptorWrites[i].type))
            .setDescriptorCount(1)
            .setStageFlags(Utils::RhiToShaderStage(_DescriptorWrites[i].StagesBits))
            .setPImmutableSamplers(nullptr);
        Bindings.emplace_back(std::move(uboLayoutBinding));
    }

    return CreateLayout(Bindings);
}

size_t Vulkan::VulkanDescritptorManager::HashLayout(const std::vector<vk::DescriptorSetLayoutBinding>& _DescriptorWrites)
{
    DescriptorVkBindingHash elemHash;
    size_t h = 0;
    // Hashing
    for (const auto& e : _DescriptorWrites) {
        h ^= elemHash(e); // XOR is order-independent
    }

    return h;
}

vk::DescriptorSetLayout Vulkan::VulkanDescritptorManager::CreateLayout(const std::vector<vk::DescriptorSetLayoutBinding>& _Bindings)
{
    const size_t h = HashLayout(_Bindings);
    auto it = m_Layouts.find(h);
    if (it != m_Layouts.end())
        return it->second;

    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(m_Context.GetDevice())->GetDevice();

    vk::DescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo{};
    DescriptorSetLayoutCreateInfo
        .setBindingCount(_Bindings.size())
        .setBindings(_Bindings)
        .setFlags({});


    vk::DescriptorSetLayout layout = device.createDescriptorSetLayout(DescriptorSetLayoutCreateInfo, nullptr);
    assert(layout != nullptr);

    m_Layouts.emplace(h, layout);
    return layout;
}
