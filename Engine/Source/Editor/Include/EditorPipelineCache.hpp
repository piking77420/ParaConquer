#ifndef EDITOR_PIPELINE_CACHE
#define EDITOR_PIPELINE_CACHE

#include <Rendering/PipelineCache.hpp>

namespace PC_EDITOR_CORE 
{
	class EditorPipelineCache : public PC_CORE::Rendering::PipelineCache
	{
	public:
		EditorPipelineCache(PC_CORE::Rhi& _Rhi);
		virtual ~EditorPipelineCache();

	protected:
		std::expected<bool, PC_CORE::Rendering::PipelineCache::PipelineCacheQueryResult> LookForModuleFile(
			const std::string& _BaseShaderPath,
			const std::string& _VariantExpectedPath,
			PC_CORE::Rendering::ShaderFeatureFlags _ShaderFeatureFlags,
			const std::span<std::pair<std::string, int>>& _MacroDefinitions,
			bool _AsyncCompile) override;
		
	};
} // namespace PC_EDITOR_CORE

#endif // EDITOR_PIPELINE_CACHE