#include <Rendering/PipelineCache.hpp>
#include <LowRenderer/Rhi.hpp> 
#include <Io/FileLoader.hpp>

std::string_view GetShaderFormatExt(const std::string_view& shaderPath)
{
	size_t firstDot = shaderPath.find('.'); // first dot in filename
	if (firstDot != std::string::npos)
	{
		return shaderPath.substr(firstDot); // from first dot to end
	}
	return ""sv; // no extension
}


namespace PC_CORE::Rendering
{
	static constexpr std::string_view VariantPath = "Variant"sv;

	PipelineCache::PipelineCache(PC_CORE::Rhi& _Rhi)
		: m_Rhi(_Rhi)
	{

	}

	PipelineCache::PipelineQueryResult PipelineCache::CreateOrGetGraphicPipelineCache(
		PipelineCacheHandleID* _PipilineCacheID,
		const std::string_view& _PipelineName,
		const ModuleEntryList& _ModuleEntryList,
		const RhiGraphicPipeline::Descriptor& _Descriptor,
		bool _Delayable)
	{
		
		if (!_PipilineCacheID)
			return std::unexpected(PipelineCache::PipelineCacheQueryResult::InvalidCachePtr);

		const PipelineCacheHandleID Id = *_PipilineCacheID;
		PipelineQueryResult query = tryToFindInCache(_PipilineCacheID, m_GraphicPipelineCache);
		if (query.has_value())
		{
			return *query;
		}

		if (query.error() != PipelineCache::PipelineCacheQueryResult::NoneInCache)
			return query;

		std::expected<std::vector<PC_CORE::RhiPipeline::ShaderModuleBinary>, PipelineCache::PipelineCacheQueryResult> QueryModule = QueryModules(_ModuleEntryList, _Delayable);


		if (QueryModule)
		{
			// hash modules entry
			uint32_t Hash = 0;
			for (const auto& ModuleEntry : _ModuleEntryList.GetEntries())
			{
				HashCombine(Hash, ModuleEntry.FeaturesFlags);
				HashCombine(Hash, std::hash<std::string>{}(ModuleEntry.ShaderSourcePath));
			}
			HashCombine(Hash, _Descriptor.Hash());

			std::unique_ptr<RhiGraphicPipeline> pipeline(m_Rhi.CreateRhiGraphicPipeline());
			pipeline
				->FromDescriptor(_Descriptor)
				.SetName(_PipelineName)
				.SetShaderModules(*QueryModule)
				.Build();

			RhiGraphicPipeline* pipelinePtr = pipeline.get();
			_PipilineCacheID->FromUnderlying(static_cast<uint32_t>(Hash));
			m_GraphicPipelineCache.emplace(
				*_PipilineCacheID,
				std::move(pipeline)
			);
			return *pipelinePtr;
		}
		
		return std::unexpected(QueryModule.error());
	}

	PipelineCache::PipelineQueryResult PipelineCache::CreateOrGetComputePipelineCache(PipelineCacheHandleID* _PipilineCacheID,
		const std::string_view& _PipelineName,
		const ModuleEntryList& _ModuleEntryList,
		bool _Delayable)
	{
		if (!_PipilineCacheID)
			return std::unexpected(PipelineCache::PipelineCacheQueryResult::InvalidCachePtr);

		const PipelineCacheHandleID Id = *_PipilineCacheID;
		PipelineQueryResult query = tryToFindInCache(_PipilineCacheID, m_ComputePipelineCache);
		if (query.has_value())
		{
			return *query;
		}

		if (query.error() != PipelineCache::PipelineCacheQueryResult::NoneInCache)
			return query;

		std::expected<std::vector<PC_CORE::RhiPipeline::ShaderModuleBinary>, PipelineCache::PipelineCacheQueryResult> QueryModule = QueryModules(_ModuleEntryList, _Delayable);


		if (QueryModule)
		{
			// hash modules entry
			uint32_t Hash = 0;
			for (const auto& ModuleEntry : _ModuleEntryList.GetEntries())
			{
				HashCombine(Hash, ModuleEntry.FeaturesFlags);
				HashCombine(Hash, std::hash<std::string>{}(ModuleEntry.ShaderSourcePath));
			}

			std::unique_ptr<RhiComputePipeline> pipeline(m_Rhi.CreateRhiComputePipeline());
			pipeline
				->SetName(_PipelineName)
				.SetShaderModules(*QueryModule)
				.Build();

			RhiComputePipeline* pipelinePtr = pipeline.get();
			_PipilineCacheID->FromUnderlying(static_cast<uint32_t>(Hash));
			m_ComputePipelineCache.emplace(
				*_PipilineCacheID,
				std::move(pipeline)
			);
			return *pipelinePtr;
		}

		return std::unexpected(QueryModule.error());
	}

	PipelineCache::PipelineQueryResult PipelineCache::tryToFindInCache(PipelineCacheHandleID* _PipilineCacheID, CacheMap& CacheMap)
	{	
		if (!_PipilineCacheID->IsValid())
		{
			auto it = CacheMap.find(*_PipilineCacheID);
			if (it == CacheMap.end())
			{
				return std::unexpected(PipelineCache::PipelineCacheQueryResult::InvalidPipelineCache);
			}

			return *it->second;
		}
		
		return std::unexpected(PipelineCache::PipelineCacheQueryResult::NoneInCache);
	}

