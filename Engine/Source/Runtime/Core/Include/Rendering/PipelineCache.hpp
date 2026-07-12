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

namespace PC_CORE
{
	class Rhi;
} // PC_CORE

namespace PC_CORE::Rendering
{
	class PipelineCacheHandleID
	{
	private:
		static constexpr size_t MAX = std::numeric_limits<size_t>::max();
	public:

		explicit PipelineCacheHandleID() = default;
		~PipelineCacheHandleID() = default;
		DEFAULT_COPY_MOVE_OPERATIONS(PipelineCacheHandleID);

		bool IsValid() const noexcept
		{
			return m_ID == MAX;
		}

		auto operator<=>(const PipelineCacheHandleID&) const = default;

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

	struct PipelineCacheHandleIDHash
	{
		std::size_t operator()(const PipelineCacheHandleID& _Value) const noexcept
		{
			return std::hash<size_t>{}(_Value.ToUnderlying());
		}
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


		PipelineQueryResult CreateOrGetGraphicPipelineCache(PipelineCacheHandleID* _PipilineCacheID,
			const std::string_view& _PipelineName, 
			const ModuleEntryList& _ModuleEntryList, 
			const RhiGraphicPipeline::Descriptor& _Descriptor, 
			bool _Delayable = false);

		PipelineQueryResult CreateOrGetComputePipelineCache(PipelineCacheHandleID* _PipilineCacheID,
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
		using CacheMap = std::unordered_map<PipelineCacheHandleID, std::unique_ptr<RhiPipeline>, PipelineCacheHandleIDHash>;

		Rhi& m_Rhi;

		CacheMap m_GraphicPipelineCache;

		CacheMap m_ComputePipelineCache;

		PipelineQueryResult tryToFindInCache(PipelineCacheHandleID* _PipilineCacheID, CacheMap& CacheMap);

		std::expected<std::vector<PC_CORE::RhiPipeline::ShaderModuleBinary>, PipelineCache::PipelineCacheQueryResult> QueryModules(const ModuleEntryList& _ModuleEntryList, bool _Delayable);

		std::expected<std::vector<char>, PipelineCache::PipelineCacheQueryResult> GetModuleCode(const std::string& _ExpectedPath, bool _Delayable);

		std::string GetModuleExpectedPath(const ModuleEntryList::ModuleEntry& Entry);
	};

} // namespace PC_CORE::Rendering

#endif // PC_CORE_RENDERING_PIPELINE_CACHE

