#pragma once

#include <Array>
#include <String>
#include <Memory>
#include <Functional>
#include <span>

#include "CoreHeader.hpp"
#include "Math/ToolboxTypedef.hpp"

#include "RhiFrameBuffer.hpp"
#include "RhiRenderPass.hpp"
#include "Rendering/Buffer/VertexBuffer.hpp"

BEGIN_PCCORE
    class RhiFrameBuffer;
    class RhiFence;
    class RhiShaderProgram;
    class RhiDescriptorSet;


    enum ClearValueFlagBits : uint32_t
    {
        ClearValueNone = 0,
        ClearValueColor = 1 << 0,
        ClearValueDepth = 1 << 1,
        ClearValueStencil = 1 << 2,
        ClearValueCount = 1 << 3
    };

    using ClearValueFlag = uint32_t;



    struct BeginRenderPassInfo
    {
        RhiRenderPass* RenderPass;
        RhiFrameBuffer* FrameBuffer;
        Tbx::Vector2i RenderOffSet;
        Tbx::Vector2ui Extent;

        ClearValueFlag ClearValueFlag;
        std::array<float, 4>* ClearColor;
        size_t ClearValueCount;
        float ClearDepth = 0.f;
        float ClearStencil = 0.f;
    };

    struct ViewportInfo
    {
        Tbx::Vector2f Transform{ 0,0 };
        Tbx::Vector2f Size{ 0,0 };
        float MinDepth = 0.0f;
        float MaxDepth = 1.0;

        Tbx::Vector2i ScissorsOff{ 0,0 };
        Tbx::Vector2ui ScissorsExtent{ 0,0 };

        explicit ViewportInfo(RhiTexture& texture)
        {
            const uint32_t widht = texture.GetWidth();
            const uint32_t height = texture.GetHeight();

            Size = Tbx::Vector2f(static_cast<float>(widht), static_cast<float>(height));
            ScissorsExtent = Tbx::Vector2ui(widht, height);
        }

        explicit ViewportInfo(Tbx::Vector2ui size)
        {
            Size = Tbx::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y));
            ScissorsExtent = size;
        }

        ViewportInfo() = default;

        ~ViewportInfo() = default;
        
    };

    struct ImageStateTransition
    {
        RhiTexture* Texture = nullptr;

        uint32_t FirstMipLevel = 0;
        uint32_t MipLevelsCount = 0;
        uint32_t FirstLayer = 0;
        uint32_t LayerCount = 0;

        bool updateState = false;
    };

    struct BufferStateTransition
    {
        RhiBuffer* Buffer = nullptr;

        uint32_t Offset = 0;
        uint32_t Size = 0;

        bool updateState = false;
    };

   
    class CommandList : public RhiObjectT<CommandList>
    {
    public:
        enum struct PoolFamily
        {
            Graphics,
            Compute,
            Count
        };

        enum struct BufferType
        {
            Primary,
            Secondary
        };

        struct DrawBuffers
        {
            using VertexBufferAndOffSet = std::pair<const RhiBuffer*, size_t>;
            using VertexBufferArray = std::vector<VertexBufferAndOffSet>; // TODO use fixed one

            VertexBufferArray VertexBufferBinded{};
            const RhiBuffer* IndexBuffer = nullptr;
            size_t IndexBufferOffset {0ull};
            RhiBuffer::IndexFormat IndexFormat{ RhiBuffer::IndexFormat::Uint8 };

            DrawBuffers& PushVertexBuffer(
                const RhiBuffer& _RhiBuffer,
                size_t _Offset)
            {
                VertexBufferBinded.emplace_back(std::pair<const RhiBuffer*, size_t>(&_RhiBuffer, _Offset));
                return *this;
            }

            DrawBuffers& SetIndexBuffer(
                const RhiBuffer& _RhiBuffer,
                size_t _Offset,
                RhiBuffer::IndexFormat _IndexFormat)
            {
                IndexBuffer = &_RhiBuffer;
                IndexBufferOffset = _Offset;
                IndexFormat = _IndexFormat;
                return *this;
            }

          
            bool operator==(const DrawBuffers& _rhs) const noexcept
            {
                return IndexBuffer == _rhs.IndexBuffer
                    && IndexFormat == _rhs.IndexFormat
                    && IndexBufferOffset == _rhs.IndexBufferOffset
                    && VertexBufferBinded == _rhs.VertexBufferBinded;
            }

        };


        DEFAULT_COPY_MOVE_OPERATIONS(CommandList)

        PC_CORE_API explicit CommandList(Rhi& _Rhi);

        PC_CORE_API virtual ~CommandList() = default;

        PC_CORE_API virtual void Reset() = 0;

        PC_CORE_API virtual void MergeCommands(CommandList* _secondaries, size_t _count) = 0;

        PC_CORE_API virtual void BeginRecordCommands();

        PC_CORE_API virtual void EndRecordCommands();

        PC_CORE_API virtual void BeginRenderPass(const BeginRenderPassInfo& _beginRenderPassInfo);

        PC_CORE_API virtual void EndRenderPass() = 0;

        PC_CORE_API virtual void NextSubPass() = 0;

        PC_CORE_API virtual void BindProgram(const RhiShaderProgram& _RhiShaderProgram) = 0;


        PC_CORE_API virtual void BindDescriptorSet(const RhiDescriptorSet* _shaderProgramDescriptorSets,
                                                   size_t _Set,
                                                   size_t _DynamicOffset = std::numeric_limits<size_t>::max()) = 0;

        PC_CORE_API virtual void BindDescriptorSets(
            const std::span<const PC_CORE::RhiDescriptorSet*>& descriptorSets,
            size_t firstSet,
            const std::span<const size_t>& dynamicOffset = {}) = 0;

        PC_CORE_API virtual void PushConstant(const std::string& _pushConstantKey,
                                              const void* _data, size_t _size) = 0;

        PC_CORE_API virtual void SetViewPort(const ViewportInfo& _viewPort) = 0;

        PC_CORE_API virtual void SetPrimitiveTopology(PC_CORE::RhiShaderProgram::PrimitiveTopology _primitiveTopology) = 0;

        PC_CORE_API virtual void SetBlendEquation(uint32_t _firstAttachement, uint32_t _attachementCount) = 0;

        PC_CORE_API virtual void SetLineWidth(float _widht) = 0;

        PC_CORE_API virtual void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex,
                                      uint32_t _firstInstance) = 0;

        PC_CORE_API virtual void DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex,
                                             int32_t _vertexOffset, size_t _firstInstance) = 0;

        PC_CORE_API virtual void Dispatch(uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ) = 0;

        PC_CORE_API virtual void BindDrawBuffers(const DrawBuffers& _DrawBuffers) = 0;

        PC_CORE_API virtual void CopyBuffer(const RhiBuffer& _src, const RhiBuffer& _dst, size_t _srcOffSet,
                                            size_t _dstoffset, size_t _sizeInBytes) = 0;

        PC_CORE_API virtual void Barrier(RhiResourceState _OldState, RhiResourceState _NewState,
                                          const std::span<const ImageStateTransition>& _ImageStateTransition = {},
                                          const std::span<const BufferStateTransition>& _BufferStateTransition = {}) = 0;

        PC_CORE_API void RecordFetchCommand(const std::function<void(CommandList*)>& _fectFunction);

        PC_CORE_API void ExecuteExternalCommand();

        PC_CORE_API virtual void BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color) = 0;

        PC_CORE_API virtual void EndDebugLabel() = 0;

        // Setter

        CommandList& SetBufferType(BufferType _BufferType)
        {
            m_BufferType = _BufferType;
            return *this;
        }

        CommandList& SetPoolFamilly(PoolFamily _PoolFamily)
        {
            m_PoolFamily = _PoolFamily;
            return *this;
        }

        // Getter

        BufferType GetBufferType() const
        {
            return m_BufferType;
        }

        PoolFamily GetPoolFamilly() const
        {
            return m_PoolFamily;
        }

    protected:
        BufferType m_BufferType { BufferType::Primary };

        PoolFamily m_PoolFamily { PoolFamily::Graphics };

        std::vector<std::function<void(CommandList*)>> m_FetchCommands;

        DrawBuffers m_LastDrawBuffersState;

       const RhiShaderProgram* m_LastBindProgram = nullptr;

       bool DrawBufferStateChanged(const DrawBuffers& _DrawBuffers);

     private:

    };


END_PCCORE
