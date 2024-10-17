#pragma once

#include <string>
#include <vector>

#include "command_buffer.h"
#include "rhi_typedef.h"


BEGIN_PCCORE
    using RHI_LOG_CALLBACK = void (LogType, const char*);

    class RHI
    {
    public:
        static RHI* const GetInstance();
        
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

        virtual PC_CORE_API GPUBufferHandle BufferData(CommandPool* _commandPool, size_t _size, const void* _data,
                                                       PC_CORE::GPU_BUFFER_USAGE _usage)
        {
            return nullptr;
        }

        virtual PC_CORE_API GPUBufferHandle BufferData(CommandPool* _commandPool, size_t _size,
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

        virtual PC_CORE_API void BindVertexBuffer(CommandBufferHandle _commandBuffer,uint32_t _firstBinding, uint32_t _bindingCount, PC_CORE::GPUBufferHandle _handle)
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

        static PC_CORE_API void RHILog(LogType _logType, const char* _message)
        {
            if (!m_Instance)
                return;

            if (!m_Instance->m_LogCallback)
                return;

            m_Instance->m_LogCallback(_logType, _message);
        }

    protected:
        static RHI* m_Instance;

        RHI_LOG_CALLBACK* m_LogCallback = nullptr;
    };

END_PCCORE
