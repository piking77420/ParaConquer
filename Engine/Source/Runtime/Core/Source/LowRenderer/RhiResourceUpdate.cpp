#include "LowRenderer/RhiResourceUpdate.hpp"

#include "LowRenderer/CommandList.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"

template<class... Ts>
struct Overload : Ts... { using Ts::operator()...; };

constexpr float DebugAlphaUploadResource = 0.4f;

constexpr std::array<float, 4> DebugColorBufferUpload =
{
	0.17f,
	0.13f,
	0.564,
	DebugAlphaUploadResource,
};

constexpr std::array<float, 4> DebugColorTextureUpload2D =
{
	0.564,
	0.13f,
	0.17f,
	DebugAlphaUploadResource,
};

constexpr std::array<float, 4> DebugColorTextureGenerateMipMap =
{
	0.13f,
	0.564,
	0.17f,
	DebugAlphaUploadResource,
};


namespace PC_CORE::RHI	
{

namespace ResourceUpdateOperation
{

	BufferUpload::BufferUpload(RhiBuffer& _RhiBuffer, const void* _Data, size_t _Size)
		: m_RhiBuffer(&_RhiBuffer)
		, m_NbrOfUpdate(m_RhiBuffer->GetNbrOfResourcePerFrameInFlight())
		, m_DataSize(_Size)
	{
		assert(m_DataSize != 0);
		m_Data = std::make_unique<uint8_t[]>(m_DataSize);
		std::memcpy(m_Data.get(), _Data, m_DataSize);
	}

	ResourceUpdateStatus BufferUpload::Execute(CommandList& _CommandList)
	{
		_CommandList.BeginDebugLabel(std::format("BufferUpload {}", m_RhiBuffer->GetName()).c_str(), DebugColorBufferUpload);
		assert(m_RhiBuffer != nullptr);
		assert(m_Data);
		assert(m_DataSize != 0ull);

		if (!m_RhiBuffer->UploadData(&_CommandList, m_Data.get(), m_DataSize))
		{
			_CommandList.EndDebugLabel();
			return ResourceUpdateStatus::Failed;
		}
		_CommandList.EndDebugLabel();
		m_NbrOfUpdate--;
		return m_NbrOfUpdate > 0 ? ResourceUpdateStatus::Success : ResourceUpdateStatus::Complete;
	}

	TextureUpload2D::TextureUpload2D(RhiTexture& _RhiTexture, const void* _Data, RhiFormat _Format,
		size_t _ImageWidht, size_t _ImageHeight, RhiResourceState _AfterUploadState)
		: m_RhiTexture(&_RhiTexture)
		, m_NbrOfUpdate(m_RhiTexture->GetNbrOfResourcePerFrameInFlight())
		, m_ImageWidht(_ImageWidht)
		, m_ImageHeight(_ImageHeight)
		, m_RhiFormat(_Format)
		, m_AfterUploadState(_AfterUploadState)
	{
	
		const size_t size = PC_CORE::GetBytePerPixel(m_RhiFormat) * m_ImageWidht * m_ImageHeight;
		m_Data = std::make_unique<uint8_t[]>(size);
		std::memcpy(m_Data.get(), _Data, size);
	}

	[[nodiscard]] ResourceUpdateStatus TextureUpload2D::Execute(CommandList& _CommandList)
	{
		assert(m_RhiTexture != nullptr);
		assert(m_ImageWidht != 0u);
		assert(m_ImageHeight != 0u);
		assert(m_RhiFormat != RhiFormat::Undefined);
		assert(m_AfterUploadState != RhiResourceState::Undefined);


		_CommandList.BeginDebugLabel(std::format("TextureUpload2D {}", m_RhiTexture->GetName()).c_str(), DebugColorTextureUpload2D);

		ImageStateTransition transitionState =
		{
			.Texture = m_RhiTexture,
			.FirstMipLevel = 0,
			.MipLevelsCount = m_RhiTexture->GetLevel(),
			.FirstLayer = 0,
			.LayerCount = m_RhiTexture->GetLayer(),

			.updateState = false
		};

		_CommandList.Barrier(m_RhiTexture->GetResourceState(), RhiResourceState::CopyDst, std::span(&transitionState, 1));
		
		if (!m_RhiTexture->UploadData2D(&_CommandList, m_Data.get(), m_RhiFormat, m_ImageWidht, m_ImageHeight))
		{
			_CommandList.Barrier(RhiResourceState::CopyDst, RhiResourceState::FragmentShaderResource, std::span(&transitionState, 1));
			_CommandList.EndDebugLabel();
			return ResourceUpdateStatus::Failed;
		}
		_CommandList.Barrier(RhiResourceState::CopyDst, m_AfterUploadState, std::span(&transitionState, 1));
		_CommandList.EndDebugLabel();
		m_NbrOfUpdate--;
		return m_NbrOfUpdate > 0 ? ResourceUpdateStatus::Success : ResourceUpdateStatus::Complete;
	}
	
