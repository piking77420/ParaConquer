#ifndef VULKAN_GRAPHIC_PIPELINE
#define VULKAN_GRAPHIC_PIPELINE

#include <LowRenderer/RhiGraphicPipeline.hpp> 

namespace Vulkan
{
	class VulkanGraphicPipeline : public PC_CORE::RhiGraphicPipeline
	{
	public:
		VulkanGraphicPipeline(PC_CORE::Rhi& _Rhi);
		virtual ~VulkanGraphicPipeline();

	private:

	};


} // namespace Vulkan

#endif // VULKAN_GRAPHIC_PIPELINE