#pragma once

#include <string>
#include <vector>

#include "command_buffer.h"
#include "descriptor_set.hpp"
#include "rhi_typedef.h"
#include "resources/descriptor_set_layout.h"


BEGIN_PCCORE
    using RHI_LOG_CALLBACK = void (LogType, const char*);

    class RHI
    {
    public:
        PC_CORE_API static RHI& GetInstance();

        PC_CORE_API static void MakeInstance(RHI* _rhi);

        PC_CORE_API static void DestroyInstance();

        PC_CORE_API virtual ~RHI()
        {
        }

        PC_CORE_API virtual uint32_t GetCurrentImage() const
        {
            return 0;
        }

        PC_CORE_API virtual void BeginRender(CommandPoolHandle _commandBuffer)
        {
        }

        PC_CORE_API virtual void EndRender()
        {
        }


        virtual PC_CORE_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo,
                                              const std::vector<ShaderSourcePath>& _shaderSource)
        {
        }

        virtual PC_CORE_API bool DestroyShader(const std::string& _shaderProgramName)
        {
            return false;
        }


        virtual PC_CORE_API void BindShaderProgram(CommandBufferHandle _commandBuffer,
                                                   const std::string& _shaderProgramName)
        {
        }

        virtual PC_CORE_API void SwapBuffers(CommandBufferHandle* _commandBuffers, uint32_t _commandBufferCount)
        {
        }

        virtual PC_CORE_API void WaitForAquireImage()
        {
        }

        virtual PC_CORE_API void WaitDevice()
        {
        }

        virtual PC_CORE_API void RecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight)
        {
        }

        virtual PC_CORE_API void PushConstants(PC_CORE::CommandBufferHandle _commandBuffer,
                                               const std::string& _shaderProgramName,
                                               const std::string& _pushConstantName, const void* _data, uint32_t _size)
        {
        }

#pragma region CommandBuffer

        virtual PC_CORE_API void AllocCommandBuffers(PC_CORE::CommandPoolHandle _commandPoolHandle,
                                                     PC_CORE::CommandBufferCreateInfo _commandBufferCreateInfo)
        {
        }

        virtual PC_CORE_API void FreeCommandBuffer(CommandPoolHandle _commandPoolHandle, CommandBuffer* _commandBuffer,
                                                   uint32_t _commandBufferCount)
        {
        }

        virtual PC_CORE_API void CreateCommandPool(const PC_CORE::CommandPoolCreateInfo& _commandPoolCreateInfo,
                                                   PC_CORE::CommandPoolHandle* _commandPoolHandle)
        {
        }

        virtual PC_CORE_API void DestroyCommandPool(PC_CORE::CommandPoolHandle _commandPoolHandle)
        {
        }


#pragma endregion CommandBuffer

#pragma region Buffer

        virtual PC_CORE_API GPUBufferHandle BufferData(size_t _size, const void* _data,
                                                       PC_CORE::GPU_BUFFER_USAGE _usage)
        {
            return nullptr;
        }

        virtual PC_CORE_API GPUBufferHandle BufferData(size_t _size,
                                                       PC_CORE::GPU_BUFFER_USAGE _usage)
        {
            return nullptr;
        }

        virtual PC_CORE_API void MapData(GPUBufferHandle _gpuBufferHandle, void** _data)
        {
        }

        virtual PC_CORE_API void UnMapData(GPUBufferHandle _gpuBufferHandle)
        {
        }


        virtual PC_CORE_API bool DestroyBuffer(GPUBufferHandle _handle)
        {
            return false;
        }

        virtual PC_CORE_API void BindVertexBuffer(CommandBufferHandle _commandBuffer, uint32_t _firstBinding,
                                                  uint32_t _bindingCount, PC_CORE::GPUBufferHandle _handle)
        {
            return;
        }

        virtual PC_CORE_API void BindIndexBuffer(CommandBufferHandle _commandBuffer, PC_CORE::GPUBufferHandle _handle)
        {
            return;
        }

#pragma endregion Buffer

        virtual PC_CORE_API void SetViewPort(CommandBufferHandle _commandBufferHandle,
                                             const PC_CORE::ViewPort& _viewPort)
        {
        }

        virtual PC_CORE_API void SetScissor(CommandBufferHandle _commandBufferHandle,
                                            const PC_CORE::ScissorRect& _scissorRect)
        {
        }

#pragma region DrawFunction

        virtual PC_CORE_API void Draw(CommandBufferHandle _commandBufferHandle,
                                      uint32_t _vertexCount, uint32_t instanceCount, uint32_t firstVertex,
                                      uint32_t firstInstance)
        {
        }

        virtual PC_CORE_API void DrawIndexed(CommandBufferHandle _commandBufferHandle,
                                             uint32_t _indiciesCount, uint32_t instanceCount, uint32_t firstIndex,
                                             uint32_t vertexOffSet, uint32_t firstInstance)
        {
        }

