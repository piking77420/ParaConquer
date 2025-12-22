#pragma once

#include "CoreHeader.hpp"

namespace PC_CORE
{
	class CommandList;
	class RhiTexture;
}

namespace PC_CORE::Rhi
{
	class PC_CORE_API ResourceUpdate
	{
	public:
		ResourceUpdate();
		virtual ~ResourceUpdate();

		virtual void Record(CommandList& _CommandList);

	private:

	};


	class Texture2DUploadUpdate final : public ResourceUpdate
	{
	public:

		explicit Texture2DUploadUpdate(RhiTexture& _RhiTexture, uint8_t* _Data, size_t _DataSize)
			: m_Texture(_RhiTexture)
			, m_DataSize(_DataSize)
		{
			m_Data = new uint8_t[m_DataSize];
		}

		~Texture2DUploadUpdate() override
		{
			if (m_DataSize != 0)
				delete[] m_Data;
		}

	private:
		RhiTexture& m_Texture;
		uint8_t* m_Data{ nullptr };
		size_t m_DataSize{ 0 };

	};

	class GenerateMipmapsUpdate final : public ResourceUpdate
	{
	public:
		explicit GenerateMipmapsUpdate(RhiTexture& texture)
			: m_Texture(texture) {
		}

		
		~GenerateMipmapsUpdate() override
		{

		}

	private:
		RhiTexture& m_Texture;
	};

}