#ifndef WORLD_ENVIRONEMENT_HEADER
#define WORLD_ENVIRONEMENT_HEADER

#include <memory>
#include <CoreHeader.hpp>
#include <Reflection/Reflector.hpp>
#include <ObjectPtr.hpp>
#include <LowRenderer/RhiBuffer.h>
#include <Resources/Texture2d.hpp>
#include <LowRenderer/RhiDescriptorSet.hpp>

namespace PC_CORE
{
	class App;
}

namespace PC_CORE::WORLD
{
	class Environement
	{
	public:
		enum class Method
		{
			ImageBased
		};

		struct ImageBaseLighting
		{
			bool isDiry = false;

			PC_CORE::WeakObjectPtr<Texture2D> EnvironementTexture;

			std::unique_ptr<RhiTexture> Skybox = nullptr;

			std::unique_ptr<RhiTexture> IrradianceMap = nullptr;

			std::unique_ptr<RhiTexture> PrefilterMap = nullptr;

			std::unique_ptr<RhiDescriptorSet> SkyBoxDescriptorSet;

			std::unique_ptr<RhiDescriptorSet> EnvironemementDescriptorSet;
		};

		using EnvironementLighting = std::variant<ImageBaseLighting>;

		PC_CORE_API void SetMethod(Method _Method);

		PC_CORE_API bool FromEnvironementMap(App& _App, const PC_CORE::WeakObjectPtr<Texture2D>& _Texture);

		PC_CORE_API EnvironementLighting& GetEnvironementLighting();

		PC_CORE_API Environement();
		PC_CORE_API ~Environement() = default;

	private:
		Method m_Method;

		EnvironementLighting m_EnvironementRenderingData;

		static constexpr uint32_t Size = 1024;

		static constexpr uint32_t IrradianceSize = 32;

		static constexpr uint32_t PrefilterMapSize = 512;
	};

	REFLECT(Environement);
	REFLECT(Environement::Method);
}

#endif // WORLD_ENVIRONEMENT_HEADER