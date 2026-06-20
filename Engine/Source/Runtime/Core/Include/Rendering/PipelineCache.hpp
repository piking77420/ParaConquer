#ifndef PC_CORE_RENDERING_PIPELINE_CACHE
#define PC_CORE_RENDERING_PIPELINE_CACHE

#include <expected>
#include <unordered_map>
#include <functional>
#include <limits>
#include <span>

#include <CoreHeader.hpp>
#include <LowRenderer/RhiPipeline.hpp>
#include <ObjectPtr.hpp>
#include <Resources/ShaderSourceBinary.hpp>

namespace PC_CORE
{
	class Rhi;
} // PC_CORE

namespace PC_CORE::Rendering
{
	using PipilineCacheID = size_t;
    static constexpr PipilineCacheID NULL_PIPIPELINE_ID = std::numeric_limits<PipilineCacheID>::max();

	class PC_CORE_API PipielineCache
	{
	public:
        enum class PipielineCacheError
        {
            None,
            InvalidCachePtr,
            InvalidPipelineCache,
            InvalidModules,
        };

		PipielineCache(Rhi& Rhi);
		~PipielineCache() = default;
		DEFAULT_COPY_MOVE_OPERATIONS(PipielineCache);

		/*std::expected<RhiPipeline&, PipielineCacheError> CreateOrGet(PipilineCacheID* PipilineCacheID,
                                                      const PC_CORE::RhiPipeline::PipelineData& _PipelineData,
                                                      const std::span<PC_CORE::WeakObjectPtr<ShaderSourceBinary>>& _Modules);*/

	private:
		Rhi& m_Rhi;
	};

} // namespace PC_CORE::Rendering

#endif // PC_CORE_RENDERING_PIPELINE_CACHE

