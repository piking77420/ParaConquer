#include <World/Environement.hpp>
#include <Io/Image.hpp>
#include <App.hpp>

namespace PC_CORE::WORLD
{
	Environement::Environement()
	{
		SetMethod(Environement::Method::ImageBased);
	}

	void Environement::SetMethod(Method _Method)
	{
		auto& env = m_EnvironementRenderingData.emplace<ImageBaseLighting>();
		m_Method = Method::ImageBased;
	}

	bool Environement::FromEnvironementMap(App& _App, const PC_CORE::WeakObjectPtr<Texture2D>& _Texture)
	{
		
		if (m_Method != Method::ImageBased)
		{
			PC_LOGERROR("Environement must be in Method::ImageBased in order to be set from FromEnvironementMap");
			return false;
		}

		auto& env = std::get<ImageBaseLighting>(m_EnvironementRenderingData);

		const ObjectPtr<Texture2D> BaseTexture = _Texture.lock();

		if (!BaseTexture)
		{
			PC_LOGERROR("Texture dont exits");
			return false;
		}

		const std::string_view Path = BaseTexture->GetPath();

		if (Path.empty())
		{
			PC_LOGERROR("Texture dont have source path, {}", BaseTexture->Name);
			return false;
		}

		PC_CORE::Image image(Path.data(), PC_CORE::RhiChannel::Rgba);

		std::unique_ptr<RhiTexture> EnvironementMap(_App.RenderHarwareInteface.CreateTexture());

		if (!EnvironementMap)
		{
			PC_LOGERROR("Failed to create EnvironementMap {}", BaseTexture->Name);
			return false;
		}

		
		EnvironementMap->
			SetRhiFormat(BaseTexture->Get()->GetRhiFormat())
			.SetWidth(Size)
			.SetHeight(Size)
			.SetLayer(6)
			.SetTextureType(RhiTexture::Type::CubeMap)
			.SetMemoryUsage(RhiMemoryUsage::StaticGPU)
			.SetTextureUsage(RhiTexture::TextureUsageFlagBits::Sampled | RhiTexture::TextureUsageFlagBits::RenderTarget | RhiTexture::TextureUsageFlagBits::LoadAndStore)
			.SetName("Environement " + std::string(Path.data()))
			.Build();

		env.EnvironementTexture = _Texture;
		env.Skybox = std::move(EnvironementMap);
		env.isDiry = true;
		
		return true;
	}

	Environement::EnvironementLighting& Environement::GetEnvironementLighting()
	{
		return m_EnvironementRenderingData;
	}
	
}