	std::expected<std::vector<PC_CORE::RhiPipeline::ShaderModuleBinary>, PipelineCache::PipelineCacheQueryResult> PipelineCache::QueryModules(const ModuleEntryList& _ModuleEntryList, bool _Delayable)
	{
		const auto& Entries = _ModuleEntryList.GetEntries();

		std::vector<PC_CORE::RhiPipeline::ShaderModuleBinary> Modules;
		Modules.reserve(Entries.size());

		bool isCompiling = false;
		std::vector<std::pair<std::string, PC_CORE::RhiPipeline::ShaderStageTypeBits>> ExpectedPathAndShaderStage;
		ExpectedPathAndShaderStage.reserve(Entries.size());
		for (const auto& entry : Entries)
		{
			PC_CORE::RhiPipeline::ShaderStageTypeBits ShaderStageTypeBits = {};
			if (!RhiPipeline::FormatToShaderStageTypeBits(&ShaderStageTypeBits, GetShaderFormatExt(entry.ShaderSourcePath)))
			{
				return std::unexpected(PipelineCache::PipelineCacheQueryResult::BadModuleStageFormat);
			}
			ExpectedPathAndShaderStage.emplace_back(GetModuleExpectedPath(entry), ShaderStageTypeBits);

			std::expected<bool, PipelineCache::PipelineCacheQueryResult> FileQuery = LookForModuleFile(
				entry.ShaderSourcePath,
				ExpectedPathAndShaderStage.back().first,
				entry.FeaturesFlags, 
				_Delayable);
			if (!FileQuery)
			{
				if (FileQuery.error() == PipelineCache::PipelineCacheQueryResult::ModuleMissing)
					return std::unexpected(FileQuery.error());
				if (FileQuery.error() == PipelineCache::PipelineCacheQueryResult::ModuleCompiling)
					isCompiling = true;
				if (FileQuery.error() == PipelineCache::PipelineCacheQueryResult::ModuleFailedToCompile)
					return std::unexpected(FileQuery.error());
			}
		}

		if (isCompiling)
			return std::unexpected(PipelineCache::PipelineCacheQueryResult::ModuleCompiling);

		assert(ExpectedPathAndShaderStage.size() == Entries.size());
		for (size_t i = 0 ; i < ExpectedPathAndShaderStage.size(); i++)
		{
			std::expected<std::vector<char>, PipelineCache::PipelineCacheQueryResult> codeQuery = GetModuleCode(ExpectedPathAndShaderStage[i].first, false);
			if (!codeQuery && (codeQuery.error() == PipelineCache::PipelineCacheQueryResult::ModuleFailedToCompile))
				return std::unexpected(codeQuery.error());

			if (codeQuery)
			{
				
				Modules.emplace_back(std::move(*codeQuery), ExpectedPathAndShaderStage[i].second);
			}
			else
			{
				PC_LOGERROR("Unknow error when compiling shader module {}", ExpectedPathAndShaderStage[i].first);
				return std::unexpected(PipelineCache::PipelineCacheQueryResult::Error);
			}
		}

		return Modules;
	}

	std::expected<std::vector<char>, PipelineCache::PipelineCacheQueryResult> PipelineCache::GetModuleCode(const std::string& _ExpectedPath, bool _Delayable)
	{
		std::vector<char> Data;
		if (!FileLoader::ReadFile(std::filesystem::path(_ExpectedPath), &Data))
		{
			return std::unexpected(PipelineCache::PipelineCacheQueryResult::ModuleMissing);
		}
		return Data;
	}

	std::expected<bool, PipelineCache::PipelineCacheQueryResult> PipelineCache::LookForModuleFile(
		const std::string& _BaseShaderPath,
		const std::string& _VariantExpectedPath,
		PC_CORE::Rendering::ShaderFeatureFlags _ShaderFeatureFlags,
		bool _Delayable)
	{
		if (!std::filesystem::exists(std::filesystem::path(_VariantExpectedPath)))
			return std::unexpected(PipelineCache::PipelineCacheQueryResult::ModuleMissing);

		return true;
	}

	std::string PipelineCache::GetModuleExpectedPath(const ModuleEntryList::ModuleEntry& Entry)
	{

		const std::string_view ShaderModulePath = m_Rhi.GetShaderModulePath();
#ifdef _DEBUG
		const std::string_view ShaderConfig = "Debug/";
#else 
		const std::string_view ShaderConfig = "Release/";
#endif // DEBUG

		const std::string_view VariantPath = Entry.FeaturesFlags > 0 ? "Variant"sv : ""sv;
		const std::string BaseShaderName = std::filesystem::path(Entry.ShaderSourcePath).filename().generic_string();
		const std::string Features = ShaderFeatureToString(Entry.FeaturesFlags);
		const std::string_view BinarySuffix = m_Rhi.GetShaderModuleBinarySuffix();

		std::string Path;
		Path.reserve(ShaderModulePath.size() +
			ShaderConfig.size() +
			VariantPath.size() +
			BaseShaderName.size() +
			Features.size() +
			BinarySuffix.size());

		Path.append(ShaderModulePath);
		Path.append(ShaderConfig);
		Path.append(VariantPath);
		Path.append(BaseShaderName);
		Path.append(Features);
		Path.append(BinarySuffix);

		return Path;
	}

} // PC_CORE::Rendering
