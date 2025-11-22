#pragma once

#include <Vulkan/Vulkan.hpp>

#include "CoreHeader.hpp"
#include "Resources/Texture2d.hpp"


struct ImguiImage
{
    DEFAULT_CONSTRUCTOR_DESTRUCTOR(ImguiImage)

    DEFAULT_COPY_MOVE_OPERATIONS(ImguiImage)

    VkDescriptorSet descritproSet;
    PC_CORE::Texture2D texure;
};

BEGIN_PCCORE


class Rhi;
class CommandList;
class RhiSampler;
class RhiTexture;

#define IMGUI_RENDER_DEBUG_COLOR {0,1,0,1}

class IMGUIContext
{
public:

    PC_CORE_API IMGUIContext() = default;

    PC_CORE_API ~IMGUIContext() = default;

    PC_CORE_API void Init(Rhi& _Rhi, void* _glfwWindowPtr);

    PC_CORE_API void NewFrame();

    PC_CORE_API void Destroy();

    PC_CORE_API void Render(CommandList* _commandBuffer);

    PC_CORE_API void CreateImguiVulkanTexture(const RhiTexture* _texture, const RhiSampler* _sampler,
                                                VkDescriptorSet* _descriptors, size_t _descriptorsCount);

    PC_CORE_API void DestroyVulkanTexture(VkDescriptorSet* _descriptors, size_t _descriptorsCount);

private:
    Rhi* m_Rhi{nullptr};

    vk::DescriptorPool descriptorPool;

    vk::Device device;

    void VulkanInitialize(void* _glfwWindowPtr);
    //static inline PC_CORE::DescriptorPoolHandle m_DescriptorPoolHandle = NULL_HANDLE;
};

END_PCCORE
