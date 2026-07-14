#ifndef RENDERING_PIPELINE_STATE_OBJECT
#define RENDERING_PIPELINE_STATE_OBJECT

#include <Rendering/PipelineCache.hpp> // TODO as foward declare

namespace PC_CORE
{
    class RhiRenderPass;
    class RhiGraphicPipeline;
} // namespace PC_CORE 

namespace PC_CORE::Rendering
{
	class PipelineStateObject
	{
	public:
		PipelineStateObject() = default;
		~PipelineStateObject() = default;
        DEFAULT_COPY_MOVE_OPERATIONS(PipelineStateObject)

        void SetNeedRebuild(bool _NeedRebuild)
        {
            if (m_NeedsRebuild == _NeedRebuild)
                return;

            if (m_NeedsRebuild) {
                PipelineCacheID = Rendering::PipelineCacheID{};
                GraphicPipelineDescriptor = RhiGraphicPipeline::Descriptor{};
            }

            // may be atomic in future
            m_NeedsRebuild = _NeedRebuild;
        }

        [[nodiscard]] bool GetNeedRebuild() const
        {
            return m_NeedsRebuild;
        }

        PipelineCache::PipelineQueryResult UpdatePipelineData(
            PipelineCache& PipelineCache,
            PC_CORE::RhiRenderPass& _RenderPass,
            uint32_t _SubPassIndex);

        PC_CORE::Rendering::PipelineCache::ModuleEntryList ModuleList;
        PC_CORE::Rendering::PipelineCacheID PipelineCacheID;
        RhiGraphicPipeline::Descriptor GraphicPipelineDescriptor;

	private:
        bool m_NeedsRebuild = true;
	};

} // namespace PC_CORE::Rendering

#endif // RENDERING_PIPELINE_STATE_OBJECT