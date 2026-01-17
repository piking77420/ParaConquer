#include "Io/ImguiContext.h"

#include <memory>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"

#include "LowRenderer/Rhi.hpp"

#include "Resources/ResourceManager.hpp"
#include "VulkanCommandList.hpp"
#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevices.hpp"
#include "VulkanSampler.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanTexture.hpp"

namespace Vulkan
{
    struct VulkanImageHandle;
}

using namespace PC_CORE;

ImGuiIO* io = nullptr;

static void CheckError(VkResult err)
{
    if (err == 0)
        return;
    std::cout << "[vulkan] Error: VkResult = " << err << "\n";
    if (err < 0)
        abort();
}

void IMGUIContext::Init(Rhi& _Rhi, void* _glfwWindowPtr)
{
    PERF_REGION_SCOPED;
    m_Rhi = &_Rhi;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    //io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    ImGui::StyleColorsDark();


    switch (_Rhi.GetGraphicsApi())
    {
    case GraphicAPI::None:
        break;
    case GraphicAPI::Vulkan:
        VulkanInitialize(_glfwWindowPtr);
        break;
    case GraphicAPI::D3d12:
        break;
    case GraphicAPI::Count:
        break;
    default: ;
    }
}


void IMGUIContext::NewFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void IMGUIContext::Destroy()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    device.destroyDescriptorPool(descriptorPool);
}


void IMGUIContext::VulkanInitialize(void* _glfwWindowPtr)
{
    auto windowPtr = static_cast<GLFWwindow*>(_glfwWindowPtr);
    constexpr bool installCallBack = true;
    ImGui_ImplGlfw_InitForVulkan(windowPtr, installCallBack);

    auto vkcontext = reinterpret_cast<Vulkan::VulkanContext*>(&m_Rhi->GetRhiContext());

    VkDescriptorPoolSize pool_sizes[] =
    {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;

    device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(vkcontext->rhiDevice)->GetDevice();
    descriptorPool = device.createDescriptorPool(*reinterpret_cast<vk::DescriptorPoolCreateInfo*>(&pool_info), nullptr);

    std::shared_ptr<Vulkan::VulkanSwapChain> rhiSwapChain = std::reinterpret_pointer_cast<Vulkan::VulkanSwapChain>(
        vkcontext->rhiSwapChain);
    vk::RenderPass renderPass = std::reinterpret_pointer_cast<Vulkan::VulkanRenderPass>(
        rhiSwapChain->GetSwapChainRenderPass())->GetVulkanRenderPass();

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = std::reinterpret_pointer_cast<Vulkan::VulkanInstance>(vkcontext->renderInstance)->
        GetVulkanInstance();
    init_info.PhysicalDevice = std::reinterpret_pointer_cast<Vulkan::VulkanPhysicalDevices>(vkcontext->rhiPhysicalDevices)
        ->GetVulkanDevice();
    init_info.Device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(vkcontext->rhiDevice)->GetDevice();
    init_info.QueueFamily = 0;
    init_info.Queue = vkcontext->mainQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = descriptorPool;
    init_info.MinImageCount = static_cast<uint32_t>(rhiSwapChain->GetNbrOfImage());
    init_info.ImageCount = static_cast<uint32_t>(rhiSwapChain->GetNbrOfImage());
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.PipelineInfoMain.RenderPass = renderPass;
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.CheckVkResultFn = CheckError;
    ImGui_ImplVulkan_Init(&init_info);
}

void IMGUIContext::CreateImguiVulkanTexture(const RhiTexture* _texture, const RhiSampler* _sampler,
                                            VkDescriptorSet* _descriptors, size_t _descriptorsCount)
{
    const Vulkan::VulkanTexture& vulkanTexture = reinterpret_cast<const Vulkan::VulkanTexture&>(*_texture);
    const Vulkan::VulkanSampler& vulkanSampler = reinterpret_cast<const Vulkan::VulkanSampler&>(*_sampler);

    const VkSampler vkSamplers = vulkanSampler.GetVkSampler();

    if (vkSamplers == nullptr)
    {
        PC_LOGERROR("Vulkan sampler is null");
        return;
    }
    for (size_t i = 0; i < _descriptorsCount; i++)
        _descriptors[i] = ImGui_ImplVulkan_AddTexture(vkSamplers, vulkanTexture.GetTextureAndAlloc()->ImageView,
                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void IMGUIContext::DestroyVulkanTexture(VkDescriptorSet* _descriptors, size_t _descriptorsCount)
{
    for (size_t i = 0; i < _descriptorsCount; i++)
    {
        if (_descriptors[i] == VK_NULL_HANDLE)
            continue;

        ImGui_ImplVulkan_RemoveTexture(_descriptors[i]);
    }
}


void IMGUIContext::Render(CommandList* _commandBuffer)
{
    ImGui::Render();

    _commandBuffer->BeginDebugLabel("Imgui Draw", IMGUI_RENDER_DEBUG_COLOR);

    ImDrawData* draw_data = ImGui::GetDrawData();
    vk::CommandBuffer commandBuffer = reinterpret_cast<Vulkan::VulkanCommandList*>(_commandBuffer)->GetVulkanCommandBufferHandle();
    ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);

    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    _commandBuffer->EndDebugLabel();
}
