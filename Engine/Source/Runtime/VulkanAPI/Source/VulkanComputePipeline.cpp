#include <VulkanComputePipeline.hpp>
#include <Utils/VulkanPipelineHelper.hpp>
#include <LowRenderer/Rhi.hpp>
#include <VulkanContext.hpp>

namespace Vulkan
{
	VulkanComputePipeline::VulkanComputePipeline(PC_CORE::Rhi& _Rhi)
		: RhiComputePipeline(_Rhi)
	{

	}

    VulkanComputePipeline::~VulkanComputePipeline()
    {
        vk::Device device = GET_VK_DEVICE;

        if (m_PipelineLayout != VK_NULL_HANDLE)
        {
            device.destroyPipelineLayout(m_PipelineLayout);
            m_PipelineLayout = VK_NULL_HANDLE;
        }

        if (m_Pipeline != VK_NULL_HANDLE)
        {
            device.destroyPipeline(m_Pipeline);
            m_Pipeline = VK_NULL_HANDLE;
        }
    }

	bool VulkanComputePipeline::Build()
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Rhi);

		if (!m_Modules)
			return false;

		if (m_Modules->empty())
			return false;

		Utils::VulkanShaderProgramCreateContext ctx = Utils::CreateContext(GET_VK_DEVICE, *m_Modules);

        vk::Device d = GET_VK_DEVICE;

        if (ctx.pipelineShaderStageCreateInfos.size() != 1)
        {
            PC_LOGERROR("Compute Pipeline shoulde have one shader stage");
            return false;
        }

        assert(ctx.modulesReflected.size() == 1 &&
            ctx.modulesReflected[0].shader_stage & SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT);

        m_LocalSize =
        {
            ctx.modulesReflected[0].entry_points[0].local_size.x,
            ctx.modulesReflected[0].entry_points[0].local_size.y,
            ctx.modulesReflected[0].entry_points[0].local_size.z
        };

        m_PipelineLayout = Utils::CreatePipelineLayout(GET_VK_CONTEXT.descritptorManager, d, ctx);

        vk::ComputePipelineCreateInfo vkComputeCreateInfo;
        vkComputeCreateInfo.sType = vk::StructureType::eComputePipelineCreateInfo;
        vkComputeCreateInfo.layout = m_PipelineLayout;
        vkComputeCreateInfo.stage = ctx.pipelineShaderStageCreateInfos[0];

        vk::ResultValue<vk::Pipeline> r = d.createComputePipeline(nullptr, vkComputeCreateInfo);
        VK_CALL(r.result);

        m_Pipeline = r.value;

		Utils::DestroyContext(ctx);
		return true;
	}

    vk::PipelineLayout VulkanComputePipeline::GetPipelineLayout() const
    {
        return m_PipelineLayout;
    }

    vk::Pipeline VulkanComputePipeline::GetPipeline() const
    {
        return m_Pipeline;
    }
}

