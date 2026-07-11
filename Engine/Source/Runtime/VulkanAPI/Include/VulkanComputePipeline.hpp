#ifndef VULKAN_COMPUTE_PIPELINE
#define VULKAN_COMPUTE_PIPELINE

#include <VulkanHeader.h>
#include <LowRenderer/RhiComputePipeline.hpp> 

namespace Vulkan
{
	class VulkanComputePipeline : public PC_CORE::RhiComputePipeline
	{
	public:
		VulkanComputePipeline(PC_CORE::Rhi& _Rhi);
		virtual ~VulkanComputePipeline();

		bool Build() override;

		vk::PipelineLayout GetPipelineLayout() const;

		vk::Pipeline GetPipeline() const;

	private:
		size_t m_DescriptorId = std::numeric_limits<size_t>::max();

		vk::PipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		vk::Pipeline m_Pipeline = VK_NULL_HANDLE;
	};


} // namespace Vulkan

#endif // VULKAN_COMPUTE_PIPELINE