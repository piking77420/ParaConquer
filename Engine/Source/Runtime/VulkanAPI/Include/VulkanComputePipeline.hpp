#ifndef VULKAN_COMPUTE_PIPELINE
#define VULKAN_COMPUTE_PIPELINE

#include <LowRenderer/RhiComputePipeline.hpp> 

namespace Vulkan
{
	class VulkanComputePipeline : public PC_CORE::RhiComputePipeline
	{
	public:
		VulkanComputePipeline(PC_CORE::Rhi& _Rhi);
		virtual ~VulkanComputePipeline();

	private:

	};


} // namespace Vulkan

#endif // VULKAN_COMPUTE_PIPELINE