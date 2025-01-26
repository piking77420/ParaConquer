#include "vulkan_swap_chain.hpp"

#include "vulkan_context.hpp"
#include "vulkan_physical_devices.hpp"
#include "GLFW/glfw3.h"


Vulkan::VulkanSwapChain::VulkanSwapChain(size_t _width, size_t _height)
{
	VulkanContext& vulkanContext = reinterpret_cast<VulkanContext&>(VulkanContext::GetContext());
	const Vulkan::SwapChainSupportDetails swapChainSupportDetails = std::reinterpret_pointer_cast<VulkanPhysicalDevices>(vulkanContext.physicalDevices)->GetSwapChainSupportDetails();
	const auto& queueFamilyIndices = std::reinterpret_pointer_cast<VulkanPhysicalDevices>(vulkanContext.physicalDevices)->GetQueuesFamilies();

	const vk::SurfaceFormatKHR surfaceFormatKHR = ChooseSwapSurfaceFormat(swapChainSupportDetails.formats);
	const vk::PresentModeKHR presentModeKHR = ChooseSwapPresentMode(swapChainSupportDetails.presentModes);
	const vk::Extent2D extent2D = ChooseSwapExtent(swapChainSupportDetails.capabilities, _width, _height);

	uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

	if (swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.capabilities.maxImageCount)
	{
		imageCount = swapChainSupportDetails.capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR swapChainCreateInfo{};
	swapChainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
	swapChainCreateInfo.surface = vulkanContext.GetSurface();
	swapChainCreateInfo.minImageCount = imageCount;
	swapChainCreateInfo.imageFormat = surfaceFormatKHR.format;
	swapChainCreateInfo.imageColorSpace = surfaceFormatKHR.colorSpace;
	swapChainCreateInfo.imageExtent = extent2D;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

	uint32_t presentIndex = 0;
	for (int i = static_cast<int>(queueFamilyIndices.size()) - 1; i >= 0; i--)
	{
		if (queueFamilyIndices[static_cast<size_t>(i)].supportPresent)
		{
			presentIndex = static_cast<uint32_t>(i);
			break;
		}
	}

	swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
	swapChainCreateInfo.queueFamilyIndexCount = 2;
	swapChainCreateInfo.pQueueFamilyIndices = &presentIndex;
	swapChainCreateInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapChainCreateInfo.presentMode = presentModeKHR;
	swapChainCreateInfo.clipped = vk::True;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;


	vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(vulkanContext.rhiDevice)->GetDevice();
	m_SwapChain = device.createSwapchainKHR(swapChainCreateInfo, nullptr);

	VK_CALL(device.getSwapchainImagesKHR(m_SwapChain, &imageCount, nullptr));
	m_SwapChainImage.resize(imageCount);
	VK_CALL(device.getSwapchainImagesKHR(m_SwapChain, &imageCount, m_SwapChainImage.data()));

}

Vulkan::VulkanSwapChain::~VulkanSwapChain()
{
	VulkanContext& vulkanContext = VulkanContext::GetContext();
	std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>(vulkanContext.rhiDevice);

	device->GetDevice().destroySwapchainKHR(m_SwapChain);
}

vk::SurfaceFormatKHR Vulkan::VulkanSwapChain::ChooseSwapSurfaceFormat(
	const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}
	return availableFormats[0];
}

vk::PresentModeKHR Vulkan::VulkanSwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
		{
			return availablePresentMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Vulkan::VulkanSwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t _width, uint32_t _height)
{

	vk::Extent2D actualExtent =
	{
		_width,
		_height
	};

	actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return actualExtent;

}

