#include <Rendering/PipelineCache.hpp>
#include <LowRenderer/Rhi.hpp> 

namespace PC_CORE::Rendering
{
	PipielineCache::PipielineCache(PC_CORE::Rhi& _Rhi)
		: m_Rhi(_Rhi)
	{

	}
	/*
	std::expected<RhiPipeline&, PipielineCache::PipielineCacheError> PipielineCache::CreateOrGet(PipilineCacheID* PipilineCacheID,
		const std::span<PC_CORE::WeakObjectPtr<ShaderSourceBinary>>& _Modules)
	{
		if (PipilineCacheID == nullptr)
		{
			return std::unexpected(PipielineCacheError::InvalidCachePtr);
		}

		if (std::holds_alternative<std::monostate>(_PipelineData))
		{
			return std::unexpected(PipielineCacheError::InvalidCachePtr);
		}

		if (_Modules.size() == 0)
		{
			return std::unexpected(PipielineCacheError::InvalidModules);
		}

		auto it = m_PipilineCacheID.find(*PipilineCacheID);
		if (it == m_PipilineCacheID.end())
			return *it->second;


	}*/

} // PC_CORE::Rendering
