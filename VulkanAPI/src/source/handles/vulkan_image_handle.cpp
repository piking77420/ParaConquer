#include "handles/vulkan_image_handle.hpp"

#include "low_renderer/rhi.hpp"

Vulkan::VulkanImageHandle::VulkanImageHandle()
{

}

Vulkan::VulkanImageHandle::~VulkanImageHandle()
{
	if (allocation == VK_NULL_HANDLE && view == VK_NULL_HANDLE && image == VK_NULL_HANDLE)
	{
		return;
	}

	if (image == VK_NULL_HANDLE || allocation == VK_NULL_HANDLE ||
		view == VK_NULL_HANDLE)
	{
		PC_LOGERROR("One of image,allocation or view is VK_NULL_HANDLE");
		return;
	}



	if (PC_CORE::RhiContext::GetContext().gpuResourceAllocator->DestroyImage(this))
	{
		Clear();
	}
}

void Vulkan::VulkanImageHandle::Clear()
{
	allocation = VK_NULL_HANDLE;
	view = VK_NULL_HANDLE;
	image = VK_NULL_HANDLE;
}
