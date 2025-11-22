#pragma once

#include <Map>
#include <Stack>

#include "VulkanHeader.h"
#include <spirv_reflect.h>


namespace Vulkan
{
    class VulkanContext;

    struct DescriptorInfo
    {
        vk::DescriptorType type;
        size_t stages;
        auto operator<=>(const DescriptorInfo&) const = default;
    };


    using BindingMap = std::map<uint32_t, DescriptorInfo>;
    using SetBindingMap = std::map<uint32_t, BindingMap>;


    struct CacheDescriptor
    {
        size_t id;
        std::vector<vk::DescriptorSetLayout> descriptorSetLayout;
        vk::DescriptorPool descriptorPool;
    };


    struct SetBindingMapHasher
    {
        std::size_t operator()(const SetBindingMap& map) const
        {
            std::size_t h = 0;

            for (const auto& [setIndex, bindingMap] : map)
            {
                std::size_t setHash = std::hash<uint32_t>{}(setIndex);

                for (const auto& [stageFlag, descriptorTypes] : bindingMap)
                {
                    std::size_t stageHash = std::hash<uint32_t>{}(static_cast<uint32_t>(stageFlag));

                    std::size_t typeHashstage = std::hash<uint32_t>{}(static_cast<uint32_t>(descriptorTypes.stages));
                    // Combine using boost-like hash combine
                    stageHash ^= typeHashstage + 0x9e3779b9 + (stageHash << 6) + (stageHash >> 2);

                    std::size_t typeHashtype = std::hash<uint32_t>{}(static_cast<uint32_t>(descriptorTypes.stages));
                    // Combine using boost-like hash combine
                    stageHash ^= typeHashtype + 0x9e3779b9 + (stageHash << 6) + (stageHash >> 2);


                    // Combine stage hash into set hash
                    setHash ^= stageHash + 0x9e3779b9 + (setHash << 6) + (setHash >> 2);
                }

                // Combine set hash into global hash
                h ^= setHash + 0x9e3779b9 + (h << 6) + (h >> 2);
            }

            return h;
        }
    };

    inline bool operator==(const SetBindingMap& a, const SetBindingMap& b)
    {
        if (a.size() != b.size())
            return false;

        for (const auto& [setIndex, aBindingMap] : a)
        {
            auto bIt = b.find(setIndex);
            if (bIt == b.end())
                return false;

            const auto& bBindingMap = bIt->second;
            if (aBindingMap.size() != bBindingMap.size())
                return false;

            for (const auto& [stageFlag, aTypes] : aBindingMap)
            {
                auto bStageIt = bBindingMap.find(stageFlag);
                if (bStageIt == bBindingMap.end())
                    return false;

                const auto& bTypes = bStageIt->second;
                if (aTypes != bTypes)
                    return false;
            }
        }

        return true;
    }

    struct SetBindingMapEqual
    {
        bool operator()(const SetBindingMap& a, const SetBindingMap& b) const
        {
            return a == b;
        }
    };

    constexpr uint32_t MAX_ALLOC_DESCRIPTOR_SET = 100 * MaxFramesInFlight;

    // TODO FIX THIS SHIT
    class VulkanDescritptorManager
    {
    public:

        explicit VulkanDescritptorManager(VulkanContext& _Context);

        size_t GetDescriptorId(const std::vector<SpvReflectShaderModule>& _modules);

        void ClearCaches();

        CacheDescriptor* GetDescriptorSets(size_t setID) const;

    private:
        bool FindInCache(const std::vector<SpvReflectShaderModule>& _modules, SetBindingMap* _outSetBindingMap,
                         std::shared_ptr<CacheDescriptor>* cache) const;

        VulkanContext& m_Context;

        size_t m_IdCounter = 0;

        std::stack<size_t> idStack;

        std::unordered_map<SetBindingMap, std::shared_ptr<CacheDescriptor>, SetBindingMapHasher,
                           SetBindingMapEqual> descriptorLayoutCache;

        std::unordered_map<size_t, CacheDescriptor*> m_DescriptorSets;
    };
}
