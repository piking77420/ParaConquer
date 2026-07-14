#ifndef PC_CORE_RENDERING_PIPELINE_CACHE
#define PC_CORE_RENDERING_PIPELINE_CACHE

#include <expected>
#include <memory>
#include <unordered_map>
#include <functional>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <flat_map>

#include <CoreHeader.hpp>
#include <ObjectPtr.hpp>
#include <LowRenderer/RhiGraphicPipeline.hpp>
#include <LowRenderer/RhiComputePipeline.hpp>
#include <Rendering/RenderingTypedef.h>
#include <DataStructure/FreeList.hpp>

namespace PC_CORE
{
	class Rhi;
} // PC_CORE

namespace PC_CORE::Rendering
{
	class PipelineCacheID
	{
	private:
		static constexpr size_t MAX = std::numeric_limits<size_t>::max();
	public:

		explicit PipelineCacheID() = default;
		~PipelineCacheID() = default;
		DEFAULT_COPY_MOVE_OPERATIONS(PipelineCacheID);

		bool IsValid() const noexcept
		{
			return m_ID != MAX;
		}

		auto operator<=>(const PipelineCacheID&) const = default;

		[[nodiscard]] operator bool() const noexcept
		{
			return !IsValid();
		}

		void FromUnderlying(size_t _Id)
		{
			m_ID = _Id;
		}

		size_t ToUnderlying() const
		{
			return m_ID;
		}

	private:
		size_t m_ID = MAX;
	};

	class PC_CORE_API PipelineCache
	{
	public: 
		PipelineCache(Rhi& Rhi);
		virtual ~PipelineCache() = default;
		PipelineCache(const PipelineCache&) = delete;
		PipelineCache& operator=(const PipelineCache&) = delete;

		enum class PipelineCacheQueryResult
		{
			None,
			NoneInCache,
			InvalidCachePtr,
			InvalidPipelineCache,
			ModuleCompiling,
			ModuleFailedToCompile,
			ModuleMissing,
			BadModuleStageFormat,
			Error,
		};

		using GraphicPipelineQueryResult = std::expected<std::reference_wrapper<RhiPipeline>, PipelineCache::PipelineCacheQueryResult>;
		using ComputePipelineQueryResult = std::expected<std::reference_wrapper<RhiComputePipeline>, PipelineCache::PipelineCacheQueryResult>;

			// TODO make header	
		class PC_CORE_API ModuleEntryList
		{
		public:
			struct PC_CORE_API ModuleEntry
			{
				std::string ShaderSourcePath;
				PC_CORE::Rendering::ShaderFeatureFlags FeaturesFlags;

				ModuleEntry& SetSourcePath(const std::string_view& _View)
				{
					ShaderSourcePath = _View;
					return *this;
				}

				ModuleEntry& SetShaderFeaturesFlag(PC_CORE::Rendering::ShaderFeatureFlags _ShaderFeatureFlags)
				{
					FeaturesFlags = _ShaderFeatureFlags;
					return *this;
				}
			};

			ModuleEntry& Next()
			{
				return m_Entries.emplace_back();
			}

			void Reserve(size_t _Count)
			{
				m_Entries.reserve(_Count);
			}

			const std::vector<ModuleEntry>& GetEntries() const
			{
				return m_Entries;
			}

			void Clear()
			{
				m_Entries.clear();
			}

		private:
			std::vector<ModuleEntry> m_Entries;
		};


		GraphicPipelineQueryResult CreateOrGetGraphicPipelineCache(PipelineCacheID* _PipilineCacheID,
			const std::string_view& _PipelineName, 
			const ModuleEntryList& _ModuleEntryList, 
			const RhiGraphicPipeline::Descriptor& _Descriptor, 
			bool _Delayable = false);

		ComputePipelineQueryResult CreateOrGetComputePipelineCache(PipelineCacheID* _PipilineCacheID,
			const std::string_view& _PipelineName, 
			const ModuleEntryList& _ModuleEntryList, 
			bool _Delayable = false);	

	protected:
		virtual std::expected<bool, PipelineCache::PipelineCacheQueryResult> LookForModuleFile(
			const std::string& _BaseShaderPath, 
			const std::string& _VariantExpectedPath, 
			PC_CORE::Rendering::ShaderFeatureFlags _ShaderFeatureFlags, 
			bool _Delayable);

	private:
		Rhi& m_Rhi;

		using GraphicCache = std::flat_map<PipelineCacheID, std::unique_ptr<RhiGraphicPipeline>>;
		using ComputeCache = std::flat_map<PipelineCacheID, std::unique_ptr<RhiComputePipeline>>;

		GraphicCache m_GraphicPipelineCache;

		ComputeCache m_ComputePipelineCache;

		template <typename QueryResult, typename CacheMapType>
		QueryResult tryToFindInCache(const PipelineCacheID& _PipilineCacheID, const CacheMapType& _Cache, bool _CheckIfValid)
		{
			if (_CheckIfValid)
			{
				if (_PipilineCacheID.IsValid())
				{
					auto it = _Cache.find(_PipilineCacheID);
					if (it == _Cache.end())
					{
						return std::unexpected(PipelineCache::PipelineCacheQueryResult::InvalidPipelineCache);
					}

					return *it->second;
				}
			}
			else
			{
				auto it = _Cache.find(_PipilineCacheID);
				if (it == _Cache.end())
				{
					return std::unexpected(PipelineCache::PipelineCacheQueryResult::InvalidPipelineCache);
				}

				return *it->second;
			}
			
			return std::unexpected(PipelineCache::PipelineCacheQueryResult::NoneInCache);
		}

		std::expected<std::vector<PC_CORE::RhiPipeline::ShaderModuleBinary>, PipelineCache::PipelineCacheQueryResult> QueryModules(const ModuleEntryList& _ModuleEntryList, bool _Delayable);

		std::expected<std::vector<char>, PipelineCache::PipelineCacheQueryResult> GetModuleCode(const std::string& _ExpectedPath, bool _Delayable);

		std::string GetModuleExpectedPath(const ModuleEntryList::ModuleEntry& Entry, const std::string_view& ShaderFormat);
	};

} // namespace PC_CORE::Rendering

#endif // PC_CORE_RENDERING_PIPELINE_CACHE

