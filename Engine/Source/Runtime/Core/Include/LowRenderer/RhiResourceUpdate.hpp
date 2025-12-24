#pragma once

#include <bitset>

#include "RhiTypedef.h"

namespace PC_CORE
{
	class CommandList;
	class RhiTexture;
	class RhiBuffer;
}

namespace PC_CORE::RHI
{
	class PC_CORE_API ResourceUpdate
	{
	public:
		enum ResourceUpdateStatus
		{
			Failed,
			Success,
			Complet
		};

		ResourceUpdate();
		virtual ~ResourceUpdate();

		
		[[nodiscard]] virtual bool Record(CommandList& _CommandList) = 0;

	private:
		std::bitset<MaxFramesInFlight> m_FrameUpdateBitSet{};
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

	class BufferUpload final : public ResourceUpdate
	{
	public:
		explicit BufferUpload(RhiBuffer& _RhiBuffer)
			: m_RhiBuffer(_RhiBuffer) {
		}


		~BufferUpload() override
		{

		}

	private:
		RhiBuffer& m_RhiBuffer;
	};


}