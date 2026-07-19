#ifndef VULKAN_GRAPHIC_PIPELINE
#define VULKAN_GRAPHIC_PIPELINE

#include <VulkanHeader.h>
#include <LowRenderer/RhiGraphicPipeline.hpp> 

namespace Vulkan
{
	namespace Utils
	{
		struct VulkanShaderProgramCreateContext;
	} // namespace Utils


	class VulkanGraphicPipeline : public PC_CORE::RhiGraphicPipeline
	{
	public:
		VulkanGraphicPipeline(PC_CORE::Rhi& _Rhi);
		virtual ~VulkanGraphicPipeline();

		bool Build() override;

		vk::PipelineLayout GetPipelineLayout() const;

		vk::Pipeline GetPipeline() const;

	private:
		size_t m_DescriptorId = std::numeric_limits<size_t>::max();

		vk::PipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		vk::Pipeline m_Pipeline = VK_NULL_HANDLE;

		std::vector<vk::DynamicState> GetDynamicState() const;

		void CreatePipeLineGraphicsPipeline(const Utils::VulkanShaderProgramCreateContext& _VulkanShaderProgramCreateContex);

		void ParseRasterizer(vk::PipelineRasterizationStateCreateInfo* _pipelineRasterizationStateCreateInfo);

		void ParsePipelineColorAttachementBlendState(vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState, const std::optional<RhiGraphicPipeline::BlendState>& _blendInfo);

		void ParsePipelineDepthStencilAttachmentState(vk::PipelineDepthStencilStateCreateInfo* _PipelineDepthStencilStateCreateInfo, const RhiGraphicPipeline::DephStencilState& _dephInfo);

		void ParseParsePipelineColorBlendState(vk::PipelineColorBlendStateCreateInfo* _PipelineColorBlendStateCreateInfo, const vk::PipelineColorBlendAttachmentState* _PipelineColorBlendAttachmentState, size_t _PipelineColorBlendAttachmentSize, const std::optional<RhiGraphicPipeline::BlendState>& _blendInfo);

		vk::VertexInputBindingDescription ParseVertexInputBindingDescription(const PC_CORE::VertexInputBindingDescrition& _vertexInputBindingDescrition);

		vk::VertexInputAttributeDescription ParseVertexInputAttributeDescription(const PC_CORE::VertexAttributeDescription& _vertexAttributeDescription);

		vk::PipelineVertexInputStateCreateInfo ParseVertexInputState(std::vector<vk::VertexInputBindingDescription>* _vertexInputBindingDescriptions, std::vector<vk::VertexInputAttributeDescription>* _vertexInputAttributeDescriptions);
	};
		

} // namespace Vulkan

#endif // VULKAN_GRAPHIC_PIPELINE