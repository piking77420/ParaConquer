#pragma once

#include <vector>
#include <variant>
#include <deque>

#include "Resources/FileLoader.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiTexture.hpp"


namespace PC_CORE
{
	class CommandList;
	class RhiTexture;
	class RhiBuffer;

	class Image;
	struct Image::ImageDeleter;
}

namespace PC_CORE::RHI
{
	namespace ResourceUpdateOperation {
		enum ResourceUpdateStatus
		{
			Failed,
			Success,
			Complete
		};

		template <typename T>
		concept UploadBufferType = requires(T t)
		{
			{ t.get() } -> std::same_as<uint8_t*>;
		};

		struct PC_CORE_API UploadOperation
		{
			using UploadData = std::variant<std::unique_ptr<uint8_t[]>, std::unique_ptr<uint8_t[], Image::ImageDeleter>>;

			DEFAULT_COPY_MOVE_OPERATIONS(UploadOperation)

			UploadOperation() = default;

			~UploadOperation() = default;


			explicit UploadOperation(const void* _Data, size_t _Size);

			template <UploadBufferType T>
			explicit UploadOperation(T&& _Data, size_t _Size)
				: m_DataSize(_Size)
				, m_Data(std::forward<T>(_Data))
			{
				assert(m_DataSize != 0);
			}

			operator bool() const;

			uint8_t* GetData() const;

			size_t m_DataSize{ 0u };

			UploadData m_Data;
		};

		class PC_CORE_API BufferUpload
		{
		public:
			explicit BufferUpload(RhiBuffer& _RhiBuffer, const void* _Data, size_t _Size);

			template <UploadBufferType T>
			explicit BufferUpload(RhiBuffer& _RhiBuffer, T&& _Data, size_t _Size)
				: m_RhiBuffer(&_RhiBuffer)
				, m_UploadOperation(std::forward<T>(_Data), _Size)
				, m_FrameUpdateCount(m_RhiBuffer->GetNbrOfBackendObject())
			{

			}


			~BufferUpload() = default;

			DEFAULT_COPY_MOVE_OPERATIONS(BufferUpload)

			[[nodiscard]] ResourceUpdateStatus Execute(CommandList& _CommandList);

		private:
			RhiBuffer* m_RhiBuffer{ nullptr };

			UploadOperation m_UploadOperation;

			size_t m_FrameUpdateCount{ 0u };
		};

		class PC_CORE_API TextureUpload2D
		{
		public:
			explicit TextureUpload2D(RhiTexture& _RhiTexture, const void* _Data, size_t _DataSize, RhiResourceState _AfterUploadState);

			template <UploadBufferType T>
			explicit TextureUpload2D(RhiTexture& _RhiTexture, T&& _Data, size_t _DataSize, RhiResourceState _AfterUploadState)
				: m_RhiTexture(&_RhiTexture)
				, m_UploadOperation(std::forward<T>(_Data), _DataSize)
				, m_FrameUpdateCount(m_RhiTexture->GetNbrOfBackendObject())
				, m_AfterUploadState(_AfterUploadState)
			{

			}

			~TextureUpload2D() = default;

			DEFAULT_COPY_MOVE_OPERATIONS(TextureUpload2D)

			[[nodiscard]] ResourceUpdateStatus Execute(CommandList& _CommandList);

		private:
			RhiTexture* m_RhiTexture{ nullptr };

			UploadOperation m_UploadOperation;

			size_t m_FrameUpdateCount{ 0u };

			RhiResourceState m_AfterUploadState{ RhiResourceState::Undefined };

		};

		
		class PC_CORE_API GenerateMipMap
		{
		public:


			explicit GenerateMipMap(RhiTexture& _RhiTexture, Filter _Filter, RhiResourceState _StateAfterOperation);

			~GenerateMipMap() = default;

			DEFAULT_COPY_MOVE_OPERATIONS(GenerateMipMap)

			[[nodiscard]] ResourceUpdateStatus Execute(CommandList& _CommandList);

		private:
			RhiTexture* m_RhiTexture{ nullptr };

			size_t m_NbrOfUpdate{ 0u };

			Filter m_Filter = Filter::Nearest;

			RhiResourceState m_StateAfterOperation;
		};

	}

	class PC_CORE_API ResourceUpdateBranch
	{
	public:
		using ResourceUpdate = std::variant<std::monostate, ResourceUpdateOperation::BufferUpload, ResourceUpdateOperation::TextureUpload2D, 
											ResourceUpdateOperation::GenerateMipMap>;

		DEFAULT_CONSTRUCTOR_DESTRUCTOR(ResourceUpdateBranch)

		ResourceUpdateBranch(const ResourceUpdateBranch&) = delete;
		ResourceUpdateBranch& operator=(const ResourceUpdateBranch&) = delete;

		ResourceUpdateBranch(ResourceUpdateBranch&&) = default;
		ResourceUpdateBranch& operator=(ResourceUpdateBranch&&) = default;

		ResourceUpdateBranch& BufferUpload(RhiBuffer& _RhiBuffer, const void* _Data, size_t _Size);

		template <ResourceUpdateOperation::UploadBufferType T>
		ResourceUpdateBranch& BufferUpload(RhiBuffer& _RhiBuffer, T&& _Data, size_t _Size)
		{
			m_UpdateBranchs.emplace_back();
			m_UpdateBranchs.back().emplace<ResourceUpdateOperation::BufferUpload>(_RhiBuffer, std::forward<T>(_Data), _Size);
			return *this;
		}


		ResourceUpdateBranch& TextureUpload2D(RhiTexture& _RhiTexture, const void* _Data, size_t _DataSize, RhiResourceState _AfterUploadState);

		template <ResourceUpdateOperation::UploadBufferType T>
		ResourceUpdateBranch& TextureUpload2D(RhiTexture& _RhiTexture, T&& _Data,size_t _DataSize, RhiResourceState _AfterUploadState)
		{
			m_UpdateBranchs.push_back(std::make_unique<ResourceUpdate>(ResourceUpdateOperation::TextureUpload2D(_RhiTexture, std::forward<T>(_Data), _DataSize, _AfterUploadState)));
			return *this;
		}

		ResourceUpdateBranch& GenerateMipmap(RhiTexture& _RhiTexture, Filter _Filter, RhiResourceState _StateAfterOperation);

		bool IsEmpty() const
		{
			return m_UpdateBranchs.empty();
		}

		bool Proceed(CommandList& _CommandList);

	private:
		ResourceUpdateOperation::ResourceUpdateStatus Execute(CommandList& _CommandList, ResourceUpdate& _ResourceUpdate);

		std::vector<std::unique_ptr<ResourceUpdate>> m_UpdateBranchs;
	};


}