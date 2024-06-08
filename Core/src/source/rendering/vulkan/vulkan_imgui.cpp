#include "rendering/vulkan/vulkan_imgui.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanImgui::Init(const PC_CORE::Window& _window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    VkDescriptorPoolSize pool_sizes[] =
       {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    VkResult err = vkCreateDescriptorPool(VulkanInterface::vulkanDevice.device, &pool_info, nullptr, &m_DescriptorPool);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(_window.window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = VulkanInterface::vulkanInstance.instance;
    init_info.PhysicalDevice = VulkanInterface::vulkanPhysicalDevices.GetCurrentDevice().physDevice;
    init_info.Device = VulkanInterface::vulkanDevice.device;
    init_info.QueueFamily = VulkanInterface::vulkanDevice.graphicsQueue.index;
    init_info.Queue = VulkanInterface::vulkanDevice.graphicsQueue.Queue;
    init_info.PipelineCache = nullptr;
    init_info.DescriptorPool = m_DescriptorPool;
    init_info.RenderPass = VulkanInterface::vulkanSwapChapchain.mainRenderPass.renderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = VulkanInterface::vulkanSwapChapchain.nbrOfImage;
    init_info.ImageCount = VulkanInterface::vulkanSwapChapchain.nbrOfImage;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = CheckErrorImgui;
    ImGui_ImplVulkan_Init(&init_info);
}

void VulkanImgui::Destroy()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(VulkanInterface::vulkanDevice.device, m_DescriptorPool, nullptr);
}

void VulkanImgui::NewFrame() const
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void VulkanImgui::Render(VkCommandBuffer* _commandBuffer)
{
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, *_commandBuffer);
}

void VulkanImgui::CheckErrorImgui(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}
