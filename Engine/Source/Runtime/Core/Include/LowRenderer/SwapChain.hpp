#pragma once
#include <Memory>
#include <Vector>

#include "CoreHeader.hpp"
#include "RhiFrameBuffer.hpp"
#include "RhiRenderPass.hpp"
#include "CommandList.hpp"
#include "Io/Window.hpp"

BEGIN_PCCORE
    class RhiSwapChain : public RhiObjectT<RhiSwapChain>
    {
    public:
        PC_CORE_API explicit RhiSwapChain(Rhi& _Rhi, uint32_t _Widht, uint32_t _Height);

        PC_CORE_API virtual ~RhiSwapChain() = default;

        PC_CORE_API std::shared_ptr<RhiRenderPass> GetSwapChainRenderPass() const;

        PC_CORE_API virtual void Present(Window* _window) = 0;

        PC_CORE_API virtual void GetSwapChainImageIndex(Window* windowHandle) = 0;

        PC_CORE_API virtual void HandleRecreateSwapChain(Window* windowHandle) = 0;

        PC_CORE_API virtual void BeginSwapChainRenderPass(CommandList* _commandList) = 0;

        PC_CORE_API virtual void EndSwapChainRenderPass(CommandList* _commandList) = 0;

        PC_CORE_API virtual void* GetFrameBuffer() = 0;

        PC_CORE_API uint32_t GetWidth() const
        {
            return m_SwapChainWidth;
        }

        PC_CORE_API uint32_t GetHeight() const
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

        uint32_t m_SwapChainImageCount = 0;
    };

END_PCCORE
