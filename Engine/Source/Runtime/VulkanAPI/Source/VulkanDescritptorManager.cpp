#include "VulkanDescritptorManager.hpp"

#include <map>

#include "VulkanContext.hpp"
#include "Resources/VulkanDescriptorSets.hpp"

Vulkan::VulkanDescritptorManager::VulkanDescritptorManager(VulkanContext& _Context)
    : m_Context(_Context)
{

}

size_t Vulkan::VulkanDescritptorManager::GetDescriptorId(const std::vector<SpvReflectShaderModule>& _modules)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    SetBindingMap bindingMap;
    std::shared_ptr<CacheDescriptor> cache = nullptr;
    if (FindInCache(_modules, &bindingMap, &cache))
    {
        return cache->id;
    }

    std::map<vk::DescriptorType, uint32_t> descriptorTypeCount;
    std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> layoutsMap;

    vk::Device vkDevice = m_Context.GetDevice()->GetDevice();

    // Compute Unique Set
    for (auto& moduleIndex : _modules)
        for (size_t i = 0; i < moduleIndex.descriptor_set_count; i++)
            if (moduleIndex.descriptor_sets[i].set != std::numeric_limits<uint32_t>::max())
                layoutsMap.insert({moduleIndex.descriptor_bindings[i].set, {}});


    // Resize Vector
    uint32_t descritptorCount = 0;
    for (auto& moduleIndex : _modules)
    {
        for (size_t setIndex = 0; setIndex < moduleIndex.descriptor_set_count; setIndex++)
        {
            const SpvReflectDescriptorSet& s = moduleIndex.descriptor_sets[setIndex];

            if (s.binding_count == 0)
                continue;


            for (size_t descriptorIndex = 0; descriptorIndex < s.binding_count; descriptorIndex++)
            {
                SpvReflectDescriptorBinding& spvBinding = *s.bindings[descriptorIndex];

                vk::DescriptorSetLayoutBinding descriptorSetLayout{};
                descriptorSetLayout.binding = spvBinding.binding;
                descriptorSetLayout.descriptorType = static_cast<vk::DescriptorType>(spvBinding.descriptor_type);
                descriptorSetLayout.descriptorCount = spvBinding.count;
                descriptorSetLayout.stageFlags = static_cast<vk::ShaderStageFlags>(moduleIndex.shader_stage);
                descriptorSetLayout.pImmutableSamplers = nullptr; // optional

                descriptorTypeCount[static_cast<vk::DescriptorType>(spvBinding.descriptor_type)] += spvBinding.count;
                descritptorCount += spvBinding.count;

                auto& bindingsVec = layoutsMap[s.set];
                bool found = false;

                for (auto& existingBinding : bindingsVec)
                {
                    if (existingBinding.binding == descriptorSetLayout.binding)
                    {
                        existingBinding.stageFlags |= descriptorSetLayout.stageFlags;
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    bindingsVec.push_back(descriptorSetLayout);
                }
            }
        }
    }

    uint32_t maxSet = 0;
    for (const auto& it : layoutsMap)
        maxSet = std::max(maxSet, it.first);

    auto cacheDescriptor = std::make_shared<CacheDescriptor>();
    cacheDescriptor->descriptorSetLayout.clear();
    cacheDescriptor->descriptorSetLayout.resize(maxSet + 1, VK_NULL_HANDLE);

    for (const auto& it : layoutsMap)
    {
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
        descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(it.second.size());
        descriptorSetLayoutCreateInfo.pBindings = it.second.data();

        cacheDescriptor->descriptorSetLayout[it.first] =
            vkDevice.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
    }

    // Descriptor Pool
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes(descriptorTypeCount.size());

    size_t i = 0;
    for (auto it = descriptorTypeCount.begin(); it != descriptorTypeCount.end();)
    {
        vk::DescriptorPoolSize& descriptorPoolSize = descriptorPoolSizes[i];
        descriptorPoolSize.type = it->first;
        descriptorPoolSize.descriptorCount = it->second * static_cast<uint32_t>(MaxFramesInFlight);
        i++;
        ++it;
    }

    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet |
        vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    descriptorPoolCreateInfo.maxSets = MAX_ALLOC_DESCRIPTOR_SET;

    cacheDescriptor->descriptorPool = vkDevice.createDescriptorPool(descriptorPoolCreateInfo);


    descriptorLayoutCache.emplace(bindingMap, cacheDescriptor);
    m_DescriptorSets[m_IdCounter] = cacheDescriptor.get();
    cacheDescriptor->id = m_IdCounter++;

    return cacheDescriptor->id;
}


void Vulkan::VulkanDescritptorManager::ClearCaches()
{
    m_DescriptorSets.clear();

    vk::Device vkDevice = m_Context.GetDevice()->GetDevice();
    for (auto it : descriptorLayoutCache)
    {
        for (auto it : it.second->descriptorSetLayout)
            vkDevice.destroyDescriptorSetLayout(it);

        vkDevice.destroyDescriptorPool(it.second->descriptorPool);
    }
}

Vulkan::CacheDescriptor* Vulkan::VulkanDescritptorManager::GetDescriptorSets(size_t setID) const
{
    return m_DescriptorSets.at(setID);
}

bool Vulkan::VulkanDescritptorManager::FindInCache(const std::vector<SpvReflectShaderModule>& _modules,
                                                   SetBindingMap* _outSetBindingMap,
                                                   std::shared_ptr<CacheDescriptor>* cache) const
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    for (const auto& module : _modules)
    {
        for (uint32_t i = 0; i < module.descriptor_binding_count; ++i)
        {
            const SpvReflectDescriptorBinding& binding = module.descriptor_bindings[i];

            if (binding.set == std::numeric_limits<uint32_t>::max())
                continue;

            auto type = static_cast<vk::DescriptorType>(binding.descriptor_type);

            auto& setMap = (*_outSetBindingMap)[binding.set];

            auto it = setMap.find(binding.binding);
            if (it == setMap.end())
            {
                setMap.emplace(binding.binding, DescriptorInfo{type, static_cast<size_t>(module.shader_stage)});
            }
            else
            {
                it->second.stages |= module.shader_stage;
            }
        }
    }

    // Sort bindings to ensure canonical ordering

    auto it = descriptorLayoutCache.find(*_outSetBindingMap);
    if (it == descriptorLayoutCache.end())
        return false;

    *cache = it->second;
    return true;
}
