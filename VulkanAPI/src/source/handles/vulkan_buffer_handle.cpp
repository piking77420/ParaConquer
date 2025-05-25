#include "handles/vulkan_buffer_handle.hpp"

#include "vulkan_context.hpp"

Vulkan::VulkanBufferHandle::VulkanBufferHandle()
{
	buffer = VK_NULL_HANDLE;
	allocation = VK_NULL_HANDLE;
}

Vulkan::VulkanBufferHandle::~VulkanBufferHandle()
{
	if (buffer == VK_NULL_HANDLE && allocation == VK_NULL_HANDLE)
	{
		return;
	}

	if (buffer == VK_NULL_HANDLE || allocation == VK_NULL_HANDLE)
	{
		PC_LOGERROR("One of buffer or allocation is VK_NULL_HANDLE");
		return;
	}


	if (PC_CORE::RhiContext::GetContext().gpuResourceAllocator->DestroyBuffer(this))
	{
		buffer = VK_NULL_HANDLE;
		allocation = VK_NULL_HANDLE;
	}
	else
	{
		PC_LOGERROR("Failed To Destroy Buffer Handle");
	}
}

void Vulkan::VulkanBufferHandle::Clear()
{
	buffer = VK_NULL_HANDLE;
	allocation = VK_NULL_HANDLE;
}