#pragma endregion DrawFunction

#pragma region Descriptor
        virtual PC_CORE_API DescriptorSetLayoutHandle CreateDescriptorSetLayout(
            const std::vector<DescriptorLayoutBinding>& _descriptorSetLayouts)
        {
            return nullptr;
        }

        virtual PC_CORE_API void DestroyDescriptorSetLayout(const DescriptorSetLayoutHandle& _descriptorSetLayoutHandle)
        {
        }

        virtual PC_CORE_API DescriptorPoolHandle CreateDescriptorPool(const DescriptorPoolSize* desciptorPoolSize,
                                                                      uint32_t descriptorCount, uint32_t maxSets)
        {
            return nullptr;
        }

        virtual PC_CORE_API void DestroyDescriptorPool(DescriptorPoolHandle _descriptorPoolHandle)
        {
        }

        virtual PC_CORE_API void AllocDescriptorSet(DescriptorSet* descriptorSets, uint32_t _descriptorSetCount,
                                                    DescriptorPoolHandle _descriptorPoolHandle,
                                                    DescriptorSetLayoutHandle _descriptorSetLayoutHandle)
        {
        }

        virtual PC_CORE_API void UpdateDescriptorSet(uint32_t _descriptorWriteCount,
                                                     DescriptorWriteSet* _descriptorWrite)
        {
        }

        virtual PC_CORE_API void BindDescriptorSet(CommandBufferHandle _commandBuffer,const std::string& _shaderProgramName, uint32_t _firstSet,
            uint32_t _descriptorSetCount, const DescriptorSet* _pDescriptorSets, uint32_t _dynamicOffsetCount, const uint32_t* _pDynamicOffsets)
        {
            
        }

#pragma endregion Descriptor

#pragma region Images

        virtual PC_CORE_API ImageHandle CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, ImageType _imageType, RHIFormat _format,
                                                    ImageTiling _tiling, RHIImageUsage _usage)
        {
            return nullptr;
        }

        virtual PC_CORE_API void GenerateMimpMap(ImageHandle _image, RHIFormat _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t _mipLevels)
        {
            
        }

        virtual PC_CORE_API ImageViewHandle CreateImageView(const ImageViewCreateInfo& _imageViewCreateInfo)  { return nullptr; }

        virtual PC_CORE_API void DestroyImage(PC_CORE::ImageHandle _imageHandle) {} 

        virtual PC_CORE_API void DestroyImageView(PC_CORE::ImageViewHandle _imageViewHandle) {}

        virtual PC_CORE_API PC_CORE::SamplerHandle CreateSampler(const PC_CORE::SamplerCreateInfo&  _samplerCreateInfo)
        {
            return nullptr;
        }

        virtual PC_CORE_API void DestroySampler(PC_CORE::SamplerHandle _samplerHandle)
        {
            
        }

#pragma endregion Image

        static PC_CORE_API void RHILog(LogType _logType, const char* _message)
        {
            if (!m_Instance)
                return;

            if (!m_Instance->m_LogCallback)
                return;

            m_Instance->m_LogCallback(_logType, _message);
        }

#pragma region Command
        virtual PC_CORE_API void CopyBufferToImage(PC_CORE::GPUBufferHandle _buffer, PC_CORE::ImageHandle _image, const PC_CORE::CopyBufferImageInfo& _copyBufferImageInfo) {}

        virtual PC_CORE_API void CopyBuffer(PC_CORE::GPUBufferHandle _bufferSrc, PC_CORE::GPUBufferHandle _bufferDst, size_t _srcOffset, size_t _dstOffset, size_t _size) {};

        virtual PC_CORE_API void TransitionImageLayout(PC_CORE::ImageHandle _imageHandle, PC_CORE::ImageAspectFlagBits _imageAspectFlagBits, PC_CORE::RHIFormat _format, uint32_t _mipLevel, PC_CORE::ImageLayout _InitialLayout, PC_CORE::ImageLayout _finalLayout) {};
#pragma endregion Command

#pragma region RenderPass

        virtual PC_CORE_API RenderPassHandle CreateRenderPass(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo)
        {
            return nullptr;
        }

        virtual PC_CORE_API void DestroyRenderPass(RenderPassHandle _renderPassHandle)
        {
            
        }

#pragma endregion RenderPass

    protected:
        static RHI* m_Instance;

        RHI_LOG_CALLBACK* m_LogCallback = nullptr;
    };

END_PCCORE