	GenerateMipMap::GenerateMipMap(RhiTexture& _RhiTexture)
		: m_RhiTexture(&_RhiTexture)
	{

	}

	[[nodiscard]] ResourceUpdateStatus GenerateMipMap::Execute(CommandList& _CommandList)
	{
		assert(m_RhiTexture != nullptr);
		assert(m_NbrOfUpdate != 0);

		_CommandList.BeginDebugLabel(std::format("GenerateMipMap {}", m_RhiTexture->GetName()).c_str(), DebugColorTextureGenerateMipMap);

		if (!m_RhiTexture->GenerateMipMap(&_CommandList))
		{
			_CommandList.EndDebugLabel();
			return ResourceUpdateStatus::Failed;
		}
		_CommandList.EndDebugLabel();
		m_NbrOfUpdate--;
		return m_NbrOfUpdate > 0 ? ResourceUpdateStatus::Success : ResourceUpdateStatus::Complete;
	}

}

ResourceUpdateBranch& ResourceUpdateBranch::BufferUpload(RhiBuffer& _RhiBuffer, const void* _Data, size_t _Size)
{
	m_UpdateBranchs.emplace_back();
	m_UpdateBranchs.back().emplace<ResourceUpdateOperation::BufferUpload>(_RhiBuffer, _Data, _Size);
	return *this;
}

ResourceUpdateBranch& ResourceUpdateBranch::TextureUpload2D(RhiTexture& _RhiTexture,
	const void* _Data,
	RhiFormat _Format, size_t _ImageWidht, size_t _ImageHeight, 
	RhiResourceState _AfterUploadState)
{
	m_UpdateBranchs.emplace_back();
	m_UpdateBranchs.back().emplace<ResourceUpdateOperation::TextureUpload2D>(_RhiTexture, _Data, _Format, _ImageWidht, _ImageHeight, _AfterUploadState);
	return *this;
}

bool ResourceUpdateBranch::Proceed(CommandList& _CommandList)
{
	bool NeedToSendToGpu = false;

	for (auto it = m_UpdateBranchs.begin(); it != m_UpdateBranchs.end(); )
	{
		ResourceUpdateOperation::ResourceUpdateStatus Status = Execute(_CommandList, *it);
		switch (Status)
		{
		case PC_CORE::RHI::ResourceUpdateOperation::Success:
			NeedToSendToGpu = true;
			it++;
			break;
		case PC_CORE::RHI::ResourceUpdateOperation::Failed:
			it = m_UpdateBranchs.erase(it);
			break;
		case PC_CORE::RHI::ResourceUpdateOperation::Complete:
			it = m_UpdateBranchs.erase(it);
			NeedToSendToGpu = true;
			break;
		default:
			it++;
			break;
		}
	}

	return NeedToSendToGpu;
}

ResourceUpdateOperation::ResourceUpdateStatus ResourceUpdateBranch::Execute(CommandList& _CommandList, ResourceUpdate& _ResourceUpdate)
{
	return std::visit(
		Overload{
			[&](std::monostate&) {
				return ResourceUpdateOperation::Failed;
			},
			[&](ResourceUpdateOperation::BufferUpload& upload) {
				return upload.Execute(_CommandList);
			},
			[&](ResourceUpdateOperation::TextureUpload2D& upload) {
				return upload.Execute(_CommandList);
			}

		},
		_ResourceUpdate
	);
}


}
