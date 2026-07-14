#include <EditorPipelineCache.hpp>
#include <Resources/ResourceManager.hpp>
#include <Resources/ShaderSource.hpp>
#include <Resources/ShaderSourceBinary.hpp>

namespace PC_EDITOR_CORE 
{
	EditorPipelineCache::EditorPipelineCache(PC_CORE::Rhi& _Rhi)
		: PC_CORE::Rendering::PipelineCache(_Rhi)
	{

	}


	EditorPipelineCache::~EditorPipelineCache() = default;

	std::expected<bool, PC_CORE::Rendering::PipelineCache::PipelineCacheQueryResult> EditorPipelineCache::LookForModuleFile(
		const std::string& _BaseShaderPath,
		const std::string& _VariantExpectedPath,
		PC_CORE::Rendering::ShaderFeatureFlags _ShaderFeatureFlags,
		bool _Delayable)
	{
		std::filesystem::path VariantPathp = std::filesystem::path(_VariantExpectedPath);
		{
			// Variant aldread exist
			if (std::filesystem::exists(VariantPathp))
				return true;
		}


		PC_CORE::ObjectPtr<ShaderSource> Source = nullptr;
		{
			const std::filesystem::path ShaderSourcePath = std::filesystem::path(_BaseShaderPath);
			const std::string ShaderSourceName = ShaderSourcePath.filename().generic_string();
			Source = !PC_CORE::ResourceManager::Exist(ShaderSourceName)
				? PC_CORE::ResourceManager::Create<ShaderSource>(ShaderSourceName, ShaderSourcePath)
				: Source = PC_CORE::ResourceManager::Get<ShaderSource>(ShaderSourceName);

			assert(Source);
			if (!Source)
				return std::unexpected(PC_CORE::Rendering::PipelineCache::PipelineCacheQueryResult::Error);
		}

		ShaderSource::GenerateVariantResult result = Source->GenerateVariant(_VariantExpectedPath, _ShaderFeatureFlags);
		assert(result == ShaderSource::GenerateVariantResult::Sucess);

		switch (result)
		{
		case PC_EDITOR_CORE::ShaderSource::GenerateVariantResult::Failed:
			return std::unexpected(PC_CORE::Rendering::PipelineCache::PipelineCacheQueryResult::ModuleFailedToCompile);
		case PC_EDITOR_CORE::ShaderSource::GenerateVariantResult::Compiling:
			return std::unexpected(PC_CORE::Rendering::PipelineCache::PipelineCacheQueryResult::ModuleCompiling);
		case PC_EDITOR_CORE::ShaderSource::GenerateVariantResult::Sucess:
			return true;
		default:
			return false;
		}
	}

} // PC_EDITOR_CORE

