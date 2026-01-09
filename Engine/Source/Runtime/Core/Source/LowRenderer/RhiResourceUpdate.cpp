#include "LowRenderer/RhiResourceUpdate.hpp"

#include "LowRenderer/CommandList.hpp"
#include "LowRenderer/Rhi.hpp"


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
		, m_UploadOperation(_Data, _Size)
		, m_NbrOfUpdate(m_RhiBuffer->GetNbrOfResourcePerFrameInFlight())
	{

	}

	ResourceUpdateStatus BufferUpload::Execute(CommandList& _CommandList)
	{
		_CommandList.BeginDebugLabel(std::format("BufferUpload {}", m_RhiBuffer->GetName()).c_str(), DebugColorBufferUpload);
		assert(m_RhiBuffer != nullptr);
		assert(m_UploadOperation);

		if (!m_RhiBuffer->UploadData(&_CommandList, m_UploadOperation.GetData(), m_UploadOperation.m_DataSize))
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
		, m_UploadOperation(_Data, PC_CORE::GetBytePerPixel(_Format) * _ImageWidht * _ImageHeight)
		, m_NbrOfUpdate(m_RhiTexture->GetNbrOfResourcePerFrameInFlight())
		, m_ImageWidht(_ImageWidht)
		, m_ImageHeight(_ImageHeight)
		, m_RhiFormat(_Format)
		, m_AfterUploadState(_AfterUploadState)
	{
	
	}

	[[nodiscard]] ResourceUpdateStatus TextureUpload2D::Execute(CommandList& _CommandList)
	{
		assert(m_RhiTexture != nullptr);
		assert(m_UploadOperation);
		assert(m_ImageWidht != 0u);
		assert(m_ImageHeight != 0u);
		assert(m_RhiFormat != RhiFormat::Undefined);
		assert(m_AfterUploadState != RhiResourceState::Undefined);
		_CommandList.BeginDebugLabel(std::format("TextureUpload2D {}", m_RhiTexture->GetName()).c_str(), DebugColorTextureUpload2D);


		constexpr RhiResourceState RequireState = RhiResourceState::CopyDst;
		ImageStateTransition barrier =
		{
			.Texture = m_RhiTexture,
			.FirstMipLevel = 0,
			.MipLevelsCount = m_RhiTexture->GetLevel(),
			.FirstLayer = 0,
			.LayerCount = m_RhiTexture->GetLayer(),

			.updateState = true
		};

		if (m_RhiTexture->GetResourceState() != RequireState)
		{
			_CommandList.Barrier(m_RhiTexture->GetResourceState(), RequireState, std::span(&barrier, 1));
		}

		if (!m_RhiTexture->UploadData2D(&_CommandList, m_UploadOperation.GetData(), m_RhiFormat, m_ImageWidht, m_ImageHeight))
		{
			_CommandList.Barrier(RequireState, m_AfterUploadState, std::span(&barrier, 1));
			_CommandList.EndDebugLabel();
			return ResourceUpdateStatus::Failed;
		}

		_CommandList.Barrier(RequireState, m_AfterUploadState, std::span(&barrier, 1));
		_CommandList.EndDebugLabel();
		m_NbrOfUpdate--;
		return m_NbrOfUpdate > 0 ? ResourceUpdateStatus::Success : ResourceUpdateStatus::Complete;
	}
	
	GenerateMipMap::GenerateMipMap(RhiTexture& _RhiTexture, Filter _Filter, RhiResourceState _StateAfterOperation)
		: m_RhiTexture(&_RhiTexture)
		, m_Filter(_Filter)
		, m_NbrOfUpdate(m_RhiTexture->GetNbrOfResourcePerFrameInFlight())
		, m_StateAfterOperation(_StateAfterOperation)
	{

	}

	[[nodiscard]] ResourceUpdateStatus GenerateMipMap::Execute(CommandList& _CommandList)
	{
		assert(m_RhiTexture != nullptr);
		assert(m_NbrOfUpdate != 0);
		_CommandList.BeginDebugLabel(std::format("GenerateMipMap {}", m_RhiTexture->GetName()).c_str(), DebugColorTextureGenerateMipMap);


		if (!m_RhiTexture->GenerateMipMap(&_CommandList, m_Filter, m_StateAfterOperation))
		{
			_CommandList.EndDebugLabel();
			return ResourceUpdateStatus::Failed;
		}
		_CommandList.EndDebugLabel();
		m_NbrOfUpdate--;
		return m_NbrOfUpdate > 0 ? ResourceUpdateStatus::Success : ResourceUpdateStatus::Complete;
	}

	UploadOperation::UploadOperation(const void* _Data, size_t _Size)
		: m_DataSize(_Size)
		, m_Data(std::make_unique<uint8_t[]>(_Size))
	{
		assert(m_DataSize != 0);
		std::memcpy(GetData(), _Data, m_DataSize);
	}

	UploadOperation::operator bool() const
	{
		return std::visit(
			Overload{
				[&](const std::unique_ptr<uint8_t[]>& _Data) {
					return _Data.operator bool();
				},
				[&](const std::unique_ptr<uint8_t[], Image::ImageDeleter>& _Data) {
					return _Data.operator bool();
				},

			},
			m_Data
		) && m_DataSize != 0ull;
	}

	uint8_t* UploadOperation::GetData() const
	{
		return std::visit(
			Overload{
				[&](const std::unique_ptr<uint8_t[]>& _Data) {
					return _Data.get();
				},
				[&](const std::unique_ptr<uint8_t[], Image::ImageDeleter>& _Data) {
					return _Data.get();
				},

			},
			m_Data
		);
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

ResourceUpdateBranch& ResourceUpdateBranch::GenerateMipmap(RhiTexture& _RhiTexture, Filter _Filter, RhiResourceState _StateAfterOperation)
{
	m_UpdateBranchs.emplace_back();
	m_UpdateBranchs.back().emplace<ResourceUpdateOperation::GenerateMipMap>(_RhiTexture, _Filter, _StateAfterOperation);
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
			},
			[&](ResourceUpdateOperation::GenerateMipMap& mip) {
				return mip.Execute(_CommandList);
			}

		},
		_ResourceUpdate
	);
}


}
