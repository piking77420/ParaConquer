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
			return m_ID == MAX;
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
		~PipelineCache() = default;
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

		using PipelineQueryResult = std::expected<std::reference_wrapper<RhiPipeline>, PipelineCache::PipelineCacheQueryResult>;

		
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
				return m_entries.emplace_back();
			}

			void Reserve(size_t _Count)
			{
				m_entries.reserve(_Count);
			}

			const std::vector<ModuleEntry>& GetEntries() const
			{
				return m_entries;
			}

		private:
			std::vector<ModuleEntry> m_entries;
		};


		PipelineQueryResult CreateOrGetGraphicPipelineCache(PipelineCacheID* _PipilineCacheID,
			const std::string_view& _PipelineName, 
			const ModuleEntryList& _ModuleEntryList, 
			const RhiGraphicPipeline::Descriptor& _Descriptor, 
			bool _Delayable = false);

		PipelineQueryResult GetGraphicPipelineCache(const PipelineCacheID& _PipilineCacheID);

		PipelineQueryResult CreateOrGetComputePipelineCache(PipelineCacheID* _PipilineCacheID,
			const std::string_view& _PipelineName, 
			const ModuleEntryList& _ModuleEntryList, 
			bool _Delayable = false);	

		PipelineQueryResult GetComputePipelineCache(const PipelineCacheID& _PipilineCacheID);

	protected:
		virtual std::expected<bool, PipelineCache::PipelineCacheQueryResult> LookForModuleFile(
			const std::string& _BaseShaderPath, 
			const std::string& _VariantExpectedPath, 
			PC_CORE::Rendering::ShaderFeatureFlags _ShaderFeatureFlags, 
			bool _Delayable);

	private:
		using Cache = FreeList<std::unique_ptr<RhiPipeline>>;

		Rhi& m_Rhi;

		Cache m_GraphicPipelineCache;

		Cache m_ComputePipelineCache;

		PipelineQueryResult tryToFindInCache(const PipelineCacheID& _PipilineCacheID, Cache& CacheMap);

		std::expected<std::vector<PC_CORE::RhiPipeline::ShaderModuleBinary>, PipelineCache::PipelineCacheQueryResult> QueryModules(const ModuleEntryList& _ModuleEntryList, bool _Delayable);

		std::expected<std::vector<char>, PipelineCache::PipelineCacheQueryResult> GetModuleCode(const std::string& _ExpectedPath, bool _Delayable);

		std::string GetModuleExpectedPath(const ModuleEntryList::ModuleEntry& Entry);
	};

} // namespace PC_CORE::Rendering

#endif // PC_CORE_RENDERING_PIPELINE_CACHE

