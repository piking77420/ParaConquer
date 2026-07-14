#include <Rendering/PipelineStateObject.hpp>

namespace PC_CORE::Rendering
{
    PipelineCache::PipelineQueryResult PipelineStateObject::UpdatePipelineData(
        PipelineCache& PipelineCache, 
        PC_CORE::RhiRenderPass& _RenderPass, 
        const uint32_t _SubPassIndex)
	{
        if (m_NeedsRebuild)
        {
            GraphicPipelineDescriptor
                .SetRenderPass(_RenderPass)
                .SetSubPassIndex(_SubPassIndex);
            SetNeedRebuild(false);
        }
        // Find name for pso ???
        const bool Delayable = false; // TODO handle async
        PipelineCache::PipelineQueryResult Result = PipelineCache.CreateOrGetGraphicPipelineCache(&PipelineCacheID,
            "TestPSO",
            ModuleList,
            GraphicPipelineDescriptor,
            Delayable
        );
        
        return Result;
	}

} // namespace PC_CORE::Rendering
