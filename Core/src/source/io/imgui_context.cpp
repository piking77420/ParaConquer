
#include "io/imgui_context.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "vulkan_instance.hpp"
#include "Imgui/imgui_impl_vulkan.h"
#include <memory>

#include "vulkan_command_list.hpp"
#include "vulkan_context.hpp"
#include "vulkan_device.hpp"
#include "vulkan_physical_devices.hpp"
#include "vulkan_swap_chain.hpp"
#include "resources/vulkan_sampler.hpp"

/*
#include "imgui_impl_vulkan.h"
#include "front_end/vulkan_app.hpp"
*/
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

void IMGUIContext::Init(void* _glfwWindowPtr, PC_CORE::GraphicAPI _graphicApi)
{
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    ImGui::StyleColorsDark();


    
    switch (_graphicApi)
    {
    case GraphicAPI::NONE:
        break;
    case GraphicAPI::VULKAN:
        VulkanInitialize(_glfwWindowPtr);
        break;
    case GraphicAPI::DX3D12:
        break;
    case GraphicAPI::COUNT:
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

IMGUIContext::IMGUIContext()
{
    
}

IMGUIContext::~IMGUIContext()
{
    device.destroyDescriptorPool(descriptorPool);
    
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void IMGUIContext::VulkanInitialize(void* _glfwWindowPtr)
{
    GLFWwindow* windowPtr = static_cast<GLFWwindow*>(_glfwWindowPtr);
    constexpr bool installCallBack = true;
    ImGui_ImplGlfw_InitForVulkan(windowPtr, installCallBack);

    RhiContext* context = Rhi::GetRhiContext();
    Vulkan::VulkanContext* vkcontext = reinterpret_cast<Vulkan::VulkanContext*>(Rhi::GetRhiContext());
    
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
    descriptorPool = device.createDescriptorPool(*reinterpret_cast<vk::DescriptorPoolCreateInfo*>(&pool_info),nullptr);

    std::shared_ptr<Vulkan::VulkanSwapChain> swapChain = std::reinterpret_pointer_cast<Vulkan::VulkanSwapChain>(context->swapChain);
    vk::RenderPass renderPass = std::reinterpret_pointer_cast<Vulkan::VulkanRenderPass>(swapChain->GetSwapChainRenderPass())->GetVulkanRenderPass();

    ImGui_ImplVulkan_InitInfo init_info =
        {
        .Instance = std::reinterpret_pointer_cast<Vulkan::VulkanInstance>(vkcontext->renderInstance)->GetVulkanInstance(),
        .PhysicalDevice = std::reinterpret_pointer_cast<Vulkan::VulkanPhysicalDevices>(vkcontext->physicalDevices)->GetVulkanDevice(),
        .Device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(vkcontext->rhiDevice)->GetDevice(),
        .QueueFamily = 0,
        .Queue = vkcontext->graphicsQueue,
        .DescriptorPool = descriptorPool,
        .RenderPass = renderPass,
        .MinImageCount = static_cast<uint32_t>(swapChain->GetNbrOfImage()),
        .ImageCount = static_cast<uint32_t>(swapChain->GetNbrOfImage()),
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .Subpass = 0,
        .CheckVkResultFn = CheckError,
        };
    init_info.CheckVkResultFn = CheckError;
    ImGui_ImplVulkan_Init(&init_info);
}




void IMGUIContext::Render(CommandList* _commandBuffer)
{
    ImGui::Render();
    
    ImDrawData* draw_data = ImGui::GetDrawData();
    vk::CommandBuffer commandBuffer = reinterpret_cast<Vulkan::VulkanCommandList*>(_commandBuffer)->GetHandle();
    ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);
    
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}



