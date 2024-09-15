#include "back_end/vulkan_harware_wrapper.hpp"

#include <GLFW/glfw3.h>
#include <spirv/spirv_reflect.h>


#ifdef _DEBUG
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
#endif

VK_NP::VulkanHarwareWrapper::VulkanHarwareWrapper()
{
	CreateInstance("ParaConquer");
}

VK_NP::VulkanHarwareWrapper::~VulkanHarwareWrapper()
{
	m_Instance.destroy(nullptr);
}

void VK_NP::VulkanHarwareWrapper::CreateInstance(const char* _AppName)
{
	vk::ApplicationInfo appInfo = {};

	appInfo.sType = vk::StructureType::eApplicationInfo,
		appInfo.pNext = nullptr,
		appInfo.pApplicationName = _AppName,
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		appInfo.pEngineName = "ParaConquer",
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		appInfo.apiVersion = VK_API_VERSION_1_3;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	vk::InstanceCreateInfo createInfo = {};

	createInfo.sType = vk::StructureType::eInstanceCreateInfo;
		createInfo.pNext = nullptr;
		createInfo.flags = {};
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = 0;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;


	vk::Result result = vk::createInstance(&createInfo, nullptr, &m_Instance);
	VK_CALL(result);


}
