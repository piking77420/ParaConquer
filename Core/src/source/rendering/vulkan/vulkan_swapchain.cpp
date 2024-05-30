#include "rendering/vulkan/vulkan_swapchain.hpp"

#include <cassert>
#include <stacktrace>

#include "log.hpp"

using namespace PC_CORE;

uint32_t VulkanSwapchain::ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities)
{
	Log::Debug("Choose Num Images");

	const uint32_t RequestedNumImages = Capabilities.minImageCount + 1;

	int32_t FinalNumImages = 0;

	if ((Capabilities.maxImageCount > 0) && (RequestedNumImages > Capabilities.maxImageCount))
	{
		FinalNumImages = Capabilities.maxImageCount;
	}
	else {
		FinalNumImages = RequestedNumImages;
	}

	return FinalNumImages;
}

VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& PresentModes)
{
	Log::Debug("ChoosePresentMode");
	
	for (int32_t i = 0; i < PresentModes.size(); i++)
	{
		
		if (PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return PresentModes[i];
		}
	}

	// Fallback to FIFO which is always supported OpenglStyle 2 buffering
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& SurfaceFormats)
{
	Log::Debug("Choose Surface Format And ColorSpace");
	
	for (int32_t i = 0; i < SurfaceFormats.size(); i++) {
		if ((SurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) &&
			(SurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
			{
			return SurfaceFormats[i];
			}
	}

	return SurfaceFormats[0];
}

VkImageView VulkanSwapchain::CreateImageView(VkDevice Device, VkImage Image, VkFormat Format,
	VkImageAspectFlags AspectFlags, VkImageViewType ViewType, uint32_t LayerCount, uint32_t mipLevels)
{
	const VkImageViewCreateInfo ViewInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = Image,
		.viewType = ViewType,
		.format = Format,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		},
		.subresourceRange = {
			.aspectMask = AspectFlags,
			.baseMipLevel = 0,
			.levelCount = mipLevels,
			.baseArrayLayer = 0,
			.layerCount = LayerCount
		}
	};

	VkImageView ImageView;
	VkResult res = vkCreateImageView(Device, &ViewInfo, NULL, &ImageView);
	VK_CHECK_ERROR(res, "vkCreateImageView");
	return ImageView;
}



uint32_t VulkanSwapchain::Init(const PhysicalDevice& _physicalDevice, const uint32_t _qfamilyIndex, const VkSurfaceKHR& _surface ,
                           const VkDevice _device)
{
	Log::Debug("Creating SwapChain");
	
    const VkSurfaceCapabilitiesKHR& SurfaceCaps =_physicalDevice.surfaceCaps;

	const uint32_t numSwapChainImages = ChooseNumImages(SurfaceCaps);

	const std::vector<VkPresentModeKHR>& PresentModes = _physicalDevice.presentModes;
	const VkPresentModeKHR PresentMode = ChoosePresentMode(PresentModes);

	surfaceFormatKhr = ChooseSurfaceFormatAndColorSpace(_physicalDevice.surfaceFormats);

	const VkSwapchainCreateInfoKHR SwapChainCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = _surface,
		.minImageCount = numSwapChainImages,
		.imageFormat = surfaceFormatKhr.format,
		.imageColorSpace = surfaceFormatKhr.colorSpace,
		.imageExtent = SurfaceCaps.currentExtent,
		.imageArrayLayers = 1,
		.imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &_qfamilyIndex,
		.preTransform = SurfaceCaps.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = PresentMode,
		.clipped = VK_TRUE
	};

	VkResult res = vkCreateSwapchainKHR(_device, &SwapChainCreateInfo, NULL, &swapchainKhr);
	VK_CHECK_ERROR(res, "vkCreateSwapchainKHR");

	Log::Debug("Swap chain created");

	uint32_t NumSwapChainImages = 0;
	res = vkGetSwapchainImagesKHR(_device, swapchainKhr, &NumSwapChainImages, NULL);
	VK_CHECK_ERROR(res, "vkGetSwapchainImagesKHR");
	assert(numSwapChainImages <= NumSwapChainImages);
    
	Log::Debug("Requested "+ std::to_string(numSwapChainImages) +" images, created "+ std::to_string(NumSwapChainImages) +" images");

	swapChainImage.resize(NumSwapChainImages);
	swapChainImageView.resize(NumSwapChainImages);

	res = vkGetSwapchainImagesKHR(_device, swapchainKhr, &NumSwapChainImages, swapChainImage.data());
	VK_CHECK_ERROR(res, "vkGetSwapchainImagesKHR");

	// TODO wrapper
	int32_t LayerCount = 1;
	int32_t MipLevels = 1;
	
	for (uint32_t i = 0; i < NumSwapChainImages; i++)
	{
		swapChainImageView[i] = CreateImageView(_device, swapChainImage[i], surfaceFormatKhr.format,
						       VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, LayerCount, MipLevels);
	}

	return numSwapChainImages;
}

void VulkanSwapchain::Destroy(const VkDevice& device)
{
	Log::Debug("Destroy swapChainImageView SwapChain");
	for (VkImageView& i : swapChainImageView)
	{
		vkDestroyImageView(device, i, nullptr);
	}
	
	Log::Debug("vkDestroySwapchainKHR");
	vkDestroySwapchainKHR(device, swapchainKhr, nullptr);
}
