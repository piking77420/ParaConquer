#pragma once
#include <memory>
#include <vector>

#include "core_header.hpp"
#include "frame_buffer.hpp"
#include "rhi_render_pass.hpp"
#include "command_list.hpp"
#include "io/window.hpp"

BEGIN_PCCORE
    class SwapChain 
{
public:

    PC_CORE_API SwapChain(uint32_t _widht, uint32_t _height);

    PC_CORE_API SwapChain() = default;
    
    PC_CORE_API virtual ~SwapChain() = default;

    PC_CORE_API std::shared_ptr<RhiRenderPass> GetSwapChainRenderPass() const;
    
    PC_CORE_API virtual void Present(PC_CORE::Window* _window) = 0;

    PC_CORE_API virtual void GetSwapChainImageIndex(PC_CORE::Window* windowHandle) = 0;

    PC_CORE_API virtual void HandleRecreateSwapChain(PC_CORE::Window* windowHandle) = 0;

    PC_CORE_API virtual void BeginSwapChainRenderPass(CommandList* _commandList) = 0;

    PC_CORE_API virtual void EndSwapChainRenderPass(CommandList* _commandList) = 0;
    
    PC_CORE_API virtual void* GetFrameBuffer() = 0;

    PC_CORE_API inline uint32_t GetWidth() const
    {
        return m_SwapChainWidth;
    }

    PC_CORE_API inline uint32_t GetHeight() const
    {
        return m_SwapChainHeight;   
    }

    PC_CORE_API size_t GetNbrOfImage() const
    {
        return m_SwapChainImageCount;
    }

protected:
    std::shared_ptr<RhiRenderPass> m_SwapChainRenderPass;
    
    uint32_t m_SwapChainImageIndex = 0;

    uint32_t m_SwapChainWidth = 0;

    uint32_t m_SwapChainHeight = 0;

    uint8_t m_SwapChainImageCount = 0;
};

END_PCCORE