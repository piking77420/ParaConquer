
#include "io/imgui_context.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "front_end/vulkan_app.hpp"
#include "Imgui/imgui_impl_vulkan.h"

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

void IMGUIContext::Init(void* _glfwWindowPtr)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    ImGui::StyleColorsDark();

    GLFWwindow* windowPtr = static_cast<GLFWwindow*>(_glfwWindowPtr);
    constexpr bool installCallBack = true;
    ImGui_ImplGlfw_InitForVulkan(windowPtr, installCallBack);
   
    ImGui_ImplVulkan_InitInfo init_info = Vulkan::VulkanApp::GetImGuiInitInfo(&m_DescriptorPoolHandle);
    init_info.CheckVkResultFn = CheckError;
    ImGui_ImplVulkan_Init(&init_info);
}

void IMGUIContext::Destroy()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    RHI::GetInstance().DestroyDescriptorPool(m_DescriptorPoolHandle);
}

void IMGUIContext::NewFrame()
{

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void IMGUIContext::Render(PC_CORE::CommandBuffer _commandBuffer)
{
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, *reinterpret_cast<vk::CommandBuffer*>(&_commandBuffer.handle));

    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}


